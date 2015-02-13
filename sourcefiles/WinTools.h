#ifndef WINTOOLS_H
#define WINTOOLS_H
///////////////////////////////////////////////////////////////////////
//  wintools.h  -  Win32 API-based helper functions                  //
//  ver 3.2                                                          //
//  Platform:      Dell Precision T7400, Vista Ultimate, SP1         //
//  Application:   CSE775 Project #1, Spring 2009                    //
//  Author:        Jim Fawcett, Syracuse University CST 4-187        //
//                 (315) 443-3948, jfawcett@twcny.rr.com             //
///////////////////////////////////////////////////////////////////////
/*
    Module WinTools Operations:
    ---------------------------
    This module defines these classes:
    - Directory class supports directory management.
      Most functions in this class are static, are called using Directory::
    - Convert class supports conversion between basic types.
      Now limited to conversions between std::string and std::wstring.
      All members are static.
    - Path class supports manipulation of file and directory paths.
      Most Path functions are static, and are called using Path class name.
    - SystemError class provides system error reporting.
      The most important member is getLastMsg().
    - FileHandler class provides file open, close, read/write blocks of bytes.

    Public Interface:
    -----------------
    Directory dir;
    dir.SetRestorePath("..");
    dir.RestorePath();
    bool ok = Directory::DirectoryExists(path);
    bool ok = Directory::CreateDirectory(path);
    bool ok = Directory::RemoveDirectory(path);
    bool ok = Directory::SetCurrentDirectory(path);
    stdStr path = Directory::GetCurrentDirectory();
    std::vector<stdStr> files = Directory::GetFiles("*.cpp");
    std::vector<stdStr> dirs = Directory::GetDirectories();

    stdStr path = Path::getPath(fileSpec);
    stdStr path = Path::getFullPath(fileSpec);
    stdStr name = Path::getName(fileSpec);
    stdStr path = Path::getPath(fileSpec);
    stdStr ext  = Path::getExt(fileSpec);

    FileHandler fh;
    fh.OpenFileReader(rpath);
    fh.OpenFileWriter(wpath);
    const size_t size = 1024;
    byte_ buffer[size];
    size_t numBytesRead = fh.getBlock(buffer, bufSize);
    stdOut << fh.blockToString(buffer, bufSize);
    size-t numBytesWrit = fh.putBlock(buffer, bufSize);
    fh.CloseFileReader();
    fh.CloseFileWriter();

    SystemError syserr;
    stdOut << "\n  last error code = " << syserr.GetLastError();
    stdOut << "\n  last error = " << syserr.GetLastMsg();
*/

///////////////////////////////////////////////////////////////
//                      maintenance page                     //
///////////////////////////////////////////////////////////////
//  Build Process                                            //
//                                                           //
//  Files Required:                                          //
//    wintools.h, wintools.cpp, fileInfo.h, fileInfo.cpp     //
//                                                           //
//  Building with Visual C++ , 8.0, from command line:       //
//    cl -EHsc -DTEST_WINTOOLS wintools.cpp fileInfo.cpp     //
//                                                           //
///////////////////////////////////////////////////////////////
/*
    Maintenance History
    ===================
    ver 3.2 : 18 Jan 09
    - added Directory::GetFileInfos(path,pattern)
    - modified behavior of Path::getFullPath().  Still changes
      relative path to full path, but now does not strip off
      file name if attached.
    ver 3.1 : 13 Jan 09
    - fixed several bugs due to incomplete support for strings
      that are either ascii or unicode, started in ver 3.0
    - migrated most of the string support to StringConversion
      module
    ver 3.0 : 11 Jan 09
    - changed std::string to stdStr typedef
    - changed char to stdChar typedef
    ver 2.2 : 06 May 07
    - added test for no cpp files on search path for testing reading
      and writing in test stub
    ver 2.1 : 27 Mar 07
    - added FileHandler class
    ver 2.0 : 28 Feb 06
    - added Path members: fileSpec, getFullPath, toANSII, and toUnicode
    - added Directory member: CopyFiles
    - modified processing in getPath to take care of a special case
    ver 1.3 : 26 Feb 06
    - fixed bug in Directory::DirectoryExists(const stdStr& path);
    - added Path::toUpper utility function
    ver 1.2 : 21 Feb 06
    - modified WinTools::GetFiles to accept a pattern to 
      select specific files from the current directory
    ver 1.1 : 06 Feb 06
    - added Path class
    ver 1.0 : 05 Feb 06
    - first release
*/

#include <string>
#include <vector>
#include <Windows.h>
//#include "StringConversion.h"
#include "fileInfo.h"

namespace Win32Tools
{
  typedef stdChar byte_;

  ///////////////////////////////////////////////////////////////
  //  class SystemError - handle system errors

  class SystemError
  {
    public:
      SystemError(int n=0);
      ~SystemError() {}
      int GetLastError();
      stdStr GetLastMsg();
      void ThrowString(const stdStr msg, const stdStr file, int line);
    private:
      stdStr _msg;
  };

  inline SystemError::SystemError(int n) { ::SetLastError(n); }

  inline int SystemError::GetLastError() { return ::GetLastError(); }

  ///////////////////////////////////////////////////////////////
  // class Directory - manage directory operations

  class Directory
  {
  public:
    Directory();
    ~Directory() { }
    bool SetRestorePath(const stdStr& RestorePath);
    bool RestoreFirstDirectory();
    static bool DirectoryExists(const stdStr& path);
    static bool CreateDirectory(const stdStr& path);
    static bool RemoveDirectory(const stdStr& path, bool confirm=true);
    static stdStr GetCurrentDirectory();
    static bool SetCurrentDirectory(const stdStr& path);
    static std::vector<stdStr> GetFiles(const stdStr& path, const stdStr& pattern);
    static std::vector<stdStr> GetFiles(const stdStr& pattern=TEXT("*.*"));
    static std::vector<stdStr> GetDirectories(const stdStr& path, const stdStr& pattern);
    static std::vector<stdStr> GetDirectories(const stdStr& pattern=TEXT("*.*"));
    static std::vector<fileInfo> GetFileInfos(const stdStr& path, const stdStr& pattern);
    static std::vector<fileInfo> GetFileInfos(const stdStr& pattern=TEXT("*.*"));
    static bool CopyFiles(
                  const stdStr& from, 
                  const stdStr& toPath, 
                  bool FailIfExits = true
                );
  private:
    stdStr RestorePath_;
  };

  inline Directory::Directory() 
                  : RestorePath_(Directory::GetCurrentDirectory()) {}

  inline bool Directory::RestoreFirstDirectory()
  { 
    return SetCurrentDirectory(RestorePath_); 
  };

  class Path
  {
  public:
    static stdStr getPath(const stdStr& fileSpec);
    static stdStr getFullPath(const stdStr& fileSpec);
    static stdStr getName(const stdStr& fileSpec);
    static stdStr getExt(const stdStr& fileSpec);
    static stdStr fileSpec(const stdStr& path, const stdStr& name);
    static stdStr toLower(const stdStr& src);
    static stdStr toUpper(const stdStr& src);
    static std::wstring toUnicode(const stdStr& src);
    static stdStr toANSII(const std::wstring& src);
  };
  
  class FileHandler
  {
  public:
    void setReadPath(const stdStr& path);
    void setWritePath(const stdStr& path);
    bool openFileReader(const stdStr& file);
    bool openFileWriter(const stdStr& file);
    size_t getBlock(byte_* pBlock, size_t size);
    size_t putBlock(byte_* pBlock, size_t size);
    void closeFileReader();
    void closeFileWriter();
    stdStr blockToString(byte_* pBlock, size_t size);
  private:
    stdStr _ReadPath;
    stdStr _WritePath;
    HANDLE _hReadFile;
    HANDLE _hWriteFile;
  };
}
#endif
