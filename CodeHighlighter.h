
#include <bits/stdc++.h>
#include <stdio.h>
#include <regex>

using namespace std;

typedef vector< pair< wstring, wstring > > vpww;
typedef vector< pair< int, pair < int, wstring > > > vpipiw;
typedef regex_iterator< vector< wchar_t>::const_iterator > rvwc;

class FileIO
{
	public:
		FileIO()  {};
		~FileIO() {};
 
		vector<wchar_t> loadFile( string filename )
		{
			vector<wchar_t> wdata;

			FILE *fp = fopen( filename.c_str(), "r" );

			if ( !fp )
			{
				cerr << "Error opening file [" << filename << "]" << endl;
				return wdata;
			}

			fseek( fp, 0, SEEK_END );
			int filesize = ftell( fp );
			fseek( fp, 0, SEEK_SET );

			vector<char> data;
			data.resize( filesize );
 
			fread( &data.front(), 1, filesize, fp );
			fclose( fp );

			int wsize = mbstowcs( NULL, &data.front(), filesize );
			wdata.resize( wsize );
			wsize	  = mbstowcs( &wdata.front(), &data.front(), filesize );

			return wdata;
		}
};

class Brush
{
	private:
		vpww patterns;

	public:
		Brush()  {};
		~Brush() {};

		vpipiw search( vector< wchar_t > data )
		{
			vpipiw result;

			for ( auto pat: patterns )
				_search( pat.first, data, result, pat.second );
			
			return result;
		}

		vector< wchar_t > search_html_replace( vector< wchar_t > data )
		{
			int pos = 0;
			vector<wchar_t> result;
			vpipiw found_matches = search( data );

			for ( auto item: found_matches )
			{
				if ( pos > item.first )
					continue;

				for ( int i=pos; i<item.first; i++ )
				{
					if ( data[i] == L'<' )      _append( result, L"&lt;" );
					else if ( data[i] == L'>' ) _append( result, L"&gt;" );
					else
						result.push_back( data[i] );
				}

				_append( result, L"<span class='" + item.second.second + L"'>" );
				
				int i=0;
				for ( ; i<item.second.first; i++ )
				{
					wchar_t c = data[ item.first+i ];
					if ( c == L'(' && i==item.second.first-1 )
						break;
					if ( c == L'<' )      _append( result, L"&lt;" );
					else if ( c == L'>' ) _append( result, L"&gt;" );
					else
						result.push_back ( c );
				}

				_append( result, L"</span>" );

				if ( i != item.second.first )
					result.push_back( L'(' );

				pos = item.first + item.second.first;
			}

			for ( int i=pos; i<data.size(); i++ )
			{
				if ( data[i] == L'<' )      _append( result, L"&lt;" );
				else if ( data[i] == L'>' ) _append( result, L"&gt;" );
				else
					result.push_back( data[i] );
			}

			return result;
		}

	protected:
		wstring convertKeywords( wstring str )
		{
			auto it = str.begin();
			while ( it != str.end() )
			{
				if ( *(it-1) == L' ' && *it == L' ' )
					str.erase( it );
				else
					it++;
			}

			if ( !str.empty() && str.front() == L' ' )
				str.erase( 0 );
			if ( !str.empty() && str.back() == L' ' )
				str.erase( str.end() - 1 );

			std::replace( str.begin(), str.end(), L' ', L'|' );
			return L"\\b(?:" + str + L")\\b";
		}

		void addPattern( wstring pattern, wstring css_class )
		{
			patterns.push_back( make_pair( pattern, css_class ));
		}

	private:
		void _search( wstring needle, vector<wchar_t>& content, vpipiw& found_matches, wstring css_class)
		{
			wregex regEx( needle );
			rvwc matcher = rvwc( content.begin(), content.begin()+content.size(), regEx );

			int pos = 0;
			while( matcher != rvwc() )
			{
				vpipiw::iterator it = found_matches.begin();

				for ( ; it!=found_matches.end(); it++ )
				{
					if ( it->first > matcher->position() )
						break;
				}

				found_matches.insert( it, make_pair( matcher->position(), make_pair( matcher->length(), css_class )));

				matcher++;
			}
		}

		void _append( vector<wchar_t>& dest, wstring src )
		{
			for ( int i=0; i<src.length(); i++ )
				dest.push_back( src[i] );
		}

};

class BrushCpp: public Brush
{
	public:
		BrushCpp(): Brush()
		{
			addPattern( L"/\\*[\\s\\S]*?\\*/", L"comments" );
			addPattern( L"//.*", L"comments" );
			addPattern( convertKeywords( datatypes ), L"datatypes" );
			addPattern( convertKeywords( keywords ),  L"keywords" );
			addPattern( L"L?" L"\"([^\\\\\"\\r\\n]|\\\\.)*\"", L"strings" );
			addPattern( L"L?" L"'([^\\\\'\\r\\n]|\\\\.)*'", L"strings" );
			addPattern( L"include|define|pragma|once", L"preprocessors" );
			addPattern( L"([<]([^\\\\'\\r\\n]|\\\\.)*[>])", L"include" );
			addPattern( L"(\\b[0-9a-zA-Z_]+[\\s]*\\\()", L"functions" );
		}

	private:
        wstring datatypes = L"ATOM BOOL BOOLEAN BYTE CHAR COLORREF DWORD DWORDLONG DWORD_PTR "
            L"DWORD32 DWORD64 FLOAT HACCEL HALF_PTR HANDLE HBITMAP HBRUSH "
            L"HCOLORSPACE HCONV HCONVLIST HCURSOR HDC HDDEDATA HDESK HDROP HDWP "
            L"HENHMETAFILE HFILE HFONT HGDIOBJ HGLOBAL HHOOK HICON HINSTANCE HKEY "
            L"HKL HLOCAL HMENU HMETAFILE HMODULE HMONITOR HPALETTE HPEN HRESULT "
            L"HRGN HRSRC HSZ HWINSTA HWND INT INT_PTR INT32 INT64 LANGID LCID LCTYPE "
            L"LGRPID LONG LONGLONG LONG_PTR LONG32 LONG64 LPARAM LPBOOL LPBYTE LPCOLORREF "
            L"LPCSTR LPCTSTR LPCVOID LPCWSTR LPDWORD LPHANDLE LPINT LPLONG LPSTR LPTSTR "
            L"LPVOID LPWORD LPWSTR LRESULT PBOOL PBOOLEAN PBYTE PCHAR PCSTR PCTSTR PCWSTR "
            L"PDWORDLONG PDWORD_PTR PDWORD32 PDWORD64 PFLOAT PHALF_PTR PHANDLE PHKEY PINT "
            L"PINT_PTR PINT32 PINT64 PLCID PLONG PLONGLONG PLONG_PTR PLONG32 PLONG64 POINTER_32 "
            L"POINTER_64 PSHORT PSIZE_T PSSIZE_T PSTR PTBYTE PTCHAR PTSTR PUCHAR PUHALF_PTR "
            L"PUINT PUINT_PTR PUINT32 PUINT64 PULONG PULONGLONG PULONG_PTR PULONG32 PULONG64 "
            L"PUSHORT PVOID PWCHAR PWORD PWSTR SC_HANDLE SC_LOCK SERVICE_STATUS_HANDLE SHORT "
            L"SIZE_T SSIZE_T TBYTE TCHAR UCHAR UHALF_PTR UINT UINT_PTR UINT32 UINT64 ULONG "
            L"ULONGLONG ULONG_PTR ULONG32 ULONG64 USHORT USN VOID WCHAR WORD WPARAM WPARAM WPARAM "
            L"char bool short int __int32 __int64 __int8 __int16 long float double __wchar_t "
            L"clock_t _complex _dev_t _diskfree_t div_t ldiv_t _exception exception _EXCEPTION_POINTERS "
            L"FILE _finddata_t _finddatai64_t _wfinddata_t _wfinddatai64_t __finddata64_t "
            L"__wfinddata64_t _FPIEEE_RECORD fpos_t _HEAPINFO _HFILE lconv intptr_t _locale_t "
            L"jmp_buf mbstate_t _off_t _onexit_t _PNH ptrdiff_t _purecall_handler "
            L"sig_atomic_t size_t _stat __stat64 _stati64 terminate_function "
            L"time_t __time64_t _timeb __timeb64 tm uintptr_t _utimbuf "
            L"va_list wchar_t wctrans_t wctype_t wint_t signed "
            L"std const_iterator deque iterator list multimap map pair set string stringstream vector wstring wstringstream";

		wstring keywords = L"break case catch class const __finally __exception __try "
            L"const_cast continue private public protected __declspec "
            L"default delete deprecated dllexport dllimport do dynamic_cast "
            L"else enum explicit extern if for friend goto inline "
            L"mutable naked namespace new noinline noreturn nothrow "
            L"register reinterpret_cast return selectany "
            L"sizeof static static_cast struct switch template "
            L"thread throw try typedef typeid typename union "
            L"using uuid virtual void volatile whcar_t while";
};

