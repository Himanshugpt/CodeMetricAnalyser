#include "CodeAnalyzer.h"
#include "Parser.h"
#include "ConfigureParser.h"
#include <iostream>
#include "TypeInfo.h"
#include "Tokenizer.h"


CodeAnalyzer::CodeAnalyzer(void){}
CodeAnalyzer::~CodeAnalyzer(void){}

// Used to Analyze Source Code.
void CodeAnalyzer::AnalyzeSourceCode(std::map<std::string,Package*> packagesMap)
{
	std::cout << "\n\n*********** Please wait... Application is Scanning the Source Files for Analysis *********** \n";
	FindFunctionMetrics(packagesMap);
	BuildTypeTable(packagesMap);
	FindPackageControlDepth(packagesMap);
	CollectIdentifiers(packagesMap);
	FindPackageRefDenMetrics(packagesMap);
	DisplayInformation();
	DeleteDataCollections();
}

//Displays the information collected by parsing the files.
void CodeAnalyzer::DisplayInformation()
{
	FunctionList f;	
	f.DisplayFunctions();
	PackageMap::DisplayControlBreadth();
	PackageMap::DisplayInnerScopePackages();
	PackageMap::DisplayDeclaredObjects();
}

// Calls the destructors
void CodeAnalyzer::DeleteDataCollections()
{
	std::cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	std::cout << "Deleting Data Collections... \n";
	delete (new PackageMap); //calls the package map delete which delets the data sets.
	delete (new TypeTable);
	std::cout << "Collections Deleted...\n";
	std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}

//Parses the set of files 
void CodeAnalyzer::ParseFiles(IBuilder* pBuilderConfig,std::map<std::string,Package*> packagesMap)
{
	std::map<std::string,Package*>::iterator iter;
	for(iter=packagesMap.begin();iter!=packagesMap.end();++iter)
	{
		std::string fileName = iter->second->GetSourceFileName();
		if(fileName!="")
		{
			Parser* pParser = pBuilderConfig->Build();
			pBuilderConfig->Attach(fileName);
			while(pParser->next())
				pParser->parse();
		}
		fileName = iter->second->GetHeaderFileName();
		if(fileName!="")
		{
			Parser* pParser = pBuilderConfig->Build();
			pBuilderConfig->Attach(fileName);
			while(pParser->next())
				pParser->parse();
		}
	}
	delete pBuilderConfig;
}

// calculates fun size and cyclomatic complexity
void CodeAnalyzer::FindFunctionMetrics(std::map<std::string,Package*> packagesMap)
{
	ConfigParserForFunction* configFunction = new ConfigParserForFunction();
	ParseFiles((IBuilder*)configFunction,packagesMap);
}

//Collects the identifiers
void CodeAnalyzer::CollectIdentifiers(std::map<std::string,Package*> packagesMap)
{
	CollectIdentiferParser* idenParser = new CollectIdentiferParser();
	ParseFiles((IBuilder*)idenParser,packagesMap);
}

// Find the refrence counts
void CodeAnalyzer::FindPackageRefDenMetrics(std::map<std::string,Package*> packagesMap)
{
	ParserForPackageMetrics* parserConfig = new ParserForPackageMetrics();
	ParseFiles((IBuilder*)parserConfig,packagesMap);
}

//Builds Type Table by collecting the types
//and inserting them in the TypeTable
void CodeAnalyzer::BuildTypeTable(std::map<std::string,Package*> packagesMap)
{
	ParserForTypeTable* typeConfigParser = new ParserForTypeTable();
	ParseFiles((IBuilder*)typeConfigParser,packagesMap);
}

//Finds the control depth of packages.
void CodeAnalyzer::FindPackageControlDepth(std::map<std::string,Package*> packagesMap)
{
	PackageControlDepthParser* controlDepthParser = new PackageControlDepthParser();
	ParseFiles((IBuilder*)controlDepthParser,packagesMap);
}


#ifdef CODEANALYZER_TEST

#include "CodeAnalyzer.h"
#include "Parser.h"
#include "ConfigureParser.h"
#include <iostream>
#include "TypeInfo.h"
#include "Tokenizer.h"

int main(int argc, char* argv[])
{
	std::map<std::string,Package*> packages;
	Package* p = new Package();
	p->SetHeaderFileName("../CodeAnalyzer.h");
	p->SetSourceFileName("../CodeAnalyzer.cpp");
	packages["CodeAnalyzer"]=p;
	CodeAnalyzer codeAnal;
	codeAnal.AnalyzeSourceCode(packages);
}

#endif