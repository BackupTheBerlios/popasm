/***************************************************************************
                          inp_file.cpp  -  description
                             -------------------
    begin                : Sat Apr 6 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//-------------------------------------------------------------------------------
// This file contains classes regarding source files and processing lines of code
//-------------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inp_file.h"

string UngetOverflow::WhatString = "Carriage return ungot from line 1.";

string *InputFile::GetString () throw ()
{
	string *s;

	// Returns last string ungot since last call (if any)
	if (!UngotStrings.empty())
	{
		UngotString us = UngotStrings.top();
		UngotStrings.pop();
		s = us.first;
		NextChar = us.second;

		if (*s == "\n") CurrentLine++;
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
	if (*s == "\n") CurrentLine++;
	return s;
}

void InputFile::UngetString (string *s, char c) throw (UngetOverflow)
{
	if (*s == "\n")
	{
		CurrentLine--;
		if (CurrentLine == 0) throw UngetOverflow();
	}

	UngotStrings.push (pair<string *, char> (s, c));
}

void InputFile::SkipLine() throw ()
{
	while (!UngotStrings.empty())
	{
		// Gets rid of the next string
		UngotString us = UngotStrings.top();
		UngotStrings.pop();

		// End of line reached?
		if (*us.first == "\n")
		{
			delete us.first;
			CurrentLine++;
			return;
		}

		// Deletes the next string and keeps scaning
		delete us.first;
	}

	// All ungot strings scanned but no end of line found. So let's scan the RestOfLine
	if (RestOfLine == "")
	{
		// Gets a new line if the previous one has been totally read.
		::getline (*this, RestOfLine);
	}

	// Get rid of the rest of line
	RestOfLine = "";
	NextChar = 0;
	CurrentLine++;
}

bool InputFile::AlphaNumeric (char c) throw ()
{
	return (((c >= '0') && (c <= '9')) ||
			  ((c >= 'A') && (c <= 'Z')) ||
			  ((c >= 'a') && (c <= 'z')) ||
			  (c == '_') || (c == '@') || (c == '$'));
}

bool InputFile::Space (char c) throw ()
{
	return ((c == ' ') || (c == '\t') || (c == '\r'));
}

bool InputFile::Digit (char c) throw ()
{
	return ((c >= '0') && (c <= '9'));
}
