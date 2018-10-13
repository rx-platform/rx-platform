

//	//***************************************
//	// I don't know whose code is this
//	// I don't know where did i get it
//	// It's a good peace of code!
//	//*****************************************







#include <stdio.h>
#include <ctype.h>

int ConvertCase(int c, int bCaseSensitive)
{
	return bCaseSensitive ? c : toupper(c);
}



//*************************************************************************
// return TRUE if String Matches Pattern --
// -- uses Visual Basic LIKE operator syntax
// CAUTION: Function is recursive
//*************************************************************************
int match_pattern(const char *string, const char *Pattern, int bCaseSensitive)
{
	if (!string)
		return 0;
	if (!Pattern)
		return 1;
	char   c, p, l;
	for (; ;)
	{
		switch (p = ConvertCase(*Pattern++, bCaseSensitive))
		{
		case 0:                             // end of pattern
			return *string ? 0 : 1;  // if end of string TRUE

		case '*':
			while (*string)
			{               // match zero or more char
				if (match_pattern(string++, Pattern, bCaseSensitive))
					return 1;
			}
			return match_pattern(string, Pattern, bCaseSensitive);

		case '?':
			if (*string++ == 0)             // match any one char
				return 0;                   // not end of string
			break;

		case '[':
			if ((c = ConvertCase(*string++, bCaseSensitive)) == 0)      // match char set
				return 0;                   // syntax
			l = 0;
			if (*Pattern == '!')  // match a char if NOT in set []
			{
				++Pattern;

				while ((p = ConvertCase(*Pattern++, bCaseSensitive)) != '\0')
				{
					if (p == ']')               // if end of char set, then
						break;           // no match found

					if (p == '-')
					{            // check a range of chars?
						p = ConvertCase(*Pattern, bCaseSensitive);   // get high limit of range
						if (p == 0 || p == ']')
							return 0;           // syntax

						if (c >= l  &&  c <= p)
							return 0;              // if in range, return FALSE
					}
					l = p;
					if (c == p)                 // if char matches this element
						return 0;                  // return false
				}
			}
			else	// match if char is in set []
			{
				while ((p = ConvertCase(*Pattern++, bCaseSensitive)) != '\0')
				{
					if (p == ']')               // if end of char set, then
						return 0;           // no match found

					if (p == '-')
					{            // check a range of chars?
						p = ConvertCase(*Pattern, bCaseSensitive);   // get high limit of range
						if (p == 0 || p == ']')
							return 0;           // syntax

						if (c >= l  &&  c <= p)
							break;              // if in range, move on
					}
					l = p;
					if (c == p)                 // if char matches this element
						break;                  // move on
				}

				while (p  &&  p != ']')         // got a match in char set
					p = *Pattern++;             // skip to end of set
			}

			break;

		case '#':
			c = *string++;
			if (!isdigit(c))
				return 0;		// not a digit

			break;

		default:
			c = ConvertCase(*string++, bCaseSensitive);
			if (c != p)            // check for exact char
				return 0;                   // not a match

			break;
		}
	}
}




