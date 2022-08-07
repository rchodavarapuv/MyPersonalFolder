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

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = 
{ "END_OF_FILE",
  "PUBLIC",
  "PRIVATE",
  "EQUAL",
  "COLON",
  "COMMA",
  "SEMICOLON",
  "LBRACE",
  "RBRACE",
  "ID",
  "ERROR" 
};

#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}
// ****************Symbol Table****************

struct sTableItem
{
    char* name;
    char* scope;
    int permission;
};
struct sTable
{
    sTableItem* item;
    sTable* prev;
    sTable* next;
};

struct store
{
    string lhs;
    string rhs;
    char* scope1;
    char* scope2;
};

struct sTable* symbolTable; 
struct sTable* tempNode;
struct sTable* newNode;
/*string s = "::";
char* sScope = (char*) malloc (sizeof (char)*2);
memcpy(sScope, s.c_str(), sizeof (char)*2 ); 
//memcpy(st, s.c_str(), sizeof(s)*2 );
*/
char* sScope = "::";
int sPermission = 0;


vector< struct store> storeList;

void printAssignment(Token tok1, Token tok2)
{
    struct sTable* temp = newNode;
    struct sTable* temp1 = newNode;

    char* scope1 = (char*)malloc(sizeof(char)*20);			
    char* scope2 = (char*)malloc(sizeof(char)*20);

    while(temp != NULL)
    {
       // cout<<sScope << endl;
        if(temp->item->name == tok1.lexeme)
        {
            if(temp->item->permission == 2 && sScope != temp->item->scope)
            {
                temp= temp->prev;
                continue;
            }
            else
            {
                scope1 = temp->item->scope;
                break;
            }
        }
    temp= temp->prev;
    }

   while(temp1 != NULL)
   {
       if(temp1->item-> name == tok2.lexeme)
       {
            if(temp1->item->permission == 2 && sScope != temp1->item->scope)
            {
                temp1 = temp1->prev;
                continue;     
            }
            else
            {
                scope2 = temp1->item->scope;
                break;
            }
        }
   
    temp1 = temp1->prev;
   }


    if(strlen(scope1) == 0)
    {
        scope1 = new char[2];
        scope1[0] = '?';
        scope1[1] = '\0';
    }
    if(strlen(scope2) == 0)
    {
        //cout<<" entering here 2"<<scope2<<endl;
        scope2 = new char[2];
        scope2[0] = '?';
        scope2[1] = '\0';
    } 
//newNode->item->name
struct store temp_node;
temp_node.lhs = tok1.lexeme;
temp_node.rhs = tok2.lexeme;
temp_node.scope1 = scope1;
temp_node.scope2 = scope2;
storeList.push_back(temp_node);

cout << scope1;
if(scope1 != "::")
    cout <<".";

cout<< tok1.lexeme << " = " << scope2;

if(scope2 != "::")
    cout <<".";

cout << tok2.lexeme;
//cout <<endl;




/*
cout << storeList[i].scope1;
    if(storeList[i].scope1 != "::") 
        cout << "."; 
    cout<< storeList[i].lhs << " = " << storeList[i].scope2;
    if(storeList[i].scope2 != "::") 
        cout<<".";
    cout<< storeList[i].rhs << endl;*/

}


void deleteList(char* lexeme){
  //cout << "lexeme while exits scope " << lexeme << endl;
  if(symbolTable == NULL){
  return;
  }
   while(newNode->item->scope == lexeme){   
  // cout << "tempNode -> prev" << tempNode->prev << endl;
   tempNode->next = NULL;
   newNode ->prev = NULL;
   if(tempNode->prev != NULL){
   //cout << " if part inside while: temp-> prev "<< tempNode-> prev << endl; 
   tempNode=tempNode->prev;
   newNode=tempNode->next;
   newNode->next=NULL;
   return;
   }
   else if(tempNode == newNode){
  // cout << " function call for scope = :: " << endl;
   tempNode = NULL;
   newNode = NULL;
   return;
   }
   else{
   //cout << " else part inside while: temp-> prev "<< tempNode-> prev << endl; 
   tempNode->next = NULL;
   newNode ->prev = NULL;
   newNode=tempNode;
   tempNode->next =NULL;
   }

  // cout<< " Inside deleteList: temp name " << tempNode->item-> name << " temp->scope " << tempNode->item-> scope << "temp-> permission"<< tempNode->item->         permission<< endl;
  // cout<< " Inside deleteList: newNode name " << newNode->item-> name << " NewNode->scope " << newNode->item-> scope << endl;
   }
  // cout<< " out of while loop: deleteList: newNode name " << newNode->item-> name << " NewNode->scope " << tempNode->item-> scope << endl;
   sScope = newNode->item->scope;
   //cout<< " sScope " << sScope << endl;
   
}


void addList(char* lexeme)
 {
    if(symbolTable == NULL)
    { 
        symbolTable = (sTable*) malloc (sizeof(sTable));
        struct sTableItem* newItem = (sTableItem*) malloc (sizeof(sTableItem));
        symbolTable ->item = newItem;
        symbolTable-> next = NULL;   
        symbolTable-> prev = NULL;
  
        int len = strlen(lexeme) ; 
        symbolTable->item->name = new char[len+1];
        strcpy(symbolTable->item->name,lexeme);
        symbolTable->item->name[len] = '\0'; 
        symbolTable->item->scope = sScope;
        symbolTable->item->permission = sPermission; 
 
        newNode = symbolTable;
        tempNode = symbolTable; 
    }
    else
    {
        tempNode = symbolTable;
        while (tempNode->next != NULL)
        {
            tempNode = tempNode-> next;
        }

        newNode = (sTable*) malloc (sizeof(sTable));
        struct sTableItem* newItem = (sTableItem*) malloc(sizeof(sTableItem));
        newNode -> item = newItem;
        newNode -> next = NULL;
        newNode -> prev = tempNode;
        tempNode ->next = newNode; // now newNode is at top of the stack; 
        int len = strlen(lexeme) ;

        newNode -> item-> name = new char[len+1];
        strcpy(newNode ->item->name, lexeme);
        newNode -> item -> name[len] = '\0';
        newNode -> item -> scope = sScope;
        newNode -> item -> permission = sPermission;
     }
 
}
//*************************************************


LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}
 LexicalAnalyzer lexer;
 Token token1;
 Token token2;
 Token token3;

/****************************************
  program -> global_vars scope
  **************************************/

int LexicalAnalyzer::parse_program(void)
{
    lexer.parse_global_vars();
    lexer.parse_scope();
    //cout << "resolved_lhs = resolved_rhs" <<endl;

return 0;
}

/*******************************************
global_vars -> e
global_vars -> var_list SEMICOLON
*******************************************/
int LexicalAnalyzer::parse_global_vars(void)
{
    token1 = lexer.GetToken();
    if(token1.token_type == ID)
    {
        lexer.UngetToken(token1);
        lexer.parse_varlist();
        token1 = lexer.GetToken();
        if(token1.token_type ==SEMICOLON)
        {
            //cout << "Global variable parsed" <<endl;
            return 0;
        }
        else
        {
            //cout << "Global variable Syntax Error" <<endl;
            exit(1); 
        }   
    }
    else if(token1.token_type == END_OF_FILE)
    {
        return 0;
    }
return 0;
}
/**********************************************************
scope -> ID LBRACE public_vars private_vars stmt_list RBRACE
**********************************************************/

int LexicalAnalyzer::parse_scope(void)
{
    token1 = lexer.GetToken();
    if(token1.token_type == ID)
    {
        string slexeme = token1.lexeme; 
        sScope = const_cast<char *>(slexeme.c_str());
        cout << sScope <<endl;
        token1= lexer.GetToken();
        if(token1.token_type == LBRACE)
        {
           lexer.parse_public_vars();
           lexer.parse_private_vars();
           lexer.parse_stmt_list();
           token1 = lexer.GetToken();
           if(token1.token_type == RBRACE)
           {
            deleteList(sScope);
            return 0;
           }
           else if(token1.token_type == ID) 
           {
            //cout <<"new scope";
            lexer.UngetToken(token1);
            lexer.parse_scope();
           }
           else
           {
            //cout << "Right brace Syntax Error"<< endl;
            exit(1);
           }
       }
       else
       {
            //cout << "Left brace Syntax Error"<<endl;
            exit(1);
       }

    }
    
    else
    {
        cout << "Syntax Error"<< endl;
        exit(1);
    }
return -1;
}


/********************************************
public_vars -> e
public_vars -> PUBLIC COLON var_list SEMICOLON
********************************************/
int LexicalAnalyzer::parse_public_vars(void)
{
  //  cout <<"enter public";
token1 = lexer.GetToken();
if(token1.token_type == PUBLIC)
{
  sPermission = 1;
  token1 = lexer.GetToken();
  if(token1.token_type == COLON)
  {
    token1 = lexer.GetToken(); 
    if(token1.token_type == ID)
    {
        lexer.UngetToken(token1);
        lexer.parse_varlist();
        token1 = lexer.GetToken();
        if(token1.token_type == SEMICOLON)
        {
            //cout <<"public parsing done" <<endl;
            return 0;
        }
        else
        {
            cout<< "Syntax Error" << endl;
            exit(1);
        }
     }
    else
    {
     cout<< "Syntax Error" << endl;
     exit(1);
    }
  }
  else
  {
    cout<< "Syntax Error" << endl;
    exit(1);
  }
 }

  else if(token1.token_type == PRIVATE || token1.token_type == ID){
  lexer.UngetToken(token1);
   //cout << "public_vars -> epsilon"<< endl;
   }
  else{
  cout << "Syntax Error"<< endl;
  exit(1);
  }
  return 0;
}

/************************************************
private_vars -> e
private_vars -> PRIVATE COLON var_list SEMICOLON
*************************************************/

int LexicalAnalyzer::parse_private_vars(void)
{
   // cout <<"enter private";

  token1=lexer.GetToken();
 
  if(token1.token_type == PRIVATE)
  {
    sPermission = 2;
    token1 = lexer.GetToken();
   
    if(token1.token_type == COLON)
    {
        token1 =lexer.GetToken();
        if(token1.token_type == ID)
        {
            lexer.UngetToken(token1);
            lexer.parse_varlist();
            token1 = lexer.GetToken();
            if(token1.token_type == SEMICOLON)
            {
              //  cout << "private parsing done" <<endl;
                return 0;
            }
            else
            {
                cout<<"Syntax Error"<<endl;
               exit(1);
            }
        }
        else
        {
            cout<<"Syntax Error"<<endl;
            exit(1);
        }
   }
   else
   {
    cout<<"Syntax Error"<<endl;
    exit(1);
   }
 
  }
  //Check FOLLOW SET of private_vars
 else if(token1.token_type == ID) {
 lexer.UngetToken(token1);
 //cout<< "private_vars -> epsilon" << endl;
 }
 else{
 cout<< "Syntax Error"<< endl;
 }
 return 0;
}

/************************************
var_list -> ID
var_list -> ID COMMA var_list
************************************/
int parse_varlist(void)
{
    token = lexer.GetToken();
    int tempI;
    char* lexeme = (char*)malloc(sizeof(token.lexeme)+1);
    memcpy(lexeme, (token.lexeme).c_str(), (token.lexeme).size()+1);
    addList(lexeme, token.line_no, 0);

    if(token.token_type == ID)
    {
        token = lexer.GetToken();
        if(token.token_type == COMMA)
        {
            //cout << "\n Rule Parsed: var_list -> ID COMMA var_list \n";
            tempI = parse_varlist();
        }
        else if(token.token_type == COLON)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: var_list -> ID \n";
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
/*int LexicalAnalyzer::parse_varlist(void)
{
  token1 = lexer.GetToken();
  char* lexeme = (char*) malloc (sizeof (token1.lexeme)); 
  memcpy(lexeme,(token1.lexeme).c_str(),sizeof(token1)); 
  addList(lexeme); 
  
  if(token1.token_type ==ID)
  {
    token1 = lexer.GetToken();
    if(token1.token_type == COMMA)
    { 
     lexer.parse_varlist();
     //cout <<"var comma" <<endl;
     return 0;        
    }
    if(token1.token_type == SEMICOLON)
    {
       UngetToken(token1);
       //cout <<"var ;" <<endl;
       return 0;
    }
    else
    {
      //cout << "Syntax Error" << endl;   // How to handle the case: var_list -> ID
      return 0;
    }
  }
  else {
   cout << "Syntax Error" << endl;
   exit(1);
  }
 return -1;   
}*/

/******************************
   stmt list -> stmt
   stmt list -> stmt stm_list
   ***************************/

int parse_stmtlist(void)
{
    token = lexer.GetToken();
    //token.Print();
    int tempI;
    if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_stmt();
        token = lexer.GetToken();
        //token.Print();
        if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
        {
            tempTokenType = lexer.UngetToken(token);
            //cout << "\n Rule Parsed: stmt_list -> stmt stmt_list \n";
            tempI = parse_stmtlist();

        }
        else if (token.token_type == RBRACE)
        {
            tempTokenType = lexer.UngetToken(token);           
            //cout << "\n Rule parsed: stmt_list -> stmt \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}
/*int LexicalAnalyzer::parse_stmt_list(void)
{
 token1 = lexer.GetToken();
 if(token1.token_type == ID)
 {
    lexer.UngetToken(token1);
    lexer.parse_stmt();
    token2 = lexer.GetToken();
    if(token2.token_type == ID)
    {
       lexer.UngetToken(token2);
       lexer.parse_stmt_list();
       return 0;
    }
       
    else if(token2.token_type == RBRACE)
    {
       lexer.UngetToken(token2);       
       return 0;
    }  
       
    else
    {
       cout<< "Syntax Error"<<endl;
       exit(1);
    }
 }
    
else
{
    cout<< "Syntax Error"<<endl;
    exit(1);
}
 return -1; 
}*/

/******************************
   stmt -> ID EQUAL ID SEMI COLON
   stmt -> scope
   ***************************/
int parse_stmt(void)
{
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> assignment_stmt \n";
        tempI = parse_assstmt();

    }
    else if(token.token_type == IF)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> if_stmt";
        tempI = parse_ifstmt();
    }
    else if(token.token_type == WHILE)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> while_stmt";
        tempI = parse_whilestmt();
    }
    else if(token.token_type == SWITCH)
    {
        tempTokenType = lexer.UngetToken(token);
        //cout << "\n Rule parsed: stmt -> switch_stmt";
        tempI = parse_switchstmt();
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return(0);
}
/*
int LexicalAnalyzer::parse_stmt(void)
{
  token1 = lexer.GetToken(); 
  if(token1.token_type == ID)
  {
    token2 = lexer.GetToken();
    if(token2.token_type == EQUAL)
    {     
      token3 = lexer.GetToken();
      if(token3.token_type == ID)
      {
        printAssignment(token1, token3);
        token1 = lexer.GetToken();
        if(token1.token_type == SEMICOLON)
        {
         //   cout<<"stmt id=id"<<endl;
            return 0;
        }
        else
        {
          cout << "Syntax Error" << endl;
           exit(1);
        }
       }
       else 
       {
       cout << "Syntax Error" << endl;
       exit(1);
      }
    }
    
    else if(token2.token_type == LBRACE)
    {
     sScope = const_cast<char*>((token1.lexeme).c_str());  
     //cout << "new scope "<<  endl;
     lexer.UngetToken(token2);
     lexer.UngetToken(token1);
     lexer.parse_scope();     
    }
     
    else
    {
     cout << "Syntax Error" << endl;
     exit(1);
    }
  }
  
  else
  {
  cout << "Syntax Error" << endl;
  exit(1);
  }
return 0;
}

*/

/*********************************/
bool LexicalAnalyzer::SkipComment()
{
  char c;
  bool comment = false;
  input.GetChar(c);
  if(input.EndOfInput())
  {
    input.UngetChar(c);
    return comment;
  }
  
  if(c=='/')
  {
    input.GetChar(c);
    if(c=='/')
    {
        comment = true;
        while (c != '\n')
        {
            comment = true;
            input.GetChar(c);
        }
        line_no +=1;
        SkipComment();      
    }

    else
    {
        comment = false;
        //cout << "Comment not found" << endl;
        exit(1);
    }

   }

   else
   {
     input.UngetChar(c);    
     return comment;
   }
return comment;
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
        if (IsKeyword(tmp.lexeme)){          
            tmp.token_type = FindKeywordIndex(tmp.lexeme);           
            }
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
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }
 
    SkipSpace();
    SkipComment();
    SkipSpace();
    
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
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
            if (isdigit(c)){
              input.UngetChar(c);
              tmp.token_type = ERROR;
              return tmp;
             }            
            else if (isalpha(c)) {
            input.UngetChar(c);
            return ScanIdOrKeyword();
            }
            else if (input.EndOfInput())
            tmp.token_type = END_OF_FILE;
               
            else{
          //  cout << "value of char : c " << c<< endl;
            tmp.token_type = ERROR;
            }

            return tmp;
    }
}

void printlist()
{
    for(int i=0; i<storeList.size();i++)
    {
        cout << storeList[i].scope1;
        if(storeList[i].scope1 != "::")
            cout << "."; 
           
        cout<< storeList[i].lhs << " = " << storeList[i].scope2;
        
        if(storeList[i].scope2 != "::")
            cout<<".";
        
        cout<< storeList[i].rhs << endl;
        
    
    }
}

int main()
{
    int i;
    i = lexer.parse_program(); 
    //printlist();
    
}
