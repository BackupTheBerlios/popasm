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

#ifndef INP_FILE_H
#define INP_FILE_H

#include <fstream>
#include <string>
#include <stack>
#include <utility>

// Input files are like input streams, but return tokens instead
class InputFile : public ifstream
{
	typedef pair<string *, char> UngotString;

	// String containing the rest of the current line
	string RestOfLine;
	char NextChar;
	stack<UngotString> UngotStrings;

	public:
	InputFile (const char *FileName) : ifstream (FileName) {}
	~InputFile () {}

	// Return the next string found in input file. Returns 0 if EOF is reached.
	string *GetString ();
	// Puts the string back into the input file
	void UngetString (string *s) {UngetString (s, NextChar);}
	void UngetString (string *s, char c);
	// Checks if a space was found just after the string just read
	char GetNextChar () const {return NextChar;}
	// Skips all characters (including \n) until the beginning on the next line
	void SkipLine() {RestOfLine = "";}

	// Checks if a character is an alphanumeric one
	static bool AlphaNumeric (char c);
	// Returns if a character is between '0' and '9' inclusive
	static bool Digit (char c);
	// Checks if a character is a white space, teb, etc
	static bool Space (char c);
};

#endif
