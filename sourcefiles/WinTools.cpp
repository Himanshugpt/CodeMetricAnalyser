///////////////////////////////////////////////////////////////////////
//  wintools.cpp - Win32 API-based helper functions                  //
//  ver 3.2                                                          //
//  Platform:      Dell Precision T7400, Vista Ultimate, SP1         //
//  Application:   CSE775 Project #1, Spring 2009                    //
//  Author:        Jim Fawcett, Syracuse University CST 4-187        //
//                 (315) 443-3948, jfawcett@twcny.rr.com             //
///////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iomanip>
#include <iostream>
#include "wintools.h"

using namespace Win32Tools;

//----< set path to return to on call to RestorePath() >---------------

bool Directory::SetRestorePath(const stdStr& path)
{
  if(Directory::DirectoryExists(path))
  {
    RestorePath_ = path;
    return true;
  }
  return false;
}
//----< does this directory exist? >-----------------------------------

bool Directory::DirectoryExists(const stdStr& path)
{
  stdStr temp = GetCurrentDirectory();
  if(SetCurrentDirectory(path))
  {
    SetCurrentDirectory(temp);
    return true;
  }
  return false;
}
//----< create directory >---------------------------------------------

bool Directory::CreateDirectory(const stdStr& path)
{
  return (::CreateDirectory(path.c_str(),0) != 0);
}
//----< remove directory >---------------------------------------------

bool Directory::RemoveDirectory(const stdStr &path, bool confirm)
{
  return (::RemoveDirectory(path.c_str()) != 0);
}
//----< get path of current directory >--------------------------------

stdStr Directory::GetCurrentDirectory()
{
  const size_t BufSize = 256;
  stdChar buffer[BufSize];
  ::GetCurrentDirectory(BufSize, buffer);
  return stdStr(buffer);
}
//----< set path of current directory >--------------------------------

bool Directory::SetCurrentDirectory(const stdStr& path)
{
  return (::SetCurrentDirectory(path.c_str()) != 0);
}

//----< get files >----------------------------------------------------

std::vector<stdStr> Directory::GetFiles(const stdStr& path, const stdStr& pattern)
{
  std::vector<stdStr> files;
  fileInfo fi;
  if(fi.firstFile(path,pattern))
  {
    if(!fi.isDirectory())
      files.push_back(fi.name());
    while(fi.nextFile())
      if(!fi.isDirectory())
        files.push_back(fi.name());
  }
  return files;
}
//----< get files >----------------------------------------------------

std::vector<stdStr> Directory::GetFiles(const stdStr& pattern)
{
  return GetFiles(GetCurrentDirectory(),pattern);
}
//----< get directories >----------------------------------------------

std::vector<stdStr> Directory::GetDirectories(const stdStr& path, const stdStr& pattern)
{
  std::vector<stdStr> files;
  fileInfo fi;
  if(fi.firstFile(path,pattern))
  {
    if(fi.isDirectory())
    {
      if(fi.name() != TEXT(".") && fi.name() != TEXT(".."))
        files.push_back(Path::fileSpec(path,fi.name()));
    }
    while(fi.nextFile())
      if(fi.isDirectory())
        if(fi.name() != TEXT(".") && fi.name() != TEXT(".."))
          files.push_back(Path::fileSpec(path,fi.name()));
  }
  return files;
}
//----< get directories >----------------------------------------------

std::vector<stdStr> Directory::GetDirectories(const stdStr& pattern)
{
  return GetDirectories(GetCurrentDirectory(),pattern);
}
//----< get fileInfo objects >-----------------------------------------

std::vector<fileInfo> Directory::GetFileInfos(const stdStr& path, const stdStr& pattern)
{
  std::vector<fileInfo> dinfos;
  fileInfo fi;
  if(fi.firstFile(path,pattern))
  {
    dinfos.push_back(fi);
    while(fi.nextFile())
      dinfos.push_back(fi);
  }
  return dinfos;
}
//----< get fileInfo objects >-----------------------------------------

std::vector<fileInfo> Directory::GetFileInfos(const stdStr& pattern)
{
  return GetFileInfos(GetCurrentDirectory(),pattern);
}
//----< copy files matching from filespec to toPath >----------------

bool Directory::CopyFiles(
       const stdStr& from, 
       const stdStr& toPath,
       bool FailIfExists
     )
{
  stdStr SrcDir = Path::getFullPath(from);
  stdStr CurrDir = Directory::GetCurrentDirectory();
  bool ok = true;
  if(Directory::SetCurrentDirectory(SrcDir))
  {
    std::vector<stdStr> files 
      = Directory::GetFiles(Path::getName(from));
    if(files.size() == 0)
      ok = false;
    for(size_t i=0; i<files.size(); ++i)
    {
      stdStr toFileSpec = Path::fileSpec(toPath,files[i]);
      stdStr fromFileSpec = Path::fileSpec(SrcDir,files[i]);
      if(!::CopyFile(fromFileSpec.c_str(),toFileSpec.c_str(),FailIfExists))
        ok = false;
    }
  }
  else
    ok = false;
  Directory::SetCurrentDirectory(CurrDir);
  return ok;
}

//----< convert string to lower case chars >---------------------------

stdStr Path::toLower(const stdStr& src)
{
  stdStr temp;
  for(size_t i=0; i<src.length(); ++i)
    temp += tolower(src[i]);
  return temp;
}
//----< convert string to upper case chars >---------------------------

stdStr Path::toUpper(const stdStr& src)
{
  stdStr temp;
  for(size_t i=0; i<src.length(); ++i)
    temp += toupper(src[i]);
  return temp;
}
//----< convert from ANSII to Unicode >--------------------------------

std::wstring toUnicode(const stdStr& src)
{
  std::wstring temp;
  for(size_t i=0; i<src.length(); ++i)
    temp += static_cast<wchar_t>(src[i]);
  return temp;
}
//----< convert from Unicode to ANSII >--------------------------------

stdStr toANSII(const std::wstring& src)
{
  stdStr temp;
  for(size_t i=0; i<src.length(); ++i)
    temp += static_cast<char>(src[i]);
  return temp;
}

//----< get path from fileSpec >---------------------------------------

stdStr Path::getName(const stdStr &fileSpec)
{
  size_t pos = fileSpec.find_last_of(TEXT("/"));
  if(pos >= fileSpec.length())
    pos = fileSpec.find_last_of(TEXT("\\"));
  if(pos >= fileSpec.length())
    return toLower(fileSpec);
  return toLower(fileSpec.substr(pos+1,fileSpec.length()-pos));
}
//----< get extension from fileSpec >----------------------------------

stdStr Path::getExt(const stdStr& fileSpec)
{
  size_t pos = fileSpec.find_last_of(TEXT("."));
  if(pos < fileSpec.length())
    return toLower(fileSpec.substr(pos+1,fileSpec.length()-pos));
  return TEXT("");
}
//----< get path from fileSpec >---------------------------------------

stdStr Path::getPath(const stdStr &fileSpec)
{
  size_t pos = fileSpec.find_last_of(TEXT("/"));
  if(pos >= fileSpec.length())
    pos = fileSpec.find_last_of(TEXT("\\"));
  if(pos >= fileSpec.length())
    return TEXT(".");
  if(fileSpec.find(TEXT("."),pos+1))
    return toLower(fileSpec.substr(0,pos+1));
  return fileSpec;
}
//----< get absoluth path from fileSpec >------------------------------

stdStr Path::getFullPath(const stdStr &fileSpec)
{
  const size_t BufSize = 256;
  TCHAR buffer[BufSize];
  TCHAR filebuffer[BufSize];  // don't use but GetFullPathName will
  TCHAR* name = filebuffer;
  ::GetFullPathName(fileSpec.c_str(),BufSize, buffer, &name);
  return stdStr(buffer);
}

//----< create file spec from path and name >--------------------------

stdStr Path::fileSpec(const stdStr &path, const stdStr &name)
{
  stdStr fs;
  size_t len = path.size();
  if(path[len-1] == '/' || path[len-1] == '\\')
    fs = path + name;
  else
  {
    if(path.find(TEXT("/")) < path.size())
      fs = path + TEXT("/") + name;
    else if(path.find(TEXT("\\")) < path.size())
      fs = path + TEXT("\\") + name;
  }
  return fs;
}

//----< throw exception string >---------------------------------------

void SystemError::ThrowString(const stdStr msg, const stdStr file, int line) {

  stdOstringstream collect;
  collect << msg << TEXT(": ") << GetLastMsg()
          << TEXT("  file: ") << file 
          << TEXT(", line: ") << line;
  throw collect.str();
}
//----< get system error message string >----------------------

stdStr SystemError::GetLastMsg() {

// ask system what type of error occurred

  DWORD errorCode = GetLastError();
  if(errorCode == 0)
    return TEXT("no error");

// map errorCode into a system defined error string
    
  DWORD dwFlags = 
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER;
  LPCVOID lpSource = NULL;
  DWORD dwMessageID = errorCode;
  DWORD dwLanguageId = MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US);
  TCHAR* lpBuffer;
  DWORD nSize = 0;
  va_list *Arguments = NULL;

  FormatMessage(
    dwFlags,lpSource,dwMessageID,dwLanguageId, 
    (TCHAR*)&lpBuffer,nSize,Arguments
  );

  _msg = lpBuffer;
  LocalFree(lpBuffer);
  return _msg;
}

//----< save a path for reading files >------------------------------

void FileHandler::setReadPath(const stdStr& path)
{
  _ReadPath = path;
}
//----< save a path for writing files >------------------------------

void FileHandler::setWritePath(const stdStr& path)
{
  _WritePath = path;
}
//----< open file for shared reading >-------------------------------

bool FileHandler::openFileReader(const stdStr& file)
{
  stdStr _file(_ReadPath);
  _file.append(TEXT("/"));
  _file.append(file);
  _hReadFile = CreateFile(
                           _file.c_str(),GENERIC_READ,
                           FILE_SHARE_READ,NULL,OPEN_EXISTING,
                           FILE_FLAG_SEQUENTIAL_SCAN,NULL
                         );
  if(_hReadFile != INVALID_HANDLE_VALUE)
  {
    return true;
  }
  return false;
}
//----< open file for writing >--------------------------------------

bool FileHandler::openFileWriter(const stdStr& file)
{
  stdStr _file(_WritePath);
  _file.append(TEXT("/"));
  _file.append(file);
  _hWriteFile = CreateFile(
                            _file.c_str(),GENERIC_WRITE,
                            0,NULL,CREATE_ALWAYS,
                            FILE_FLAG_SEQUENTIAL_SCAN,NULL
                          );
  if(_hWriteFile != INVALID_HANDLE_VALUE)
  {
    return true;
  }
  return false;
}

//----< read block of bytes from file, returning number read >-------

size_t FileHandler::getBlock(byte_* pBlock, size_t size)
{
  DWORD numBytesRead;
  ::ReadFile(_hReadFile,pBlock,(DWORD)size,&numBytesRead,NULL);
  return (size_t)numBytesRead;
}
//----< write block of bytes to file, returning number written >-----

size_t FileHandler::putBlock(byte_* pBlock, size_t size)
{
  DWORD numBytesWritten;
  ::WriteFile(_hWriteFile,pBlock,(DWORD)size,&numBytesWritten,NULL);
  return (size_t)numBytesWritten;
}
//----< close FileReader >-------------------------------------------

void FileHandler::closeFileReader()
{
  CloseHandle(_hReadFile);
}
//----< close FileWriter >-------------------------------------------

void FileHandler::closeFileWriter()
{
  
  CloseHandle(_hWriteFile);
}
//----< convert block of bytes to a string >-------------------------

stdStr FileHandler::blockToString(byte_* pBlock, size_t size)
{
  stdStr temp;
  for(size_t i=0; i<size; ++i)
    temp += static_cast<char>(pBlock[i]);
  return temp;
}

#ifdef TEST_WINTOOLS

//----< test stub >----------------------------------------------------

#include <iostream>

int main() {

  stdOut << "\n  Testing WinTools ";
  stdOut << "\n ==================\n";

  SystemError se;
  stdOut << TEXT("\n  Last error code = ") << ::GetLastError();
  stdOut << TEXT("\n  Last error message = ") << se.GetLastMsg();
  stdOut << std::endl;

  std::cout << "\n  testing directory management ";
  std::cout << "\n ------------------------------";

  Directory dir;
  if(dir.DirectoryExists(TEXT("test")))
  {
    std::cout << "\n  Directory test exists";
    if(dir.RemoveDirectory(TEXT("test")))
      std::cout << "\n  successfully removed directory test";
  }
  else
  {
    if(dir.CreateDirectory(TEXT("test")))
      std::cout << "\n  successfully created directory test";
    else
      std::cout << "\n  Directory creation failed";
  }
  stdOut << TEXT("\n  Current directory is: ") << dir.GetCurrentDirectory();
  if(dir.SetCurrentDirectory(TEXT("./test")))
  {
    stdOut << TEXT("\n  changed to: ")  << dir.GetCurrentDirectory();
    dir.SetCurrentDirectory(TEXT(".."));
    stdOut << TEXT("\n  changed back to: ") << dir.GetCurrentDirectory();
  }
  std::cout << std::endl;

  std::vector<stdStr> files = Directory::GetFiles();
  std::cout << "\n  files in this directory are:";
  for(size_t i=0; i<files.size(); ++i)
    stdOut << TEXT("\n    ") << files[i];
  std::cout << std::endl;

  files = Directory::GetFiles(TEXT("*.h*"));
  std::cout << "\n  *.h* files in this directory are:";
  for(size_t i=0; i<files.size(); ++i)
    stdOut << TEXT("\n    ") << files[i];
  std::cout << std::endl;

  std::vector<stdStr> dirs = Directory::GetDirectories();
  std::cout << "\n  directories in this directory are:";
  for(size_t i=0; i<dirs.size(); ++i)
    stdOut << TEXT("\n    ") << dirs[i];
  std::cout << std::endl;

  std::cout << "\n  testing Path management ";
  std::cout << "\n -------------------------";

  stdStr paths[] = { TEXT("aFile"), TEXT("../../aFile"), TEXT("test/aFile"), TEXT("../../") };
  for(int i=0; i<4; ++i)
  {
    stdOut << TEXT("\n  fileSpec: ") << std::setw(12) << paths[i];
    stdOut << TEXT(", name: ") << Path::getName(paths[i]);
    stdOut << TEXT("\n  fileSpec: ") << std::setw(12) << paths[i];
    stdOut << TEXT(", path: ") << Path::getPath(paths[i]);
  }
  std::cout << std::endl;

  std::cout << "\n  testing error messages ";
  std::cout << "\n ------------------------";

  int err = GetLastError();
  std::cout << "\n  Last error code = " << err;

  try {
    se.ThrowString(TEXT("throw message"),Convert::ToStdStr(__FILE__),__LINE__);
  }
  catch(const stdStr &msg)
  { 
    stdOut << TEXT("\n  ") << msg; 
  }
  std::cout << "\n";

  std::cout << "\n  test writing to \"Program Files\" Folder ";
  std::cout << "\n ------------------------------------------";

  stdStr CurrDir = Path::getFullPath(TEXT("."));
  stdStr fileSpec = CurrDir + TEXT("*.*");

  if(Directory::SetCurrentDirectory(TEXT("C:/Program Files")))
  {
    std::cout << "\n  sucessfully set directory to \"C:\\Program Files\"";
    if(Directory::DirectoryExists(TEXT("foobar stuff")))
    {
      std::cout << "\n  \"C:\\Program Files\\foobar stuff\" exists";
      if(!Directory::CopyFiles(fileSpec,TEXT("C:\\Program Files\\foobar stuff")))
        std::cout << "\n  one or more file copy operations failed";
      else
        std::cout << "\n  all file copy operations succeeded";
    }
    else if(Directory::CreateDirectory(TEXT("foobar stuff")))
    {
      std::cout << "\n  successfully created \".\\foobar stuff\"";
      if(!Directory::CopyFiles(fileSpec,TEXT("C:\\Program Files\\foobar stuff")))
        std::cout << "\n  one or more file copy operations failed";
      else
        std::cout << "\n  all file copy operations succeeded";
    }
  }

  std::cout << "\n";
  std::cout << "\n  File file operations: ";
  std::cout << "\n ----------------------";

  stdStr path = TEXT("C:\\temp");
  if(!Directory::SetCurrentDirectory(path))
  {
    stdOut << TEXT("\n\n  invalid path ") << path << TEXT("\n\n");
    return 1;
  }
  std::vector<stdStr> file = Directory::GetFiles();

  // find a fairly large cpp file to read

  stdStr displayFile = TEXT("");
  fileInfo fi;
  size_t fileSize = 0;
  for(size_t i=0; i<file.size(); ++i)
  {
    stdOut << TEXT("\n  ") << file[i];
    if(TEXT("cpp") == Path::getExt(file[i]))
    {
      fi.firstFile(file[i]);
      if(fileSize < fi.size() && fi.size() < 50000)
      {
        fileSize = fi.size();
        displayFile = file[i];
      }
    }
  }

  if(displayFile == TEXT(""))
  {
    stdOut << TEXT("\n  no *.cpp files in ") << path.c_str() << TEXT("\n\n");
    return 1;
  }


  stdOut << "\n\n  Reading Blocks";
  stdOut << "\n ----------------\n";

  if(file.size() > 0)
  {
    FileHandler fh;
    fh.setReadPath(path);
    if(fh.openFileReader(displayFile))
    {
      stdOut << "\n  opening file " << displayFile << "\n\n";
      const size_t size = 1024;
      size_t bytesRead;
      byte_ buffer[size];
      do
      {
        bytesRead = fh.getBlock(buffer,size);
        stdOut << fh.blockToString(buffer,bytesRead);
      } while(bytesRead == size);
    }
    fh.closeFileReader();
  }

  stdOut << "\n\n  Writing Blocks";
  stdOut << "\n ----------------";

  if(file.size() > 0)
  {
    FileHandler fh;
    fh.setReadPath(path);
    fh.setWritePath(path);
    if(!fh.openFileReader(displayFile))
    {
      stdOut << "\n  open " << displayFile << " failed\n\n";
      return 1;
    }
    else
      stdOut << "\n  opening file " << displayFile << " for reading";
    if(fh.openFileWriter(TEXT("test.txt")))
    {
      std::cout << "\n  opening file " << "test.txt" << " for writing";
      const size_t size = 1024;
      size_t bytesRead;
      byte_ buffer[size];
      int count = 0;
      do
      {
        std::cout << "\n    writing block #" << ++count;
        bytesRead = fh.getBlock(buffer,size);
        fh.putBlock(buffer,bytesRead);
      } while(bytesRead == size);
    }
    std::cout << "\n  closing write file";
    fh.closeFileWriter();
    std::cout << "\n  closing read file";
    fh.closeFileReader();
    std::cout << "\n\n";
  }
}

#endif
