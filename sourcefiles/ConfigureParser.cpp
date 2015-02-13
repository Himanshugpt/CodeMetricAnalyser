/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 1.1                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      Dell Dimension 9150, Windows XP SP2             //
//  Application:   Prototype for CSE687 Pr1, Sp06                  //
//  Author:        Jim Fawcett, CST 2-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

//----< constructor initializes pointers to all parts >--------------

ConfigParseToConsole::ConfigParseToConsole() 
  : pToker(0), pSemi(0), pParser(0), pPreprocStatement(0), pPrintPreproc(0), 
  pFunctionDefinition(0), pPrintFunction(0), pPrettyPrintFunction(0), pGetFunSize(0) {}

//----< destructor releases all parts >------------------------------

ConfigParseToConsole::~ConfigParseToConsole()
{
  // when Builder goes out of scope, everything must be deallocated
  delete pPrettyPrintFunction;
  delete pPrintFunction;
  delete pFunctionDefinition;
  delete pPrintPreproc;
  delete pPreprocStatement;
  delete pParser;
  delete pSemi;
  delete pToker;
  delete pGetFunSize;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}
//
//----< Here's where alll the parts get assembled >----------------

Parser* ConfigParseToConsole::Build()
{
  try
  {
    // configure to detect and act on preprocessor statements
    pToker = new Toker;
    pSemi = new SemiExp(pToker);
    pParser = new Parser(pSemi);

    pPreprocStatement = new PreprocStatement;
    pPrintPreproc = new PrintPreproc;
    pPreprocStatement->addAction(pPrintPreproc);
    pParser->addRule(pPreprocStatement);

    // configure to detect and act on function definitions
	pGetFunSize = new GetFunSize(pSemi,pToker);//action
    pFunctionDefinition = new FunctionDefinition;
	pFunctionDefinition->addAction(pGetFunSize);
    pPrintFunction = new PrintFunction;
    pFunctionDefinition->addAction(pPrintFunction);
    pPrettyPrintFunction = new PrettyPrintFunction;
    pFunctionDefinition->addAction(pPrettyPrintFunction);
    pParser->addRule(pFunctionDefinition);
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}
//
//----< constructor initializes pointers to all parts >--------------

ConfigParseToQueue::ConfigParseToQueue() 
  : pToker(0), pSemi(0), pParser(0), pPreprocStatement(0), pPreprocToQ(0), 
    pFunctionDefinition(0), pPrettyPrintToQ(0) {}

//----< destructor releases all parts >------------------------------

ConfigParseToQueue::~ConfigParseToQueue()
{
  // when Builder goes out of scope, everything must be deallocated

  delete pPrettyPrintToQ;
  delete pFunctionDefinition;
  delete pPreprocToQ;
  delete pPreprocStatement;
  delete pQueue;
  delete pParser;
  delete pSemi;
  delete pToker;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToQueue::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}
//----< Here's where alll the parts get assembled >----------------

Parser* ConfigParseToQueue::Build()
{
  try
  {
    // configure to detect and act on preprocessor statements
    pToker = new Toker;
    pSemi = new SemiExp(pToker);
    pParser = new Parser(pSemi);
    pQueue = new std::queue<std::string>;

    pPreprocStatement = new PreprocStatement;
    pPreprocToQ = new PreprocToQ(*pQueue);
    pPreprocStatement->addAction(pPreprocToQ);
    pParser->addRule(pPreprocStatement);

    // configure to detect and act on function definitions
    pFunctionDefinition = new FunctionDefinition;
    pFunctionDefinition->addAction(pPrettyPrintToQ);
    pParser->addRule(pFunctionDefinition);
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

ConfigParserForFunction::ConfigParserForFunction():pParser(0), pToker(0), pSemi(0), pFunctionDefinition(0), 
	 pGetFunSize(0){}

//----< attach toker to a file stream or stringstream >------------
bool ConfigParserForFunction::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}

Parser* ConfigParserForFunction::Build()
{
	try
  {
    // configure to detect and act on preprocessor statements
    pToker = new Toker;
    pSemi = new SemiExp(pToker);
    pParser = new Parser(pSemi);
    
	//rules
    pFunctionDefinition = new FunctionDefinition;
	//actions
	pGetFunSize = new GetFunSize(pSemi,pToker);
	//adding rules to action
	pFunctionDefinition->addAction(pGetFunSize);

	pParser->addRule(pFunctionDefinition);
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}


ConfigParserForFunction::~ConfigParserForFunction()
{
  // when Builder goes out of scope, everything must be deallocated
  delete pFunctionDefinition;
  delete pGetFunSize;
  delete pParser;
  delete pSemi;
  delete pToker;
}

ControlSpanParser :: ControlSpanParser():pParser(0),
	pToker(0),pSemi(0),pDetectControlStatement(0),
	pDetectIdentifier(0),pDetectClass(0),
	pInsertToTypeTable(0){}

ControlSpanParser ::~ControlSpanParser(){
	delete pDetectControlStatement;
	delete pDetectIdentifier;
	delete pDetectClass;
	delete pInsertToTypeTable;
}
//----< attach toker to a file stream or stringstream >------------

bool ControlSpanParser::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}

Parser* ControlSpanParser::Build()
{
	try
  {
    // configure to detect and act on preprocessor statements
    pToker = new Toker;
    pSemi = new SemiExp(pToker);
	pSemi->makeCommentSemiExp();
    pParser = new Parser(pSemi);
	//rules
	pDetectControlStatement = new DetectControlStatement;
	pDetectIdentifier = new DetectIdentifier;
	pDetectClass = new DetectClass;
	//actions
	pFindControlBreadth = new FindControlBreadth(pToker);
	pInsertToTypeTable = new InsertToTypeTable(pToker);
	pDetectControlStatement->addAction(pFindControlBreadth);
	pParser->addRule(pDetectControlStatement);
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}
PackageControlDepthParser::PackageControlDepthParser():pParser(0),
	pToker(0),pSemi(0),pDetectControlStatement(0),pFindControlBreadth(0)
{}

bool PackageControlDepthParser::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}

Parser* PackageControlDepthParser::Build()
{
	try
	{
		// configure to detect and act on preprocessor statements
		pToker = new Toker;
		pSemi = new SemiExp(pToker);
		pSemi->makeCommentSemiExp();
		pParser = new Parser(pSemi);
		//rules
		pDetectControlStatement = new DetectControlStatement();
		//actions
		pFindControlBreadth = new FindControlBreadth(pToker);
		// add actions to rule
		pDetectControlStatement->addAction(pFindControlBreadth);
		pParser->addRule(pDetectControlStatement);
		return pParser;
	}
	catch(std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}

PackageControlDepthParser::~PackageControlDepthParser()
{
	delete pToker;
	delete pSemi;
	delete pParser;
	delete pDetectControlStatement;
	delete pFindControlBreadth;
}


ParserForTypeTable::ParserForTypeTable():pParser(0),pToker(0),pSemi(0),pDetectClass(0),pInsertToTypeTable(0)
,pDetectIdentifier(0),pFunctionDeclaration(0),pUpdatePackageIdentifiers(0){}

ParserForTypeTable::~ParserForTypeTable()
{
	delete pDetectClass;
	delete pInsertToTypeTable;
	delete pDetectIdentifier;
	delete pFunctionDeclaration;
	delete pUpdatePackageIdentifiers;
	delete pBraceDepth;
	delete pDetermineScopeDepth;
}

Parser* ParserForTypeTable::Build()
{
	try
  {
    // configure to detect and act on preprocessor statements
    pToker = new Toker;
    pSemi = new SemiExp(pToker);
    pParser = new Parser(pSemi);
	//rules
	pDetectClass = new DetectClass();
	pFunctionDeclaration = new FunctionDeclaration();

	pBraceDepth = new BraceDepth();
	pDetermineScopeDepth = new DetermineScopeDepth(pToker);
	pBraceDepth->addAction(pDetermineScopeDepth);

	//actions
	pInsertToTypeTable = new InsertToTypeTable(pToker);
	// add actions to rule
	pDetectClass->addAction(pInsertToTypeTable);
	pParser->addRule(pBraceDepth);
	pParser->addRule(pDetectClass);
	pParser->addRule(pFunctionDeclaration);
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

bool ParserForTypeTable::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}


ParserForPackageMetrics::ParserForPackageMetrics():pParser(0),pToker(0),pSemi(0),pDetermineDataLocality(0){}

ParserForPackageMetrics::~ParserForPackageMetrics()
{
	delete pDetermineDataLocality;
}

Parser* ParserForPackageMetrics::Build()
{
	try
  {
    // configure to detect and act on preprocessor statements
    pToker = new Toker;
    pSemi = new SemiExp(pToker);
    pParser = new Parser(pSemi);
	//rules
	pDetermineDataLocality = new DetermineDataLocality(pToker);
	pParser->addRule(pDetermineDataLocality);
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

bool ParserForPackageMetrics::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}

CollectIdentiferParser::CollectIdentiferParser(){}
CollectIdentiferParser::~CollectIdentiferParser()
{
	delete pDetectIdentifier;
	delete pUpdatePackageIdentifiers;
}

bool CollectIdentiferParser::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  return pToker->attach(name, isFile);
}

Parser* CollectIdentiferParser::Build()
{
	try
  {
    // configure to detect and act on preprocessor statements
    pToker = new Toker;
    pSemi = new SemiExp(pToker);
    pParser = new Parser(pSemi);
	//rules
	pDetectIdentifier = new DetectIdentifier();
	pUpdatePackageIdentifiers = new UpdatePackageIdentifiers(pToker);
	pDetectIdentifier->addAction(pUpdatePackageIdentifiers);
	pParser->addRule(pDetectIdentifier);
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}


//
#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ConfigureParser module\n "
            << std::string(32,'=') << std::endl;

  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

    ConfigParseToConsole configure;
    Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      // now that parser is built, use it

      while(pParser->next())
        pParser->parse();
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }

//
    std::cout << "\n  Processing file for Queued Outputs " << argv[i];
    std::cout << "\n  " << std::string(35 + strlen(argv[i]),'-');

    ConfigParseToQueue Qconfigure;
    pParser = Qconfigure.Build();
    try
    {
      if(pParser)
      {
        if(!Qconfigure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      // now that parser is built, use it

      while(pParser->next())
        pParser->parse();
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
    std::queue<std::string>* pQueue = Qconfigure.GetQueue();
    size_t len = pQueue->size();
    for(size_t i=0; i<len; ++i)
    {
      std::cout << "\n  " << pQueue->front();
      pQueue->pop();
    }
    std::cout << "\n\n";
  }
}

#endif
