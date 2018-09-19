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
