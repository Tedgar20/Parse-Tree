#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "parser.h"

/*
 * main.cpp
 */
//int errorCount = 0;
int lineNumber;
string fileName;
int errorCount = 0;

void error(int linenum, const string& message) {
	cout << fileName << ":" << linenum+1 << ":" << "Syntax error " << message << endl;
}

void semanticError(int linenum, const string& message) {
   	++errorCount;
	cout << fileName << ":" << linenum+1 << ":" << message << endl;
}

void NodeTraverse(ParseTree *t) 
{
   		
        if( t->getLeft() ) 
           	NodeTraverse(t->getLeft());
       	if( t->getRight() )
           	NodeTraverse(t->getRight());
   		try
  		{
    		t->eval();
  		}
  		catch (const std::exception& e)
  		{
    		cout << e.what() << '\n';
  		}
}

void SemanticTraverse(ParseTree *t) 
{
        if( t->getLeft() ) 
           	SemanticTraverse(t->getLeft());
       	if( t->getRight() )
           	SemanticTraverse(t->getRight());
   		t->findSemanticErrors();
}
map<string,Value> SymbolTable;
int main(int argc, char *argv[])
{
	int clArgs = 1;
   	if( argc == 1 ) 														//Check to see if the program is given only its name as an argument
	{																
		ParseTree *tree = Prog( &cin );
       	if( tree != 0 )
        {
           	SemanticTraverse(tree);
           	if( errorCount == 0 )
           		NodeTraverse(tree);
           	return 0;
        }
       	else if( tree == 0 ) 
        {
		   // there was some kind of parse error
		   return 1;
		}
	}
   	else if( clArgs != (argc-1) )
    {
    	cerr << "TOO MANY FILES" << endl;
        return -1;
    }
   	
   	ifstream infile(argv[clArgs]);
   	fileName = argv[clArgs];
   	if(infile.is_open() == false)
   	{
      	cerr << argv[clArgs] << " FILE NOT FOUND" << endl;
      	return -1;
   	}
   	else
    {
		ParseTree *tree = Prog( &infile );
       	if( tree != 0 )
        {
           	
           	SemanticTraverse(tree);
           	if( errorCount == 0 )
               NodeTraverse(tree);
         	return 0;
        }
       	else if( tree == 0 ) 
        {
           cout << "EEROR";
		   // there was some kind of parse error
		   return 1;
		}
    }
	return 0;
}