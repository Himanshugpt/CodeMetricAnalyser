///////////////////////////////////////////////////////////////////////
//  StringConversion.h  -  Win32 API-based helper functions          //
//  ver 1.0                                                          //
//  Language:      Visual C++, ver 8.0                               //
//  Platform:      Dell Precision T7400, Vista Ultimate, SP1         //
//  Application:   CSE775 Project #1, Spring 2009                    //
//  Author:        Jim Fawcett, Syracuse University CST 4-187,       //
//                 (315) 443-3948, jfawcett@twcny.rr.com             //
///////////////////////////////////////////////////////////////////////

#include "StringConversion.h"
using namespace Win32Tools;

//----< convert std::string to std::wstring >----------------------------

std::wstring Convert::ToWstring(const std::string& src)
{
  std::wstring dest;
  for(size_t i=0; i<src.length(); ++i)
    dest += static_cast<wchar_t>(src[i]);
  return dest;
}
//----< convert std::wstring to std::wstring >---------------------------

std::wstring Convert::ToWstring(const std::wstring& src)
{
  return src;
}
//----< convert std::wstring to std::string >----------------------------

std::string Convert::ToString(const std::wstring& src)
{
  std::string dest;
  for(size_t i=0; i<src.length(); ++i)
    dest += static_cast<char>(src[i]);
  return dest;
}
//----< convert std::string to std::string >-----------------------------

std::string Convert::ToString(const std::string& src)
{
  return src;
}
//----< convert std::string to stdStr >--------------------------------

stdStr Convert::ToStdStr(const std::string& src)
{
#ifdef UNICODE
  std::wstring dest;
  for(size_t i=0; i<src.length(); ++i)
    dest += static_cast<wchar_t>(src[i]);
  return dest;
#else
  return src;
#endif
}
 //----< convert std::wstring to stdStr >--------------------------------

stdStr Convert::ToStdStr(const std::wstring& src)
{
#ifdef UNICODE
  return src;
#else
  std::string dest;
  for(size_t i=0; i<src.length(); ++i)
    dest += static_cast<char>(src[i]);
  return dest;
#endif
}

//----< test stub >------------------------------------------------------

#ifdef TEST_STRINGCONVERSION

void main()
{
  std::cout << "\n  testing string conversions ";
  std::cout << "\n ============================\n";

  std::string asciiTestString = "an ascii test string";
  std::wstring unicodeTestString = L"a unicode test string";

  // test converting either ascii or unicode to std::wstring
  std::wstring wtest = Convert::ToWstring(asciiTestString);
  std::wcout << L"\n  wstring converted from string: " << wtest;
  wtest = Convert::ToWstring(unicodeTestString);
  std::wcout << L"\n  wstring converted from wstring: " << wtest << std::endl;

  // test converting either ascii or unicode to std::string
  std::string atest = Convert::ToString(asciiTestString);
  std::cout << "\n  string converted from string: " << atest;
  atest = Convert::ToString(unicodeTestString);
  std::cout << "\n  string converted from wstring: " << atest << std::endl;

  // test converting either ascii or unicode to stdStr
  stdStr stest = Convert::ToStdStr(asciiTestString);
  stdOut << TEXT("\n  stdStr converted from string: ") << stest;
  stest = Convert::ToStdStr(unicodeTestString);
  stdOut << TEXT("\n  stdStr converted from wstring: ") << stest << TEXT("\n");
  std::cout << "\n\n";

}
#endif
