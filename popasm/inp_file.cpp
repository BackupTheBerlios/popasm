/***************************************************************************
                          inp_file.cpp  -  description
                             -------------------
    begin                : Sat Apr 6 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inp_file.h"

string *InputFile::GetString ()
{
	string *s;

	// Returns last string ungot since last call (if any)
	if (!UngotStrings.empty())
	{
		s = UngotStrings.top().first;
		NextChar = UngotStrings.top().second;
		UngotStrings.pop();
		return s;
	}

	// If there's nothing left from the previous line, let's read another one
	if (RestOfLine.empty())
	{
		// But first checks if EOF has been reached
		if (*this)
		{
			// Reads a new line and append a \n to it
			::getline (*this, RestOfLine);
			RestOfLine += "\n";
		}
		else
		{
			// Return EOF
			return 0;
		}
	}

	// Start scanning from the beginning
	string::iterator i = RestOfLine.begin();

	// Skip whitespaces and tabs
	while (Space (*i)) i++;

	// Is it an alphanumeric string or a special char like '[', '.', etc.?
	if (!AlphaNumeric (*i))
	{
		s = new string (i, i + 1);
		RestOfLine = string (++i, RestOfLine.end());
	}
	else
	{
		// Saves in j the start point of the token string
		string::iterator j = i;

		// If the first character is alphanumeric, the token will consist of the
		// substring starting from j up to the first non-alphanumeric character
		while (AlphaNumeric (*i)) i++;

		// TokenString contains all the characters until before the first
		// non-alphanumeric character. RestOfLine will hold the
		// remaining of the string
		s = new string (j, i);
		RestOfLine = string (i, RestOfLine.end());
	}

	NextChar = (RestOfLine == "") ? 0 : RestOfLine[0];
	return s;
}

void InputFile::UngetString (string *s, char c)
{
	UngotStrings.push (pair<string *, char> (s, c));
}

bool InputFile::AlphaNumeric (char c)
{
	return (((c >= '0') && (c <= '9')) ||
			  ((c >= 'A') && (c <= 'Z')) ||
			  ((c >= 'a') && (c <= 'z')) ||
			  (c == '_') || (c == '@') || (c == '$'));
}

bool InputFile::Space (char c)
{
	return ((c == ' ') || (c == '\t') || (c == '\r'));
}

bool InputFile::Digit (char c)
{
	return ((c >= '0') && (c <= '9'));
}
