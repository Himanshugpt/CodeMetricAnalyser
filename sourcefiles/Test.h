#include <queue>
#include <string>
#include <stack>
#include "Parser.h"
#include "SemiExpression.h"s
#include "Tokenizer.h"
#include "ITokCollection.h"
#include "ScopeStack.h"
#include <set>

////////////////////////////////
class Test
{

public:
    enum mode { code, xml, custom };
   bool parse();
  bool parse3();
   bool next();
   bool parseBeta();
   Toker* pToker;

};


bool Test :: parse(){
	if(true){
		
		Test* t;
		Test& t6=t;
		t6=t;
		t->next();
		t->next();
		t->next();
		unsigned int i=0;
		//ITokCollection* pTc;
		//ITokCollection& tc = *pTc;
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
}

class Test1
{
	bool parseHimahu()
	{
		for(int i=0;i<89;i++)
		{

			if(true)
				if(true)
				if(true)



			Test* t;
		Test& t6=t;
		t->next();
		t->next();
		t->next();
		t->next();
		t->next();
		t->next();
		t->next();
		t->next();
		t->next();
		t->next();


		{{{{{{{{{{int i = 0;}}}}}}}}}}






		}

		if(0)
		{





















		}
	}

};

enum OK {};

enum PRASAD
{

};

struct Person{

std::string name;

};

struct PersonMadeCarWithoutWheels : Person{

std::string nameLOp;

};

class GetFunSizes : public IAction
{
	public:
		GetFunSizes(SemiExp* semi, Toker* toker);	 
		void doAction(ITokCollection* pTc);
	private:
		std::stack<std::string> scopeStk;
		SemiExp* pSemi ;
		Toker* pToker ;
		int cyclyComplexity;
};


class PreprocStatement : public IRule{
public:
	// void doAction ( int pTc ) ;
  bool doTest(ITokCollection* pTc)
  {
    if(pTc->find("#") < pTc->length())
    {
		item.first = tc[tc.length()-3] ;
      //doActions(pTc);
      //return true;
    }
    //return false;
  }
};
