/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stack>
#include <algorithm>

#include "lexer.h"
#include "inputbuf.h"

#define DEBUG

using namespace std;
int parse_body(void);
Token token1;

string reserved[] = 
{ 
"END_OF_FILE", 
"INT",
"REAL",
"BOO",
"TR",
"FA",
"IF",
"WHILE",
"SWITCH",
"CASE",
 "PUBLIC",
 "PRIVATE",
 "NUM",
 "REALNUM",
 "NOT",
 "PLUS",
 "MINUS", "MULT", "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL", "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR","BASE08","BASE16","UNKNOWN" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private" };

    LexicalAnalyzer lexer;
    Token token;
    TokenType tempTokenType;

/*
struct scopeResolve {
 char* scope;
 scopeResolve* next;
};

struct sTableEntry {
    char* name;
    char* scope;
    int pubpriv;
    int binNo;

};

struct sTable {
sTableEntry* item;
sTable *prev;
sTable *next;

};*/

SymbolTable::SymbolTable()
{
current = new scopeTable;
current->previous = NULL;
pointer = current;
}


void Token::Print()
{/*
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";*/
}


LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}



bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComments()
{
    char c,c2;
    bool comment_encountered = false;

    input.GetChar(c);
    while (c == '/')
    {
        input.GetChar(c);
        if (c2 != '/')
        {
            input.UngetChar(c2);
            comment_encountered = false;
            tmp.token_type = ERROR;
            return comment_encountered;
        }
        else
        {
            while (c != '\n' && !input.EndOfInput())
            {
                comment_encountered = true;
                input.GetChar(c);
                line_no += (c == '\n');
            }
            SkipSpace();
            input.GetChar(c);
        }
    }
return comment_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}



bool ispdigit(char c) {
    return c >= '1' && c <= '9';
}

bool ispdigit8(char c) {
    return c >= '1' && c <= '7';
}

bool ispdigit16(char c) {
    return (c >= '1' && c <= '9') || (c >= 'A' && c <= 'F');
}

bool isdigit(char c) {
    return c == '0' || ispdigit(c);
}

bool isdigit8(char c) {
    return c == '0' || ispdigit8(c);
}

bool isdigit16(char c) {
    return c == '0' || ispdigit16(c);
}

bool LexicalAnalyzer::MatchSequence(string s) 
{
    char c;

    for (int i = 0; i < s.length(); i++) {
        input.GetChar(c);
        if (input.EndOfInput() || c != s[i]) {
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            for (int j = 0; j < i; j++) {
                input.UngetChar(s[i-j-1]);
            }
            return false;
        }
    }
    return true;
}

Token LexicalAnalyzer::ScanNumber() 
{
    string lex;

    lex = ScanRealNumIfPossible();
    if (lex.length() > 0) {
        tmp.lexeme = lex;
        tmp.token_type = REALNUM;
        tmp.line_no = line_no;
        return tmp;
    }
    lex = ScanBase08NumIfPossible();
    if (lex.length() > 0) {
        tmp.lexeme = lex;
        tmp.token_type = BASE08NUM;
        tmp.line_no = line_no;
        return tmp;
    }
    lex = ScanBase16NumIfPossible();
    if (lex.length() > 0) {
        tmp.lexeme = lex; 
        tmp.token_type = BASE16NUM;
        tmp.line_no = line_no;
        return tmp;
    }
    lex = ScanNumIfPossible();
    if (lex.length() > 0) {
        tmp.lexeme = lex;
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    }

    tmp.lexeme = "";
    tmp.token_type = ERROR;
    tmp.line_no = line_no;
    return tmp;
}

// Returns the lexeme of the NUM to scan or empty string if 
// there is not a valid NUM
string LexicalAnalyzer::ScanNumIfPossible()
{
    char c;
    string scanned = "";

    input.GetChar(c);
    scanned += c;
    if (isdigit(c)) {
        if (c == '0') {
            return scanned;
        } else {
            input.GetChar(c);
            while (!input.EndOfInput() && isdigit(c)) {
                scanned += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            return scanned;
        }
    } 
    input.UngetString(scanned);
    return "";
}

// Returns the lexeme of the REALNUM to scan or empty string if 
// there is not a valid REALNUM
string LexicalAnalyzer::ScanRealNumIfPossible() 
{
    char c;
    bool pdigit_scanned_digit_after_dot = false;
    string scanned = "";
    input.GetChar(c);
    scanned += c;
    if (c == '0') {
        if (MatchSequence(".")) {
            scanned += '.';
            input.GetChar(c);
            while(!input.EndOfInput() && c == '0') {
                scanned += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput() && ispdigit(c)) {
                while(!input.EndOfInput() && isdigit(c)) {
                    scanned += c; 
                    input.GetChar(c);
                }
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                return scanned;
            }
        }
    } else if (ispdigit(c)) {
        input.GetChar(c);
        while(!input.EndOfInput() && isdigit(c)) {
            scanned += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        if (MatchSequence(".")) {
            scanned += '.';
            pdigit_scanned_digit_after_dot = false;
            input.GetChar(c);
            while (!input.EndOfInput() && isdigit(c)) {
                pdigit_scanned_digit_after_dot = true;
                scanned += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
            if (pdigit_scanned_digit_after_dot) {
                return scanned;
            }
        }
    }
    input.UngetString(scanned);
    return "";
}

// returns the lexeme of the longest base16num that exists or empty string 
// if the next token cannot be a base16num
string LexicalAnalyzer::ScanBase16NumIfPossible()
{
    char c; 
    string scanned = "";
    input.GetChar(c);
    scanned += c;   
    if (c == '0') {
        if (MatchSequence("x16")) {
            scanned += "x16";
            return scanned;
        }
    } else if (ispdigit16(c)) {
        input.GetChar(c);
        while (!input.EndOfInput() && isdigit16(c)) {
            scanned += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        if (MatchSequence("x16")) {
            scanned += "x16";
            return scanned;
        }
    }
    input.UngetString(scanned);
    return "";
}

// Returns the lexeme of the base08num to scan or empty string if 
// there is not a valid base08num
string LexicalAnalyzer::ScanBase08NumIfPossible() 
{
    char c;
    string scanned = "";
    input.GetChar(c);
    scanned += c;
    if (c == '0') {
        if (MatchSequence("x08")) {
            scanned += "x08";
            return scanned; 
        }
    } else if (ispdigit8(c)) {
        input.GetChar(c);
        while(!input.EndOfInput() && isdigit8(c)) {
            scanned += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        if (MatchSequence("x08")) {
            scanned += "x08";
            return scanned;
        }
    }
    input.UngetString(scanned);
    return "";
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);
    
    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c, c2;
    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    while (c == '/')
    {
        input.GetChar(c2);
        if (c2 != '/')
        {
            input.UngetChar(c2);
            tmp.token_type = ERROR;
            return tmp;
        }
        else
        {
            while (c != '\n' && !input.EndOfInput())
            {
                input.GetChar(c);
            }
            SkipSpace();
            input.GetChar(c);
        }
    }
    switch (c) {
        case '!':
            tmp.token_type = NOT;
            return tmp;    
        case '+':
            tmp.token_type = PLUS;
            return tmp;    
        case '-':
            tmp.token_type = MINUS;
            return tmp; 
        case '*':
            tmp.token_type = MULT;
            return tmp;       
        case '/':
            tmp.token_type = DIV;
            return tmp;    
        case '>':
            input.GetChar(c);
            if(c == '='){
                tmp.token_type = GTEQ;   
            }else{
                input.UngetChar(c);
                tmp.token_type = GREATER;
            }
            return tmp;    
        case '<':
            input.GetChar(c);
            if(c == '='){
                tmp.token_type = LTEQ;   
            }else if (c == '>'){
                tmp.token_type = NOTEQUAL;    
            }else{
                input.UngetChar(c);
                tmp.token_type = LESS;
            }
            return tmp;            
        case '(':
            //cout << "\n I am here" << c << " \n";
            tmp.token_type = LPAREN;
            return tmp;    
        case ')':
            tmp.token_type = RPAREN;
            return tmp;    
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                //cout << "\n ID scan " << c << " \n"; 
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}




// parse scope

/*************************************************************
 * unary_operator ® NOT
 * *************************************************************/
TokenType Parser::parse_unaryOperator(void)
{
    token = lexer.GetToken();
    if(token.token_type == NOT)
        return NOT;
    else
        return ERROR;
}
/*************************************************************
 * binary_operator ® PLUS | MINUS | MULT | DIV
 * binary_operator ® GREATER | LESS | GTEQ | LTEQ | EQUAL | NOTEQUAL
 * *************************************************************/
int Parser::parse_binaryOperator(void)
{
    token = lexer.GetToken();
    if(token.token_type == PLUS  )
    {
        return 1;
    }
    else if(token.token_type == MINUS )
    {
        return 1;
    }
    else if(token.token_type == MULT)
    {
        return 1;
    }
    else if(token.token_type == DIV )
    {
        return 1;
    }
    else if(token.token_type == GREATER)
    {
        return 2;
    }
    else if(token.token_type == LESS  )
    {
        return 2;
    }
    else if(token.token_type == GTEQ )
    {
        return 2;
    }
    else if(token.token_type == LTEQ)
    {
        return 2;
    }
    else if(token.token_type == EQUAL )
    {
        return 2;
    }
    else if(token.token_type == NOTEQUAL)
    {
        return 2;
    }
    else
    {
        return -1;
    }
}
/*************************************************************
 * primary ® ID
 * primary ® NUM
 * primary ® REALNUM
 * primary ® TRUE
 * primary ® FALSE
 * *************************************************************/
TokenType Parser::parse_primary(void)
{
    token = lexer.GetToken();
    if(token.token_type == ID )
    {
        return ID; 
    }
    else if(token.token_type == NUM )
    {
        //enumType = 1;
        //cout << "\n Rule parsed: primary -> NUM \n";
        return INT;
    }
    else if(token.token_type == REALNUM)
    {
        //enumType = 2;
        //cout << "\n Rule parsed: primary -> REALNUM\n";
        return REAL;
    }
    else if(token.token_type == TR )
    {
        //enumType = 3;
        //cout << "\n Rule parsed: primary -> TRUE \n";
        return BOO;
    }
    else if(token.token_type == FA)
    {
        //enumType = 3;
        //cout << "\n Rule parsed: primary -> FALSE \n";
        return BOO;
    }
    else
    {
        cout << "\n Syntax Error \n";
        return ERROR;
    }
}

/*************************************************************
 * expression ® primary
 * expression ® binary_operator expression expression
 * expression ® unary_operator expression
 * unary_operator ® NOT
 * binary_operator ® PLUS | MINUS | MULT | DIV
 * binary_operator ® GREATER | LESS | GTEQ | LTEQ | EQUAL | NOTEQUAL
 * primary ® ID
 * primary ® NUM
 * primary ® REALNUM
 * primary ® TRUE
 * primary ® FALSE
 * *************************************************************/
/*bool isExpress(int c){
    if(c != 15 && c != 16 && c != 17 && c != 18 && c != 19 && c != 20 && c != 21 && c != 22 && c != 23 && c != 26){
        return true;
    }
    else{
        return false;
    }
}*/

bool isEpr(int i){
    if(i == PLUS || i == MINUS || i == MULT || i == DIV || i == GREATER || i == LESS || i == GTEQ || i == LTEQ || i == EQUAL || i == NOTEQUAL){
        return true;
    }
    else{
        return false;
    }
}
/*
void compare_L(int line_No, int token_Type)
{
    sTable* temp = symbolTable;
    while(temp->next != NULL)
    {
        if(temp->item->lineNo == line_No)
        {
            temp->item->type = token_Type;
        }
        temp = temp->next;
    }
    if(temp->item->lineNo == line_No)
    {
        temp->item->type = token_Type;
    }
}*/



TokenType Parser::parse_expression(void)
{
    int opcodeset;
    TokenType tempI;
    symbolTable L,R;
    token = lexer.GetToken();
    #ifdef DEBUG
    cout << "first: " <<reserved[token.token_type] <<endl;
    #endif
    if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA )
    {
        lexer.UngetToken(token);
        tempI = parse_primary();
        if(tempI == ID)
        {
            symbolTable n;
            n = table.findVariable(token.lexeme);
            #ifdef DEBUG
            cout << "variable: " << n.variable << "\t" << n.token_field << endl;
            #endif
            tempI = n.token_field;
        }
    }
    else if(isEpr(token.token_type))
    {
        TokenType leftExp;
        TokenType rightExp;
        tempTokenType = lexer.UngetToken(token);
        opcodeset = parse_binaryOperator(); // this returns 0 if lType cant be bool, 1 if lType can be anything, -1 if type error

        leftExp = parse_expression();
        if(leftExp == ID)
        {
            L = table.findVariable(token.lexeme);
            #ifdef DEBUG
            cout << "LHS Exp: " << L.variable << "\t" << L.token_field << endl;
            #endif
            leftExp = L.token_field;
        }
        rightExp = parse_expression();
        if(rightExp == ID)
        {
            R = table.findVariable(token.lexeme);
            #ifdef DEBUG
            cout << "RHS Exp:" << R.variable << R.token_field << endl;
            #endif
            rightExp = R.token_field;
        }
            
        if(leftExp != rightExp)
        {
            #ifdef DEBUG
            cout << "left value" <<leftExp <<"\t"<<"right value:"<<rightExp <<endl;
            #endif
            if(opcodeset == 1) //PLUS | MINUS | MULT | DIV
            {
                if(leftExp == UNKNOWN || rightExp != UNKNOWN)
                {
                    leftExp = rightExp;
                    table.update_Types(L.variable, leftExp);
                }
                else if(leftExp != UNKNOWN || rightExp == UNKNOWN)
                {
                    rightExp = leftExp;
                    table.update_Types(R.variable, rightExp);
                }
                else if(leftExp == UNKNOWN || rightExp == UNKNOWN)
                {
                    
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            #ifdef DEBUG
            cout << "left value" <<leftExp <<"\t"<<"right value:"<<rightExp <<endl;
            #endif
            }

            else if(opcodeset == 2) //GREATER | LESS | GTEQ | LTEQ | EQUAL | NOTEQUAL
            {
                if(leftExp == UNKNOWN || rightExp != UNKNOWN)
                {
                    leftExp = rightExp;
                    table.update_Types(L.variable, leftExp);
                }
                else if(leftExp != UNKNOWN || rightExp == UNKNOWN)
                {
                    rightExp = leftExp;
                    table.update_Types(R.variable, rightExp);
                }
                else if(leftExp == UNKNOWN || rightExp == UNKNOWN)
                {
                    
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            #ifdef DEBUG
            cout << "left value" <<leftExp <<"\t"<<"right value:"<<rightExp <<endl;
            #endif
            }
            else
            {
                cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                exit(1);
            }
        }
        if(opcodeset == 2)
        {
            tempI = BOO;
        }
        else
        {
            tempI = rightExp;
        }
    }
    else if(token.token_type == NOT)
    {
        //cout << "\n Rule parsed: expression -> unary_operator expression \n";
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_unaryOperator(); // return 0 for error and return 1 for NOT
        tempI = parse_expression();
        if(tempI != 3)
        {
            cout << "TYPE MISMATCH " << token.line_no << " C3"<<endl;
            exit(1);
        }
    }
    else
    {
        syntax_error();
        return ERROR;
    }
return tempI;
}
/*************************************************************
 * assignment_stmt ® ID EQUAL expression SEMICOLON
 * expression ® primary
 * expression ® binary_operator expression expression
 * expression ® unary_operator expression
 * unary_operator ® NOT
 * binary_operator ® PLUS | MINUS | MULT | DIV
 * binary_operator ® GREATER | LESS | GTEQ | LTEQ | EQUAL | NOTEQUAL
 * primary ® ID
 * primary ® NUM
 * primary ® REALNUM
 * primary ® TRUE
 * primary ® FALSE
 * 
 * *************************************************************/

int Parser::parse_assstmt(void){
    int tempI;
    TokenType lType, rType;
    symbolTable n;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
          n = table.findVariable(token.lexeme);
          #ifdef DEBUG
          cout << "LHS Var: " << n.variable <<"\t" << n.token_field << endl;
          #endif
          lType = n.token_field;
        // it is not available in the searchList, add the token to the list, make its type = -1; make its scope = "h".
        token = lexer.GetToken();
        if(token.token_type == EQUAL)
        {
             token = lexer.GetToken();  
             if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA || token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == LESS || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL || token.token_type == NOT)
             {
                tempTokenType = lexer.UngetToken(token);
                rType = parse_expression();
                #ifdef DEBUG
                cout << "RHS Var: " << rType <<endl;
                #endif
                if(lType != rType)
                {
                     if(lType == UNKNOWN && rType != UNKNOWN)
                     {
                        lType = rType;
                        table.update_Types(n.variable, lType);
                     }
                     else
                     {
                        cout << "TYPE MISMATCH " << token.line_no << " C1"<<endl;
                        exit(1);
                     }
                    
                }
                if(lType == UNKNOWN && rType == UNKNOWN)
                {
                    #ifdef DEBUG
                    cout<<"Unknown variable type";
                    #endif
                }
               
                // if any one of lType or rType is -1 then should not throw type mismatch. 
                // if lType != -1 && rType is -1 then you search for left ID token to extract its type. searchList should return type. 
                // you have to use search list again with the right token to update the right token's type to lType 
                token = lexer.GetToken();
                if(token.token_type == SEMICOLON)
                {
                    #ifdef DEBUG
                    cout << "\n Rule parsed: assignment_stmt -> ID EQUAL expression SEMICOLON \n";
                    #endif
                }
                else
                {
                     syntax_error();    
                }
             }
             else
             {
                 syntax_error();   
             }

        }
        else
        {
            syntax_error();    
        }  
        
    }else{
        syntax_error();    
    }
    return(0);
}


/*****************************************************************
* switch_stmt ® SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE
* case_list ® case
* case_list ® case case_list
* case ® CASE NUM COLON body
* *************************************************************/
void Parser::parse_case(void){

    int tempI;
    token = lexer.GetToken();
    if(token.token_type == CASE ){
        token = lexer.GetToken();
        if(token.token_type == NUM){
            token = lexer.GetToken();
            if(token.token_type == COLON)
            {
                #ifdef DEBUG
                cout << "\n Rule parsed: case -> CASE NUM COLON body";
                #endif
                parse_body();
            }
            else
            {
                syntax_error();
            }
        
        }
        else
        {
            syntax_error();
        }
    
    }
    else
    {
        syntax_error();
    }
}


int Parser::parse_caselist(void){
    
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == CASE)
    {
        tempTokenType = lexer.UngetToken(token);
        parse_case();
        token = lexer.GetToken();
        if(token.token_type == CASE){
            tempTokenType = lexer.UngetToken(token);
            #ifdef DEBUG
            cout << "\n Rule parsed: case_list -> case case_list \n";
            #endif
            parse_caselist();
        }
        else if(token.token_type == RBRACE)
        {
            tempTokenType = lexer.UngetToken(token);
            #ifdef DEBUG
            cout << "\n Rule parsed: case_list -> case  \n";
            #endif
        }
    }
    return(0);
}


int Parser::parse_switchstmt(void){
    int tempI;
    
    token = lexer.GetToken();
    if(token.token_type == SWITCH)
    {
        token = lexer.GetToken();
        if(token.token_type == LPAREN)
        {
            tempI = parse_expression();
            if(tempI != INT)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C5"<<endl;
                exit(1);
            } 
            // else if tempI = -1 ==> parse_expresssion retunred an ID, then go and change using searchList the type of ID to 1.
            token = lexer.GetToken();
            if(token.token_type == RPAREN)
            {
                token = lexer.GetToken();
                if(token.token_type == LBRACE)
                {
                    tempI = parse_caselist();
                    token = lexer.GetToken();
                    if(token.token_type == RBRACE)
                    {
                        #ifdef DEBUG
                        cout << "\n Rule parsed: switch_stmt -> SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE \n"; 
                        #endif       
                    }
                    else
                    {
                        cout << "\n Syntax Error \n";
                    }   
                }
                else
                {
                    cout << "\n Syntax Error \n";
                }
                
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }    
    }

    else
    {
        cout << "\n Syntax Error \n";
    }
return(0);
}
/*************************************************
* while_stmt ® WHILE LPAREN expression RPAREN body
* ************************************************/
int Parser::parse_whilestmt(void){
    int tempI;
    
    token = lexer.GetToken();
    if(token.token_type == WHILE){
        token = lexer.GetToken();
        if(token.token_type == LPAREN){
            tempI = parse_expression();
            //cout << "tempI: " <<reserved[tempI]<<endl;
            if(tempI != BOO)
            {
                syntax_error();
            }
            // else if tempI = -1 ==> parse_expresssion retunred an ID, then go and change using searchList the type of ID to 2.
            token = lexer.GetToken();
            if(token.token_type == RPAREN)
            {
                #ifdef DEBUG
                cout << "\n Rule parsed: whilestmt -> WHILE LPAREN expression RPAREN body \n";
                #endif
                parse_body();
                
            }else{
                cout << "\n Syntax Error \n";
            }
        }else{
            cout << "\n Syntax Error \n";
        }    
    }else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}
/*******************************************
* if_stmt ® IF LPAREN expression RPAREN body
* ******************************************/
int Parser::parse_ifstmt(void){
    TokenType tempI;
    token = lexer.GetToken();
    if(token.token_type == IF){
        token = lexer.GetToken();
        if(token.token_type == LPAREN){
            tempI = parse_expression();

            #ifdef DEBUG
            cout << reserved[tempI];
            #endif

            if(tempI != BOO)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C4"<<endl;
                exit(1);
            }
            // else if tempI = -1 ==> parse_expresssion retunred an ID, then go and change using searchList the type of ID to 2.

            token = lexer.GetToken();
            if(token.token_type == RPAREN)
            {
                #ifdef DEBUG
                cout << "\n Rule parsed: ifstmt -> IF LPAREN expression RPAREN body \n";
                #endif
                parse_body();
                
            }
            else
            {
                
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }    
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}
/**********************************************************
 * stmt ® assignment_stmt
 * stmt ® if_stmt
 * stmt ® while_stmt
 * stmt ® switch_stmt
 * if_stmt ® IF LPAREN expression RPAREN body
 * while_stmt ® WHILE LPAREN expression RPAREN body
 * switch_stmt ® SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE
 * assignment_stmt ® ID EQUAL expression SEMICOLON
****************************************************************/
int Parser::parse_stmt(void)
{
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        #ifdef DEBUG
        cout << "\n Rule parsed: stmt -> assignment_stmt \n";
        #endif
        tempI = parse_assstmt();                    
    }
    else if(token.token_type == IF)
    {
        tempTokenType = lexer.UngetToken(token);
        #ifdef DEBUG
        cout << "\n Rule parsed: stmt -> if_stmt\n";
        #endif
        tempI = parse_ifstmt();
    }
    else if(token.token_type == WHILE)
    {
        tempTokenType = lexer.UngetToken(token);
        #ifdef DEBUG
        cout << "\n Rule parsed: stmt -> while_stmt\n";
        #endif
        tempI = parse_whilestmt();
    }
    else if(token.token_type == SWITCH)
    {
        tempTokenType = lexer.UngetToken(token);
        #ifdef DEBUG
        cout << "\n Rule parsed: stmt -> switch_stmt\n";
        #endif
        tempI = parse_switchstmt();
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}
/********************************
 * stmt_list ® stmt
 * stmt_list ® stmt stmt_list
 * stmt ® assignment_stmt
 * stmt ® if_stmt
 * stmt ® while_stmt
 * stmt ® switch_stmt
 * if_stmt ® IF LPAREN expression RPAREN body
 * while_stmt ® WHILE LPAREN expression RPAREN body
 * switch_stmt ® SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE
 * assignment_stmt ® ID EQUAL expression SEMICOLON
 * ********************************/
int Parser::parse_stmtlist(void){
    
    token = lexer.GetToken();
    // First(stmt_list) = First(assign, if,while, switch) = {ID, IF, WHILE, SWITCH}
    int tempI;
    if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
    {
        tempTokenType = lexer.UngetToken(token);
        parse_stmt();
        token = lexer.GetToken();
        if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: stmt_list -> stmt stmt_list \n";
            tempI = parse_stmtlist();
            
        }
        else if (token.token_type == RBRACE)
        {
            tempTokenType = lexer.UngetToken(token);
            #ifdef DEBUG
            cout << "\n Rule parsed: stmt_list -> stmt \n";
            #endif
        }
    }
    else
    {
        cout << "\n Syntax Error \n";    
    }
    return(0);
}



/**************************************************
 * body ® LBRACE stmt_list RBRACE
 * **********************************************/
void Parser::parse_body(void){
    
    token = lexer.GetToken();
    int tempI;
    table.addScope(token.lexeme);
    
    if(token.token_type == LBRACE)
    {
        parse_stmtlist();
        token = lexer.GetToken();
        if (token.token_type == RBRACE)
        {
            printdata();
            table.exitScope();
        }
        else
        {
            #ifdef DEBUG
            cout << "here3";
            #endif
            syntax_error();
        }    
    }
    else if(token.token_type == END_OF_FILE)
    {
        tempTokenType = lexer.UngetToken(token);
    }
    else
    {
        syntax_error();
    }

}




// parse scope end

void SymbolTable::update_Types(std::string lexeme, TokenType newType)
{
    pointer = current;
    symbolTable n = symbolTable();
    
    while (pointer != NULL)
    {
       // cout <<"symboltype:"<< type;
        vector<symbolTable>::iterator i;
        for (i = pointer->symbols.begin(); i != pointer->symbols.end(); ++i)
        {
            if(i->variable == lexeme)
                i-> token_field = newType; 
        }
        if (pointer->previous != NULL)
        {
            pointer = pointer->previous;
        }
        else
        {
            pointer = NULL;
        }
    }
}

void SymbolTable::addScope(string scope)
{
    pointer = current;
    scopeTable* n = new scopeTable();
    n->scope = scope;
    if (pointer != NULL)
    {
        n->previous = pointer;
    }
    pointer = n;
    current = pointer;
}

void SymbolTable::exitScope()
{
    assignments.clear();
    pointer = current;
    if (pointer->previous == NULL)
    {
        pointer = NULL;
    }
    else
    {
        pointer = pointer->previous;
    }
current = pointer;
}

void SymbolTable::addVariable(string var) 
{
    pointer = current;
    symbolTable n = symbolTable();
    n.variable = var;
    n.token_field = UNKNOWN;
    assignments.push_back(n);
}

void SymbolTable::addVarType(TokenType type)
{        
    vector<symbolTable>::iterator i;
    for (i = assignments.begin(); i != assignments.end(); ++i)
    {
            i-> token_field = type; 
    }
    
}

void SymbolTable::printdata()
{
    pointer = current;
    symbolTable n = symbolTable();
    string output = "";
    TokenType type_field;
    bool questionmark = false, variablesExist = false;
    int counter=0;
    
    vector <symbolTable> ::iterator i;
    for (i = assignments.begin(); i != assignments.end(); ++i)
    {
        output += i->variable;
        if(i->variable.length())
            variablesExist = true;

        counter++;
        if(counter!= assignments.size())
            output += ", ";
        else
            output = output;

        if(i->token_field == UNKNOWN)
            questionmark = true;
        else
            type_field = i ->token_field;
       
    }

    if(variablesExist)
    {
        if(questionmark)
            cout << output << ":" <<" ? #" << endl;
        else
        {
           std::string data = reserved[type_field];
           // convert string to lower case
           std::for_each(data.begin(), data.end(), [](char & c){ c = ::tolower(c);});
           cout << output << ": " <<data <<" #" << endl;
        }
    }
      
}




symbolTable SymbolTable::findVariable(string var)
{
    pointer = current;
    symbolTable n = symbolTable();
    
    vector<symbolTable>::iterator i;
    for (i = assignments.begin(); i != assignments.end(); ++i)
    {
        if (i->variable == var)
        {
            n.variable = i->variable;
            n.access = i->access;
            n.token_field = i ->token_field;
            n.declaration = pointer->scope;
            return n;
        }
    }
        
n.variable = var;
n.declaration = "?";
n.token_field = UNKNOWN;
pointer = current;
assignments.push_back(n);
#ifdef DEBUG
cout << "variable added: " <<var <<endl;
#endif

return n;
}



/*************************************
 * type_name ® INT
 * type_name ® REAL
 * type_name ® BOOL
**************************************/

TokenType Parser::parse_typename(void){
    token = lexer.GetToken();
    if(token.token_type == INT || token.token_type == REAL || token.token_type == BOO)
    {
        //cout  << "\n Rule parse: type_name -> " << token.token_type << "\n";
        return token.token_type;
    }
    else
    {
        syntax_error();
    }

    return ERROR;// if Int ret 0 if float ret 1 if bool ret 2
}

/********************************
 * var_list      ® ID 
 * var_list      ® ID COMMA var_list
 * ******************************/
void Parser::parseVarList()
{
    token = lexer.GetToken();
    if (token.token_type == ID)
    {
        //cout << token.lexeme << endl;
        table.addVariable(token.lexeme);
        token = lexer.GetToken();
        if (token.token_type == COMMA)
        {
            parseVarList();
        }
        else if (token.token_type == COLON)
        {
            lexer.UngetToken(token);
        }
    }
    else if(token.token_type == COLON)
    {
        lexer.UngetToken(token);
    }
    else
    {
        syntax_error();
    }
}

/********************************
 * var_decl      ® var_list COLON type_name SEMICOLON 
 * var_list      ® ID 
 * var_list      ® ID COMMA var_list
 * ******************************/
int Parser::parse_vardecl()
{
    TokenType tempI;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        parseVarList();
        token = lexer.GetToken();
        if(token.token_type == COLON)
        {
            tempI = parse_typename();
            //cout << "variable type" << reserved[tempI];

            //use the searchList to update the types of variables that are already in the symbolTable
            //searchList(&reserved[tempI],0);
            //symbolTable n;
            //n = findVariable(ID);
            //n.token_field = tempI;
            table.addVarType(tempI);
            
            token = lexer.GetToken();
            if(token.token_type == SEMICOLON)
            {
                //cout << "\n Rule parsed: var_decl -> var_list COLON type_name SEMICOLON";
            }
            else
            {
                syntax_error();
            }
        }
        else
        {
            syntax_error();
        } 
    }
    else
    {
        syntax_error();
    }
    return(0);
}
/***************************************
 * var_decl_list ® var_decl
 * var_decl_list ® var_decl var_decl_list
 * var_decl ® var_list COLON type_name SEMICOLON
 * var_list ® ID
 * var_list ® ID COMMA var_list
*****************************************/

int Parser::parseVardeclList()
{
    int tempI;    
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_vardecl();
        token = lexer.GetToken();
        if(token.token_type != ID)
        {
            //cout << "\n Rule Parsed: var_decl_list -> var_decl \n";
        }
        else
        {
           // cout << "\n Rule Parsed: var_decl_list -> var_decl var_decl_list \n";
            tempTokenType = lexer.UngetToken(token);
            parseVardeclList();
        }
    }
    tempTokenType = lexer.UngetToken(token);
    return(0);
}


/***********************************
 * global_vars   ® var_decl_list
 * var_decl_list ® var_decl
 * var_decl_list ® var_decl var_decl_list
 * var_decl      ® var_list COLON type_name SEMICOLON 
 * var_list      ® ID 
 * var_list      ® ID COMMA var_list
 * *********************************/
void Parser::parseGlobalVars()
{
    token = lexer.GetToken();
    if (token.token_type == ID) //First(global_vars) = First(var_decl) ={ID}
    {
        lexer.UngetToken(token);
        parseVardeclList();
    }
    else
    {
        syntax_error();
    }
    
}

/***********************************
 * program       -> global_vars body
 * global_vars   ® var_decl_list
 * var_decl_list ® var_decl
 * var_decl_list ® var_decl var_decl_list
 * var_decl      ® var_list COLON type_name SEMICOLON 
 * var_list      ® ID 
 * var_list      ® ID COMMA var_list
 * *********************************/
void Parser::parse_Main()
{
    bool scopeParsed = false;
    table.addScope("::");
    token = lexer.GetToken();
    if (token.token_type == ID) //First(program) = First(global_vars,body) = First(var_list,body) = ID
    {
        Token token2 = lexer.GetToken();
        if (token2.token_type == COMMA || token2.token_type == SEMICOLON) //Follow(ID) = {Comma, Lbrace, Equal, Semi Colon}
        {
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parseGlobalVars();
            parse_body();
            //cout <<"parsed variable and body";
            scopeParsed = true;
        }
        else if (token2.token_type == LBRACE)
        {
            lexer.UngetToken(token2);
            lexer.UngetToken(token);
            parse_body();
            //cout << "parsed body only";
            scopeParsed = true;
        }
        else 
        {
            //cout << "here 1";
            syntax_error();
        }
    }

    else if (token.token_type == LBRACE || scopeParsed)
    {
        lexer.UngetToken(token);
        parse_body();
        //cout << "parsed body only";
        scopeParsed = true;
    }
    else
    {
        syntax_error();
    }
}


void Parser::syntax_error()
{
    cout << "Syntax Error\n";
    //exit(EXIT_FAILURE);
}


char null[] = "NULL";

int main()
{
    Parser parser;
    parser.parse_Main();
return 0;
}