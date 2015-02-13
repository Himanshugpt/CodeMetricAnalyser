#ifndef STRINGCONVERSION_H
#define STRINGCONVERSION_H
///////////////////////////////////////////////////////////////////////
//  StringConversion.h  -  Win32 API-based helper functions          //
//  ver 1.0                                                          //
//  Language:      Visual C++, ver 8.0                               //
//  Platform:      Dell Precision T7400, Vista Ultimate, SP1         //
//  Application:   CSE775 Project #1, Spring 2009                    //
//  Author:        Jim Fawcett, Syracuse University CST 4-187,       //
//                 (315) 443-3948, jfawcett@twcny.rr.com             //
///////////////////////////////////////////////////////////////////////
/*
    Module StringConversion Operations:
    -----------------------------------
    This module defines:
      typedefs: stdStr, stdChar, stdOstream, stdOstringstream
      macros:   stdOut, compare, AM, PM, and a few message strings

    This module also defines the class Convert with static members:
      std::wstring ToWstring(const std::string& src);
      std::wstring ToWstring(const std::wstring& src);
      std::string ToString(const std::string& src);
      std::string ToString(const std::wstring& src);
      stdStr ToStdStr(const std::string& src);
      stdStr ToStdStr(const std::wstring& src);

    The idea is to accept either ascii or unicode strings and convert
    them to one of three specific types: std::string, std::wstring, or
    stdStr.
*/
///////////////////////////////////////////////////////////////
//                      maintenance page                     //
///////////////////////////////////////////////////////////////
//  Build Process                                            //
//                                                           //
//  Files Required:                                          //
//    StringConversion.h, StringConversion.cpp               //
//                                                           //
//  Building with Visual C++ , 8.0, from command line:       //
//    cl -EHsc -DTEST_STRINGCONVERSION StringConversion.cpp  //
//                                                           //
///////////////////////////////////////////////////////////////
/*
    Maintenance History
    ===================
    ver 1.0 : 13 Jan 09
    - first release
*/

#include <string>
#include <iostream>
#include <sstream>
#include <Windows.h>

#ifdef UNICODE
typedef std::wstring stdStr;
typedef wchar_t stdChar;
typedef std::wostream stdOstream;
typedef std::wostringstream stdOstringstream;
#define stdOut std::wcout
#define compare wcscmp
#define AM L"am"
#define PM L"pm"
#define SP L" "
#define SP2 L"  "
#define MSG1 L" - path not found"
#else
typedef std::string stdStr;
typedef char stdChar;
typedef std::ostream stdOstream;
typedef std::ostringstream stdOstringstream;
#define stdOut std::cout
#define compare strcmp
#define AM "am"
#define PM "pm"
#define SP " "
#define SP2 "  "
#define MSG1 " - path not found"
#endif

///////////////////////////////////////////////////////////////
// class Convert - convert between basic types

namespace Win32Tools
{
  class Convert
  {
  public:
    static std::wstring ToWstring(const std::string& src);
    static std::wstring ToWstring(const std::wstring& src);
    static std::string ToString(const std::string& src);
    static std::string ToString(const std::wstring& src);
    static stdStr ToStdStr(const std::string& src);
    static stdStr ToStdStr(const std::wstring& src);
  };
}
#endif
