/////////////////////////////////////////////////////////////////////////
//  fileInfo.cpp  -  manage file information extracted from Win32 API  //
//  ver 3.1                                                            //
//  Language:      Visual C++, ver 7.1                                 //
//  Platform:      Dell Precision T7400, Vista Ultimate, SP1           //
//  Application:   CSE775 Project #1, Spring 2009                      //
//  Author:        Jim Fawcett, Syracuse University CST 4-187,         //
//                 (315) 443-3948, jfawcett@twcny.rr.com               //
/////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4786)
#include <iostream>
#include <iomanip>
#include <set>
#include "fileInfo.h"

using namespace Win32Tools;

//----< convert to ASCII >-------------------------------------

std::string ConvToASCII(const stdStr& s)
{
#ifdef UNICODE
  std::string temp;
  for(size_t i=0; i<s.size(); ++i)
  {
    char ch = static_cast<char>(s[i]);
    temp += ch;
  }
  return temp;
#else
  return s;
#endif
}
//----< convert to std::wstring >------------------------------

std::wstring ConvToWide(const stdStr& s)
{
#ifndef UNICODE
  std::wstring temp;
  for(size_t i=0; i<s.size(); ++i)
    temp += static_cast<wchar_t>(s[i]);
  return temp;
#else
  return s;
#endif
}
//----< void constructor >-------------------------------------

fileInfo::fileInfo()
        : showSeconds_(false), showTime_(true), showDate_(true),
          showSize_(true), showAttrib_(true)
{ 
  _CurrPath = _OrigPath = getPath();
}
//----< constructor taking path >------------------------------

fileInfo::fileInfo(const stdStr &path) 
        : showSeconds_(false), showTime_(true), showDate_(true),
          showSize_(true), showAttrib_(true)
{
  _OrigPath = getPath();
  setPath(path);
}
//----< copy constructor >-------------------------------------

fileInfo::fileInfo(const fileInfo &fi) 
: showSeconds_(fi.showSeconds_), showTime_(fi.showTime_),
showDate_(fi.showDate_), showSize_(fi.showSize_), showAttrib_(fi.showAttrib_)
{
  data.dwFileAttributes    = fi.data.dwFileAttributes;
  data.ftCreationTime      = fi.data.ftCreationTime;  
  data.ftLastAccessTime    = fi.data.ftLastAccessTime;
  data.ftLastWriteTime     = fi.data.ftLastWriteTime; 
  data.nFileSizeHigh       = fi.data.nFileSizeHigh;
  data.nFileSizeLow        = fi.data.nFileSizeLow; 
  data.dwReserved0         = fi.data.dwReserved0;
  data.dwReserved1         = fi.data.dwReserved1;
  int i;
  for(i=0; i<MAX_PATH; i++) 
  {
    data.cFileName[i]      = fi.data.cFileName[i];
    if(fi.data.cFileName[i] == '\0')
      break;
  }
  for(i=0; i<14; i++)
    data.cAlternateFileName[i] = fi.data.cAlternateFileName[i];

  _OrigPath = fi._OrigPath;
  _CurrPath = fi._CurrPath;
  showSeconds_ = fi.showSeconds_;
}
//----< destructor >-------------------------------------------

fileInfo::~fileInfo() { setPath(_OrigPath); }

//----< assignment operator >----------------------------------

fileInfo& fileInfo::operator=(const fileInfo &fi) 
{
  if(this == &fi) return *this;
  data.dwFileAttributes    = fi.data.dwFileAttributes;
  data.ftCreationTime      = fi.data.ftCreationTime;  
  data.ftLastAccessTime    = fi.data.ftLastAccessTime;
  data.ftLastWriteTime     = fi.data.ftLastWriteTime; 
  data.nFileSizeHigh       = fi.data.nFileSizeHigh;
  data.nFileSizeLow        = fi.data.nFileSizeLow; 
  data.dwReserved0         = fi.data.dwReserved0;
  data.dwReserved1         = fi.data.dwReserved1;
  showSize_ = fi.showSize_;
  showSeconds_ = fi.showSeconds_;
  showTime_ = fi.showTime_;
  showDate_ = fi.showDate_;
  showAttrib_ = fi.showAttrib_;
  int i;
  for(i=0; i<MAX_PATH; i++)
    data.cFileName[i]      = fi.data.cFileName[i];
  for(i=0; i<14; i++)
    data.cAlternateFileName[i] = fi.data.cAlternateFileName[i];
  _OrigPath = fi._OrigPath;
  _CurrPath = fi._CurrPath;

  return *this;
}
//----< get file size >----------------------------------------

unsigned long int fileInfo::size() const 
{
  DWORDLONG myDWL = (static_cast<DWORDLONG>(data.nFileSizeHigh) << 32);
  myDWL += (data.nFileSizeLow & 0xFFFFFFFF);
  return static_cast<unsigned long int>(myDWL);
}
//----< is my size smaller? >----------------------------------

bool fileInfo::smaller(const fileInfo &fi) const 
{  
  DWORDLONG myDWL = (static_cast<DWORDLONG>(data.nFileSizeHigh) << 32);
  myDWL += data.nFileSizeLow & 0xFFFFFFFF;
  DWORDLONG fiDWL = (static_cast<DWORDLONG>(fi.data.nFileSizeHigh) << 32);
  fiDWL += fi.data.nFileSizeLow & 0xFFFFFFFF;
  return (myDWL < fiDWL);
}
//----< is my size larger? >-----------------------------------

bool fileInfo::larger(const fileInfo &fi) const 
{  
  DWORDLONG myDWL = (static_cast<DWORDLONG>(data.nFileSizeHigh) << 32);
  myDWL += data.nFileSizeLow & 0xFFFFFFFF;
  DWORDLONG fiDWL = (static_cast<DWORDLONG>(fi.data.nFileSizeHigh) << 32);
  fiDWL += fi.data.nFileSizeLow & 0xFFFFFFFF;
  return (myDWL > fiDWL);
}
//----< return FILETIME >--------------------------------------
//
//  typedef struct _FILETIME
//  { DWORD dwLowDateTime;  DWORD dwHighDateTime; } FILETIME;
//
FILETIME fileInfo::getFILETIME()
{
  FILETIME lft;
  FileTimeToLocalFileTime(&data.ftLastWriteTime,&lft);
  return lft;
}
//----< compare file times >-----------------------------------
//
//  returns 1 if my FILETIME is later than ft
//          0 if my FILETIME equals ft
//         -1 if my FILETIME is earlier than ft
//
int fileInfo::compareFileTime(FILETIME& ft)
{
  FILETIME myft = getFILETIME();
  return (int)::CompareFileTime(&myft,&ft);
}
//----< private date and time extraction >---------------------

SYSTEMTIME fileInfo::DateAndTime() const 
{
 SYSTEMTIME st;
 FILETIME  lft;
 FileTimeToLocalFileTime(&data.ftLastWriteTime,&lft);
 FileTimeToSystemTime(&lft,&st);
 return st;
}
//----< get file date string >---------------------------------

stdStr fileInfo::date() const 
{
  SYSTEMTIME st = DateAndTime();
  stdStr date;
  date.resize(0);
  date += '0' + (st.wMonth / 10);
  date += '0' + (st.wMonth % 10);
  date += '/';
  date += '0' + (st.wDay / 10);
  date += '0' + (st.wDay % 10);
  date += '/';
  int tmp = st.wYear;
  date += '0' + (tmp/1000);
  tmp  %= 1000;
  date += '0' + (tmp / 100);
  tmp %= 100;
  date += '0' + (tmp / 10);
  tmp %= 10;
  date += '0' + (tmp);
  return date;
}
//----< get file time string >---------------------------------

stdStr fileInfo::time() const 
{
  SYSTEMTIME st = DateAndTime();
  stdStr time;
  stdStr AMPM = AM;
  if(st.wHour > 12) 
  {
    st.wHour -= 12;
    AMPM = PM;
  }
  time.erase();
  time += '0' + (st.wHour   / 10);
  time += '0' + (st.wHour   % 10);
  time += ':';
  time += '0' + (st.wMinute / 10);
  time += '0' + (st.wMinute % 10);
  if(showSeconds_)
  {
    time += ':';
    time += '0' + (st.wSecond / 10);
    time += '0' + (st.wSecond % 10);
  }
  time += SP;
  time += AMPM;
  return time;
}

//----< make attributes string >-------------------------------

stdStr fileInfo::attributes() const 
{
  stdStr temp;
  if(isArchive()   ) temp += 'A';
  if(isCompressed()) temp += 'C';
  if(isDirectory() ) temp += 'D';
  if(isEncrypted() ) temp += 'E';
  if(isHidden()    ) temp += 'H';
  if(isOffLine()   ) temp += 'O';
  if(isReadOnly()  ) temp += 'R';
  if(isSystem()    ) temp += 'S';
  if(isTemporary() ) temp += 'T';
  return temp;
}
//----< pragma needed to disable performance warning >---------
//
//  for some reason casts don't inhibit warning as they should

#pragma warning(disable : 4800)

//----< is this file Archive? >--------------------------------

bool fileInfo::isArchive() const 
{ 
  return (data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE);
}
//----< is this file Compressed? >-----------------------------

bool fileInfo::isCompressed() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED); 
}
//----< is this file Directory? >------------------------------

bool fileInfo::isDirectory() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY); 
}
//----< is this file Encrypted? >------------------------------

bool fileInfo::isEncrypted() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED); 
}
//----< is this file Hidden? >---------------------------------

bool fileInfo::isHidden() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN); 
}
//----< is this file Normal? >---------------------------------

bool fileInfo::isNormal() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL); 
}
//----< is this file OffLine? >--------------------------------

bool fileInfo::isOffLine() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE); 
}
//----< is this file ReadOnly? >-------------------------------

bool fileInfo::isReadOnly() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY); 
}
//----< is this file System? >---------------------------------

bool fileInfo::isSystem() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM); 
}
//----< is this file Temporary? >------------------------------

bool fileInfo::isTemporary() const 
{   
  return (data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY); 
}
//----< display line of file data >----------------------------

void fileInfo::showData(stdOstream &out, stdChar ch, int width) const 
{
  long save = out.flags();
  out.setf(std::ios::right, std::ios::adjustfield);
  if(showSize_)
    out << std::setw(10) << size();
  if(showDate_)
    out << SP2 << date().c_str();
  if(showTime_ && showDate_)
    out << SP2 << time().c_str();
  if(showAttrib_)
    out << SP2 << attributes().c_str();
  out.setf(std::ios::left, std::ios::adjustfield);
  out << SP2 << std::setw(width) << name().c_str();
  out << ch;
  out.flush();
  out.flags(save);
}
//----< get current searchPath >-------------------------------
//
//  fill with current directory path if empty
//
stdStr fileInfo::getPath() 
{
  stdChar buffer[256];
  ::GetCurrentDirectory(256,buffer);
  return stdStr(buffer);
}
//----< set current directory >--------------------------------

void fileInfo::setPath(const stdStr& s) 
{
  if(::SetCurrentDirectory(s.c_str()) == 0)
  {
    stdStr temp = s + TEXT(" - path not found");
    throw std::invalid_argument(ConvToASCII(temp.c_str()));
  }
  _CurrPath = s;
}
//----< find first file >--------------------------------------

bool fileInfo::firstFile(stdStr path, const stdStr& filePattern) 
{
  closeFile();    // close any previous search
  if(path[path.size()-1] != '\\')
    path.append(TEXT("\\"));
  path.append(filePattern);
  _handle = ::FindFirstFile(path.c_str(),&data);
  return (_handle != INVALID_HANDLE_VALUE);
}
//----< find first file >--------------------------------------

bool fileInfo::firstFile(const stdStr &filePattern) 
{
  stdStr path = getPath();
  return firstFile(path,filePattern);
}
//----< find next file >---------------------------------------

bool fileInfo::nextFile() 
{
  if(_handle == INVALID_HANDLE_VALUE)
    return false;
  return (::FindNextFile(_handle,&data) == TRUE);
}
//----< close search for current file >------------------------

void fileInfo::closeFile() {  FindClose(_handle); }

//----< test stub >--------------------------------------------

#ifdef TEST_FILEINFO

///////////////////////////////////////////////////////////////
// The classes below are private - only used by this test
// stub, and so are placed here, and not in fileInfo.h
//
//----< function object which detects case insensitive order >-------

class lessNoCase
{
public:
  bool operator()(const fileInfo &fi1, const fileInfo &fi2) {
    stdStr name1 = fi1.name(), name2 = fi2.name();
    size_t leastSize = min(name1.size(), name2.size());
    for(size_t i=0; i<leastSize; ++i)
      if(tolower(name1[i]) > tolower(name2[i]))
        return false;
    return name1.size() < name2.size();
  }
};
//----< function object which detects date order >-------------

class earlier 
{
public:
  bool operator()(const fileInfo &fi1, const fileInfo &fi2) {
    return fi1.earlier(fi2);
  }
};
//----< function object which detects size order >-------------

class smaller 
{  
public:
  bool operator()(const fileInfo &fi1, const fileInfo &fi2) {
    return fi1.smaller(fi2);
  }
};

//----< test entry point >-------------------------------------

void main(int argc, char *argv[]) 
{
  std::cout << "\n  Testing Fileinfo objects "
            << "\n ==========================\n";

  // file names are unique, so use set

  typedef std::set< fileInfo, std::less<fileInfo>  > setNames;
  
  // lowercase file names, file dates, and file sizes
  // may not be unique, so use multiset

  typedef std::multiset< fileInfo, lessNoCase > setLCNames;
  typedef std::multiset< fileInfo, earlier > setDates;
  typedef std::multiset< fileInfo, smaller > setSizes;

  fileInfo f;
  f.showSeconds(true);
  try
  {
    if(argc > 1)
      f.setPath(TEXT(argv[1]));
  }
  catch(std::exception& e) 
  { 
    std::cout << "\n\n  " << e.what() << "\n\n";
    f.setPath(TEXT(".."));
  }
  std::cout << "\n  Files in C:\\Temp";
  if(f.firstFile(TEXT("C:\\Temp"),TEXT("*.*")))
  {
    if(!f.isDirectory())
      std::cout << "\n  " << f.name();
    while(f.nextFile())
      if(!f.isDirectory())
        std::cout << "\n  " << f.name();
  }
  std::cout << "\n";
  std::cout << "\n  path = " << ConvToASCII(f.getPath()) << std::endl;
  std::cout << "\n  Files in FindNextFile Order\n";
  setNames sn;
  setLCNames sl;
  setDates sd;
  setSizes ss;

  if(!f.firstFile(TEXT("*.*")))
    return;
  sn.insert(f);
  sl.insert(f);
  sd.insert(f);
  ss.insert(f);
  f.showData(std::cout);
  while(f.nextFile()) {
    sn.insert(f);
    sl.insert(f);
    sd.insert(f);
    ss.insert(f);
    f.showData(std::cout);
  }
  f.closeFile();

  std::cout << "\n  Files ordered alphabetically:\n";
  setNames::iterator fnIt;
  for(fnIt = sn.begin(); fnIt != sn.end(); fnIt++) {
    fnIt->showData(std::cout);
  }
  std::cout << "\n  Files with case insensitive alphabetic order:\n";
  setLCNames::iterator flIt;
  for(flIt = sl.begin(); flIt != sl.end(); flIt++) {
    flIt->showData(std::cout);
  }
  std::cout << "\n  Files ordered by date:\n";
  setDates::iterator fdIt;
  for(fdIt = sd.begin(); fdIt != sd.end(); fdIt++) {
    fdIt->showData(std::cout);
  }
  std::cout << "\n  Files ordered by size:\n";
  setSizes::iterator fsIt;
  for(fsIt = ss.begin(); fsIt != ss.end(); fsIt++) {
    fsIt->showData(std::cout);
  }
  std::cout << "\n\n";

  std::cout << "\n  Comparing file times:\n";
  setDates::iterator first = sd.begin();
  setDates::iterator second = first;
  ++second;
  fileInfo fiFirst = *first;
  fileInfo fiSecond = *second;
  std::cout << "\n  " << ConvToASCII(fiFirst.name()).c_str() << " has date: "
            << ConvToASCII(fiFirst.date()) << ", " 
            << ConvToASCII(fiFirst.time());
  std::cout << "\n  " << ConvToASCII(fiSecond.name()) << " has date: "
            << ConvToASCII(fiSecond.date()) << ", " 
            << ConvToASCII(fiSecond.time());
  int retVal = fiFirst.compareFileTime(fiSecond.getFILETIME());
  if(retVal > 0)
    std::cout << "\n  " << ConvToASCII((*first).name()) 
              << " is later than " << ConvToASCII((*second).name());
  if(retVal == 0)
    std::cout << "\n  " << ConvToASCII((*first).name()) 
              << " is same time as " << ConvToASCII((*second).name());
  if(retVal < 0)
    std::cout << "\n  " << ConvToASCII((*first).name()) 
              << " is earlier than " << ConvToASCII((*second).name());

  retVal = fiSecond.compareFileTime(fiFirst.getFILETIME());
  if(retVal > 0)
    std::cout << "\n  " << ConvToASCII((*second).name()) 
              << " is later than " << ConvToASCII((*first).name());
  if(retVal == 0)
    std::cout << "\n  " << ConvToASCII((*second).name()) 
              << " is same time as " << ConvToASCII((*first).name());
  if(retVal < 0)
    std::cout << "\n  " << ConvToASCII((*second).name()) 
              << " is earlier than " << ConvToASCII((*first).name());
  retVal = fiFirst.compareFileTime(fiFirst.getFILETIME());
  if(retVal > 0)
    std::cout << "\n  " << ConvToASCII(fiFirst.name()) 
              << " is later than " << ConvToASCII(fiFirst.name());
  if(retVal == 0)
    std::cout << "\n  " << ConvToASCII(fiFirst.name()) 
              << " is same time as " << ConvToASCII(fiFirst.name());
  if(retVal < 0)
    std::cout << "\n  " << ConvToASCII(fiFirst.name()) 
              << " is earlier than " << ConvToASCII(fiFirst.name());

  std::cout << "\n\n  Last search path = " << ConvToASCII(f.lastSetPath());
  std::cout << "\n\n";
  std::cin >> retVal;
}
#endif
