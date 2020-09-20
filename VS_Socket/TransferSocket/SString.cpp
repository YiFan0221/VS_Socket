//---------------------------------------------------------------------------
#include "stdafx.h"
//#include "sulib.h"
#include "BinData.h"
#include "SString.h"
#pragma warning(disable: 4786)
#include <memory>
#include <assert.h>
//===========================================================================
int StrCompareI(const SStringA& s1, const SStringA& s2) 
{
   return StrCompareI(s1.c_str(), s2.c_str());
}
//---------------------------------------------------------------------------
int StrCompareI(const SStringW& s1, const SStringW& s2) 
{
   return StrCompareI(s1.c_str(), s2.c_str());
}
//===========================================================================
int ToInt(LPCSTR s, int base) 
{
   int rtn = 0;
   char*   endptr;
   rtn = strtoul(s, &endptr, base);
   return (rtn);
}
//---------------------------------------------------------------------------
int ToInt(LPCWSTR s, int base) 
{
   int rtn = 0;
   LPWSTR  endptr;

   rtn = wcstoul(s, &endptr, base);

   return (rtn);
}

bool HexToByte(LPCSTR str, BYTE& value)
{
    const char* ptr = str;
    value = 0;
	int ii;
    for (int i=0; i<2 && ptr; ++i, ++ptr)
    {
		ii = i;
        char ch = *ptr;
        if ( (ch>='0') && (ch<='9') )
        {
            value = (value << 4) | (BYTE) (ch - '0');
        }
        else if ( (ch>='a') && (ch<='f') )
        {
            value = (value << 4) | (BYTE) ((ch - 'a') + 10);
        }
        else if ( (ch>='A') && (ch<='F') )
        {
            value = (value << 4) | (BYTE) ((ch - 'A') + 10);
        }
        else 
            break;
    }
	return (ii == 2);
}
//---------------------------------------------------------------------------
bool HexToByte(LPCWSTR str, BYTE& value)
{
    const TCHAR* ptr = str;
    value = 0;
	int ii = 0;
    for (int i=0; i<2 && ptr; ++i, ++ptr)
    {
		ii = i;
        TCHAR ch = *ptr;
        if ( (ch>=_T('0')) && (ch<=_T('9')) )
        {
            value = (value << 4) | (BYTE) (ch - _T('0'));
        }
        else if ( (ch>=_T('a')) && (ch<=_T('f')) )
        {
            value = (value << 4) | (BYTE) ((ch - _T('a')) + 10);
        }
        else if ( (ch>=_T('A')) && (ch<=_T('F')) )
        {
            value = (value << 4) | (BYTE) ((ch - _T('A')) + 10);
        }
        else 
            break;
    }
    return (ii==2);
}
//===========================================================================
const SStringA IntToStrA(int value, int base)
{
   char buf[80];
   switch (base) {
   case 16:
      wsprintfA(buf, "%X", value);
      break;
   default:
      wsprintfA(buf, "%ld", value);
      break;
   }
   return SStringA(buf);
}
//---------------------------------------------------------------------------
const SStringW IntToStrW(int value, int base)
{
   WCHAR buf[80];
   switch (base) {
   case 16:
      wsprintfW(buf, L"%X", value);
      break;
   default:
      wsprintfW(buf, L"%ld", value);
      break;
   }
   return SStringW(buf);
}
//---------------------------------------------------------------------------

//===========================================================================
const SStringA Trim(const SStringA& s, const SStringA& trimStr)
{
   if (s.length()==0)
      return s;

   SStringA str(s);
   int b = str.find_first_not_of(trimStr);
   if (b == str.npos)
      return SStringA("");

   str.erase(0, b);
   b = str.find_last_not_of(trimStr);
   if (b != str.npos)
      str.erase(b+1, str.npos);

   return str;
}
//---------------------------------------------------------------------------
const SStringW Trim(const SStringW& s, const SStringW& trimStr)
{
   if (s.length()==0)
      return s;

   SStringW str(s);
   int b = str.find_first_not_of(trimStr);
   if (b == str.npos)
      return SStringW(L"");

   str.erase(0, b);
   b = str.find_last_not_of(trimStr);
   if (b != str.npos)
      str.erase(b+1, str.npos);

   return str;
}
//===========================================================================
const SStringA StrRemoveChars(SStringA str, const SStringA& removeChars)
{
   int b;
   while ( (b = str.find_first_of(removeChars)) != str.npos) {
      int e = str.find_first_not_of(removeChars, b);
      if (e != str.npos)
         str.erase(b, e-b);
      else
         str.erase(b, e);
   }

   return (str);
}
//----------------------------------------------------------
const SStringW StrRemoveChars(SStringW str, const SStringW& removeChars)
{
   int b;
   while ( (b = str.find_first_of(removeChars)) != str.npos) {
      int e = str.find_first_not_of(removeChars, b);
      if (e != str.npos)
         str.erase(b, e-b);
      else
         str.erase(b, e);
   }

   return (str);
}
//===========================================================================
int WildCmp(LPCSTR wild, LPCSTR string) 
{
	LPCSTR cp =NULL;
	LPCSTR mp = NULL;
	
	while ((*string) && (*wild != '*')) {
		if ((*wild != *string) && (*wild != '?')) {
			return 0;
		}
		wild++;
		string++;
	}
		
	while (*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}
		
	while (*wild == '*') {
		wild++;
	}
	return !*wild;
}
//----------------------------------------------------------
int WildCmp(LPCWSTR wild, LPCWSTR string) 
{
	LPCWSTR cp = NULL;
	LPCWSTR mp = NULL;
	
	while ((*string) && (*wild != _T('*'))) {
		if ((*wild != *string) && (*wild != _T('?'))) {
			return 0;
		}
		wild++;
		string++;
	}
		
	while (*string) {
		if (*wild == _T('*')) {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == _T('?'))) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}
		
	while (*wild == _T('*')) {
		wild++;
	}
	return !*wild;
}
//---------------------------------------------------------------------------
const SString BinaryToString(const unsigned char *buffer, int buflen)
{
   if (buflen <=0 )
      return SString(_T(""));

   std::auto_ptr<TCHAR> buf( new TCHAR[ (buflen+1) * 3] );
   LPTSTR ptr = buf.get();
   const unsigned char *sptr = buffer;
   for (int i=0; i<buflen; i++, ptr+=3, sptr++) {
      wsprintf(ptr, _T("%02X "), *sptr);
   }

   return SString(buf.get());
}
//---------------------------------------------------------------------------
const SString ReplaceNonPrintable(const SString& strOriginal, TCHAR repChar)
{
   SString str = strOriginal;

   int len = str.length();
   for (int i=0; i<len; i++) {
#ifdef _UNICODE
      if ( !iswprint(str[i]) )
#else
      if ( !isprint( str[i] ) )
#endif
         str[i] = repChar;
   }
   return (str);
}
//---------------------------------------------------------------------------
DWORD LenCopyW(LPWSTR buf, DWORD buflen, LPCWSTR src, bool bFailedOnNotEnough)
{
   DWORD len = wcslen( src );
   if (buflen < len) {
      if (bFailedOnNotEnough)
         return len;
      len = buflen - 1;
   }

   wcsncpy(buf, src, len);
   buf[len] = L'\0';
   return len;
}
//---------------------------------------------------------------------------
DWORD LenCopyA(LPSTR buf, DWORD buflen, LPCSTR src, bool bFailedOnNotEnough)
{
   DWORD len = strlen(src);
   if (buflen < len) {
      if (bFailedOnNotEnough)
         return len;
      len = buflen - 1;
   }

   strncpy(buf, src, len);
   buf[len] = '\0';
   return len;
}
//===========================================================================
const SStringW StrExtractW(SStringW& str, const SStringW& leftMark, 
                               const SStringW& rightMark)
{
   SStringW rtn;

   int b = str.find(leftMark);
   if (b != str.npos) {
      int e = str.find(rightMark, b+leftMark.length());
      if (e != str.npos) {
         b = str.rfind( leftMark, e-1);

         int len = (e + rightMark.length()) - b;
         rtn = str.substr(b, len);
         str.erase(b, len);
      }
   }
   return (rtn);
}
//---------------------------------------------------------------------------
const SStringA StrExtractA(SStringA& str, const SStringA& leftMark, 
                              const SStringA& rightMark)
{
   SStringA rtn;

   int b = str.find(leftMark);
   if (b != str.npos) {
      int e = str.find(rightMark, b+leftMark.length());
      if (e != str.npos) {
         b = str.rfind( leftMark, e-1);

         int len = (e + rightMark.length()) - b;
         rtn = str.substr(b, len);
         str.erase(b, len);
      }
   }
   return (rtn);
}
//---------------------------------------------------------------------------
const SStringA KBSizeStringA(DWORD size, int mode) 
{
   char kbstr[80];
   if ( (mode == 3) || ((mode==0) && (size>1024*1024)) ) {
      sprintf(kbstr, "%.2fM", (float) size / (float) (1024 * 1024));
   }
   else if ((mode==2) || ((mode==0) && (size > 1024)) ) {
      sprintf(kbstr, "%.2fK", (float) size / (float) (1024));
   }
   else 
      sprintf(kbstr, "%ld", size );
   return SStringA( kbstr );
}
//---------------------------------------------------------------------------
const SStringW KBSizeStringW(DWORD size, int mode) 
{
   WCHAR kbstr[80];
   if ( (mode == 3) || ((mode==0) && (size>1024*1024)) ) {
      swprintf(kbstr, L"%.2fM", (float) size / (float) (1024 * 1024));
   }
   else if ((mode==2) || ((mode==0) && (size > 1024)) ) {
      swprintf(kbstr, L"%.2fK", (float) size / (float) (1024));
   }
   else 
      swprintf(kbstr, L"%ld", size );
   return SStringW( kbstr );
}
//---------------------------------------------------------------------------
const SString FormatKBString(DWORD value, LPCTSTR szFormat/*=_T("%a%d %u")*/)
{
    // %a, 設定模式為自動
    // %k, 設定以 K 為單位
    // %m, 設定以 M 為單位
    // %d, 顯示數字部分
    // %u, 顯示單位 (K, M)
    // %,, 顯示數字部分, 並設每三位一個逗號

    SString result;

    if ( szFormat )
    {

    }
    return result;
}
//---------------------------------------------------------------------------
//template <class TCH>
//struct TCharType
//{
//};
//
//template <>
//struct TCharType<WCHAR>
//{
//    enum {
//        SPACE = L' ',
//        TAB   = L'\t',
//        CR    = L'\r',
//        NL    = L'\n',
//        QUOTE = L'\"',
//        SINGLEQUOTE = L'\'',
//        BACKSLASH = L'\\',
//    };
//
//};
//
//template <>
//struct TCharType<char>
//{
//    enum {
//        SPACE = ' ',
//        TAB   = '\t',
//        CR    = '\r',
//        NL    = '\n',
//        QUOTE = '\"',
//        SINGLEQUOTE = '\'',
//        BACKSLASH = '\\',
//    };
//
//    void NeverCallJustForTest( char c) { (void) c; }
//};

template < class TCH >
struct GetTokenOp
{
    static inline bool IsWhiteSpace(TCH c) 
{
        return (c == TCSWRAP<TCH>::SPACE) || (c == TCSWRAP<TCH>::TAB) 
                || (c == TCSWRAP<TCH>::CR) || (c == TCSWRAP<TCH>::NL);
}

    static inline bool IsSeperate(const CStdStr<TCH>& sSeperate, TCH c) 
{
   return (sSeperate.find(c)!=sSeperate.npos);
}

    static const CStdStr<TCH> GetToken( CStdStr<TCH>& str, const CStdStr<TCH>& sSeperate)
{
   // remove white space first, SPACE, RETURN )\r\n, TAB
   int pos = 0;
   while ( IsWhiteSpace(str[pos]) ) pos++;

        CStdStr<TCH> result;

   int total_len = str.length();
   bool in_str = false;
        TCH pre_char = 0;
   
        TCH ch;
   while (ch = str[pos++], pos != total_len)
   {
      if (IsSeperate(sSeperate, ch))
      {
         if ( result.length() )
            break;
      }

      if (IsWhiteSpace(ch))
      {
         if ( in_str )
         {
            result += ch;
         }
         else 
            break;
      }
            else if (ch == TCSWRAP<TCH>::QUOTE)
      {
                if ( pre_char == TCSWRAP<TCH>::BACKSLASH)
         {  // we have a     \" sequence
            result += ch;
         }
         else if ( in_str )
         {  // end of string "....."
            in_str = false;
            break;
         }
         else
         {
            in_str = true;
         }
      }
            else if (ch == TCSWRAP<TCH>::BACKSLASH)
      {
                if (pre_char == TCSWRAP<TCH>::BACKSLASH)
         {
            result += ch;
         }
      }
      else 
         result += ch;

            if ( (pre_char == TCSWRAP<TCH>::BACKSLASH) && (ch == TCSWRAP<TCH>::BACKSLASH))
                pre_char = 0;
      else
         pre_char = ch;
   }

   str.erase(0, pos);

   return result;
}
};

const SStringA GetToken( SStringA& str, const SStringA& sSeperate)
{
    return GetTokenOp<char>::GetToken( str, sSeperate );
}
//---------------------------------------------------------------------------
const SStringW GetToken( SStringW& str, const SStringW& sSeperate)
{
    return GetTokenOp<WCHAR>::GetToken( str, sSeperate );
}

const BinData HexStrToBinData(const SString& str)
{
   LPCTSTR p = str.c_str();
   BinData result;
   SString s(str);
   result.SetCapacity( s.length() );
   while (p)
   {
      SString h = GetToken( s );
      if ( h.length() )
      {
         result.append( ToInt(h,16) );
      }
      else
         break;
   }
   return result;
}

int HexStrToBinary(const SString& str, LPBYTE buf, DWORD buflen)
{
   int cnt  = 0;
   LPCTSTR p = str.c_str();
   SString s(str);
   while ((cnt < (int)buflen) && p && *p)
   {
      SString h = GetToken( s );
      if ( h.length() )
      {
         int v = ToInt(h, 16);
         *buf++ = v;
         cnt++;
      }
      else
         break;
   }

   return (cnt);
}

const BinData AscHexStrToBinData(const SString& str)
{
   LPCTSTR p = str.c_str();
   BinData result;
   SString s(str);
   result.SetCapacity( s.length() );
   while (p && *p)
   {
      unsigned char ch = *p++;
      if (ch == _T('\\') ) {
         if (*p == _T('\\')) {
            result.append( ch );
            ++p;
         }
         else if ( *p!=0 && *(p+1)!=0) {
            SString h(p,2);
            result.append( ToInt(h,16) );
            p += 2;
         }
         else {
            // failed, ignore this '\\'
         }
      }
      else
         result.append( ch );
   }
   return result;
}

const SString BinaryToHexStr(const char * buf, DWORD buflen)
{
   SString result;

   int len = (int) buflen;
   for(; len>0; --len, ++buf)
   {
	  
      result += FormatString(_T("%02x "), * (unsigned char*) buf);
   }
   if (buflen>0)
      result.erase( result.length()-1 );

   return result;
}

const SStringA BinaryToHexStrA(const char * buf, DWORD buflen)
{
	SStringA result;

	int len = (int)buflen;
	for (; len>0; --len, ++buf)
	{

		result += FormatString("%02x ", *(unsigned char*)buf);
	}
	if (buflen>0)
		result.erase(result.length() - 1);

	return result;
}

const SString BinaryToAscHexStr(const char* buf, DWORD buflen)
{
    TCHAR tmp = _T('\0');
    SString result;

    int len = (int) buflen;
    const unsigned char* ptr = (const unsigned char*) buf;
    for (; len>0; --len, ++ptr) {
        if (*ptr == '\\')
            result += _T("\\\\");
        else if (isprint(*ptr))
        {
            * (char*) &tmp = *ptr;
            result.append(1, tmp );
        }
        else 
            result += FormatString(_T("\\%02x"), *ptr );
    }

    return result;
}

const SString BinDataToHexStr(const BinData& data)
{
   return BinaryToHexStr( data.buffer(), data.length() );
}

const SString BinDataToAscHexStr(const BinData& data)
{
   return BinaryToAscHexStr(data.buffer(), data.length());
}

//---------------------------------------------------------------------------
void SubtituteNoCase(SStringW& str, const SStringW& mark, const SStringW& ReplacyBy)
{
   SStringW _str = str;
   SStringW _mark = mark;

   _str.ToUpper();
   _mark.ToUpper();

   // find mark
   int pos = _str.find(_mark);
   while ( pos != str.npos ) {
      str.replace( pos, mark.length(), ReplacyBy);
      _str = str;
      _str.ToUpper();
      pos = _str.find(_mark);
   }
}
//---------------------------------------------------------------------------
void SubtituteNoCase(SStringA& str, const SStringA& mark, const SStringA& ReplacyBy)
{
   SStringA _str = str;
   SStringA _mark = mark;

   _str.ToUpper();
   _mark.ToUpper();
   // find mark
   int pos = _str.find(_mark);
   while ( pos != str.npos ) {
      str.replace( pos, mark.length(), ReplacyBy);
      _str = str;
      _str.ToUpper();
      pos = _str.find(_mark);
   }
}
//---------------------------------------------------------------------------
const SStringA FormatString(LPCSTR lpszFormat, ...)
{
   SStringA str;
   va_list argList;
   va_start( argList, lpszFormat);
   str.FormatV( lpszFormat, argList);
   va_end(argList);
   return str;
}
//---------------------------------------------------------------------------
const SStringW FormatString(LPCWSTR lpszFormat, ...)
{
   SStringW str;
   va_list argList;
   va_start( argList, lpszFormat);
   str.FormatV( lpszFormat, argList);
   va_end(argList);
   return str;
}
//---------------------------------------------------------------------------
const SStringA ConvertToCFormatString(const SStringA& string)
{
    // only support \\ \r \n \t \" \' \0
    static const char* szSearch = "\\rnt\"\'0";
    static const char* szReplace= "\\r\n\t\"\'\0";

    SStringA result = string;
    bool bDone = false;
    size_t pos = 0;
    while (!bDone)
    {
        pos = result.find('\\', pos);    // find escape character
        if (pos == result.npos)
            bDone = true;
        else 
        {   
            char next = result[ pos + 1];
            if ( next == '\0')
                bDone = true;
            else
            {
                // find match?
                const char* posSearch = strchr( szSearch, next );
                if (posSearch == NULL)
                {
                    // skip, not in support conversion
                }
                else {
                    result[pos] = szReplace[posSearch - szSearch];
                    result.erase( pos+1, 1 );
                }

                ++pos;
            }
        }
    }
    return result;
}
//---------------------------------------------------------------------------
const SStringW ConvertToCFormatString(const SStringW& string)
{
    // only support \\ \r \n \t \" \' \0
    static const wchar_t* szSearch = L"\\rnt\"\'0";
    static const wchar_t* szReplace= L"\\r\n\t\"\'\0";

    SStringW result = string;
    bool bDone = false;
    size_t pos = 0;
    while (!bDone)
    {
        pos = result.find(L'\\', pos);    // find escape character
        if (pos == result.npos)
            bDone = true;
        else 
        {   
            wchar_t next = result[ pos + 1];
            if ( next == _T('\0'))
                bDone = true;
            else
            {
                // find match?
                const wchar_t* posSearch = wcschr( szSearch, next );
                if (posSearch == NULL)
                {
                    // skip, not in support conversion
                }
                else {
                    result[pos] = szReplace[posSearch - szSearch];
                    result.erase( pos+1, 1 );
                }

                ++pos;
            }
        }
    }
    return result;
}
//---------------------------------------------------------------------------
const SStringW MakeLower(const SStringW& s)
{
   SStringW result(s);
   result.ToLower();
   return result;
}
//---------------------------------------------------------------------------
const SStringA MakeLower(const SStringA& s)
{
   SStringA result(s);
   result.ToLower();
   return result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const SStringW MakeUpper(const SStringW& s)
{
   SStringW result(s);
   result.MakeUpper();
   return result;
}
//---------------------------------------------------------------------------
const SStringA MakeUpper(const SStringA& s)
{
   SStringA result(s);
   result.MakeUpper();
   return result;
}
//---------------------------------------------------------------------------
const SStringA RemoveCommentString(SStringA& str, char comment_mark)
{
   SStringA cm;
   cm.Format( "\"%c", comment_mark);

   int pos = str.find_first_of(cm);
   while (pos != str.npos)
   {
      if ( str[pos] == '\"')
      { // find next "
         pos = str.find( '\"', pos+1);
         if (pos == str.npos)
            break;
      }
      else 
      { // reached the comment mark
         SStringA result = str.substr(pos);
         str.erase(pos);
         return result;
      }

      pos = str.find_first_of(cm, pos + 1);
   }
   return "";
}
//---------------------------------------------------------------------------
const SStringW RemoveCommentString(SStringW& str, WCHAR comment_mark)
{
   SStringW cm;
   cm.Format( L"\"%c", comment_mark);

   int pos = str.find_first_of(cm);
   while (pos != str.npos)
   {
      if ( str[pos] == L'\"')
      { // find next "
         pos = str.find( L'\"', pos+1);
         if (pos == str.npos)
            break;
      }
      else 
      { // reached the comment mark
         SStringW result = str.substr(pos);
         str.erase(pos);
         return result;
      }

      pos = str.find_first_of(cm, pos + 1);
   }
   return L"";
}
//---------------------------------------------------------------------------
