#include "Decimal.hpp"
#include "Lexer.hpp"
#include "Num.hpp"
#include "Tag_enum.hpp"
#include "Token.hpp"
#include "Word.hpp"
#include "inter.hpp"
#include "symbols.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>

Lexer lexer{"/home/squarewinter/school/programming/third_year/Compilers/Compiler_project/src/file.sb"};
Scope* top = nullptr;
// used declarations
int used {0};
std::shared_ptr<Token> look;

void move(){look = lexer.scan();}
void error(std::string err){throw std::runtime_error{err};}
void match(Tag tag){
    if (look->tag == tag) move();
    else error("Syntax error");
}
bool basic_tags(std::shared_ptr<Token> lk){
    switch (look->tag) {
        case Tag::INT:
        case Tag::FLOAT:
        case Tag::BOOL:
        case Tag::CHAR:
            return true;
        default:
            return false;
    }
}
void match(){
    switch (look->tag) {
        case Tag::INT:
        case Tag::FLOAT:
        case Tag::BOOL:
        case Tag::CHAR:
            move();
        default:
            error("Not a basic type\n");
    }
}
void program();
Statement* block();
void declarations();
Type type();
Type dimensions(Type tp);
Statement* statements();
Statement* statement();
Statement* assign();
Expression* Or_func();
Expression* And_func();
Expression* equality();
Expression* relational();
Expression* expression();
Expression* mutl_div();
Expression* unary();
Expression* factor();
Access* offset(Identifier* identifier);

Constant True(Word{Tag::TRUE, "true"}, types::Bool);
Constant False(Word{Tag::FALSE, "false"}, types::Bool);

void program(){
    Statement* stmt = block();
    int begin = stmt->new_label(), after = stmt->new_label();
    stmt->emit_label(begin); stmt->gen(begin, after); stmt->emit_label(after);
}

Statement* block(){
    match(Tag::BRACKET_OPEN); Scope* saved_scope = top; top = new Scope(saved_scope);
    declarations(); Statement* stmt = statements();
    match(Tag::BRACKET_CLOSED); top = saved_scope;
    return stmt;
}

void declarations(){
    while (basic_tags(look)){

    }
}

Type type(){
    Type tp;
    switch (look->tag) {
        case Tag::TRUE:
        case Tag::FALSE:
            tp = types::Bool;
            break;
        case Tag::NUMBER:
            tp = types::Int;
            break;
        case Tag::DECIMAL:
            tp = types::Float;
            break;
        match();
        if (look->tag != Tag::SQUARE_BRACKET_OPEN) return tp;
        else{
            if (auto n = std::dynamic_pointer_cast<Number>(look)){
                if (std::holds_alternative<types>(tp)){
                    return Array(n->value, std::get<types>(tp));
                }
                error("Array type is ");
            }
        }
    }
}

Type dimensions(Type tp){
    // [number]
    match(Tag::SQUARE_BRACKET_CLOSED); std::shared_ptr<Token>tok = look; match(Tag::NUMBER); match(Tag::SQUARE_BRACKET_CLOSED);
    if (look->tag == Tag::SQUARE_BRACKET_OPEN){
        tp = dimensions(tp);
    }
    if (auto num = std::dynamic_pointer_cast<Number>(tok)){
        if (std::holds_alternative<types>(tp)){
            return Array(num->value, std::get<types>(tp));
        }
        error("Wrong array type in Dimensions\n");
    }
    error("Could not cast to number in dimesnisons\n");
}

Statement* statments(){
    // }
    if (look->tag == Tag::BRACKET_CLOSED) return nullptr;
    else return new Sequence(statement(), statements());
}

Statement* staement(){
    Expression* expr; Statement* stmt, *stmt1, *stmt2;
    Statement* saved_statement; // save enclosing loop for break to jump out of
    switch (look->tag) {
        case Tag::SEMICOLON:
            move();
            // empty statement, aka we finished the statemnt x = 5;
            return nullptr;
        case Tag::IF:
            // if (expr)
            match(Tag::IF); match(Tag::PAREN_OPEN); expr = Or_func(); match(Tag::PAREN_CLOSED);
            stmt1 = statement();
            if (look->tag != Tag::ELSE) return new If(expr, stmt1);
            match(Tag::ELSE);
            stmt2 = staement();
            return new Else(expression(), stmt1, stmt2);
        case Tag::WHILE:
            // need the enclosing block because of the varialbe declaration
            {While* wihle_node = new While{};
            saved_statement = Enclosing; Enclosing = wihle_node;
            // while (expr)
            match(Tag::WHILE); match(Tag::PAREN_OPEN); expr = Or_func(); match(Tag::PAREN_CLOSED);
            stmt1 = staement();
            wihle_node->init(expr, stmt1);
            Enclosing = saved_statement;
            return wihle_node;}
        case Tag::DO:
            {Do* do_node = new Do();
            saved_statement = Enclosing; Enclosing = do_node;
            match(Tag::DO);
            stmt1 = statement();
            match(Tag::WHILE); match(Tag::PAREN_OPEN); expr = Or_func(); match(Tag::PAREN_CLOSED); match(Tag::SEMICOLON);}
        
        case Tag::BREAK:
            match(Tag::BREAK); match(Tag::SEMICOLON);
            return new Break{};
        case Tag::BRACKET_OPEN:
            return block();
        default:
            return assign();

    }
}

Statement* assign(){
    Statement* stmt; 
    Token tok{*look};
    Identifier* id = nullptr;//top->get(tok);
    if (id == nullptr) {error(tok.to_string() + "is undeclared");}
    if (look->tag == Tag::ASSIGN){
        move(); stmt = new Set(*id, Or_func());
    }
    else {
        Access* x = offset(id);
        match(Tag::ASSIGN); stmt = new Set_element(*x, Or_func());
    }
    match(Tag::SEMICOLON);
    return stmt;
}

Expression* Or_func(){
    Expression* expr{And_func()};
    while(look->tag == Tag::OR){
        Token tok{*look}; move(); expr = new Or(tok, expr, And_func());
    }
    return expr;
}

Expression* And_func(){
    Expression* expr{equality()};
    while (look->tag == Tag::AND) {
        Token tok{*look}; move(); expr = new And(tok, expr, equality());
    }
    return expr;
}

Expression* equality(){
    Expression* expr{relational()};
    while (look->tag == Tag::EQUAL || look->tag == Tag::NOT_EQUAL) {
        Token tok{*look}; move(); expr = new Relational(tok, expr, relational());
    }
    return expr;
}

Expression* relational(){
    Expression* expr{expression()};
    switch (look->tag) {
        // Leveraging fall through
        case Tag::LESS_THAN:
        case Tag::LESS_THEN_EQUAL:
        case Tag::MORE_THAN:
        case Tag::MORE_THAN_EQUAL:
            // must be encased in block because of variable declaration
            {Token tok{*look}; move(); return new Relational(tok, expr, expression());}
        default:
            // controll always reaches here, that is why there is no break above
            return expr;
    }
}

Expression* expression(){
    Expression* expr{mutl_div()};
    while (look->tag == Tag::ADD || look->tag == Tag::SUB){
        Token tok{*look}; move(); expr = new Arithmetic(tok, expr, mutl_div());
    }
    
}

Expression* mutl_div(){
    // evaluating the left side of the expression
    Expression* expr = unary();
    while (look->tag == Tag::MULT || look->tag == Tag::DIV){
        Token tok{*look}; move(); expr = new Arithmetic(tok, expr, unary());
    }
    return expr;
}

Expression* unary(){
    if (look->tag == Tag::SUB){
        move(); return new Unary(Word(Tag::SUB, "minus"), unary());
    }
    else if (look->tag == Tag::NOT){
        Token tok{*look}; move(); return new Not(tok, unary());
    }
    else return factor();
}

Expression* factor(){
    using std::string;
    Expression *expr = nullptr;
    switch (look->tag) {
        // (
        case Tag::PAREN_OPEN:
            move(); expr = Or_func();; match(Tag::PAREN_CLOSED);
            return expr;
        case Tag::NUMBER:
            if(auto n = std::dynamic_pointer_cast<Number>(look)){
                expr = new Constant(n->value); move(); return expr;
            }
            else {throw std::runtime_error{"Token with tag number, cannot be cast to number\n"};}
        case Tag::DECIMAL:
            if (auto n = std::dynamic_pointer_cast<Decimal>(look)){
                expr = new Constant(n->value); move(); return expr;
            }
            else {throw std::runtime_error{"Token with tag decimal, cannot be cast to a decimal\n"};}
        case Tag::TRUE:
            // implicit copy constructor go brr
            expr = new Constant{True}; move(); return expr;
        case Tag::FALSE:
            expr = new Constant{False}; move(); return expr;
        case Tag::IDENTIFIER:{ // because of variable initialization, the case needs to be in a block
            Identifier* identifier = nullptr;//top->get(*look);
            if (identifier == nullptr){
                error(look->to_string() + "is undeclared");
            }
            move();
            //              [
            if (look->tag != Tag::SQUARE_BRACKET_OPEN) return identifier;
            else return offset(identifier);}
        default:
            error("Syntax error from factor\n");
            return expr;
    }
}

// Access* offset(Identifier* identifier){
//     Expression* index, *w, *t1, *t2, *loc;
//     Type type = identifier->type;
//     // [number], or is the root of the numeric expression
//     match(Tag::SQUARE_BRACKET_OPEN); index = Or(); match(Tag::SQUARE_BRACKET_CLOSED);

// }