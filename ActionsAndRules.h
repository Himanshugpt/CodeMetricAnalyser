#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 1.1                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines several action classes.  Its classes provide 
  specialized services needed for specific applications.  The modules
  Parser, SemiExpression, and Tokenizer, are intended to be reusable
  without change.  This module provides a place to put extensions of
  these facilities and is not expected to be reusable.
  

  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
  Parser parser(se);              // now we have a parser
  Rule1 r1;                       // create instance of a derived Rule class
  Action1 a1;                     // create a derived action
  r1.addAction(&a1);              // register action with the rule
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
    parser.parse();               //   and parse it

  Build Process:
  ==============
  Required files
    - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp,
      SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv Project1HelpS06.sln
    - cl /EHsc /DTEST_PARSER parser.cpp ActionsAndRules.cpp \
         semiexpression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.2 : 17 Feb 2011
  - added rules and actions.
  ver 1.1 : 17 Jan 09
  - changed to accept a pointer to interfaced ITokCollection instead
    of a SemiExpression
  ver 1.0 : 12 Jan 06
  - first release

*/
//
#include <queue>
#include <string>
#include <stack>
#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ITokCollection.h"
#include "TypeInfo.h"

///////////////////////////////////////////////////////////////
// rule to detect preprocessor statements
class PreprocStatement : public IRule
{
public:
  bool doTest(ITokCollection* pTc)
  {
    if(pTc->find("#") < pTc->length())
    {
      doActions(pTc);
      return true;
    }
    return false;
  }
};

///////////////////////////////////////////////////////////////
// action to insert in symbol table
class InsertToTypeTable : public IAction
{
public:
	InsertToTypeTable(){}
	InsertToTypeTable(Toker* t){pToker = t;}
	void doAction(ITokCollection* pTc);
private:
	Toker* pToker;
	
};

/////////////////////////////////////////////////////////
// class to get function size
class GetFunSize : public IAction
{
	public:
		GetFunSize(SemiExp* semi, Toker* toker);	 
		void doAction(ITokCollection* pTc);
	private:
		std::stack<std::string> scopeStk;
		SemiExp* pSemi ;
		Toker* pToker ;
		int cyclyComplexity;

		//returns the count of a pattern matched in a string.
		int countPatterns(const std::string &pattern, const std::string &target)
		{
			int i=0, count = 0, patLen = pattern.size(), targLen = target.size();
			if (patLen == 0 || targLen == 0) return 0;
			if (patLen > targLen) return 0;
			const char *pat = pattern.c_str();
			const char *targ = target.c_str();
			while (i<targLen){
				if (targ[i] == pat[0]){
					if (!strncmp(&targ[i], pat, patLen))
						count++;
				}
				i++;
			}
			return count;
		}
};

////////////////////////////////////////////////////////////
//rule to detect class,interface or struct of the namespace
class DetectClass : public IRule
{
public:
  bool doTest(ITokCollection* pTc)
  {
    if(pTc->find("struct") < pTc->length() || pTc->find("class") < pTc->length() || pTc->find("enum") < pTc->length())
    {
		if(pTc->find("{")==(pTc->length()-1))
		{
		  doActions(pTc);
		  return true;
		}
    }
    return false;
  }
};

////////////////////////////////////////////////////////////
//rule to detect control statement
class DetectControlStatement : public IRule
{
public:
  bool doTest(ITokCollection* pTc)
  {
    if(pTc->find("if") < pTc->length() || pTc->find("for") < pTc->length() || pTc->find("while") < pTc->length())
    {
		doActions(pTc);
		return true;
    }
    return false;
  }
};

////////////////////////////////////////////////////////////
//rule to detect depth of the namespace
class BraceDepth : public IRule
{
public:
	bool doTest(ITokCollection* pTc)
	{
		ITokCollection& tc = *pTc;
		const static std::string openBrace = "{";
		if(tc.find(openBrace) < tc.length())
		{
			doActions(pTc);
			return true;
		}
		else
			return false;	
	}
}; 

///////////////////////////////////////////////////////////////
// Rule to detect Identifiers
class DetectIdentifier : public IRule
{
public:
	DetectClass dc;

	bool ContainsSpecialKeywords(ITokCollection* pTc)
	{
		ITokCollection& tc = *pTc;
		bool flag = true;
		const static std::string specialKeyword[] = {"include","#","define","cin","cout","++","endl"};
		for(int i=0; i<8; ++i)
			if(tc.find(specialKeyword[i])<tc.length())
				flag = true;
			else
				flag =  false;
		return flag;
	}

	bool doTest(ITokCollection* pTc)
	{
		ITokCollection& tc = *pTc;
		bool flag = false;
		const static std::string keyArr[] = {"const","for","static" ,"std","else","volatile",
											"auto", "unsigned","signed" ,"::"};
		if(ContainsSpecialKeywords(pTc))return false;
		if(dc.doTest(pTc)){ doActions(pTc);return true;}
		if(tc.length()>3)
			if(tc[tc.length()-1] == ";" && tc[tc.length()-2] == ")")
				return true;
		if(!isFunction(pTc) && !containsControlKeyword(pTc))
		{
			for(int i=0; i<10; ++i)
				 if(tc.find(keyArr[i])<tc.length()){tc.remove(keyArr[i]);flag = true;}
				 flag = removeSpecialChars(pTc);
				 flag = RemoveAssignmetRight(pTc);
				 flag = removeRefrenceChar(pTc);
				}else if(isFunction(pTc))
					flag = true;
	
		if(flag){doActions(pTc);}
		return true;
	}
	//Function to delete the tokens to the right of =
	bool RemoveAssignmetRight(ITokCollection* pTc)
	{
		bool flag = false;
		ITokCollection& tc = *pTc;
		 if(tc.find("=") < tc.length() ) {	
						 int index=tc.find("=");
							while(tc.length()!=index){
								tc.remove(tc[index]);
							}
							flag = true;
					 }
		return flag;
	}

	// remove the -> or . from the expression
	bool removeRefrenceChar(ITokCollection* pTc)
	{
		bool flag = false;
		ITokCollection& tc = *pTc;
		 if(tc.find(".") < tc.length() || tc.find("->") < tc.length() )
					 {	
						 int i=tc.find(".");
						 int index = tc.find("->") ;
						 if(index>i)
							 index=i;
							while(tc.length()!=index)
								tc.remove(tc[index]);
							flag = true;
					 }
		return flag;
	}

	//removes special chars like *,[,],& 
	//and returns true if it finds any and removes it.
	bool removeSpecialChars(ITokCollection* pTc)
	{
		ITokCollection& tc = *pTc;
		bool flag=false;
		if(tc.find("*") < tc.length()) {flag = true;tc.remove("*");}
		if(tc.find("[") < tc.length()) {flag = true;tc.remove("[");}
		if(tc.find("]") < tc.length()) {flag = true;tc.remove("]");}
		if(tc.find("&") < tc.length()) {tc.remove("&");flag = true;}
		return flag;
	}

	// Function to test that it contains any control keyword
	// returns true if it finds any.
	bool containsControlKeyword(ITokCollection* pTc)
	{
		bool flag = false;
		const static std::string keys[]
		  = { "for", "while", "switch", "if", "catch","cout","return","void" };
		  for(int i=0; i<8; i++)
			  if(pTc->find(keys[i]) < pTc->length()){ return true; }
		  return flag;
	}

	// function to test that it has special keyword or not..
	// returns true if it finds any.
	 bool isSpecialKeyWord(const std::string& tok)
	  {
		const static std::string keys[]
		  = { "for", "while", "switch", "if", "catch" };
		for(int i=0; i<5; ++i)
		  if(tok == keys[i])
			return true;
		return false;
	  }

	// returns true of the semiex is a function.
	bool isFunction(ITokCollection* pTc)
	  {
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
		  int len = tc.find("(");
		  if(len < tc.length() && !isSpecialKeyWord(tc[len-1]))
		  {
			return true;
		  }
		}
		return false;
	 }

};

///////////////////////////////////////////////////////////////
// rule to detect function definitions
class FunctionDeclaration : public IRule
{
public:
  bool isSpecialKeyWord(const std::string& tok)
  {
    const static std::string keys[]
      = { "for", "while", "switch", "if", "catch" };
    for(int i=0; i<5; ++i)
      if(tok == keys[i])
        return true;
    return false;
  }
  bool doTest(ITokCollection* pTc)
  {
    ITokCollection& tc = *pTc;
	if(tc[tc.length()-1] == ";")
    {
      int len = tc.find("(");
	  if(!(tc.find("return")<tc.length()) && !(tc.find("+")<tc.length()) && !(tc.find("++")<tc.length()) && !(tc.find("=")<tc.length()))
	  {
	  if(len < tc.length() &&  (tc[tc.length()-2] == ")") &&!isSpecialKeyWord(tc[len-1]) && !(tc.find("cout")<tc.length()))
      {
        doActions(pTc);
        return true;
      }
	  }
    }
    return false;
  }
};

///////////////////////////////////////////////////////////////
// rule to detect function definitions
class FunctionDefinition : public IRule
{
public:
  bool isSpecialKeyWord(const std::string& tok)
  {
    const static std::string keys[]
      = { "for", "while", "switch", "if", "catch" };
    for(int i=0; i<5; ++i)
      if(tok == keys[i])
        return true;
    return false;
  }
  bool doTest(ITokCollection* pTc)
  {
    ITokCollection& tc = *pTc;
    if(tc[tc.length()-1] == "{")
    {
      int len = tc.find("(");
      if(len < tc.length() && !isSpecialKeyWord(tc[len-1]))
      {
        doActions(pTc);
        return true;
      }
    }
    return false;
  }
};

//////////////////////////////////////////////////////////////////
/// Rule to determine package data locality
class DetermineDataLocality : public IRule
{
public:
	DetermineDataLocality(){}
	DetermineDataLocality(Toker* t){pToker=t;}
	~DetermineDataLocality()
	{
		delete pToker;
	}

private:
	Toker *pToker;

	void UpdateIdentifier(ITokCollection* pTc)
	{
		ITokCollection& tc = *pTc;
		if(pTc->find("->") < pTc->length() || pTc->find(".") < pTc->length())
			{
				int poniterIndex = pTc->find("->");
				int dotIndex = pTc->find(".");
				int min = 0;
				if(poniterIndex < dotIndex)
					min = poniterIndex;
				else 
					min = dotIndex;
				std::string idenName = tc[min-1];
				PackageMap::UpdateIdentifierInfo(idenName,pToker);
			}
	}

	bool doTest(ITokCollection* pTc)
	{
		ITokCollection& tc = *pTc;
			FunctionDefinition fd;
			UpdateIdentifier(pTc);
		if( !(pTc->find("struct") < pTc->length() || pTc->find("~") < pTc->length() ||
			pTc->find("class") < pTc->length() || pTc->find("enum") < pTc->length())
			&& !fd.doTest(pTc) && !(pTc->find("(")<pTc->length()))
		{
			for(int i=0;i<tc.length();i++)
			{
				std::string s= tc[i];
				if(TypeTable::Contains(s))
				{
					if(pTc->find("*")<pTc->length()) pTc->remove("*");
					if(pTc->find("&")<pTc->length()) pTc->remove("&");
					int colonIndex = pTc->find(";");
					int equalsIndex = pTc->find("=");
					if((colonIndex<equalsIndex))
					{
						IdentifierLocality* idL = new IdentifierLocality();
						std::string identifierName = tc[colonIndex-1];
						idL->SetIdentiferName(identifierName);
						idL->SetIdentiferType(tc[colonIndex-2]);
						idL->SetFileDeclared(pToker->GetFileName());
						idL->SetLineDeclared(pToker->GetLineNumber()+1);
						PackageMap::AddIdentifierToLocalityMap(pToker->GetFileName(),idL);
					}else if(equalsIndex > 1 && (pTc->find("new")<pTc->length()) )
					{
						IdentifierLocality* idL = new IdentifierLocality();
							idL->SetIdentiferName(tc[equalsIndex-1]);
							if(equalsIndex != 1)//means using the same refrence
							idL->SetIdentiferType(tc[equalsIndex-2]);
							idL->SetFileDeclared(pToker->GetFileName());
							idL->SetLineDeclared(pToker->GetLineNumber()+1);
							PackageMap::AddIdentifierToLocalityMap(pToker->GetFileName(),idL);
					}else
					{
						std::string idenName = tc[equalsIndex+1];
						PackageMap::UpdateIdentifierInfo(idenName,pToker);
					}
				}
			}
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////
// action to print preprocessor statement to console
class PrintPreproc : public IAction
{
public:
  void doAction(ITokCollection* pTc)
  {
    std::cout << "\n\n  Preproc Stmt: " << pTc->show().c_str();
  }
};

///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console
class PrintFunction : public IAction
{
public:
  void doAction(ITokCollection* pTc)
  {
    std::cout << "\n\n  FuncDef Stmt: " << pTc->show().c_str();
  }
};

///////////////////////////////////////////////////////////////
// action to send signature of a function def to console
class PrettyPrintFunction : public IAction
{
public:
  void doAction(ITokCollection* pTc)
  {
    pTc->remove("public");
    pTc->remove(":");
    pTc->trimFront();
    int len = pTc->find(")");
    std::cout << "\n\n  Pretty Stmt:    ";
    for(int i=0; i<len+1; ++i)
      std::cout << (*pTc)[i] << " ";
  }
};

///////////////////////////////////////////////////////////////
// action to store a preprocessor statement in queue
class PreprocToQ : public IAction
{
  std::queue<std::string>* pQ;

public:
  PreprocToQ(std::queue<std::string>& resultQ) : pQ(&resultQ)
  {
  }
  void doAction(ITokCollection* pTc)
  { pQ->push("Preproc Stmt: " + pTc->show()); }
};

class UpdatePackageIdentifiers : public IAction
{
public:
	UpdatePackageIdentifiers(Toker* t)
	{
		pToker = t;
	}
	~UpdatePackageIdentifiers(){ delete pToker;}
private:
	Toker* pToker;

	void doAction(ITokCollection* pTc)
	{	
		PackageMap::AddIdentifierToPackage(pToker->GetFileName(),pTc);
	}
};


///////////////////////////////////////////////////////////////
// action to calculate Scope depth of a package
class DetermineScopeDepth : public IAction
{
	public:
		int depthLevel;
		DetermineScopeDepth(Toker* tok)
		{
			pToker = tok;
			depthLevel = 1;
		}
 
	private: 
		Toker* pToker;

  void doAction(ITokCollection* pTc)
  {	
	  depthLevel = 1;
	  int braceLevel = pToker->braceLevel();
	  if(depthLevel < braceLevel)
	  {
		  depthLevel = braceLevel;
		  PackageMap::UpdatePackageScopeDepth(pToker->GetFileName(),braceLevel);
	  }
  }
};

///////////////////////////////////////////////////////////////
// action to store function signature in queue
class PrettyPrintToQ : public IAction
{
  std::queue<std::string>* pQ;

public:
  PrettyPrintToQ(std::queue<std::string>& resultQ) : pQ(&resultQ)
  {
  }
  void doAction(ITokCollection* pTc)
  {
    pTc->remove("public");
    pTc->remove(":");
    pTc->trimFront();
    int len = pTc->find(")");
    std::string tempStr = "Pretty Stmt:      ";
    for(int i=0; i<len+1; ++i)
      tempStr += (*pTc)[i] + " ";
    pQ->push(tempStr);
  }

   void doActionLOck(ITokCollection* pTc)
  {
    pTc->remove("public");
    pTc->remove(":");
    pTc->trimFront();
    int len = pTc->find(")");
    std::string tempStr = "Pretty Stmt:      ";
    for(int i=0; i<len+1; ++i)
      tempStr += (*pTc)[i] + " ";
    pQ->push(tempStr);
  }
};

///////////////////////////////////////////////////////////
// to find the breadth of control statements.
class FindControlBreadth : public IAction 
{
	std::stack<std::string> scopeStk;
	int startLineNumber, endLineNumber;
	private: 
		Toker* pToker;

	public:
		FindControlBreadth(Toker* toker)
		{pToker = toker;}
		
		void doAction(ITokCollection* pTc)
		{  
			std::string controlStatement = pTc->show();
			if(pTc->find("for") < pTc->length())
			{
				pTc->get();
				pTc->get();
			}
			bool endsInBrace = pTc->show().find("{") <  pTc->show().length();
			if(endsInBrace)
			{
				scopeStk.push("{");
				startLineNumber = pToker->lines();
				while(scopeStk.size()!=0 && pTc->get())
				 {
					 std::string str = pTc->show().c_str();
					  if(str.find("{")<str.length()){
							scopeStk.push("{");
					  }
					  if(str.find("}")<str.length()){
							scopeStk.pop();
						}
				}
					 endLineNumber = pToker->lines();
			}
			 int controlBreadthSpan = (endLineNumber - startLineNumber);
			 if(controlBreadthSpan > 19)
			 {
				 ControlSpan* cs = new ControlSpan();
				 cs->SetControlKeyword(controlStatement);
				 cs->SetLineNums(startLineNumber,endLineNumber);
				 cs->SetFileName(pToker->GetFileName());
				 PackageMap::UpdatePackageControlBreadth(pToker->GetFileName(),cs);
			 }
		}
};

#endif
