#ifndef TAG_ENUM
#define TAG_ENUM
enum Tag{
    // key words
    WHILE=256, FOR, IF, ELSE, RETURN, 
    // single character ones.
    // 261
    ASSIGN, PAREN_OPEN, PAREN_CLOSED, BRACKET_OPEN, BRACKET_CLOSED, SQUARE_BRACKET_OPEN, SQUARE_BRACKET_CLOSED, DOT, NOT,
    
    // relation operators
    //270
    EQUAL, LESS_THAN, MORE_THAN, LESS_THEN_EQUAL, MORE_THAN_EQUAL, NOT_EQUAL,
    
    // types
    //276
    INT, FLOAT, CHAR, STRING, BOOL,
    
    // logical
    // 281
    AND, OR,
    
    //oprators
    //283
    ADD, SUB, MULT, DIV,
    
    // LITERALS
    //287
    NUMBER, DECIMAL,  STRING_LITERAL, IDENTIFIER, TRUE, FALSE, COMMENT, MULTILINE_COMMENT, EOF_=-1}
    ;
#endif

/*
**/