#if defined(_MSC_VER) && (_MSC_VER > 1100)
	#pragma once
   #pragma warning(disable: 4786)
#endif
//---------------------------------------------------------------------------
#ifndef __SSTRING_H
#define __SSTRING_H
//---------------------------------------------------------------------------
#include <windows.h>
#include <vector>
#include <excpt.h>
//---------------------------------------------------------------------------
#include "StdString.h"
//---------------------------------------------------------------------------
//typedef CStdStringW  SStringW;
//typedef CStdStringA  SStringA;
typedef std::string  SStringW;
typedef std::string  SStringA;
#ifdef _UNICODE
//typedef SStringW SString;
typedef std::string  SString;
#else
typedef SStringA SString;
#endif
//---------------------------------------------------------------------------
template <class TCH>
struct TCSWRAP  // function about char/wchar_t
{
};

template <>
struct TCSWRAP< char >
{
    enum {
        SPACE = ' ',
        TAB   = '\t',
        CR    = '\r',
        NL    = '\n',
        QUOTE = '\"',
        SINGLEQUOTE = '\'',
        BACKSLASH = '\\',

    };

    inline static size_t tcslen(const char* sz)    { return strlen(sz); }
    inline static char* tcsstr(char* sz, char* str) { return strstr(sz, str); }
    inline static char* tcscpy(char* dest, const char* src) { return strcpy(dest, src); }
    inline static int icmp(const char* s1, const char* s2) { return _stricmp(s1, s2); }
};

template <>
struct TCSWRAP< wchar_t >
{
    enum {
        SPACE = L' ',
        TAB   = L'\t',
        CR    = L'\r',
        NL    = L'\n',
        QUOTE = L'\"',
        SINGLEQUOTE = L'\'',
        BACKSLASH = L'\\',
    };

    inline static size_t tcslen(const wchar_t* sz)   { return wcslen(sz); }
    inline static wchar_t* tcsstr(wchar_t* sz, wchar_t* str) { return wcsstr(sz, str); }
    inline static wchar_t* tcscpy(wchar_t* dest, const wchar_t* src) { return wcscpy(dest, src); }
    inline static int icmp(const wchar_t* s1, const wchar_t* s2) { return _wcsicmp(s1, s2); }
};
//---------------------------------------------------------------------------
int StrCompareI(const SStringA& s1, const SStringA& s2);
int StrCompareI(const SStringW& s1, const SStringW& s2);
inline int StrCompareI(LPCSTR s1, LPCSTR s2) { return TCSWRAP<char>::icmp(s1,s2); }
inline int StrCompareI(const wchar_t* s1, const wchar_t* s2) { return TCSWRAP<wchar_t>::icmp(s1,s2); }
//---------------------------------------------------------------------------
const SStringA RemoveCommentString( SStringA& str, char comment_mark);
const SStringW RemoveCommentString( SStringW& str, WCHAR comment_mark);
//---------------------------------------------------------------------------
const SStringA StrRemoveChars(SStringA str, const SStringA& removeChars);
const SStringW StrRemoveChars(SStringW str, const SStringW& removeChars);
//---------------------------------------------------------------------------
const SStringA Trim(const SStringA& s, const SStringA& trimStr);
const SStringW Trim(const SStringW& s, const SStringW& trimStr);
//---------------------------------------------------------------------------
int ToInt(LPCSTR str, int base = 10 );
int ToInt(LPCWSTR str, int base = 10);
inline int ToInt(const SString& str, int base = 10 ) { return ToInt(str.c_str(), base); }

bool HexToByte(LPCSTR str, BYTE& value);
bool HexToByte(LPCWSTR str, BYTE& value);
//---------------------------------------------------------------------------
const SStringW MakeLower( const SStringW& s);
const SStringA MakeLower( const SStringA& s);
const SStringW MakeUpper( const SStringW& s);
const SStringA MakeUpper( const SStringA& s);
//---------------------------------------------------------------------------
const SStringA  IntToStrA(int value, int base=10);
const SStringW IntToStrW(int value, int base=10);
#ifdef _UNICODE
#define  IntToStr IntToStrW
#else
#define  IntToStr IntToStrA
#endif
//---------------------------------------------------------------------------
int WildCmp(LPCSTR *Wild, LPCSTR *string);
int WildCmp(LPCWSTR *Wild, LPCWSTR *string);
//---------------------------------------------------------------------------
inline SString MultiByteToString(LPCSTR lpStr) { return SString(lpStr); }
inline SString WideCharToString(LPCWSTR lpStr) { return SString(lpStr); }
inline const SStringA StringToMultiByte( const SStringW& str) { return SStringA(str.c_str()); }
inline const SStringA StringToMultiByte( const SStringA& str) { return str; }
inline const SStringW StringToWideChar(const SStringW& str) { return str; }
inline const SStringW StringToWideChar(const SStringA& str) { return SStringW(str.c_str()); }
//---------------------------------------------------------------------------
const SString BinaryToString(const unsigned char* buffer, int buflen);
const SString ReplaceNonPrintable(const SString& str, TCHAR Replace);
//---------------------------------------------------------------------------
template <class SSTRING>
int Subtitute(SSTRING& str, const SSTRING& mark, const SSTRING& ReplaceBy)
{
   // find mark
   int cnt = 0;
   int rlen = ReplaceBy.length();
   int pos = str.find(mark);
   while ( pos != str.npos ) 
   {
      str.replace( pos, mark.length(), ReplaceBy);

      pos = str.find(mark, pos + rlen);
      ++cnt;
   }
   return (cnt);
}
//---------------------------------------------------------------------------
void SubtituteNoCase(SStringA& str, const SStringA& mark, const SStringA& ReplaceBy);
void SubtituteNoCase(SStringW& str, const SStringW& mark, const SStringW& ReplaceBy);
//---------------------------------------------------------------------------
const SStringW StrExtractW(SStringW& str, const SStringW& leftMark, 
                               const SStringW& rightMark);
const SStringA StrExtractA(SStringA& str, const SStringA& leftMark, 
                              const SStringA& rightMark);
#ifdef _UNICODE
#define StrExtract   StrExtractW
#else
#define StrExtract   StrExtractA
#endif

//---------------------------------------------------------------------------
DWORD LenCopyW(LPWSTR buf, DWORD buflen, LPCWSTR src, bool bFailedOnNotEnough);
DWORD LenCopyA(LPSTR buf, DWORD buflen, LPCSTR src, bool bFailedOnNotEnough);
#ifdef _UNICODE
#define  LenCopy  LenCopyW
#else
#define  LenCopy  LenCopyA
#endif
//---------------------------------------------------------------------------
inline const SStringW StringToWString (const SStringA& rhs) { return SStringW(rhs.c_str()); }
inline const SStringA  WStringToString (const SStringW& rhs) { return SStringA(rhs.c_str()); }

const SStringA GetToken(SStringA& str, const SStringA& sSeparate = "");
const SStringW GetToken(SStringW& str, const SStringW& sSeparate = _T(""));

const SStringA BinaryToHexStrA(const char* buf, DWORD buflen);
const SString BinaryToHexStr(const char* buf, DWORD buflen);
int HexStrToBinary(const SString& str, LPBYTE buf, DWORD buflen);
const SString BinaryToAscHexStr(const char* buf, DWORD buflen);
int AscHexStrToBinary(const SString& str, LPBYTE buf, DWORD buflen);

struct BinData;
const SString  BinDataToHexStr(const BinData& data);
const BinData HexStrToBinData( const SString& str );

const SString BinDataToAscHexStr(const BinData& data);
const BinData AscHexStrToBinData(const SString& str);

const SStringA FormatString(LPCSTR lpszFormat, ...);
const SStringW FormatString(LPCWSTR lpszFormat, ...);
const SStringA ConvertToCFormatString(const SStringA& string);
const SStringW ConvertToCFormatString(const SStringW& string);
//---------------------------------------------------------------------------
// Format integer to many bytes, mode=0 (auto), 1=bytes, 2=Kbytes, 3= Mbytes
const SStringA KBSizeStringA(DWORD size, int mode=0) ;
const SStringW KBSizeStringW(DWORD size, int mode=0);
#ifdef _UNICODE
#define  KBSizeString   KBSizeStringW
#else
#define  KBSizeString   KBSizeStringA
#endif 
//---------------------------------------------------------------------------
typedef std::vector<SString> StringVector;
typedef std::vector<SString> StringList;


template <class T>
size_t TCSLEN(const T* str) { return TCSWRAP<T>::tcslen(str); }

template <class T>
T* TCSSTR(const T* s1, const T* s2) { return TCSWRAP<T>::tcsstr(s1,s2); }

template <class T>
T* TCSCPY(T* dest, const T* src) { return TCSWRAP<T>::tcscpy(dest,src); }

//---------------------------------------------------------------------------
template <class TCH >
bool Split( TCH* pszSource, const TCH* pszSplitter, TCH** ppLHS, TCH** ppRHS)
{
    TCH* pos = TCSSTR(pszSource, pszSplitter);

    if (ppLHS)  *ppLHS = pszSource;

    if ( pos == NULL )
    {
        if (ppRHS)  *ppRHS = pszSource + TCSLEN(pszSource);
    }
    else {
        *pos = 0;
        if (ppRHS)  *ppRHS = pos + 1;
    }

    return (pos!=0);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif // __SSTRING_H
