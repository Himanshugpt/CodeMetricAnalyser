#ifndef CODEANALYZER_H
#define CODEANALYZER_H
/////////////////////////////////////////////////////////////////////
//  CodeAnalyzer.h - Analyzes C++ language constructs              //
//  ver 1.2                                                        //
//  Language:      Visual C++ 2010                                 //
//  Author:        Himanshu Gupta, Syracuse University             //
//                 (315) 447-1667, hgupta01@syr.edu                //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines a CodeAnalyzer class.  Its functions finds
  the code metrics on the package and funciton level.

  Public Interface:
  =================
  AnalyzeSourceCode(std::map<std::string,Package*>) //This functions analyses the source code.
													//It accepts a map which has a key value pair of package name and the
													//package Obj refremce.

  Build Process:
  ==============
  Required files
    - TypeInfo.h, TypeInfo.cpp, ConfigureParser.h, ConfigureParser.cpp,
      tokenizer.h, tokenizer.cpp,
      ActionsAndRules.h, ActionsAndRules.cpp,
      ConfigureParser.h, ConfigureParser.cpp,
	  Parser.h, Parser.cpp,
	  SemiExpression.h, SemiExpression.cpp
  Build commands
    - devenv CodeMetricAnal.sln
*/

#include <map>
#include "TypeInfo.h"
#include "ConfigureParser.h"

class CodeAnalyzer
{
public:
	CodeAnalyzer(void);
	~CodeAnalyzer(void);
	void AnalyzeSourceCode(std::map<std::string,Package*>);

private:
	void FindFunctionMetrics(std::map<std::string,Package*>);
	void BuildTypeTable(std::map<std::string,Package*> packagesMap);
	void FindPackageRefDenMetrics(std::map<std::string,Package*> packagesMap);
	void CollectIdentifiers(std::map<std::string,Package*> packagesMap);
	void FindPackageControlDepth(std::map<std::string,Package*>);
	void ParseFiles(IBuilder* h,std::map<std::string,Package*> packagesMap);
	void DeleteDataCollections();
	void DisplayInformation();
};


#endif
