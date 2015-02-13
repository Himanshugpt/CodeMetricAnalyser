/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.cpp - implements new parsing rules and actions //
//  ver 1.1                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <string>
#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ITokCollection.h"
#include "ActionsAndRules.h"
#include "TypeInfo.h"

///////////////////////////////////////////////////////////////
// action to insert in symbol table
void InsertToTypeTable::doAction(ITokCollection* pTc)
{
	ITokCollection& tc = *pTc;
	std::string s = pTc->show().c_str();
	std::string endIndex;
	int minIndex = s.find(":")<s.find("{")?s.find(":"):s.find("{");//finds the first/min index of { or :
	if(pTc->find("public")<pTc->length())pTc->remove("public");
	if(pTc->find(":")<pTc->length()) pTc->remove(":");
	if(pTc->find("private")<pTc->length())pTc->remove("private");
	pTc->trimFront();
	if(pTc->find("class")<pTc->length())
	{
		TypeInfo* typeinfo = new TypeInfo();
		typeinfo->SetType("class");
		typeinfo->SetLineNum(pToker->lines()+1);
		std::string className = s.substr(8,minIndex-9);
		typeinfo->SetTypeName(className);
		typeinfo->SetPackageName(pToker->GetFileName());
		TypeTable::Add(className,typeinfo);
	}
	if(pTc->find("struct")<pTc->length()){
		std::string structName = s.substr(9,minIndex-10);
		TypeInfo* typeinfo = new TypeInfo();
		typeinfo->SetType("struct");
		typeinfo->SetLineNum(pToker->lines()+1);
		typeinfo->SetTypeName(structName);
		typeinfo->SetPackageName(pToker->GetFileName());
		TypeTable::Add(structName,typeinfo);
	}
	if(pTc->find("enum")<pTc->length()){
		pTc->remove("enum");pTc->remove("{");pTc->trimFront();
		std::string enumName = pTc->show();
		if(pTc->find("\n")<pTc->length())pTc->remove("\n");
		if(pTc->find(" ")<pTc->length()) pTc->remove(" ");
		TypeInfo* typeinfo = new TypeInfo();
		typeinfo->SetType("enum");
		typeinfo->SetLineNum(pToker->lines()+1);
		typeinfo->SetTypeName(enumName);
		typeinfo->SetPackageName(pToker->GetFileName());
		TypeTable::Add(enumName,typeinfo);
	}
}

GetFunSize :: GetFunSize(SemiExp* semi, Toker* toker)
{		
	  pSemi = semi;
	  pToker = toker;
}
	 
void GetFunSize ::doAction(ITokCollection* pTc)
{  
	  std::string funName = pTc->show().c_str();
	  scopeStk.push("{");
	  int lineNum = pToker->lines();
	  int endLineNum = 0;
	  cyclyComplexity= 1;
	  while(scopeStk.size()!=0 && pTc->get())
	  {
		  std::string str = pTc->show().c_str();
		  if(str.find("{")==(str.length()-1)){
			  scopeStk.push("{");
		  }
		  if(str.find("}")==(str.length()-1)){
			  scopeStk.pop();
		  }
		  //used to find multiple controls in single semiexpression.
		  bool containsFor = str.find("for")<str.length();
		  bool containsWhile = str.find("while")<str.length() ;
		  bool containsIf = str.find("if (")<str.length() || str.find("if(")<str.length();
		  bool conatinsBreak = str.find("break")<str.length();
		  bool containsGoto = str.find("goto")<str.length();
		  bool containsCatch = str.find("catch")<str.length();
			  if(containsFor) cyclyComplexity++;
			  if(conatinsBreak) cyclyComplexity++;
			  if(containsGoto)  cyclyComplexity++;
			  if(containsCatch)  cyclyComplexity++;
			  if(containsWhile)
			  {
				  int whileCount = countPatterns("while",pTc->show());
				  cyclyComplexity = cyclyComplexity+whileCount;
			  }

			  if(containsIf)
			  {
				   int ifCount = countPatterns("if",pTc->show());
				   cyclyComplexity = cyclyComplexity+ifCount;
			  }
		}
		  endLineNum =  pToker->lines();
		  FunctionInfo* funInfo = new FunctionInfo();
		  funInfo->SetCyclomaticComplexity(cyclyComplexity);
		  funInfo->SetStartLineNumber(lineNum);
		  funInfo->SetEndLineNumber(endLineNum);
		  funInfo->SetFileName(pToker->GetFileName());
		  funInfo->SetFunctionName(funName);
		  FunctionList* funList = new FunctionList();
		  funList->AddToFunctionList(funInfo);
	  }



#ifdef TEST_ACTIONSANDRULES

#include <iostream>
#include "ActionsAndRules.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ActionsAndRules class\n "
            << std::string(30,'=') << std::endl;

  try
  {
    std::queue<std::string> resultsQ;
    PreprocToQ ppq(resultsQ);
    PreprocStatement pps;
    pps.addAction(&ppq);

    FunctionDefinition fnd;
    PrettyPrintToQ pprtQ(resultsQ);
    fnd.addAction(&pprtQ);

    Toker toker("../ActionsAndRules.h");
    SemiExp se(&toker);
    Parser parser(&se);
    parser.addRule(&pps);
    parser.addRule(&fnd);
    while(se.get())
      parser.parse();
    size_t len = resultsQ.size();
    for(size_t i=0; i<len; ++i)
    {
      std::cout << "\n  " << resultsQ.front().c_str();
      resultsQ.pop();
    }
    std::cout << "\n\n";
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
}
#endif
