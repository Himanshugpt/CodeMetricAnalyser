#include "TypeInfo.h"
#include <list>
#include <iterator>
#include "itokcollection.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "Tokenizer.h"
#include <map>
#include <iomanip>
#include <fstream>

std::list<FunctionInfo*> FunctionList::funList;

// Map used to store Package Objects
std::map<std::string,Package*> PackageMap::packages;
std::map<std::string,TypeInfo*> TypeTable::typeTable;

TypeInfo::~TypeInfo(void){}

FunctionInfo::FunctionInfo(){}

FunctionInfo::~FunctionInfo(void){}

FunctionList::~FunctionList()
{
	std::list<FunctionInfo*>::iterator iter;
	for(iter=funList.begin();iter!=funList.end();++iter)
		delete (*iter);
}

// Adds FunctionInfo to Function List.. 
void FunctionList::AddToFunctionList(FunctionInfo* fi)
{
	if(0 != fi)
		FunctionList::funList.push_back(fi);
}

// Used to Display Functions.
void FunctionList::DisplayFunctions()
{ 
	
	std::cout << "\n=======================================================================================================================" << std::endl ;
	std::cout << "						Cyclomatic Complexity of Function Greater then 10" << std::endl;
	std::cout << "=======================================================================================================================\n" << std::endl ;
	
	std::list<FunctionInfo*>::iterator iter;
	for(iter=FunctionList::funList.begin();iter!=FunctionList::funList.end();++iter)
	{
		if((*iter)->GetCyclomaticComplexity()>10)
		{	
			std::cout<< "Function Name:: " <<(*iter)->GetFunctionName() <<std::endl;
			std::cout<< "Cyclomatic Complexity:: " <<(*iter)->GetCyclomaticComplexity()<< std::endl;
			std::cout<< "File Name:: " <<  (*iter)->GetFileName() <<std::endl;
			std::cout<< "Line Number:: " <<(*iter)->GetStartLineNumber() << std::endl << std::endl;
			std::cout<< ".........................................................................................................\n";
		}
		
	}

	std::cout << "\n=======================================================================================================================" << std::endl ;
	std::cout << "						Functions having size more than 50" << std::endl;
	std::cout << "=======================================================================================================================\n" << std::endl ;
	for(iter=FunctionList::funList.begin();iter!=FunctionList::funList.end();++iter)
	{
		if(((*iter)->GetEndLineNumber() - (*iter)->GetStartLineNumber() + 1)>50)
		{	
			std::cout<< "Function Name:: " <<(*iter)->GetFunctionName() <<std::endl;
			std::cout << "Function Size:: " <<   (*iter)->GetEndLineNumber() - (*iter)->GetStartLineNumber() + 1 << std::endl ;
			std::cout<< "File Name:: " <<  (*iter)->GetFileName() <<std::endl;
			std::cout<< "Line Number:: " <<(*iter)->GetStartLineNumber() << std::endl << std::endl;
			std::cout<< ".........................................................................................................\n";
		}
		
	}
}

std::map<std::string,Package*> PackageMap::GetPackagesColl()
{
	return packages;
}

// Used to Display the Inner most scope of the package.
void PackageMap::DisplayInnerScopePackages(){
	std::map<std::string,Package*>::iterator iter;
	std::cout << "\n===============================================================================" << std::endl ;
	std::cout << "			Innermost Scope of Packages" << std::endl;
	std::cout << "===============================================================================\n" << std::endl ;
	std::cout << std::setw(20) << "Package Name" << "	"<<std::setw(10) << "Innermost Scope" << "	" << std::setw(5)<< "Number of Identifies Used" << std::endl;
	std::cout<< "..............................................................................\n";
	for(iter=packages.begin(); iter!=packages.end(); ++iter)
	{
		std::cout << std::setw(20) << (*iter).first << "	"<< std::setw(10) << (*iter).second->GetDeepestScope()<< "	" << iter->second->GetIdentifiersCount() <<std::endl;
	}
	std::cout<< "..............................................................................\n";
}	

// Used to Display the Control Breadth 
void PackageMap::DisplayControlBreadth()
{
	
	std::cout << "\n=======================================================================================================================" << std::endl ;
	std::cout << "						Control Statement having size more than 19" << std::endl;
	std::cout << "=======================================================================================================================\n" << std::endl ;
	std::map<std::string,Package*>::iterator iter;
	for(iter=packages.begin(); iter!=packages.end(); ++iter)
	{
		std::list<ControlSpan*>::iterator contolListIter;
		
		std::list<ControlSpan*> controlList = (*iter).second->GetControlList();
		if(controlList.size() >0)
		{	
			std::cout << "\nDisplaying Results for Package:: " << (*iter).first << std::endl;
			for(contolListIter = controlList.begin();contolListIter!= controlList.end();++contolListIter)
			{
				std::cout << "File Name: " << (*contolListIter)->GetFileName() << std::endl;
				std::cout << "Start Line Number: " << (*contolListIter)->GetStartLineNum() << std::endl;
				std::cout << "Control Statement: " << (*contolListIter)->GetControlKeyword() << std::endl;
				std::cout << "Control Breadth: " <<  (*contolListIter)->GetControlBreadth() << std::endl<<std::endl;

			}
			std::cout<< ".........................................................................................................\n\n";
		}
	}
}

// returns the package name from the file name.
std::string PackageMap::GetPackageName(std::string fName)
{
	std::string packageName =  fName.substr(fName.find_last_of("\\")+1,(fName.find_last_of(".")-fName.find_last_of("\\")-1));
	return packageName;
}

// This method adds a package to the package map.
void PackageMap::AddToPackageCollection(std::string fName)
{
	std::string key;
	std::map<std::string,Package*>::iterator iter;
	key = GetPackageName(fName);
	if(packages.find(key) == packages.end())
	{
		Package* pac = new Package;
		if(fName.find(".cpp")<fName.length())
			pac->SetSourceFileName(fName);
		else 
			pac->SetHeaderFileName(fName);
		packages[key]=pac;
	}else{
		iter = packages.find(key);
		if(fName.find(".cpp")<fName.length())
			iter->second->SetSourceFileName(fName);
		else 
			iter->second->SetHeaderFileName(fName);
	}
}

//------<default contructor Package>--------------------
Package::Package(){
	this->headerFile = "";
	this->sourceFile = "";
	this->deepestScope = 1;
}

//---- Destructor-----
Package::~Package()
{
	std::multimap<std::string,IdentifierLocality*>::iterator iter;
	for(iter=packageDataLocalityMap.begin();iter!=packageDataLocalityMap.end();++iter)
		delete iter->second;
	std::list<ControlSpan*>::iterator it;
	for(it=controls.begin();it!=controls.end();++it)
		delete *it ;
}

//----------<parametrized contructor of Package class>--------
Package::Package(std::string hFileName, std::string sFileName)
{
	this->headerFile = hFileName;
	this->sourceFile = sFileName;
}

// This method displays the Identifiers.
void PackageMap::DisplayIdentifiers()
{
	std::cout << "\n=======================================================================================================================" << std::endl ;
	std::cout << "						Number of Identifiers Used in Packages" << std::endl;
	std::cout << "=======================================================================================================================\n" << std::endl ;
	std::cout << std::setw(20) << "Package Name" << "	"<<std::setw(5) << "Number of Identifies Used" <<std::endl;
	std::cout<< "..............................................................................\n";
	std::map<std::string,Package*>::iterator iter;
	for(iter= packages.begin(); iter!=packages.end();++iter)
	{
		std::cout << std::setw(20) << iter->first.c_str() << "	"<<std::setw(5) << iter->second->GetIdentifiersCount() <<std::endl;
	}
}

//Displays the number of identifiers
void Package::DisplayIdentifiers()
{
	std::set<std::string>::iterator iter;
	std::multimap<std::string,IdentifierLocality*>::iterator it;
	std::cout << "No of identifiers " << this->identifiersSet.size() << " "; 
}

//Sets the deepest scope by comparing it to the 
//present value.
void Package::SetDeepestScope(int scopeValue)
{
	if(deepestScope > scopeValue)
		return;
	deepestScope = scopeValue;
}

//returns the package ref
Package* PackageMap::GetPackage(std::string fName)
{
	std::string key = GetPackageName(fName);
	if(packages.find(key) != packages.end())
		return packages.find(key)->second;
	else 
	{
		Package* pac = new Package;
		if(fName.find(".cpp")<fName.length())
			pac->SetSourceFileName(fName);
		else 
			pac->SetHeaderFileName(fName);
		PackageMap::packages[key]=pac;
		return pac;
	}
}

//Adds the value to the locality Map
void Package::AddToLocalityMap(std::string fname,IdentifierLocality* iden)
{
	packageDataLocalityMap.insert(std::pair<std::string,IdentifierLocality*>(fname,iden));   //[fname]=iden;
}

// Destructor
PackageMap::~PackageMap()
{
	std::map<std::string,Package*>::iterator itr;
	for(itr=packages.begin();itr!=packages.end();++itr)
		delete itr->second;
}

// Adds Identifers to the Package.
void PackageMap::AddIdentifierToPackage(std::string fName, ITokCollection* pTc)
{
	ITokCollection& tc = *pTc;
	std::string key = PackageMap::GetPackageName(fName);
	Package* pac = PackageMap::GetPackage(fName);
	pTc->trimFront();
	if(pTc->find("\n")<pTc->length()) pTc->remove("\n");
	if(pTc->find(";")<pTc->length()) pTc->remove(";");
	const static std::string keywords[] = {"int","char","size_t","bool","string","long","double","float","std::string"};
	if((pTc->length() == 1) && !(pTc->find("{") < pTc->length())) pac->AddToIdentifierSet(pTc->show());
	if(pTc->length() >= 3) pac->AddToIdentifierSet(pTc->show());

	if(pTc->length() == 2)
	{	
		for(int i=0; i<9;i++)
		{
			if(pTc->find(keywords[i]) < pTc->length())
			{
				pTc->remove(keywords[i]);
				pTc->trimFront();
				pac->AddToIdentifierSet(pTc->show());
			}
		}
	}
}

//Updates the value of control depth
void PackageMap::UpdatePackageControlBreadth(std::string fName,ControlSpan* cs)
{
	std::string key =  GetPackageName(fName);
	if(packages.find(key) != packages.end())
		packages.find(key)->second->AddToControlList(cs);
	else
	{
		Package* pac = new Package;
		if(fName.find(".cpp")<fName.length())
			pac->SetSourceFileName(fName);
		else 
			pac->SetHeaderFileName(fName);
		pac->AddToControlList(cs);
	}
}

// This Method addsIdentifier to the Locality Map.
void PackageMap::AddIdentifierToLocalityMap(std::string fname,IdentifierLocality* idLoc)
{
	std::string key = GetPackageName(fname);
	Package* pac = GetPackage(key);
	std::string idenName = idLoc->GetIdentifierName();
	std::multimap<std::string,IdentifierLocality*>::iterator iter = pac->GetLocalityMultiMap().begin();
	pac->AddToLocalityMap(idenName,idLoc);
}

// Updates the indentifier information.
void PackageMap::UpdateIdentifierInfo(std::string idenName,Toker* toker)
{
	int tempLineNum = 0, tokerLine = toker->lines()+1;
	IdentifierLocality* idenLocality = 0;
	std::string packageName = GetPackageName(toker->GetFileName());
	Package* pac = GetPackage(packageName);
	std::multimap<std::string,IdentifierLocality*> localityMultiMap = pac->GetLocalityMultiMap();
	std::multimap<std::string,IdentifierLocality*>::iterator lMultiMapIter;
	if(localityMultiMap.find(idenName)!= localityMultiMap.end())
	{
		for(lMultiMapIter=localityMultiMap.begin();lMultiMapIter!=localityMultiMap.end();++lMultiMapIter)
		{
			if(lMultiMapIter->second->GetIdentifierName()==idenName)
			{
				if(lMultiMapIter->second->GetLineDeclared()<tokerLine)
				{
					if(tempLineNum<lMultiMapIter->second->GetLineDeclared())
					{
						tempLineNum = lMultiMapIter->second->GetLineDeclared();
						idenLocality = lMultiMapIter->second;
					}
				}
			}
		}
		idenLocality->IncrementTimesUsed();
		idenLocality->SetLastusedAtLine(toker->lines());
	}
}

void PackageMap::DisplayDeclaredObjects()
{
	bool flag = true;
	std::map<std::string,Package*>::iterator iter;
	for(iter=packages.begin();iter!=packages.end();++iter)
	{
		std::multimap<std::string,IdentifierLocality*> localityMultiMap = iter->second->GetLocalityMultiMap();
		std::multimap<std::string,IdentifierLocality*>::iterator lMultiMapIter;
		for(lMultiMapIter=localityMultiMap.begin();lMultiMapIter!=localityMultiMap.end();++lMultiMapIter)
		{
			if(((lMultiMapIter->second->GetLastUsedAtLine()-lMultiMapIter->second->GetLineDeclared()) > 20) || (lMultiMapIter->second->GetTimesUsed()>10))
			{
				if(flag)
				{

					std::cout << "\nDisplaying Objects Declaration for Package::  " << iter->first << std::endl;
					std::cout << "=======================================================================================================================" << std::endl ;
					std::cout << std::setw(20) << "Identifier Name" << "	" << std::setw(30) << "Locality Of Reference" 
						<< "	" << std::setw(10) << "Times Used" << "	" 
						<< std::setw(15) << "Line Declared At"   << "	" << std::setw(15) << "Last Used At Line"<<std::endl; 
					std::cout << "=======================================================================================================================" << std::endl ;
					flag = false;
				}
				std::cout << std::setw(20) << lMultiMapIter->second->GetIdentifierName() << "	" << std::setw(30) << 
					(lMultiMapIter->second->GetLastUsedAtLine()-lMultiMapIter->second->GetLineDeclared()) 
					<< "	" << std::setw(10) << lMultiMapIter->second->GetTimesUsed()	<< 
					std::setw(15) << lMultiMapIter->second->GetLineDeclared()  << "	" << std::setw(15) << 
					 lMultiMapIter->second->GetLastUsedAtLine()<<std::endl; 
			}
		}
		if(!flag)
			std::cout<< ".......................................................................................................................\n";
		flag = true;

	}
}

//This method updates the Package Scope Depth.
void PackageMap::UpdatePackageScopeDepth(std::string pacName,int scpDepth)
{
	std::string packageName = GetPackageName(pacName);
	std::map<std::string,Package*>::iterator iter;
	if(packages.find(packageName) != packages.end())
		packages.find(packageName)->second->SetDeepestScope(scpDepth);
	else
	{
		Package* pac = new Package;
		if(pacName.find(".cpp")<pacName.length())
			pac->SetSourceFileName(pacName);
		else 
			pac->SetHeaderFileName(pacName);
		pac->SetDeepestScope(scpDepth);
		packages[packageName]=pac;
	}
}	

// Sets the start and the end line number.
void ControlSpan::SetLineNums(int start, int end)
{
	startLine = start;
	endLine = end;
}

ControlSpan::ControlSpan(){}
ControlSpan::~ControlSpan(){}

TypeTable::TypeTable(void){}

TypeTable::~TypeTable(void)
{
	std::map<std::string,TypeInfo*>::iterator itr;
	for(itr=typeTable.begin();itr!=typeTable.end();++itr)
		delete itr->second;
}

// Displays the Types Contained in the Type Table.
void TypeTable::Display()
{
	std::map<std::string,TypeInfo*>::iterator iter;
	for(iter= typeTable.begin(); iter!=typeTable.end();++iter)
	{
		std::cout << (*iter).first << " :: ";
		std::cout << (*iter).second->GetPackageName() << " " << (*iter).second->GetLineNumber() << std::endl << std::endl;
	}
}

#ifdef TYPEINFO_TEST

#include "TypeInfo.h"
#include <list>
#include <iterator>
#include <iomanip>

int main(int argc, char* argv[])
{
	std::cout << "Testing TypeInfo Package....\n\n\n";
	std::cout << "Populating TypeInfo Object... \n";
	TypeInfo* tInfo = new TypeInfo();
	tInfo->SetTypeName("TypeInfo");
	tInfo->SetPackageName("TypeInfo");
	tInfo->SetType("class");
	tInfo->SetLineNum(4);

	//Display the TypeInfo
	std::cout << "Displaying the TypeInfo Object Values " << std::endl;
	std::cout << "------------------------------------------\n";
	std::cout << std::setw(20) << "Type Name:: " << tInfo->GetTypeName() << std::endl;
	std::cout << std::setw(20) <<  "Type:: " << tInfo->GetType() <<   std::endl;
	std::cout << std::setw(20) << "Package Name:: " <<   tInfo->GetPackageName() <<  std::endl;
	std::cout << std::setw(20) << "Declared At Line:: " << tInfo->GetLineNumber() <<  std::endl;
	
	std::cout << "------------------------------------------\n";
	std::cout << "Inserting element to type table... " << std::endl;
	TypeTable::Add("Type",tInfo);
	std::cout << "Item Added to the type table.. " <<  std::endl;

	std::cout << "------------------------------------------\n";
	std::cout << "Testing Function Info.....\n";

	FunctionInfo* fi = new FunctionInfo() ;
	fi->SetFunctionName("bool doTest()");
	fi->SetPackageName("ActionsAndRulesDemo");
	fi->SetFileName("ActionsAndRulesDemo.cpp");
	fi->SetEndLineNumber(34);
	fi->SetStartLineNumber(12);
	fi->SetCyclomaticComplexity(6);

	std::cout << "Inserting element to Function List... " << std::endl;
	FunctionList flist;
	flist.AddToFunctionList(fi);
	std::cout << std::endl;
	flist.DisplayFunctions();
	std::cout << "------------------------------------------\n";

	std::cout << "Testing Package and Package Map .....\n";
	Package* pac = new Package();
	pac->SetHeaderFileName("Nav.h");
	pac->SetSourceFileName("Nav.cpp");
	pac->SetDeepestScope(8);
	std::cout << "Package Object being added to PackageMap..\n";
	PackageMap::AddToPackageCollection("Nav.cpp");
	PackageMap::AddToPackageCollection("Nav.h");
	PackageMap::GetPackagesColl()["Nav"]=pac;
	//PackageMap::DisplayPackages();
	std::cout << "-------------------------------------------\n";

	std::cout << "Testing Identifier Locality \n" ;
	IdentifierLocality* il = new IdentifierLocality();
	il->SetFileDeclared("Nav.cpp");
	il->SetLastusedAtLine(67);
	il->SetIdentiferName("fileInfo");
	il->SetLineDeclared(33);
	il->IncrementTimesUsed();
	
	std::cout << "Displaying Identifier Locality..\n" ;

	std::cout << " File Declared:: " << il->GetFileDeclared() << std::endl;
	std::cout << " Line Declared:: " << il->GetLineDeclared() << std::endl;
	std::cout << " Last Used at Line:: " << il->GetLastUsedAtLine() << std::endl;
	std::cout << " Identifer Name:: " << il->GetIdentifierName() << std::endl;
	std::cout << " Times Used:: " <<il->GetTimesUsed() << std::endl;
	std::cout << "------------------------------------------\n";

}

#endif