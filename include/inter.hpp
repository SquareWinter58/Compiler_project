#ifndef CLASSES_
#define CLASSES_
// base class for node in ast
#include "Decimal.hpp"
#include "Lexer.hpp"
#include "Num.hpp"
#include "Tag_enum.hpp"
#include "Token.hpp"
#include "Word.hpp"
#include "symbols.hpp"
#include <bitset>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

class Node{
    int symbol_line;
    public:
        Node(){
            // initialize line
        }
        void error(std::string err){throw std::runtime_error{err};}
        // used for the labels in three adress code
        inline static int labels{0};
        int new_label(){return ++labels;}
        void emit_label(int i){std::cout << "L"<<i<<":\n";}
        void emit(std::string s){std::cout << "\t" << s << '\n';}
};

// For convenieance, devide the base class into two possible representations an expression or a statement

class Expression: public Node{
    public:
        Token operation;
        Type type;
        Expression(Token tok, Type tp): operation{tok}, type{tp}{}
        // As per the dragon book:
        // gen returns a term that can fit the right side of a thre adress instruction.
        // Given the expression E = E1 + E2, method gen returns a term x1 + x2, where x2 and x2
        // are adresses for the values of E1 and E2.

        // Reduce, reduces an expression down to a temporary name.
        // Given an expression e reduce returns a temporary t holding the value of E.
        // Both of these will be reimplemented by the inherited classes
        Expression* gen(){return this;}
        Expression* reduce(){return this;}

        // Jumping code, boolean expressions, short circuting
        // SDD's
        void emit_jumps(std::string test, int true_exit, int false_exit){
            // 0 is the special value for the fallthrough case of the exit
            // meaning that the controll falls through to the next staement
            // no need for a jump
            std::ostringstream command;
            if (true_exit != 0 && false_exit != 0){
                // test for relationl operator, example
                // if 20 > x goto L4
                // goto L5
                // L4: some_code here
                // L5: some other code here
                command << "if " << test << " goto L" << true_exit;
                emit(command.str());
                //reset the buffer
                command.str("");
                command << "goto L" << false_exit;
                emit(command.str());
            }
            else if (true_exit == 0){
                // if (x > 200) x = 0
                // ...other_code...
                
                // iffalse x > 200 goto L5
                // L4: x = 0
                // L5: ...other_code...
                // Here L4 is the true exit and the controll falls to it
                // And L5 is the false exit and the code jumps.
                command << "iffalse " << test << "goto L" << false_exit;
                emit(command.str());
            }
            else if (false_exit == 0) {
                // if (x > 200) x = 0
                // ...other_code...
                // if x > 200 goto L4
                // L5: if ...other_code...
                // L4: x = 0
            
                // Here L5 is the false exit, and the controll falls to it
                // unless the test evaluates true.
                // The L4 is the true exit where the variable x gets set.
                command << "if " << test << " goto L" << true_exit;
                emit(command.str());
            }
            else ;// If the code reaches here, nothing needs be emited, since
            // controll just falls through so no jumps.
        }
        
        std::string to_string(){return operation.to_string();}

        void jumping(int true_exit, int false_exit){
            emit_jumps(to_string(), true_exit, false_exit);
        }
};

class Temp: public Expression{
    public:
        static inline int temp_var_count{0};
        int number{0};
        Temp(Type tp): Expression{Word{Tag::TEMP, "t"}, tp} {number = ++temp_var_count;}
        std::string to_string(){return "t" + std::to_string(number);}

};

class Identifier: public Expression{
    public:
        int offset; // relative offset
        Identifier(Word id, Type tp, int ofst): Expression{id, tp}, offset{ofst} {}
};

class Op: public Expression{
    public:
        Op(Token token, Type type): Expression(token, type){}
        Expression* reduce(){
            Expression* expr = gen();
            Temp* t = new Temp{type};
            emit(t->to_string() + " = " + expr->to_string());
            return t;
        }
};

class Arithmetic: public Op{
    public:
        Expression *expression1, *expression2;
        Arithmetic(Token tok, Expression* epxpr1, Expression* expr2):Op(tok, 0),
        expression1{epxpr1}, expression2{expr2} {
            if (auto max_type = max(expression1->type, expression2->type)){
                // not a pointer dereference, just that optional defines the operator *
                type = *max_type;
            }
            else {throw std::runtime_error{"Type error in Arithmetic\n"};}
        }
        Expression* gen(){
            return new Arithmetic(operation, expression1->reduce(), expression2->reduce());
        }
        std::string to_string(){
            return expression1->to_string() + " " + operation.to_string() + " " + expression2->to_string();
        }

};

class Unary: public Op{
    public:
        Expression* expression;
        Unary(Token token, Expression* expr): Op(token, 0), expression{expr}{
            if (auto max_type = max(types::Int, expr->type)){
                type = *max_type;
            }
            else {throw std::runtime_error{"Type error in Unary\n"};}
        }
        Expression* gen(){
            return new Unary(operation, expression->reduce());
        }
        std::string to_string(){
            return operation.to_string() + " " + expression->to_string();
        }
};

class Access: public Op{
    public:
        Identifier array;
        Expression* index;
        Access(Identifier arr, Expression* ind, Type tp): Op{Word{Tag::INDEX, "[]"}, tp}, array{arr}, index{ind} {}
        Expression* gen(){
            return new Access{array, index->reduce(), type};            
        }
        void jumping(int true_exit, int false_exit){
            // reduce defined in the op class saves a reduced expression to a temporary
            // and returnes the said temporary
            emit_jumps(reduce()->to_string(), true_exit, false_exit);
        }
        std::string to_string(){
            return array.to_string() + " [" + index->to_string() + " ]";
        }
};

class Constant: public Expression{
    public:
        Constant(Token token, Type type): Expression(token, type){}
        Constant(int i): Expression(Number(i), types::Int){}
        Constant(float f): Expression(Decimal(f), types::Float){}
        void jumping(int true_exit, int false_exit){
            std::ostringstream code;
            if (operation.tag == Tag::TRUE && true_exit != 0){
                code << "goto L" << true_exit;
                emit(code.str());
            }
            else if (operation.tag == Tag::FALSE && false_exit != 0){
                code << "goto L" << false_exit;
                emit(code.str());
            }
        }
};

class Logical: public Expression{
    public:
        Expression *expression1, *expression2;
        Logical(Token token, Expression* expr1, Expression* expr2): Expression(token, 0),
        expression1{expr1}, expression2{expr2}{
            if (auto max_type = check(expr1->type, expr2->type)){
                type = *max_type;
            }
            else {throw std::runtime_error{"Type error in LOGICAL\n"};}
        }
        Op_type check(Type t1, Type t2){
            if (std::holds_alternative<Array>(t1) || std::holds_alternative<Array>(t2)) return {};
            else if (std::get<types>(t1) == types::Bool && std::get<types>(t2) == types::Bool) return types::Bool;
            // empty return, the optianal part of optional
            return {};
        }
        Expression* gen(){
            std::string command_{};
            int false_exit_local = new_label(), end = new_label();
            Temp* temp_ = new Temp{type};
            // generate conditional jumping code for this expr
            // set true gate to fall through, since the command temp = true is right under
            this->jumping(0, false_exit_local);
            command_ = temp_->to_string() + " = true";
            emit(command_);
            // after setting the value goto the end of the statement to avoid setting 
            // the temp to false later down this flow
            command_ = "goto L" + std::to_string(end);
            emit(command_);
            
            //False exit of the above jumping command 
            emit_label(false_exit_local); 
            // setting temp to false
            command_ = temp_->to_string() + " = false";
            emit(command_);
            // End of statement
            emit_label(end);
            return temp_;
        }
        
        std::string to_string(){
            return expression1->to_string() + " " + operation.to_string() + " " + expression2->to_string();
        }
};

class Or: public Logical{
    public:
        Or(Token token, Expression* expr1, Expression* expr2): Logical{token, expr1, expr2} {}
        void jumping(int true_exit, int false_exit){
            int label = true_exit !=0? true_exit: new_label();
            // B -> B1||B2, generall case the true exit of B2 is the same as the true exit of B
            // hence B2.true = B.true, hence the call to jumping with label as argument
            // The false exit of B1, is however the first insttruction of B2, since the other side
            // of the or needs to evaluate before giving a final value
            expression1->jumping(label, 0);
            // It would be nonsensical for the ture exit of expression B1 to be 0, since it would
            // give controll over to B2, therefore the label insures this is never the case.

            // Assuming the controll reaches B2, B1 must have evaluated to false, meaning the 
            // value of this expression is thus soley dependent on the B2, if B2 is false B is false
            // if B2 is ture B is true, meaning the true and false exits of B2 are the same as those
            // for B
            expression2->jumping(true_exit, false_exit);
            // here it does not matter if the ture exit is 0, since the controll just falls through
            // to the next statement anyways.
            if (true_exit == 0) emit_label(label); // Emiting label here so that B1 has something to jump to
                                                     // in case B was called with the true_exit as 0
        }
};

class And: public Logical{
    public:
        And(Token token, Expression* expr1, Expression* expr2): Logical{token, expr1, expr2} {}
        void jumping(int true_exit, int false_exit){
            int label = false_exit !=0? false_exit: new_label();
            // B -> B1||B2, The true exit of B1 is the first instruction of B2, the reason
            // for this is that both B1, and B2 have to evaluate to true for B to be true.
            // Hence the B1(jumping) is called with 0 as its true_exit, meaning the controll
            // falls through to b
            
            // In case the false exit of b is not 0, the label wil contain the false exit, 
            // otherwise the label wil contain a new label, this is becasue it would be 
            // nonsensicdal for the controll to fall through if B1 evaluates to false, it 
            // should instead short circut.
            expression1->jumping(0, label);
            
            // Assuming the controll reaches here the whole expression is once again dependent
            // on the evaluation of B2, the ture and false gates of B2, are therfore identical 
            // to those of B2
            expression2->jumping(true_exit, false_exit);
            if (false_exit == 0) emit_label(label);
        }
};

class Not: public Logical{
    public:
        // not has only one expr, so it is repeated in the constructor for logical
        Not(Token token, Expression* expr): Logical{token, expr, expr} {}
        void jumping(int true_exit, int false_exit){
            // Not flips the true and false exits, simple enough
            expression2->jumping(false_exit, true_exit);
        }
        std::string to_string(){
            return operation.to_string() + " " + expression2->to_string();
        }
};

class Relational: public Logical{
    public:
        Relational(Token token, Expression* expr1, Expression* expr2): Logical{token, expr1, expr2} {}
        Op_type check(Type t1, Type t2){
            if (std::holds_alternative<Array>(t1) || std::holds_alternative<Array>(t2)) return {};
            // for now relational operations only allowed between same types
            else if (std::get<types>(t1) == std::get<types>(t2) ) return types::Bool;
            // empty return, the optianal part of optional
            return {};
        }
        void jumping(int true_exit, int false_exit){
            Expression* temp_B1 = expression1->reduce(), *temp_B2 = expression2->reduce();
            std::string log_test{temp_B1->to_string() + " " + operation.to_string() + " " + temp_B2->to_string()};
            // uses the jumps for relational operators defined in expression
            emit_jumps(log_test, true_exit, false_exit);
        }
};


enum class helper_stmts{
    Null, Enclosing
};

class Statement: public Node{
    public:
        Statement() {}
        virtual void gen(int begin, int after){}
        // used by while, so break can jump out of the loop
        int glob_after = 0;
};
using Op_statement = std::optional<Statement*>;
// keeps track of the enclosing statement
Statement* Enclosing{nullptr};


class If: public Statement{
    // if(E) -> Stmt
    Expression* expression;
    Statement* statement;
    public:
        If (Expression* expr, Statement* stmt): expression{expr}, statement{stmt} {
            if (!(std::holds_alternative<types>(expr->type) && std::get<types>(expr->type)== types::Bool)){
                throw std::runtime_error{"statement expects boolean expression IF class\n"};
            }
        }
        void gen(int begin, int after) override{
            int label = new_label(); // the code of the statement is attached to this label
            // if the expressionevaluates to true, just fall through to the statement
            // otherwise jump to the code after the statement
            expression->jumping(0, after);
                                // statement code (->Stmt)
            emit_label(label); statement->gen(label, after);
        }
};

class Else: public Statement{
    public:
        Expression* expression;
        Statement* statement1, *statement2;
        Else(Expression* expr, Statement* stmt1, Statement* stmt2): expression{expr}, 
        statement1{stmt1}, statement2{stmt2} {
            if (!(std::holds_alternative<types>(expr->type) && std::get<types>(expr->type)== types::Bool)){
                throw std::runtime_error{"If-else expects a boolean expression\n"};
            }
        }
        void gen(int begin, int after) override{
            // stmt1 label            stmt2 label
            int label1 = new_label(), label2 = new_label();
            expression->jumping(0, label2);
            // If expression evaluates to true, fall through to stmt 1, else jump to stmt 2
            // code for statement 1
            emit_label(label1); statement1->gen(label1, after);
            // if the statement one gets executed and the control reaches here this means
            // the code executed the if breanch and must jump over the else branch
            // hence the goto
            std::string jump{"goto L" + std::to_string(after)};
            emit(jump);
            emit_label(label2); statement2->gen(label2, after);

        }
};


class While: public Statement{
    public:
        Expression* expression;
        Statement* statement;
        While(): expression{nullptr}, statement{nullptr} {}
        void init(Expression* expr, Statement* stmt){
            if (!(std::holds_alternative<types>(expr->type) && std::get<types>(expr->type)== types::Bool)){
                throw std::runtime_error{"While expects a boolean expression\n"};
            }
            expression = expr;
            statement = stmt;
        }
        void gen(int begin, int after) override{
            glob_after = after;
            // if false break from while aka goto after, else fall through
            expression->jumping(0, after);
            int label = new_label();
            // begin is given as the second argument, because it is supposed to loop
            // when it does the statement
            emit_label(label); statement->gen(label, begin);
            std::string jump{"goto L" + begin};
        }
};

class Do: public Statement{
    public:
        Expression* expression;
        Statement* statement;
        Do(): expression{nullptr}, statement{nullptr}{}
        void init(Statement* stmt, Expression* expr){
            if (!(std::holds_alternative<types>(expr->type) && std::get<types>(expr->type)== types::Bool)){
                throw std::runtime_error{"Do-While expects a boolean expression\n"};
            }
            statement = stmt;
            expression = expr;
        }
        void gen(int begin, int after) override{
            glob_after = after;
            int label = new_label();
            // since this is dowhile, the statement goes before the expression
            statement->gen(begin, label);
            // attaches label to the expression evaluation
            emit_label(label);
            expression->jumping(begin, 0);
            // if true, jumps to begin, else falls through
        }
};

class Set: public Statement{
    // assignment where identifier is lfs, and epxression is rhs
    public:
        Identifier identifier;
        Expression* expression;
        Set(Identifier id, Expression* expr): identifier{id}, expression{expr} {
            if (auto decided_type = check(identifier.type, expression->type)){
                
            }
            else {
                throw std::runtime_error{"Identifier and expression types do not match in Statement\n"};
            }
        }
        Op_type check(Type t1, Type t2){
            if (std::holds_alternative<types>(t1) && std::holds_alternative<types>(t2)){
                types type1 = std::get<types>(t1), type2 = std::get<types>(t2);
                if (numeric(type1) && numeric(type2)) return type2;
                else if(type1 == types::Bool && type2 == types::Bool)return type2;
                return {};
            }
            // empty return, the optianal part of optional
            return {};
        }
        void gen(int begin, int after) override{
            std::string code{identifier.to_string() + " = " + expression->gen()->to_string()};
            emit(code);
        }
};  

class Set_element: public Statement{
    // same as set, but for an element of an array
    public:
        Identifier array;
        Expression* index;
        Expression* expression;
        Set_element(Access x, Expression* y): array{x.array}, index{x.index}, expression{y} {
            if (auto decided_type = check(x.type, expression->type)){

            }
            else {
                throw std::runtime_error{"Type error Set element\n"};
            }
        }
        Op_type check(Type t1, Type t2){
            if (std::holds_alternative<types>(t1) && std::holds_alternative<types>(t2)){
                types type1 = std::get<types>(t1), type2 = std::get<types>(t2);
                if (numeric(type1) && numeric(type2)) return type2;
                else if(type1 == type2) return type2;
                return {};
            }
            // empty return, the optianal part of optional
            return {};
        }
        void gen(int begin, int after) override{
            std::string str3{array.to_string() + " [ " + index->reduce()->to_string()
             + " ] = " + expression->reduce()->to_string()};
            // calculates/reduces expression to a number, sets the element of array array
            // at the location index, to the location expression
            emit(str3);
        }
};

class Sequence: public Statement{
    // This is a sequence of statements
    public:
        Statement* statement1, *statement2;
        Sequence(Statement* stmt1, Statement* stmt2): statement1{stmt1}, statement2{stmt2} {}
        void gen(int begin, int after) override{
            if (statement1 == nullptr) statement2->gen(begin, after);
            else if (statement2 == nullptr) statement1->gen(begin, after);
            else{
                int label = new_label();
                statement1->gen(begin, label);
                emit_label(label);
                statement2->gen(label, after);
            }
        }
};

class Break: public Statement{
    public:
        Statement* statement;
        Break(){
            if (Enclosing == nullptr){
                throw std::runtime_error{"Break: Unenclosed break\n"};
            }
            statement = Enclosing;
        }
        void gen(int begin, int after) override{
            std::string jump{"goto L" + std::to_string(statement->glob_after)};
            emit(jump);
        }
};

// This code should technically be in Inter, but then there would be recursive includes
// Can be solved by #pragma once, but I would have to seperate the method bodies 
// from the declarations
class Scope{
    protected:
        Scope* previous = nullptr;
        // the second arg is a placeholder
        std::unordered_map<Token, Identifier*, token_hash> map;
        
    public:
        Scope(Scope* prev): previous{prev} {}
        void insert(Token word, Identifier id){map.insert({word, new Identifier{id}});}
        Identifier* get(Token word){
            Scope* current = this;
            while (current){
                auto it = map.find(word);
                if (it != map.end()) return it->second;
                else current = current->previous;
            }
            // The identifier was not found
            return nullptr;
        }
        // implement map insertion once id is defined
        // implement map retrieaval once id is defined

};

// starting from the gighest priority classes in the expression grammar:

// The last step in the grammar is the factor production, this is an indevisible production
// It does not have its own calss since it returns one of many nodes, such as number literal, string literal and so on.

enum class Literal_types{
    String_literal, Number_literal, 
    Real_literal, Bool_literal, char_literal
};

class Literals: public Expression{
    Literal_types type;
    std::variant<std::string, int, float, bool> value;
};




#endif