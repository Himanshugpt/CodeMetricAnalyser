#ifndef NAV_H
#define NAV_H
/////////////////////////////////////////////////////////////////
//  nav.h    --    recursively walk a directory tree           //
//  ver 1.8        starting at a specified path                //
//                                                             //
//  Language:      Visual C++, ver 7.1                         //
//  Platform:      Dell Precision T7400, Vista Ultimate, SP1   //
//  Application:   CSE775 Project #1, Spring 2009              //
//  Author:        Jim Fawcett, Syracuse University CST 4-187  //
//                 (315) 443-3948, jfawcett@twcny.rr.com       //
/////////////////////////////////////////////////////////////////
/*
    class navig Operations:
    -----------------------
    An object of the navig class walks a directory (sub)tree,
    processing files and directories encountered along the walk.
    Root of the walk is passed to member function start.  The 
    root can be any valid path including "." and ".."

      userProc udp;      construct a user defined proc object
      navig nav(udp);    construct a navigator object
      nav.walk(root);   start a recursive walk at root

    When the walk is completed the starting directory will be
    restored.  Note that you will have to define the userProc
    class.  See the test stub in nav.cpp for an example of
    how to do that.
*/
///////////////////////////////////////////////////////////////
//                      maintenance page                     //
///////////////////////////////////////////////////////////////
//  Build Process                                            //
//                                                           //
//  Files Required:                                          //
//    nav.h, nav.cpp, fileInfo.h, fileInfo.cpp,              //
//    WinTools.h, WinTools.cpp,                              //
//    StringConversion.h, StringConversion.cpp               //
//                                                           //
//  Building with Visual C++ , ver 7.1, from command line:   //
//    cl /GX /DTEST_NAV nav.cpp fileInfo.cpp Wintools.cpp \  //
//       StringConversion.cpp                                //
//                                                           //
///////////////////////////////////////////////////////////////
/*
    Maintenance History
    ===================
    ver 1.8 : 18 Jan 2009
      - fixed bug in navig::walk(path,pattern)
      - cosmetic fixes to manual page
    ver 1.7 : 13 Jan 2009
      - added namespace Win32Tools
    ver 1.6 : 19 Jan 2006
      - added getPath()
    ver 1.5 : 14 Mar 2005
      - added using relationship arrow to diagram below.
    ver 1.4 : 17 Jan 2005
      - simplified processing in walk function
    ver 1.3 : 30 Jan 2000
      - incorporated use of fileInfo objects to simplify
        navig processing
      - changed default and user defined processing objects
        to eliminate requirement for them to be allocated
        on the heap, e.g., dynamically
    ver 1.2 : 27 Apr 1999
      - cosmetic changes to these pages
    ver 1.1 : 22 Apr 1999
      - cosmetic changes to walk and start functions
    ver 1.0 : 30 Jan 1998
      - first release


    Design Notes:
    =============
    The navig object owns a fileInfo object that provides all the
    intelligence about extracting file information from a directory.
    Navig's job is simply to do a depth-first traversal of the dir
    structure rooted at the path provided its start function.

                                         The navig object also contains
     +-------+             +---------+   a reference to a base class
     |       |-----------> | defProc |   defProc object that defines
     | navig |             +---------+   default processing of file
     |       |<>-+          |  |         and directory information.
     +-------+   |      +---+ / \        This reference is obtained
                 |      |      |         by passing one when the object
            +--------+  | +----------+   is constructed.
            |fileInfo|<-+ | userProc |
            +--------+    +----------+   The user may override that
                                         default processing by defining
                                         a derived userProc class with
                                         the desired processing and
                                         passing that instead.

    An example of how to override the default processing by defining
    a userProc class is presented in the test stub.  That should be
    enough to show you how to implement your own processing.

*/


///////////////////////////////////////////////////////////////
//                         Declarations                      //
///////////////////////////////////////////////////////////////


#include <string>
#include <windows.h>
#include <map>
#include "StringConversion.h"

///////////////////////////////////////////////////////////////
// default file and directory processing
//
namespace Win32Tools
{

 class defProc {

  public:
    virtual ~defProc() { }
    virtual void dirsProc(const stdStr& dir);
    virtual void fileProc(const fileInfo& fi);
  };

  ///////////////////////////////////////////////////////////////
  // directory navigator

  class navig {

  public:

    navig(defProc &dp);                 // accept user defined proc
    ~navig();                           // restore user's dir
    std::string getPath();              // get current path
    void walk(const stdStr &dir, const stdStr& fileMask, bool rec);
                                        // start dir navigation
	

  private:
    static const int PathBufferSize = 256;
    stdStr userDir_;                     // user's working directory
    defProc &dp_;                        // provides extendable processing
                                         //   of file and directory names
	
  };

  //----< get current path >-------------------------------------

  std::string navig::getPath() { return Win32Tools::fileInfo::getPath(); }

  ///////////////////////////////////////////////////////////////
  // Note:
  //   The fact that navig holds a reference member implies that copy
  //   operations will fail to compile.
  //
}
#endif
