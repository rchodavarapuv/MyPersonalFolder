#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include "stdlib.h"
#include "inputbuf.h"
#include "lexer_ext.h"
using namespace std;
string reserved[] = { "END_OF_FILE",
"PUBLIC", "PRIVATE",
"EQUAL", "COLON", "COMMA", "SEMICOLON",
"LBRACE", "RBRACE", "ID", "ERROR"
};
#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private" };
void Token::Print()
{
cout << "{" << this->lexeme << " , "
<< reserved[(int)this->token_type] << " , "
<< this->line_no << "}\n";
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
	char c; bool space_encountered = false;
	input.GetChar(c);
	line_no += (c == '\n');
	while (!input.EndOfInput() && isspace(c))
	{
		space_encountered = true;
		input.GetChar(c);
		line_no += (c == '\n');
	}
	if (!input.EndOfInput())
	{
		input.UngetChar(c);
	}
return space_encountered;
}


bool LexicalAnalyzer::SkipComment() {
char c,c2;
bool comment_encountered = false;
input.GetChar(c);
while (c == '/') {
input.GetChar(c);
if (c2 != '/') {
input.UngetChar(c2);
comment_encountered = false;
tmp.token_type = ERROR;
return comment_encountered;
}
else {
while (c != '\n' && !input.EndOfInput()) {
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
	for (int i = 0; i < KEYWORDS_COUNT; i++) 
	{
		if (s == keyword[i])
		{
			return (TokenType)(i + 1);
		}
	}
return ERROR;
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
	char c;
	input.GetChar(c);
	if (isalpha(c))
	{
		tmp.lexeme = "";
		while (!input.EndOfInput() && isalnum(c)) 
		{
			tmp.lexeme += c;
			input.GetChar(c);
		}
		if (!input.EndOfInput())
		{
			input.UngetChar(c);
		}
		tmp.line_no = line_no;
		if (IsKeyword(tmp.lexeme))
		tmp.token_type = FindKeywordIndex(tmp.lexeme);
		else
		tmp.token_type = ID;
	}
	else
	{
		if (!input.EndOfInput())
		{
			input.UngetChar(c);
		}
		tmp.lexeme = "";
		tmp.token_type = ERROR;
	}
return tmp;
}

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

switch (c) 
{
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
if (isalpha(c)) {
input.UngetChar(c);
return ScanIdOrKeyword();
}
else if (input.EndOfInput())
tmp.token_type = END_OF_FILE;
else
tmp.token_type = ERROR;
return tmp;
}
}

SymbolTable::SymbolTable()
{
current = new scopeTable;
current->previous = NULL;
pointer = current;
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

void SymbolTable::addVariable(string var, AccessSpecifier access) 
{
	pointer = current;
	symbolTable n = symbolTable();
	n.variable = var;
	n.access = access;
	pointer->symbols.push_back(n);
}

symbolTable SymbolTable::findVariable(string var)
{
	pointer = current;
	symbolTable n = symbolTable();
	while (pointer != NULL)
	{
		vector<symbolTable>::iterator i;
		for (i = pointer->symbols.begin(); i != pointer->symbols.end(); ++i)
		{
			if (i->variable == var)
			{
				n.variable = i->variable;
				n.access = i->access;
				n.declaration = pointer->scope;
				if (i->access == PRIVATE_ACCESS)
				{
					if (pointer->scope == current->scope)
					{
						return n;
					}
				}
				else
				{
					return n;
				}
			}
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
n.variable = var;
n.declaration = "?";
return n;
}

void SymbolTable::addAssignment(string lhs, string rhs)
{
	assignments.push_back(std::make_pair(findVariable(lhs),findVariable(rhs)));
}

void Parser::print()
{
	vector < pair<symbolTable, symbolTable> >::iterator i;
	for (i = table.assignments.begin(); i != table.assignments.end(); ++i)
	{
		cout << i->first.declaration << ((i->first.declaration != "::") ?"." : "") << i->first.variable << " = " << i->second.declaration << ((i->second.declaration != "::") ? "." : "") << i->second.variable << endl;
	}
}


/********************************
 * a) stmt         --> ID EQUAL ID SEMICOLON 
 * b) stmt         --> scope  --> ID LBRACE public_vars private_vars stmt_list RBRACE
 * ********************************/
void Parser::parseSt()
{
	token = lexer.GetToken();
	if (token.token_type == ID)
	{
		Token token2 = lexer.GetToken();
		if (token2.token_type == EQUAL)
		{
			Token token3 = lexer.GetToken();
			Token token4 = lexer.GetToken();
			if (token3.token_type == ID && token4.token_type == SEMICOLON)
			{
				table.addAssignment(token.lexeme, token3.lexeme);
			}
			else
			{
				syntax_error();
			}
		}
		else if (token2.token_type == LBRACE)
		{
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parseScope();
		}
		else
		{
			syntax_error();
		}
	}
	else
	{
		lexer.UngetToken(token);
	}
}

/********************************
 * m) stmt_list    --> stmt stmt_list 
 * a) stmt         --> ID EQUAL ID SEMICOLON 
 * b) stmt         --> scope  --> ID LBRACE public_vars private_vars stmt_list RBRACE
 * ********************************/
void Parser::parseStList()
{
token = lexer.GetToken();
 if (token.token_type == ID) // First(m) = First(a, b) = ID
 {
	Token token2 = lexer.GetToken();
	if (token2.token_type == EQUAL || token2.token_type == LBRACE) //Follow(ID) = {Comma, Lbrace, Equal, Semi Colon}
	{
		lexer.UngetToken(token2);
		lexer.UngetToken(token);
		parseSt();
	}
	else
	{
		lexer.UngetToken(token2);
		lexer.UngetToken(token);
		return;
	}

	//after parsing stmt, Follow(stmt) = stmt_list
	token = lexer.GetToken();
	if (token.token_type == ID)
	{
		lexer.UngetToken(token);
		parseStList();
	}
	else
	{
		lexer.UngetToken(token);
	}
 }
}
/***********************************************
 * private_vars  ® e 
 * private_vars  ® PRIVATE COLON var_list SEMICOLON  
 * ********************************************/
void Parser::parsePrivVars()
{
token = lexer.GetToken();
	if (token.token_type == PRIVATE) //First(private_vars) = {e, PRIVATE}
	{
		token = lexer.GetToken();
		if (token.token_type == COLON)
		{
			token = lexer.GetToken();
			if (token.token_type == ID)
			{
				lexer.UngetToken(token);
				parseVarList(PRIVATE_ACCESS);
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
		lexer.UngetToken(token);
		return;
	}
	token = lexer.GetToken();
	if (token.token_type == SEMICOLON)
	{ }
	else
	{
		syntax_error();
	}
}


/*********************************************
 * public_vars   ® e 
 * public_vars   ® PUBLIC COLON var_list SEMICOLON
 * var_list      ® ID 
 * var_list      ® ID COMMA var_list
**********************************************/
void Parser::parsePubVars()
{
	token = lexer.GetToken();
	if (token.token_type == PUBLIC) //First(public_vars) = {PUBLIC}
	{
		token = lexer.GetToken();
		if (token.token_type == COLON)
		{
			token = lexer.GetToken();
			if (token.token_type == ID) //Follow(colon) = First(var_list) = ID
			{
				lexer.UngetToken(token);
				parseVarList(PUBLIC_ACCESS);
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
		lexer.UngetToken(token);
		return;
	}
	token = lexer.GetToken();
	if (token.token_type == SEMICOLON)
	{
	}
	else
	{
		syntax_error();
	}
}
/****************************************************
 * scope  --> ID LBRACE public_vars private_vars stmt_list RBRACE 
 * public_vars   ® e 
 * public_vars   ® PUBLIC COLON var_list SEMICOLON 
 * private_vars  ® e 
 * private_vars  ® PRIVATE COLON var_list SEMICOLON 
 * stmt_list     ® stmt 
 * stmt_list     ® stmt stmt_list
 * stmt          ® ID EQUAL ID SEMICOLON 
 * stmt          ® scope 
 * ***************************************************/

void Parser::parseScope()
{
token = lexer.GetToken();
if (token.token_type == ID) //First(scope) = ID
{
	table.addScope(token.lexeme);
	token = lexer.GetToken();
	if (token.token_type == LBRACE)
	{
		parsePubVars();
		parsePrivVars();
		token = lexer.GetToken();
		if (token.token_type != RBRACE) //Follow(Colon) = First(var_list)
		{
			lexer.UngetToken(token);
			parseStList();
			token = lexer.GetToken();
			if (token.token_type == RBRACE)
			{
				table.exitScope();
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
else {
syntax_error();
}
}
/********************************
 * var_list      ® ID 
 * var_list      ® ID COMMA var_list
 * ******************************/
void Parser::parseVarList(AccessSpecifier access)
{
	token = lexer.GetToken();
	if (token.token_type == ID)
	{
		table.addVariable(token.lexeme, access);
		token = lexer.GetToken();
		if (token.token_type == COMMA)
		{
			parseVarList(access);
		}
		else if (token.token_type == SEMICOLON)
		{
			lexer.UngetToken(token);
		}
	}
	else
	{
		syntax_error();
	}
}
/********************************
 * global_vars   ® e 
 * global_vars   ® var_list SEMICOLON 
 * var_list      ® ID 
 * var_list      ® ID COMMA var_list
 * ******************************/

void Parser::parseGlobalVars()
{
	token = lexer.GetToken();
	if (token.token_type == ID) //First(global_vars) ={e, ID}
	{
		lexer.UngetToken(token);
		parseVarList(GLOBAL_ACCESS);
	}
	else
	{
		syntax_error();
	}
	token = lexer.GetToken();
	if (token.token_type == SEMICOLON)
	{ }
	else
	{
		syntax_error();
	}
}

/***********************************
 * program       -> global_vars scope
 * global_vars   ® e 
 * global_vars   ® var_list SEMICOLON 
 * var_list      ® ID 
 * var_list      ® ID COMMA var_list
 * *********************************/
void Parser::parse_Main()
{
	bool scopeParsed = false;
	table.addScope("::");
	token = lexer.GetToken();
	if (token.token_type == ID) //First(program) = First(global_vars,scope) = First(var_list,scope) = ID
	{
		Token token2 = lexer.GetToken();
		if (token2.token_type == COMMA || token2.token_type == SEMICOLON) //Follow(ID) = {Comma, Lbrace, Equal, Semi Colon}
		{
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parseGlobalVars();
			parseScope();
			scopeParsed = true;
		}
		else if (token2.token_type == LBRACE)
		{
			lexer.UngetToken(token2);
			lexer.UngetToken(token);
			parseScope();
			scopeParsed = true;
		}
		else 
		{
			syntax_error();
		}
	}
	token = lexer.GetToken();
	if (token.token_type == LBRACE || scopeParsed)
	{
		table.exitScope();
	}
	else
	{
		syntax_error();
	}
}





void Parser::syntax_error()
{
cout << "Syntax Error\n";
exit(EXIT_FAILURE);
}

int main()
{
	Parser parser;
	parser.parse_Main();
	parser.print();
return 0;
}
