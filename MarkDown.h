
#include <stdlib.h>
#include <bits/stdc++.h>

using namespace std;

typedef vector< string > vs;
typedef vector< pair< string, string > > vpss;

vpss url_index;

class MD_Block
{
	public:
		virtual bool parse( vs *md_data ) {
			return true;
		}

	protected:
		string form_string( vs *md_data, char sep )
		{
			string str = "";
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
				str += *it + sep;
			return str;
		}

		vs split( string str, char sep )
		{
			vs tokens;
			string token;
			istringstream tokenStream( str );

			while( getline( tokenStream, token, sep ))
				tokens.push_back( token );

			return tokens;
		}

		int find_outside_tags( string needle, string haystack, int idx )
		{
			if ( idx == -1 )
				return -1;

			int htag  = haystack.rfind( "<" , idx );
			int cblk  = haystack.rfind( "```", idx );

			if ( htag > cblk )
			{
				if ( htag != -1 && ( haystack.at( htag+1 ) == 'a' || haystack.at( htag+1 ) == 'i' || haystack.at( htag+1 ) == 'b' ))
				{
					htag = haystack.find( '>', htag+1  );
					if ( htag > idx )
						idx = haystack.find( needle, htag+1 );
				}
			}
			else if ( cblk > -1 )
			{
				cblk = haystack.find( "```", cblk+1 );
				if ( cblk > idx )
					idx = haystack.find( needle, cblk+1 );
			}
			
			return idx;
		}
};

class UrlIndex: public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).at( 0 ) == '[' )
				{
					int idx = (*it).find( "]:", 1 );
					if ( idx == -1 )
						continue;

					string ref = (*it).substr( 1, idx-1 );
					string url = (*it).substr( idx+2, (*it).length() - idx -2 );

					url_index.push_back( make_pair( ref, url ));

					(*it).clear();
				}
			}

			return true;
		}
};

class HorizontalRule : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ((*it).length() != 3 )
					continue;

				if ((*it).find( "---" ) == 0 || (*it).find( "***" ) == 0 )
					(*it).replace( 0, 3, "<hr>" );
			}
			return true;
		}
};

class InlineCode : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() < 2 )
					continue;

				if ( (*it).at( 0 ) == '`' && (*it).at(1) != '`' )
				{
					int idx = (*it).find( '`', 1 );
					if ( idx != -1 )
					{
						(*it).replace( idx, 1, "</code>" );
						(*it).replace( 0, 1, "<code>" );
					}
				}
			}

			return true;
		}
};

class CodeBlock : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			int found = 0;
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() < 3 )
					continue;

				if ( (*it).at(0) == '`' && (*it).at(1) == '`' && (*it).at(2) == '`' )
				{
					if ( !found )
					{
						(*it).replace( 0, (*it).length(), "<pre class='code_block'>" );
						found = 1;
					}
					else if ( found == 1 )
					{
						(*it).replace( 0, (*it).length(), "</pre>" );
						found = 0;
					}
				}
				else if ( (*it).at(0) == ' ' && (*it).at(1) == ' ' && (*it).at(2) == ' ' && (*it).at(3) == ' ' )
				{
					if ( !found )
					{
						(*it).insert( 0, "<pre class='code_block'>" );
						found = 2;
					}
				}
				else if ( found == 2 )
				{
					(*(it-1)).insert( (*(it-1)).length(), "</pre>" );
					found = 0;
				}
			}
			return true;
		}
};

class Image : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				while( 1 )
				{
					int idx1 = (*it).find( "![Image]" );
					int idx2 = (*it).find( '(', idx1+8 );
					int idx3 = (*it).find( ')', idx2+1 );
					bool old = false;

					if ( idx1 == -1 )
						break;

					if ( idx2 == -1 || idx3 == -1 )
					{
						old  = true;
						idx2 = (*it).find( '[', idx1+8 );
						idx3 = (*it).find( ']', idx2+1 );

						if ( idx2 == -1 || idx3 == -1 )
							break;
					}

					string src = (*it).substr( idx2+1, idx3-idx2-1 );

					if ( old )
					{
						for ( vpss::iterator vit = url_index.begin(); vit != url_index.end(); vit++ )
						{
							if ( (*vit).first == src )
							{
								src = (*vit).second;
								break;
							}
						}
					}

					string str = "<img src=\"" + src + "\">";
					(*it).replace( idx1, idx3-idx1+1, str );
				}
			}

			return true;
		}
};
class Link : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				while( 1 )
				{
					bool old = false;
					int idx1 = (*it).find( '[' );
					int idx2 = (*it).find( ']', idx1+1 );
					int idx3 = (*it).find( '(', idx2+1 );
					int idx4 = (*it).find( ')', idx3+1 );

					if ( idx1 == -1 || idx2 == -1 )
						break;
				
					if ( idx3 == -1 || idx4 == -1 )
					{
						old = true;
						idx3 = (*it).find( '[', idx2+1 );
						idx4 = (*it).find( ']', idx3+1 );

						if ( idx3 == -1 || idx4 == -1 )
							break;
					}

					string link = (*it).substr( idx1+1, idx2-idx1-1 );
					string href = (*it).substr( idx3+1, idx4-idx3-1 );

					if ( old )
					{
						for ( vpss::iterator vit = url_index.begin(); vit != url_index.end(); vit++ )
						{
							if ( (*vit).first == href )
							{
								href = (*vit).second;
								break;
							}
						}
					}

					string str  = "<a href='" + href + "'>" + link + "</a>";
					(*it).replace( idx1, idx4-idx1+1, str );
				}
			}

			return true;
		}
};

class BlockQuote : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			bool start = false;
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() == 0 )
					continue;

				if ( (*it).at( 0 ) == '>' )
				{
					if ( !start )
					{
						(*it).replace( 0, 1, "<blockquote class='block_quote'><p>" );
						(*it).insert( (*it).length(), "</p>" );
						start = true;
					}
					else
					{
						(*it).replace( 0, 1, "<p>" );
						(*it).insert( (*it).length(), "</p>" );
					}
				}
				else if ( start )
				{
					(*(it-1)).insert( (*(it-1)).length(), "</blockquote>" );
					start = false;
				}
			}

			return true;
		}
};

class Heading_H6 : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() < 7 )
					continue;

				const char *b = (*it).c_str();
				if ( b[0] == '#' && b[1] == '#' && b[2] == '#' && b[3] == '#' && b[4] == '#' && b[5] == '#' && b[6] == ' ' )
				{
					(*it).replace( 0, 7, "<h6>" );
					(*it).insert( (*it).length(), "</h6>" );
				}
			}
			return true;
		}
};
class Heading_H5 : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() < 6 )
					continue;

				const char *b = (*it).c_str();
				if ( b[0] == '#' && b[1] == '#' && b[2] == '#' && b[3] == '#' && b[4] == '#' && b[5] == ' ' )
				{
					(*it).replace( 0, 6, "<h5>" );
					(*it).insert( (*it).length(), "</h5>" );
				}
			}
			return true;
		}
};

class Heading_H4 : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() < 5 )
					continue;

				const char *b = (*it).c_str();
				if ( b[0] == '#' && b[1] == '#' && b[2] == '#' && b[3] == '#' && b[4] == ' ' )
				{
					(*it).replace( 0, 5, "<h4>" );
					(*it).insert( (*it).length(), "</h4>" );
				}
			}
			return true;
		}
};

class Heading_H3 : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() < 4 )
					continue;

				const char *b = (*it).c_str();
				if ( b[0] == '#' && b[1] == '#' && b[2] == '#' && b[3] == ' ' )
				{
					(*it).replace( 0, 4, "<h3>" );
					(*it).insert( (*it).length(), "</h3>" );
				}
			}
			return true;
		}
};

class Heading_H2 : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() < 3 )
					continue;

				if ( (*it).at(0) == '#' && (*it).at(1) == '#' && (*it).at(2) == ' ' )
				{
					(*it).replace( 0, 3, "<h2>" );
					(*it).insert( (*it).length(), "</h2>" );
				}
				else
				{
					int i = 0;
					for ( ; i<(*it).length(); i++ )
						if ( (*it).at( i ) != '-' )
							break;

					if ( i && i == (*it).length() && it != md_data->begin() )
					{
						(*(it-1)).insert( 0, "<h2>" );
						(*(it-1)).insert( (*(it-1)).length(), "</h2>" );
						(*it).clear();
					}
				}
			}
			return true;
		}
};

class Heading_H1 : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			for ( vs::iterator it = md_data->begin(); it != md_data->end(); it++ )
			{
				if ( (*it).length() < 2 )
					continue;

				if ( (*it).at(0) == '#' && (*it).at(1) == ' ' )
				{
					(*it).replace( 0, 2, "<h1>" );
					(*it).insert( (*it).length(), "</h1>" );
				}
				else
				{
					int i = 0;
					for ( ; i<(*it).length(); i++ )
						if ( (*it).at( i ) != '=' )
							break;

					if ( i && i == (*it).length() && it != md_data->begin() )
					{
						(*(it-1)).insert( 0, "<h1>" );
						(*(it-1)).insert( (*(it-1)).length(), "</h1>" );
						(*it).clear();
					}
				}
			}
			return true;
		}
};

class List : public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			bool start = false;
			for ( int idx = 0; idx < md_data->size(); idx++ )
			{
				string *str = &( md_data->at( idx ));
				int min		= find_min( *str );

				int indent = 0;
				for ( ; indent < min; indent++ )
					if ( str->at( indent ) != ' ' )
						break;

				string tmp = str->substr( indent, min-indent );

				char number[10];
				sprintf( number, "%d", atoi( tmp.c_str() ));
				string num_str  = number;

				idx = form_list( md_data, idx, indent, num_str.at(0) > '0' );
			}

			return true;
		}

		int find_min( string str )
		{
			int dot 	= str.find( ". " );
			int bracket = str.find( ") " );
			int star	= str.find( "* " );
			int dash	= str.find( "- " );

			if ( dot == -1 )	 dot	 = 10000;
			if ( bracket == -1 ) bracket = 10000;
			if ( star == -1 ) 	 star	 = 10000;
			if ( dash == -1 )	 dash	 = 10000;

			if ( dot > bracket ) dot	 = bracket;
			if ( dot > star )	 dot	 = star;
			if ( dot > dash )	 dot	 = dash;

			return ( dot == 10000 ) ? -1 : dot;
		}

		int form_list( vs *md_data, int idx, int indent, bool numbered = false )
		{
			bool start = false;

			while( idx < md_data->size() )
			{
				string *str = &( md_data->at( idx ));
				int min		= find_min( *str );

				int i = 0;
				for ( ; i<min; i++ )
					if ( str->at( i ) != ' ' )
						break;

				string tmp = str->substr( i, min-i );

				char number[10];
				sprintf( number, "%d", atoi( tmp.c_str() ));
				string num_str  = number;

				if ( !( num_str.at(0) > '0' ))
					num_str.clear();

				min -= num_str.length();

				if ( min == i && min == indent )
				{
					if ( !start )
					{
						if ( num_str.length() > 0 )
							str->replace( 0, min+num_str.length()+2, "<ol><li>" );
						else
							str->replace( 0, min+2, "<ul><li>" );
						start = true;
					}
					else
						str->replace( 0, min+num_str.length()+2, "</li><li>" );
				}
				else if ( min == i && min > indent )
				{
					idx = form_list( md_data, idx, i, num_str.length() > 0 );
					continue;
				}
				else if ( start )
				{
					string *tmp = &( md_data->at( idx - 1 ));
					if ( numbered )
						tmp->insert( tmp->length(), "</li></ol>" );
					else
						tmp->insert( tmp->length(), "</li></ul>" );
					return idx;
				}

				idx++;
			}

			return idx;
		}
};

class Bold: public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			string str = form_string( md_data, '\n' );

			int pos = 0;
			while( 1 )
			{
				int idx1 = find_outside_tags( "**", str, str.find( "**", pos ));
				int idx2 = find_outside_tags( "__", str, str.find( "__", pos ));

				if ( idx1 == -1 && idx2 == -1 )
					break;

				bool star = true;
				if ( idx1 == -1 )
				{
					idx1 = idx2;
					star = false;
				}

				if ( idx2 == -1 )
					idx2 = idx1;

				if ( idx1 > idx2 )
				{
					idx1 = idx2;
					star = false;
				}

				int idx = find_outside_tags( star ? "**" : "__", str, str.find( star ? "**" : "__", idx1+1 ));

				if ( idx == -1 )
				{
					pos = idx1+1;
					continue;
				}
			
				if ( idx > idx1 )
				{
					while( 1 )
					{
						int tmp1 = find_outside_tags( star ? "__" : "**", str, str.find( star ? "__" : "**", idx1+1 ));
						if ( tmp1 == -1 || tmp1 > idx )
							break;

						int tmp2 = find_outside_tags( star ? "__" : "**", str, str.find( star ? "__" : "**", tmp1+1 ));

						if ( tmp2 != -1 && tmp2 < idx )
						{
							str.erase( tmp2, 1 );
							str.erase( tmp1, 1 );
							idx -= 2;
						}
						else
							break;
					}
				}

				str.replace( idx, 1, "</b>" );
				str.replace( idx1, 1, "<b>" );
				pos = idx + 7;
			}

			vs tmp = split( str, '\n' );
			md_data->clear();

			for ( vs::iterator it = tmp.begin(); it != tmp.end(); it++ )
				md_data->push_back( *it );

			return true;
		}
};

class Italic: public MD_Block
{
	public:
		bool parse( vs *md_data )
		{
			string str = form_string( md_data, '\n' );

			int pos = 0;
			while( 1 )
			{
				int idx1 = find_outside_tags( "*", str, str.find( '*', pos ));
				int idx2 = find_outside_tags( "_", str, str.find( '_', pos ));

				if ( idx1 == -1 && idx2 == -1 )
					break;

				bool star = true;
				if ( idx1 == -1 )
				{
					idx1 = idx2;
					star = false;
				}

				if ( idx2 == -1 )
					idx2 = idx1;

				if ( idx1 > idx2 )
				{
					idx1 = idx2;
					star = false;
				}

				int idx = find_outside_tags( star ? "*" : "_", str, str.find( star ? "*" : "_", idx1+1 ));

				if ( idx == -1 )
				{
					pos = idx1+1;
					continue;
				}
			
				if ( idx > idx1 )
				{
					if ( str.at( idx1+1 ) == ( star ? '*' : '_' ))
					{
						pos = idx+2;
						continue;
					}

					while( 1 )
					{
						int tmp1 = find_outside_tags( star ? "_" : "*", str, str.find( star ? "_" : "*", idx1+1 ));
						if ( tmp1 == -1 || tmp1 > idx )
							break;

						int tmp2 = find_outside_tags( star ? "_" : "*", str, str.find( star ? "_" : "*", tmp1+1 ));

						if ( tmp2 != -1 && tmp2 < idx )
						{
							str.erase( tmp2, 1 );
							str.erase( tmp1, 1 );
							idx -= 2;
						}
						else
							break;
					}
				}

				str.replace( idx, 1, "</em>" );
				str.replace( idx1, 1, "<em>" );
				pos = idx + 7;
			}

			vs tmp = split( str, '\n' );
			md_data->clear();

			for ( vs::iterator it = tmp.begin(); it != tmp.end(); it++ )
				md_data->push_back( *it );

			return true;
		}

};

class MarkDown
{
	public:
		MarkDown();
		~MarkDown();

		string MarkDown_Text( string text );
	private:
		vector< MD_Block* > blocks;
};

MarkDown::MarkDown()
{
	blocks.push_back( new UrlIndex() );
	blocks.push_back( new Image() );
	blocks.push_back( new Link() );
	blocks.push_back( new HorizontalRule() );
	blocks.push_back( new BlockQuote() );
	blocks.push_back( new Heading_H6() );
	blocks.push_back( new Heading_H5() );
	blocks.push_back( new Heading_H4() );
	blocks.push_back( new Heading_H3() );
	blocks.push_back( new Heading_H2() );
	blocks.push_back( new Heading_H1() );
	blocks.push_back( new List() );
	blocks.push_back( new Bold() );
	blocks.push_back( new Italic() );
	blocks.push_back( new CodeBlock() );
	blocks.push_back( new InlineCode() );
}

MarkDown::~MarkDown()
{
}

string MarkDown::MarkDown_Text( string text )
{
	vs tokens;
	string token;
	istringstream tokenStream( text );

	while( getline( tokenStream, token, '\n' ))
		tokens.push_back( token );

	for ( vector< MD_Block* >::iterator it = blocks.begin(); it != blocks.end(); it++ )
		(*it)->parse( &tokens );

	string ret_str = "";

	for ( vs::iterator it = tokens.begin(); it != tokens.end(); it++ )
		ret_str += *it + '\n';

	return ret_str;
}

