/***************************************************************************
                          symbol.h  -  description
                             -------------------
    begin                : Tue Jun 4 2002
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

#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "lexnum.h"

// Contains data necessary to describe a basic symbol (i.e. one which has no type)
class BasicSymbol
{
	string Name;

	public:
	BasicSymbol (const string &n) : Name(n) {}
	virtual ~BasicSymbol () {}

	const string &GetName () const throw () {return Name;}
	string Print () const throw () {return Name;}

	// This method is used to compare symbols' names, to put them in lexicographical order in a set
	virtual bool operator< (const BasicSymbol &s) const throw () {return Name < s.Name;}
};

/*
class Segment : public BasicSymbol
{
	public:
	Segment (const string &n) : BasicSymbol (n) {}
	~Segment () {}

	void Print () const;
};

class Variable : public BasicSymbol
{
	unsigned int Size;		// Variable size in bits
	Number Offset;				// Offset within its segment

	public:
	Variable (const string &n, const Number &off, unsigned int sz = 0) : BasicSymbol(n), Size(sz), Offset(off) {}
	~Variable () {}

	const Number &GetOffset () const throw () {return Offset;}
	void Print () const;
};

class Constant : public BasicSymbol
{
	Number Value;

	public:
	Constant (const string &n, const Number &val) : BasicSymbol (n), Value(val) {}
	~Constant () {}

	const Number &GetValue () const throw () {return Value;}
	void Print () const;
};

class Structure : public BasicSymbol
{
	public:
	Structure (const string &n) : BasicSymbol (n) {}
	~Structure () {}

	void Print () const;
};

class Union : public BasicSymbol
{
	public:
	Union (const string &n) : BasicSymbol (n) {}
	~Union () {}

	void Print () const;
};

class Macro : public BasicSymbol
{
	public:
	Macro (const string &n) : BasicSymbol (n) {}
	~Macro () {}

	void Print () const;
};
*/
#endif
