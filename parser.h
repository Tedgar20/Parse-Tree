/*
 * parser.h
 *
 *  Created on: Oct 23, 2017
 *      Author: gerardryan
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <algorithm>
#include <iostream>
using namespace std;
using std::istream;

#include <string>
#include <map>
using std::string;
using std::stoi;
using std::map;
   
#include "lexer.h"
extern bool err;
extern void error(int linenum, const string& message);
extern void semanticError(int linenum, const string& message);
   
enum TypeForNode { INT_TYPE, STRING_TYPE, ERROR_TYPE };

class Value {
	TypeForNode type;
   	int i;
   	string s;
public:
   Value(){
   		 type = ERROR_TYPE;
   }
   Value( int iv ){
   		 type = INT_TYPE;
   		 i = iv;
   }
   Value( string sv){
   		 type = STRING_TYPE;
   		 sv.erase( remove( sv.begin(), sv.end(), '\"' ), sv.end() );
   		 s = sv;
   }
   
   Value operator+(const Value op)
   {
   		if( this->GetType() == INT_TYPE )
   		{
   			return this->GetIntValue() + op.GetIntValue();
   		}
   		else if( this->GetType() == STRING_TYPE )
   		{
   			return this->GetStringValue() + op.GetStringValue();
   		}
   		else
   			return ERROR_TYPE;
   }
   Value operator-(const Value op)
   {
   		if( this->GetType() == INT_TYPE )
   		{
   			return this->GetIntValue() - op.GetIntValue();
   		}
   		else
   			return ERROR_TYPE;
   }
   Value operator*(const Value op)
   {
   		if( this->GetType() == INT_TYPE && op.GetType() == STRING_TYPE )
   		{
   			int count = 0;
   			string result = "";
   			while( count < this->GetIntValue() )
   			{
   				result += op.GetStringValue();
   				count++;
   			}
   			return result;
   		}
   		else if( this->GetType() == STRING_TYPE && op.GetType() == INT_TYPE )
   		{
   			int count = 0;
   			string result = "";
   			while( count < op.GetIntValue() )
   			{
   				result += this->GetStringValue();
   				count++;
   			}
   			return result;
   		}
   		else if( this->GetType() == INT_TYPE && op.GetType() == INT_TYPE )
   		{
   			return this->GetIntValue() * op.GetIntValue();
   		}
   		else
   			return ERROR_TYPE;
   }
   Value operator/(const Value op)
   {
   		if( this->GetType() == INT_TYPE )
   		{
   			return this->GetIntValue() / op.GetIntValue();
   		}
   		else if( this->GetType() == STRING_TYPE )
   		{
   			string s1 = this->GetStringValue();
   			string s2 = op.GetStringValue();
   			
   			if( s2.size() == 1 )
   			{
   				std::size_t x = 0;
   				if( ( x = s1.find(s2) ) != std::string::npos )
   				{
   					s1.replace(x, 1, "");
   				}
   			}
   			else
   			{
   				std::string::size_type n = 0;
   				while ( ( n = s1.find( s2, n ) ) != std::string::npos )
   				{
   					s1.replace( n, s2.size(), "" );
   					n += s2.size();
   				}
   			}
   			return s1;
   		}
   		else
   			return ERROR_TYPE;
   }
   TypeForNode GetType() const { return type; }
   int GetIntValue() const { return i; }
   string GetStringValue() const { return s; }
};
   
static ostream& operator<<(ostream& out, const Value Result)
   {
		if( Result.GetType() == INT_TYPE )
			out << Result.GetIntValue();
   		else if( Result.GetType() == STRING_TYPE )
   			out << Result.GetStringValue();
		return out;
   }
   
extern map<string,Value> SymbolTable;

class ParseTree {
	int			linenumber;
	ParseTree	*left;
	ParseTree	*right;

public:
	ParseTree(int n, ParseTree *l = 0, ParseTree *r = 0) : linenumber(n), left(l), right(r) {}
	virtual ~ParseTree() {}

	ParseTree* getLeft() const { return left; }
	ParseTree* getRight() const { return right; }
	int getLineNumber() const { return linenumber; }

   	virtual void NodeTraverse(ParseTree *t) {}
	virtual TypeForNode GetType() const { return ERROR_TYPE; }
	virtual int GetIntValue() const { throw "Not an int"; }
	virtual string GetStringValue() const { return "NOT A STRING"; }
   	virtual Value eval() const { return 0; }
   	virtual int findSemanticErrors() const { return 0; }
   	virtual string GetVariable() const { return "Ya fucked up"; }
};
   
class StatementList : public ParseTree {
public:
	StatementList(ParseTree *first, ParseTree *rest) : ParseTree(0, first, rest) {}
};

class Addition : public ParseTree {
   ParseTree* num1;
   ParseTree* num2;
public:
	Addition(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {
   		num1 = op1;
   		num2 = op2;
   	}
   	Value eval() const
   	{ 
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( ex1->GetType() == STRING_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" && ex2->GetStringValue() == "foo" )
   			{
            	return SymbolTable[ex1->GetVariable()] + SymbolTable[ex2->GetVariable()];
   			}
            else if( ex1->GetStringValue() == "foo" )
            {
            	Value N2( ex2->eval() );
            	return SymbolTable[ex1->GetVariable()] + N2;
            }
            else if( ex2->GetStringValue() == "foo" )
            {
            	Value N1( ex1->eval() );
            	return N1 + SymbolTable[ex2->GetVariable()];
            }
            Value N1( ex1->eval() );
            Value N2( ex2->eval() );
            return N1 + N2;
   		}
   		Value N1( ex1->eval() );
   		Value N2( ex2->eval() );
   		return N1 + N2;
   	}
   	TypeForNode GetType() const
   	{
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( ex1->GetType() == STRING_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" && ex2->GetStringValue() == "foo" )
   			{
   				if( ( SymbolTable.find(ex1->GetVariable() ) != SymbolTable.end() ) && ( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() ) )
   				{
   					if( SymbolTable[ex1->GetVariable()].GetType() == SymbolTable[ex2->GetVariable()].GetType() )
   					{
   						return SymbolTable[ex1->GetVariable()].GetType();
   					}
   					return ERROR_TYPE;
   				}
   				return ERROR_TYPE;
   			}
   			else if( ex1->GetStringValue() == "foo" )
   			{
   				if( SymbolTable.find(ex1->GetVariable() ) != SymbolTable.end() )
   				{
   					if( SymbolTable[ex1->GetVariable()].GetType() == ex2->GetType() )
   						return ex2->GetType();
   
   					return ERROR_TYPE;
   				}
   				return ERROR_TYPE;
   			}
   			else if( ex2->GetStringValue() == "foo" )
   			{
   				if( SymbolTable.find(ex2->GetVariable() ) != SymbolTable.end() )
   				{
   					if( SymbolTable[ex2->GetVariable()].GetType() == ex1->GetType() )
   						return ex1->GetType();
   
   					return ERROR_TYPE;
   				}
   				return ERROR_TYPE;
   			}
   			return ex1->GetType();
   		}
   		else if( ex1->GetType() == STRING_TYPE && ex1->GetStringValue() == "foo" )
   		{
   			if( ( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() ) && ( SymbolTable[ex1->GetVariable()].GetType() == ex2->GetType() ) )
   			{
   				return ex2->GetType();
   			}
   			return ERROR_TYPE;  
   		}
   		else if( ex2->GetType() == STRING_TYPE && ex2->GetStringValue() == "foo" )
   		{
   			if( ( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() ) && ( SymbolTable[ex2->GetVariable()].GetType() == ex1->GetType() ) )
   			{
   				return ex1->GetType();
   			}
   			return ERROR_TYPE;
   		}
   		else if( ex1->GetType() == ex2->GetType() )
   		{
   			return ex1->GetType();
   		}
   		return ERROR_TYPE;
   	}
   	int findSemanticErrors() const 
   	{
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( (ex1->GetType() == STRING_TYPE && ex1->GetStringValue() == "foo") && SymbolTable.find(ex1->GetVariable()) == SymbolTable.end() ) 
   		{
			semanticError(ex1->getLineNumber(), "variable " + ex1->GetVariable() + " is used before being declared");
   			semanticError(this->getLineNumber(), "type error");
			return 1;
   		}
   		else if( (ex2->GetType() == STRING_TYPE && ex2->GetStringValue() == "foo") && SymbolTable.find(ex2->GetVariable()) == SymbolTable.end() ) 
   		{
			semanticError(ex2->getLineNumber(), "variable " + ex2->GetVariable() + " is used before being declared");
   			semanticError(this->getLineNumber(), "type error");
			return 1;
   		}
   		else if( this->GetType() == ERROR_TYPE )
   		{
   			semanticError(this->getLineNumber(), "type error");
   			return 1;
   		}
   		else
   			return 0;
   	}
   	ParseTree* getLeft() const { return num1; }
   	ParseTree* getRight() const { return num2; }
};

class Subtraction : public ParseTree {
   ParseTree* num1;
   ParseTree* num2;
public:
	Subtraction(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {
   		num1 = op1;
   		num2 = op2;
   	}
   	Value eval() const
   	{ 
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( ex1->GetType() == STRING_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" && ex2->GetStringValue() == "foo" )
   			{
            	return SymbolTable[ex1->GetVariable()] - SymbolTable[ex2->GetVariable()];
   			}
			
            else if( ex1->GetStringValue() == "foo" )
            {
            	Value N2( ex2->eval() );
            	return SymbolTable[ex1->GetVariable()] - N2;
            }
            else if( ex2->GetStringValue() == "foo" )
            {
            	Value N1( ex1->eval() );
            	return SymbolTable[ex2->GetVariable()] - N1;
            }
            Value N1( ex1->eval() );
            Value N2( ex2->eval() );
            return N1 - N2;
   		}
   		Value N1( ex1->eval() );
   		Value N2( ex2->eval() );
   		return N1 - N2;
   	}
   	TypeForNode GetType() const
   	{
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( ex1->GetType() == STRING_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" && ex2->GetStringValue() == "foo" )
   			{
   				if( ( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() ) && ( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() ) ) //Two Vars
   				{
   					if( SymbolTable[ex1->GetVariable()].GetType() == INT_TYPE && SymbolTable[ex2->GetVariable()].GetType() == INT_TYPE )
   						return SymbolTable[ex1->GetVariable()].GetType();
   				}
   				return ERROR_TYPE;
   			}
   			return ERROR_TYPE;
   		}
   		else if( ex1->GetType() == STRING_TYPE && ex2->GetType() == INT_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" )
   			{
   				if( ( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() ) && ( SymbolTable[ex1->GetVariable()].GetType() ==  ex2->GetType() ) )
   				{
   					return ex2->GetType();
   				}
   				return ERROR_TYPE;
   			}
   			return ERROR_TYPE;
   		}
   		else if( ex1->GetType() == INT_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex2->GetStringValue() == "foo" )
   			{
   				if( ( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() ) && ( SymbolTable[ex2->GetVariable()].GetType() ==  ex1->GetType() ) )
   				{
   					return ex1->GetType();
   				}
   				return ERROR_TYPE;
   			}
   			return ERROR_TYPE;
   		}
   		else if( ex1->GetType() == INT_TYPE && ex2->GetType() == INT_TYPE )	//Two Integers
   		{
   			return ex1->GetType();
   		}
   		return ERROR_TYPE;
   	}
   	int findSemanticErrors() const 
   	{
   		if( this->GetType() == ERROR_TYPE )
   		{
   			semanticError(this->getLineNumber(), "type error");
   			return 1;
   		}
   		else
   			return 0;
   	}
   	ParseTree* getLeft() const { return num1; }
   	ParseTree* getRight() const { return num2; }
};

class Multiplication : public ParseTree {
   ParseTree* num1;
   ParseTree* num2;
public:
	Multiplication(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {
   		num1 = op1;
   		num2 = op2;
   	}
   	Value eval() const
   	{ 
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( ex1->GetType() == STRING_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" && ex2->GetStringValue() == "foo" )
   			{
            	return SymbolTable[ex1->GetVariable()] * SymbolTable[ex2->GetVariable()];
   			}
            else if( ex1->GetStringValue() == "foo" )
            {
            	Value N2( ex2->eval() );
            	return SymbolTable[ex1->GetVariable()] * N2;
            }
            else if( ex2->GetStringValue() == "foo" )
            {
            	Value N1( ex1->eval() );
            	return SymbolTable[ex2->GetVariable()] * N1;
            }
   		}
   		else if( ex1->GetType() == STRING_TYPE && ex1->GetStringValue() == "foo" )
   		{
   			Value N2( ex2->eval() );
   			return SymbolTable[ex1->GetVariable()] * N2;
   		}
   		else if( ex2->GetType() == STRING_TYPE && ex2->GetStringValue() == "foo" )
   		{
   			Value N1( ex1->eval() );
   			return SymbolTable[ex2->GetVariable()] * N1;
   		}
   		Value N1( ex1->eval() );
   		Value N2( ex2->eval() );
   		return N1 * N2;
   	}
   	TypeForNode GetType() const
   	{
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( ex1->GetType() == STRING_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" && ex2->GetStringValue() == "foo" )
   			{
   				if( ( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() ) && ( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() ) )
   				{
   					if( SymbolTable[ex1->GetVariable()].GetType() == INT_TYPE && SymbolTable[ex2->GetVariable()].GetType() == INT_TYPE )
   						return SymbolTable[ex1->GetVariable()].GetType();
   
   					else if( SymbolTable[ex1->GetVariable()].GetType() == INT_TYPE && SymbolTable[ex2->GetVariable()].GetType() == STRING_TYPE )
   						return STRING_TYPE;
   
   					else if( SymbolTable[ex1->GetVariable()].GetType() == STRING_TYPE && SymbolTable[ex2->GetVariable()].GetType() == INT_TYPE )
   						return STRING_TYPE;
   				}
   				return ERROR_TYPE;
   			}
   			return ERROR_TYPE;
   		}
   		else if( ex1->GetType() == STRING_TYPE && ex2->GetType() == INT_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" )
   			{
   				//cout << SymbolTable[ex1->GetVariable()].GetStringValue() << endl;
   				if( ( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() ) && ( SymbolTable[ex1->GetVariable()].GetType() ==  ex2->GetType() ) )
   					return ex2->GetType();
   				
   				else if( ( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() ) && ( SymbolTable[ex1->GetVariable()].GetType() !=  ex2->GetType() ) )
   					return SymbolTable[ex1->GetVariable()].GetType();
   				
   				return ERROR_TYPE;
   			}
   			return STRING_TYPE;
   		}
   		else if( ex1->GetType() == INT_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex2->GetStringValue() == "foo" )
   			{
   				if( ( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() ) && ( SymbolTable[ex2->GetVariable()].GetType() ==  ex1->GetType() ) )
   					return ex1->GetType();
   
   				else if( ( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() ) && ( SymbolTable[ex2->GetVariable()].GetType() != ex1->GetType() ) )
   					return SymbolTable[ex2->GetVariable()].GetType();
   				
   				return ERROR_TYPE;
   			}
   			return STRING_TYPE;
   		}
   		else if( ex1->GetType() == INT_TYPE && ex2->GetType() == INT_TYPE )
   			return ex1->GetType();
   		else
   		{
   			return ERROR_TYPE;
   		}
   	}
   	int findSemanticErrors() const 
   	{
   		if( this->GetType() == ERROR_TYPE )
   		{
   			semanticError(this->getLineNumber(), "type error");
   			return 1;
   		}
   		else
   			return 0;
   	}
   	ParseTree* getLeft() const { return num1; }
   	ParseTree* getRight() const { return num2; }
};
   
class Division : public ParseTree {
   ParseTree* num1;
   ParseTree* num2;
public:
	Division(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {
   		num1 = op1;
   		num2 = op2;
   	}
   	Value eval() const
   	{ 
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( ex1->GetType() == STRING_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" && ex2->GetStringValue() == "foo" )
   			{
   				return SymbolTable[ex1->GetVariable()] / SymbolTable[ex2->GetVariable()];
   			}
   			else if( ex1->GetStringValue() == "foo" )
   			{
   				Value N2( ex2->eval() );
   				return SymbolTable[ex1->GetVariable()] / N2;
   			}
   			else if( ex2->GetStringValue() =="foo" )
   			{
   				Value N1( ex1->eval() );
   				return N1 / SymbolTable[ex2->GetVariable()];
   			}
   		}
   		Value N1( ex1->eval() );
   		Value N2( ex2->eval() );
   
   		return N1 / N2;
   	}
   	TypeForNode GetType() const
   	{
   		ParseTree* ex1 = getLeft();
   		ParseTree* ex2 = getRight();
   		if( ex1->GetType() == STRING_TYPE && ex2->GetType() == STRING_TYPE )
   		{
   			if( ex1->GetStringValue() == "foo" && ex2->GetStringValue() == "foo" )
   			{
   				if( ( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() ) && ( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() ) )
   				{
   					if( SymbolTable[ex1->GetVariable()].GetType() == SymbolTable[ex2->GetVariable()].GetType() )
   						return SymbolTable[ex1->GetVariable()].GetType();
   
   					return ERROR_TYPE;
   				}
   				return ERROR_TYPE;
   			}
   			else if( ex1->GetStringValue() == "foo" )
   			{
   				if( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() && SymbolTable[ex1->GetVariable()].GetType() == STRING_TYPE )
   				{
   					return STRING_TYPE;
   				}
   				return ERROR_TYPE;
   			}
   			else if( ex2->GetStringValue() == "foo" )
   			{
   				if( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() && SymbolTable[ex2->GetVariable()].GetType() == STRING_TYPE )
   				{
   					return STRING_TYPE;
   				}
   				return ERROR_TYPE;
   			}
   			return STRING_TYPE;
   		}
   		else if( ex1->GetType() == STRING_TYPE && ex1->GetStringValue() == "foo" )
   		{
   			if( SymbolTable.find(ex1->GetVariable()) != SymbolTable.end() && SymbolTable[ex1->GetVariable()].GetType() == ex2->GetType() )
   			{
   				return ex2->GetType();
   			}
   			return ERROR_TYPE;
   		}
   		else if( ex2->GetType() == STRING_TYPE && ex2->GetStringValue() == "foo" )
   		{
   			if( SymbolTable.find(ex2->GetVariable()) != SymbolTable.end() && SymbolTable[ex2->GetVariable()].GetType() == ex1->GetType() )
   			{
   				return ex1->GetType();
   			}
   			return ERROR_TYPE;
   		}
   		else if( ex1->GetType() == ex2->GetType() )
   		{
   			return ex1->GetType();
   		}
   		else
   		{
   			return ERROR_TYPE;
   		}
   	}
   	int findSemanticErrors() const 
   	{
   		if( this->GetType() == ERROR_TYPE )
   		{
   			semanticError(this->getLineNumber(), "type error");
   			return 1;
   		}
   		else
   			return 0;
   	}
   	ParseTree* getLeft() const { return num1; }
   	ParseTree* getRight() const { return num2; }
};
   
class DeclStatement : public ParseTree {
TypeForNode	type;
	string 		id;

public:
	DeclStatement(int line, TypeForNode ty, string id) : ParseTree(line), type(ty), id(id) {}

	int findSemanticErrors() const {
		if( SymbolTable.find(id) != SymbolTable.end() ) {
			semanticError(this->getLineNumber(), "variable " + id + " was already declared");
			return 1;
		}
   		if( type == STRING_TYPE )
   		{
   			SymbolTable[id] = Value("");
   		}
   		else
			SymbolTable[id] = Value(type);
		return 0;
	}
   	
};

class SetStatement : public ParseTree {
string id;
ParseTree* num1;
public:
	SetStatement(int line, string id, ParseTree *ex) : ParseTree(line, ex), id(id) {
   			num1 = ex;
   	}
	int findSemanticErrors() const {
		if( SymbolTable.find(id) == SymbolTable.end() ) {
			semanticError(this->getLineNumber(), "variable " + id + " is used before being declared");
			return 1;
		}
   		else if( SymbolTable.find(id) != SymbolTable.end() )
   		{
   			if( SymbolTable[id].GetType() != num1->GetType() )
   			{
   				semanticError(this->getLineNumber(), "type error");
				return 1;
   			}
   			return 0;
   		}
   		else
			return 0;
	}
   	Value eval() const
   	{
   		SymbolTable[id] = Value( num1->eval() );
   		return SymbolTable[id];
   	}
   	ParseTree* getLeft() const { return num1; }
};
   
class PrintStatement : public ParseTree {
	bool addNL;
	ParseTree* num1;
public:
	PrintStatement(int line, bool isPrintln, ParseTree *ex) : ParseTree(line, ex), addNL(isPrintln) {
   		num1 = ex;
   	}
	int findSemanticErrors() const 
   	{
   		if( (num1->GetType() == STRING_TYPE && num1->GetStringValue() == "foo") && SymbolTable.find(num1->GetVariable()) == SymbolTable.end() ) 
   		{
			semanticError(this->getLineNumber(), "variable " + num1->GetVariable() + " is used before being declared");
			return 1;
   		}
   		return 0;
	}
	void doprinting() const {
		if( addNL ) std::cout << std::endl;
	}
   	Value eval() const
   	{
   		ParseTree* ex = getLeft();
   		Value prt(ex->eval());
   		if( addNL )
   			cout << prt << endl;
   		else
   			cout << prt;
   		return prt;
   	}
   	ParseTree* getLeft() const { return num1; }
};
   
class IntegerConstant : public ParseTree {
	int	value;
public:
	IntegerConstant(const Token& tok) : ParseTree(tok.GetLinenum()) {
		value = stoi( tok.GetLexeme() );
	}
	Value eval() const 
   	{ 
   		Value num(value);
   		return num; 
   	}
	TypeForNode GetType() const { return INT_TYPE; }
	int GetIntValue() const { return value; }
};
   

class Identifier : public ParseTree {
	string	value;
public:
	Identifier(const Token& tok) : ParseTree(tok.GetLinenum()) {
		value = tok.GetLexeme();
	}
   	Value eval() const 
   	{
   		return SymbolTable[value];
   	}
	TypeForNode GetType() const { return STRING_TYPE; }
	string GetStringValue() const { return "foo"; }
   	string GetVariable() const { return value; }
   	int GetIntValue() const { return 0; }
};

class StringConstant : public ParseTree {
	string	value;
public:
	StringConstant(const Token& tok) : ParseTree(tok.GetLinenum()) {
		value = tok.GetLexeme();
	}
   	Value eval() const 
   	{ 
   		Value word(value);
   		return word; 
   	}
	TypeForNode GetType() const { return STRING_TYPE; }
	string GetStringValue() const { return value; }
};

extern ParseTree *	Prog(istream* in);
extern ParseTree *	StmtList(istream* in);
extern ParseTree *	Stmt(istream* in);
extern ParseTree *	Decl(istream* in);
extern ParseTree *	Set(istream* in);
extern ParseTree *	Print(istream* in);
extern ParseTree *	Expr(istream* in);
extern ParseTree *	Term(istream* in);
extern ParseTree *	Primary(istream* in);

#endif /* PARSER_H_ */
