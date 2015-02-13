#ifndef TYPEINFO_H
#define TYPEINFO_H

/////////////////////////////////////////////////////////////////////
//  TypeInfo.h - Declares classes used for type & package coll     //
//  ver 1.2                                                        //
//  Language:      Visual C++ 2010                                 //
//  Author:        Himanshu Gupta, Syracuse University             //
//                 (315) 447-1667, hgupta01@syr.edu                //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines the following classes:

  TypeInfo						// Used to represent the type information
  TypeTable						// Maintains a map of type name with its corresponding TypeInfo Object.
  FunctionInfo					// Information about the function
  FunctionList					// Maintains a list of all the detected funciton.(FunctionInfo Object Refs)
  ControlSpan					// Used to store information of the control span of the project
  IdentifierLocality			// Used to store information abt identifiers
  Package						// Represents the Package which is a combination of header + source file.
  PackageMap					// Maintains a map of packages corresponding to their names.

  Build Process:
  ==============
  Required files
    - itokcollection.h, Tokenizer.cpp
  Build commands
    - devenv CodeMetricAnal.sln
*/

#include <string>
#include <iostream>
#include <map>
#include <list>
#include <set>
#include "itokcollection.h"
#include "Tokenizer.h"

//////////////////////////////////////////////////////////////////
// Class to represent the type information of a declared type.
class TypeInfo
{
public:
	TypeInfo(void){}
	~TypeInfo(void);
	std::string GetType();				//returns the type liek class,enum,struct 
	std::string GetTypeName();			//returns name of the type
	std::string GetPackageName();		//returns package name
	int GetLineNumber();                //declared line number
	void SetType(std::string);
	void SetTypeName(std::string);
	void SetPackageName(std::string);
	void SetLineNum(int num);

private:
	std::string type;
	std::string typeName;
	std::string packageName;
	std::string scopeName;
	int lineNumber;
};

inline std::string TypeInfo::GetType(){return type;}
inline std::string TypeInfo::GetTypeName(){return typeName;}
inline int TypeInfo::GetLineNumber(){return lineNumber;}
inline void TypeInfo::SetLineNum(int ln){this->lineNumber = ln;}
inline void TypeInfo::SetType(std::string t){this->type =t;}
inline void TypeInfo::SetTypeName(std::string tn){this->typeName =tn;}
inline void TypeInfo::SetPackageName(std::string pName){this->packageName =pName;}
inline std::string TypeInfo::GetPackageName(){return packageName;}

class ControlSpan;

/////////////////////////////////////////////////////////////////
// Class for storing typeinfo objects
class TypeTable
{
public:
	TypeTable(void);
	~TypeTable(void);
	static void Add(std::string key,TypeInfo* value){ typeTable[key]=value;}
	static void Remove(std::string key)	{typeTable.erase(key);}
	static void Display();

	//returns if it has a corresponding key 
	//otherwise returns false
	static bool Contains(std::string key)
	{
		if(typeTable.end() == typeTable.find(key))
				return false;
			else
				return true;
	}

private:
	static std::map<std::string,TypeInfo*> typeTable;
};

///////////////////////////////////////////////////////////////////////
// Represents information about the function.
class FunctionInfo
{
public:
	FunctionInfo(void);
	~FunctionInfo(void);
	void SetStartLineNumber(int);
	void SetEndLineNumber(int);
	void SetCyclomaticComplexity(int);
	void SetFunctionName(std::string);
	void SetPackageName(std::string);
	void SetFileName(std::string);
	std::string GetFileName();
	std::string GetPackageName();
	std::string GetFunctionName();
	int GetCyclomaticComplexity();
	int GetStartLineNumber();
	int GetEndLineNumber();
	int GetFunctionSize();

private:
	std::string functionName;
	std::string packageName;
	std::string fileName;
	int startLineNumber;
	int cycloComplexity;
	int endLineNumber;
};

inline void  FunctionInfo :: SetStartLineNumber(int n){ this->startLineNumber = n;}
inline void  FunctionInfo :: SetEndLineNumber(int n){this->endLineNumber = n;}
inline void  FunctionInfo :: SetCyclomaticComplexity(int n){this->cycloComplexity = n;}
inline void  FunctionInfo :: SetFunctionName(std::string s){ this->functionName = s;}
inline void  FunctionInfo :: SetPackageName(std::string s){ this->packageName = s;}
inline void  FunctionInfo :: SetFileName(std::string s){ this->fileName = s;}
inline  std::string FunctionInfo::GetFileName(){return this->fileName;}
inline	std::string FunctionInfo::GetPackageName(){return this->packageName;}
inline	std::string FunctionInfo::GetFunctionName(){return functionName;}
inline	int FunctionInfo::GetCyclomaticComplexity(){return cycloComplexity;}
inline	int FunctionInfo::GetStartLineNumber(){return startLineNumber;}
inline	int FunctionInfo::GetEndLineNumber(){return endLineNumber;}
inline	int FunctionInfo::GetFunctionSize(){return endLineNumber - startLineNumber;};

//////////////////////////////////////////////////////////////////////////////
// Class to maintain a list of the all the detected functions(FunctionInfo)
class FunctionList
{
public:
	FunctionList(){};
	~FunctionList();
	void AddToFunctionList(FunctionInfo*);
	void DisplayFunctions();

private:
	static std::list<FunctionInfo*> funList; //list of the pointers to the functioinfo objects.
};

////////////////////////////////////////////////////////////////
// Class to represent information about the identifier.
class IdentifierLocality
{
public:
	IdentifierLocality(){timesUsed=0;lastUsedAtLine=0;}
	~IdentifierLocality(){}
	void SetIdentiferName(std::string);
	void SetIdentiferType(std::string);
	void SetFileDeclared(std::string);
	void SetLineDeclared(int);
	void SetLastusedAtLine(int);
	void IncrementTimesUsed();
	int GetLineDeclared();
	int GetLastUsedAtLine();
	int GetTimesUsed();
	std::string GetIdentifierName();
	std::string GetIdentifierType();
	std::string GetFileDeclared();

private:
	std::string identifierName;
	std::string identifierType;
	std::string fileDeclared;
	int lineDeclared;
	int lastUsedAtLine;
	int timesUsed;
};

inline std::string IdentifierLocality::GetFileDeclared(){return fileDeclared;}
inline std::string IdentifierLocality::GetIdentifierName(){return identifierName;}
inline std::string IdentifierLocality::GetIdentifierType(){return identifierType;}
inline int  IdentifierLocality::GetLineDeclared(){return lineDeclared;} 
inline int  IdentifierLocality::GetTimesUsed(){return timesUsed;}
inline int  IdentifierLocality::GetLastUsedAtLine(){return lastUsedAtLine;}
inline void IdentifierLocality::SetFileDeclared(std::string fName){fileDeclared=fName;}
inline void IdentifierLocality::SetLastusedAtLine(int lNum){lastUsedAtLine=lNum;}
inline void IdentifierLocality::SetLineDeclared(int lNum){lineDeclared=lNum;}
inline void IdentifierLocality::SetIdentiferName(std::string name){identifierName=name;}
inline void IdentifierLocality::SetIdentiferType(std::string type){identifierType=type;}
inline void IdentifierLocality::IncrementTimesUsed(){timesUsed++;}

//////////////////////////////////////////////////////////
// This class represents the information 
// about the packages detected.
class Package{
	public:
	  Package();
	  Package(std::string, std::string);
	  ~Package();
	  void SetHeaderFileName(std::string);
	  void SetSourceFileName(std::string);
	  std::string GetHeaderFileName();
	  std::string GetSourceFileName();
	  void SetDeepestScope(int);
	  int GetDeepestScope();
	  void AddToControlList(ControlSpan*);
	  std::list<ControlSpan*> GetControlList();
	  void AddToIdentifierSet(std::string);
	  int GetIdentifiersCount();
	  void DisplayIdentifiers();
	  void AddToLocalityMap(std::string fname,IdentifierLocality*);
	  void AddToLocalityMap1(std::string fname,IdentifierLocality*);
	  std::multimap<std::string,IdentifierLocality*>& GetLocalityMultiMap();

	private:
	  std::string headerFile;
	  std::string sourceFile;
	  int deepestScope;
	  std::list<ControlSpan*> controls;
	  std::set<std::string> identifiersSet;    // collection of identifiers.
	  std::multimap<std::string,IdentifierLocality*> packageDataLocalityMap;
   };

  inline std::string Package::GetHeaderFileName(){return headerFile;}
  inline std::string Package::GetSourceFileName(){return sourceFile;}
  inline std::list<ControlSpan*> Package::GetControlList(){return controls;}
  inline void Package::SetHeaderFileName(std::string hFileName){headerFile=hFileName;}
  inline void Package::SetSourceFileName(std::string sFileName){sourceFile=sFileName;}
  inline int Package::GetDeepestScope(){return deepestScope;}
  inline int Package::GetIdentifiersCount(){return identifiersSet.size();}
  inline std::multimap<std::string,IdentifierLocality*>& Package::GetLocalityMultiMap(){return packageDataLocalityMap;}
  inline void Package::AddToControlList(ControlSpan* cs){controls.push_back(cs);}
  inline void Package::AddToIdentifierSet(std::string identifier){identifiersSet.insert(identifier);}

////////////////////////////////////////////////////////////
// This class is used to maintian the packages collection.
class PackageMap
{
public:
	~PackageMap();
	static void AddToPackageCollection(std::string fName); //to add filename to file set.
	static void DisplayInnerScopePackages();	
	static std::map<std::string,Package*> GetPackagesColl();
	static void UpdatePackageScopeDepth(std::string,int);
	static void UpdatePackageControlBreadth(std::string fName,ControlSpan*);
	static void DisplayControlBreadth();
	static void AddIdentifierToPackage(std::string,ITokCollection* pTc);
	static Package* GetPackage(std::string); // returns the package obj relevant to its key.
	static void AddIdentifierToLocalityMap(std::string fname,IdentifierLocality*);
	static void DisplayIdentifiers();		//displays the number of identifiers
	static void DisplayDeclaredObjects();
	static void UpdateIdentifierInfo(std::string identifierName,Toker*);
	
private:
	static std::map<std::string,Package*> packages; // map to keep packages.
	static std::string GetPackageName(std::string);
};

///////////////////////////////////////////////////////
// Classs to represent the control span.
class ControlSpan
{
public:
	ControlSpan();
	~ControlSpan();
	void SetLineNums(int,int);
	void SetControlKeyword(std::string);
	void SetScope(std::string);
	std::string GetScope();
	std::string GetControlKeyword();
	int GetControlBreadth();
	int GetStartLineNum();
	int GetEndLineNum();
	std::string GetFileName();
	void SetFileName(std::string);

private:
	int startLine;
	int endLine;
	std::string controlKeyword;
	std::string scope;
	std::string fileName;
};

inline void ControlSpan::SetControlKeyword(std::string controlStr){ controlKeyword = controlStr;}
inline void ControlSpan::SetScope(std::string sc){scope=sc;}
inline std::string ControlSpan::GetScope(){return scope;}
inline std::string ControlSpan::GetControlKeyword(){return controlKeyword;}
inline int ControlSpan::GetControlBreadth(){return endLine-startLine;}
inline int ControlSpan::GetStartLineNum(){return startLine;}
inline int ControlSpan::GetEndLineNum(){return endLine;}
inline std::string ControlSpan::GetFileName(){	return fileName;}
inline void ControlSpan::SetFileName(std::string fName){fileName=fName;}
#endif
