#include <vector>
#include <string>
#include <cctype>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    INT, REAL, BOO, TR, FA, IF, WHILE, SWITCH, CASE, PUBLIC, PRIVATE, NUM, REALNUM, NOT, PLUS, MINUS, MULT, DIV, GTEQ, GREATER, LTEQ, NOTEQUAL, LESS, LPAREN, RPAREN, EQUAL, COLON, COMMA, SEMICOLON,
    LBRACE, RBRACE, ID, ERROR, BASE08NUM, BASE16NUM, UNKNOWN// TODO: Add labels for new token types here
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer
{
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();
    int parse_program(void);
    int parse_global_vars(void);
    int parse_scope(void);
    int parse_varlist(void);
    int parse_public_vars(void);
    int parse_private_vars(void);
    int parse_stmt_list(void);
    int parse_stmt(void);


  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    std::string ScanNumIfPossible();
    std::string ScanBase08NumIfPossible();
    std::string ScanBase16NumIfPossible();
    std::string ScanRealNumIfPossible();
    bool MatchSequence(std::string);
    bool SkipSpace();
    bool SkipComments();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
};

typedef enum 
{
 GLOBAL_ACCESS, 
 PUBLIC_ACCESS, 
 PRIVATE_ACCESS
} AccessSpecifier;

struct symbolTable {
 std::string variable;
 AccessSpecifier access;
 std::string declaration;
 TokenType token_field;
};
struct scopeTable {
 std::string scope;
 scopeTable* previous;
 std::vector<symbolTable> symbols;
 scopeTable()
 {
 previous = NULL;
 }
};

std::vector<symbolTable> assignments;


class SymbolTable
{
public:
 SymbolTable();
 void addScope(std::string);
 void exitScope();
 void addVariable(std::string);
 symbolTable findVariable(std::string);
 void addVarType(TokenType);
 void update_Types(std::string, TokenType);
 void printdata();
private:
 scopeTable* current;
 scopeTable* pointer;
};

class Parser : public SymbolTable
{
public:
 void parse_Main();
private:
 //variable parsing
 void parseGlobalVars();
 int parseVardeclList();
 int parse_vardecl();
 void parseVarList();
 TokenType parse_typename();
 // body parsing
 TokenType parse_unaryOperator(void);
 int parse_binaryOperator(void);
 TokenType parse_primary(void);
 TokenType parse_expression(void);
 int parse_assstmt();
 void parse_case(void);
 int parse_caselist(void);
 int parse_switchstmt(void);
 int parse_whilestmt(void);
 int parse_ifstmt(void);
 int parse_stmt(void);
 int parse_stmtlist(void);
 void parse_body(void);
 void syntax_error();
 Token token;
 LexicalAnalyzer lexer;
 SymbolTable table;
};

