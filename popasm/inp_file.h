//--------------------------------------------------------------------------
//                          inp_file.h  -  description
//                             -------------------
//    begin                : Mon Apr 23 2001
//    copyright            : (C) 2001 by Helcio Mello
//    email                : helciomello@bol.com.br
//--------------------------------------------------------------------------

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


#ifndef INP_FILE_H
#define INP_FILE_H

#include <fstream>
#include <string>
#include <stack>
#include <utility>
#include <exception>

// Thrown when the user is in the first line and attempts to unget a '\n'
class UngetOverflow : public exception
{
	static string WhatString;

	public:
	UngetOverflow () {}
	~UngetOverflow () {}

	const char *what() const {return WhatString.c_str();}
};

// Input files are like input streams, but return tokens instead
class InputFile : ifstream
{
	typedef pair<string *, char> UngotString;

	// String containing the rest of the current line
	string RestOfLine;
	// The character found immediatelly after the last string read
	char NextChar;
	stack<UngotString> UngotStrings;

	// Line of code we are currently in. First line is 1, NOT 0.
	unsigned long int CurrentLine;

	public:
	InputFile (const char *FileName) throw () : ifstream (FileName), NextChar(0), CurrentLine(1) {}
	~InputFile () throw () {}

	bool operator! () const throw () {return ifstream::operator!();}
	void ResetFile () throw ();

	// Return the next string found in input file. Returns 0 if EOF is reached.
	string *GetString () throw ();
	// Puts the string back into the input file
	void UngetString (string *s) throw (UngetOverflow) {UngetString (s, NextChar);}
	void UngetString (string *s, char c) throw (UngetOverflow);
	// Returns the character that follows the string read the last time
	char GetNextChar () const throw () {return NextChar;}
	// Returns the current line of code. First line is 1, NOT 0.
	unsigned long int GetCurrentLine () const throw () {return CurrentLine;}
	// Skips all characters (including \n) until the beginning on the next line
	void SkipLine() throw ();

	// Checks if a character is an alphanumeric one
	static bool AlphaNumeric (char c) throw ();
	// Returns if a character is between '0' and '9' inclusive
	static bool Digit (char c) throw ();
	// Checks if a character is a white space, teb, etc
	static bool Space (char c) throw ();
};

#endif
