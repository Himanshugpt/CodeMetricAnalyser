/////////////////////////////////////////////////////////////////
//  nav.cpp - recursively walk a directory tree starting at a  //
//  ver 1.6   specified path                                   //
//                                                             //
//  Language:      Visual C++, ver 8.0                         //
//  Platform:      Dell Precision T7400, Vista Ultimate, SP1   //
//  Application:   CSE775 Project #1, Spring 2009              //
//  Author:        Jim Fawcett, Syracuse University CST 4-187  //
//                 (315) 443-3948, jfawcett@twcny.rr.com       //
/////////////////////////////////////////////////////////////////

#include <iostream>     // standard output for test
#include <iomanip>      // output formatting
#include <vector>
#include <iterator>
#include <set>
#include <map>
#include "fileInfo.h"
#include "nav.h"
#include "wintools.h"
#include "conio.h"
#include "CodeAnalyzer.h"
#include  "TypeInfo.h"

using namespace std;
using namespace Win32Tools;

//----< default dir name display function >--------------------

void defProc::dirsProc(const stdStr& dir) 
{
  stdOut << TEXT("\n  ") << dir.c_str() << endl;
}

//----< default file data display function >-------------------

void defProc::
fileProc(const fileInfo& fi) 
{
  if(!fi.isDirectory())
    fi.showData(stdOut); 
}

//----< save user's working directory >------------------------

navig::navig(defProc &DP) : dp_(DP) 
{
// save user's working directory
  TCHAR buffer[PathBufferSize];
  GetCurrentDirectory( PathBufferSize, buffer);
  userDir_ = buffer;
}

//----< restore user's working directory >---------------------

navig::~navig() { SetCurrentDirectory(userDir_.c_str()); }

//----< walk directory tree rooted at dir >--------------------

void navig::walk(const stdStr& dir, const stdStr& fileMask, bool rec) {

  stdStr path = Path::getFullPath(dir);
  //std::cout<< "PATH IN NAV>CPP " << path << std::endl;
  dp_.dirsProc(path);
  std::vector<stdStr> dirs;
  std::vector<fileInfo> fis;
  fis = Directory::GetFileInfos(path,fileMask);
  for(size_t i=0; i<fis.size(); ++i)
  {
	fis[i].setPath(path);
    dp_.fileProc(fis[i]);
  }

  if(rec)
	{
	  dirs = Directory::GetDirectories(path,TEXT("*.*"));
		for(size_t i=0; i<dirs.size(); ++i)
			walk(dirs[i],fileMask,true);
	}
}

//----< test stub >--------------------------------------------
//
//  - Recursively walk directory subtree pointed to
//    by the last command line argument.
//  - use default navig processing each time a directory
//    or file is encountered
//  - replace default processing with user defined processing
//    and repeat
//
#ifdef TEST_NAV

//----< wait for key press: requires conio >-------------------


void main(int argc, char *argv[]) {
  
	cout << "\n =======================================================================================\n";
	 cout << "				SOURCE CODE ANALYZER "
       << "\n =======================================================================================\n";

  set<std::string> patterns, paths;
  bool rec = false;

  if(argc == 1) {
    cout << "\n  please enter starting path\n";
    return;
  }

  for(int i=2;i<argc;i++)
  {	
	std::string str= argv[i];
	if(str.find("*")<str.length() || str.find(".cpp")<str.length() || str.find(".h")<str.length()){
		patterns.insert(str);
	}else
	{
		paths.insert(str);
	}
  }

  if(argv[0]=="rec")
	  rec=true;

  try {
// create default processing object and start navigation
// User defined processing definded here.  Note that
// local classes can be defined as long as all members
// are inline.  Otherwise, just declare and define above
// main.
//
// These do nothing but prepend each output with some
// "-" characters to show that user defined processing
// is actually being used.  In a real application user 
// defined processing will certainly be quite different
// from the default
//
  class userProc : public defProc {

  public:
    virtual void fileProc(const fileInfo &fi) {
      fileInfo newfi = fi;
	  std::string fullFname  = newfi.getPath() + "\\" + newfi.name();
	  PackageMap::AddToPackageCollection(fullFname);
    }
    virtual void dirsProc(const stdStr &dir) {
     // stdOut << TEXT("\n  ") << dir.c_str() << endl;
    }
  };

  userProc udp;
  navig newNav(udp);
  
   set<std::string>::const_iterator it;
   set<std::string>::const_iterator it2;

    for(it2 = paths.begin(); it2 != paths.end(); it2++)
	{
		 for(it = patterns.begin(); it != patterns.end(); it++)
		 {
		  newNav.walk(Convert::ToStdStr(*it2),*it,rec);
		 }
	}
  CodeAnalyzer codeAnal;
  
  if(PackageMap::GetPackagesColl().size()>0)
  codeAnal.AnalyzeSourceCode(PackageMap::GetPackagesColl());
  else
	  std::cout << "No Files Found for Analysis.... Please provide some different path..\n";
   }
  catch(std::exception& ex)
  {
    cout << ex.what();
  }
}

#endif
