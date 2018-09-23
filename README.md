# Programming


##### simple_md_httpd_server.py
> cd to directory which contains md file(s)
>
> python /path/to/simple_md_httpd_server.py
>
> Browser: http://127.0.0.1:8080/
>

##### MarkDown.h
```
#include "MarkDown.h"

int main()
{
	MarkDown md;
        cout << md.MarkDown_Text( "> MarkDown Text" );

	return 0;
}
```
>
> gcc test.cpp -lstdc++
>

##### CodeHighlighter.h
```cpp
#include "CodeHighlighter.h"

int main()
{
        FileIO f;
        BrushCpp brush;

        vector<wchar_t> data   = f.loadFile( "example.cpp" );
        vector<wchar_t> result = brush.search_html_replace( data );

        wcout << L"<!DOCTYPE html><html><head><style type=\"text/css\">" << endl;
        wcout << L".strings { color: blue; }" << endl;
        wcout << L".datatypes { color: green; font-weight: bold; }" << endl;
        wcout << L".keywords { color: red; }" << endl;
        wcout << L".preprocessors { color: yellow; font-weight: bold; }" << endl;
        wcout << L".functions { color: purple; font-weight: bold; }" << endl;
        wcout << L".comments { color: gray; }" << endl;
        wcout << L"</style></head><body><pre>" << endl;

        for ( auto item: result )
        {
                wcout << item;
        }

        wcout << L"</pre></body></html>" << endl;
        return 0;
}
```
>
> gcc test.cpp -lstdc++ -fpermissive
>
> Note: gcc version need to be 8.2 or above
>

