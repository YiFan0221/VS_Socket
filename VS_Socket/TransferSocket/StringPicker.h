//---------------------------------------------------------------------------
#ifndef __STRINGPICKER_H
#define __STRINGPICKER_H
//---------------------------------------------------------------------------
#include "SString.h"
#include <vector>
//---------------------------------------------------------------------------
template <class _S>
class SStringPicker
{
public:

    SStringPicker(const _S& left = _S( _T("{{") ),
        const _S& right = _S( _T("}}")) ) 
        : lMark(left), rMark(right) 
    {
    }

    ~SStringPicker(void)
    {
        Clear();
    }

    int Restore(_S& str) const
    {
        // find LMark and RMark
        int cnt = 0;
        int b, e;
        int mark_len = lMark.length() + rMark.length();
        while ( ((b = str.find(lMark)) != str.npos) 
            &&((e = str.find(rMark, b+lMark.length())) !=str.npos) ) {

                // get index.. {{index}}
                int len = e - b + rMark.length();
                if ( len >= (mark_len+1) ) { // at least, five character(s)
                    _S s = str.substr(b+lMark.length(), len - mark_len);

                    // replace it
                    str.replace( b, len, refs[ToInt(s, 10)]);
                    cnt++;
                }
            }

            return (cnt);
    }

    //----------------------------------------------------------
    int Pick(_S& str, const _S& leftMark, const _S& rightMark)
    {
        int cnt = 0;
        int b = 0;
        _S buf;
        while ( (b = str.find(leftMark)) != str.npos) {
            // we have find first leftMark "<"
            int e = str.find(rightMark, b+leftMark.length());
            if (e != str.npos) {
                // reverse find a leftMark
                b = str.rfind(leftMark, e-1);
            }

            // Add to subtitue, any way
            refs.push_back( str.substr(b, (e==str.npos) ? e : e-b+rightMark.length() ) );
            int index = refs.size() - 1;

            // replace it with mark
            buf.Format(_S(_T("%s%d%s")).c_str(), lMark.c_str(), index, rMark.c_str());
            str.replace(b, ((e != str.npos) ? (e-b+rightMark.length()) : (str.length() - b)), buf);
            cnt++;
        }
        return (cnt);
    }

    //----------------------------------------------------------
    int MakeFields(const _S& s, const _S& Delimeter, WORD MaxFields=0)
    {
        _S str = s;
        WORD cnt = 0;
        int len_del = Delimeter.size();
        if ( str.length() != 0 && len_del!=0) {
            _S trimMark = Delimeter + _S(_T(" \t"));
            int b;
            while ( (MaxFields==0 || (cnt+1<MaxFields))  && ((b=str.find(Delimeter))!=str.npos)) {
                _S s = str.substr(0,b);
                //      if (s.length()) {
                fields.push_back( Trim(s, trimMark) );
                cnt++;
                //      }
                str.erase(0, b + len_del);
            }

            fields.push_back(Trim(str, trimMark));
            cnt++;
        }

        return (cnt);
    }

    //---------------------------------------------------------------------------
    const _S GetField(WORD index) const
    {
        if ( index < fields.size() ) {
            _S s = fields[index];
            Restore(s);
            return s;
        }
        else
            return _S(_T(""));
    }

    //---------------------------------------------------------------------------
    int GetFieldBoolean(WORD index, int def=-1) const   // return 1(TRUE), 0(FALSE), -1(INVALID)
    {
        int rtn = def;
        if ( index < fields.size() )
        {
            _S s = fields[index];
            Restore(s);
            if ( _S(_T("TtYy") ).find( s[0] ) != _S::npos )
                rtn = 1;
            else if (_S(_T("FfNn")).find(s[0]) != _S::npos )
                rtn = 0;
        }
        return rtn;
    }

    //---------------------------------------------------------------------------
    int GetFieldInteger(WORD index, int base=10, int def=0) const
    {
        if ( index < fields.size() ) {
            _S s = fields[index];
            Restore(s);
            return ToInt(s, base);
        }
        else
            return def;
    }

    //---------------------------------------------------------------------------
    void PutField(WORD index, const _S& string)
    {
        assert(index<fields.size());

        if ( (index>=0) && (index<fields.size()) )
            fields[index] = string;
    }

    //---------------------------------------------------------------------------
    const _S PopField(void) 
    {
        _S str = GetField(0);
        if (!fields.empty())
            fields.erase(fields.begin());

        return str;
    }

    //---------------------------------------------------------------------------
    int GetFieldCount(void) const
    {
        return fields.size();
    }

    //---------------------------------------------------------------------------
    void Clear(void)
    {
        refs.clear();
        fields.clear();
    }

protected:
    const _S lMark;   // {{
    const _S rMark;   // }}

    std::vector<_S> refs;    // reference string
    std::vector<_S> fields;
};
//---------------------------------------------------------------------------
typedef SStringPicker<SStringW> SStringPickerW;
typedef SStringPicker<SStringA> SStringPickerA;

#ifdef _UNICODE
typedef SStringPickerW StringPicker;
#else
typedef SStringPickerA StringPicker;
#endif
//---------------------------------------------------------------------------
#endif // __STRINGPICKER_H


