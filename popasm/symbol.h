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

#include <iostream>
#include <string>
#include <vector>
#include "lexnum.h"
#include "defs.h"
#include "express.h"

// Contains data necessary to describe a basic symbol (i.e. one which has no type)
class BasicSymbol
{
	string Name;

	public:
	BasicSymbol (const string &n) : Name(n) {}
	virtual ~BasicSymbol () {}

	const string &GetName () const throw () {return Name;}
	string Print () const throw () {return Name;}

	virtual BasicSymbol *Clone() const throw () {cout << "Not implemented: BasicSymbol::Clone()" << endl; return 0; }

	// This method is used to compare symbols' names, to put them in lexicographical order in a set
	virtual bool operator< (const BasicSymbol &s) const throw () {return Name < s.Name;}
};

class Variable : public BasicSymbol
{
	Dword Offset;				// Offset within its segment
	unsigned int Size;		// Variable size in bits
	unsigned int Length;		// Quantity of data items.

// Example:
// VAR_NAME		DD		58, 47, 12
//
// Size = 32 (because doublewords are 32-bits wide)
// Length = 3 (three doublewords defined in the same line)

	public:
	Variable (const string &n, Dword off, unsigned int sz = 0, unsigned int len = 1) throw () :
		BasicSymbol(n), Offset(off), Size(sz), Length(len) {}
	~Variable () {}

	Dword GetOffset() const throw () {return Offset;}
	unsigned int GetSize() const throw () {return Size;}
	unsigned int GetLength() const throw () {return Length;}
};

class Label : public BasicSymbol
{
	Dword Offset;

	public:
	Label (const string &n) throw () : BasicSymbol(n), Offset(CurrentAssembler->GetCurrentOffset()) {}
	Label (const string &n, Dword off) throw () : BasicSymbol(n), Offset(off) {}
	~Label () throw () {}

	Dword GetOffset() const throw () {return Offset;}
};

// Aggregates are a common designation for structs, unions and the like
class Aggregate : public BasicSymbol
{
	vector<const Variable *> Members;

	protected:
	unsigned int Size;

	public:
	Aggregate (const string &n, unsigned int sz) throw () : BasicSymbol (n), Size(sz) {}
	~Aggregate () throw () = 0;

	virtual void AddMember (const Variable *v) throw () {Members.push_back(v);}
	const Variable *FindMember (const string &s) const throw ();

	unsigned int GetSize () const throw () {return Size;}
};

class Structure : public Aggregate
{
	public:
	Structure (const string &n) throw () : Aggregate (n, 0) {}
	~Structure () throw () {}

	void AddMember (const Variable *s) throw ();
};

// Unions are like structures, but all members share the same memory space
class Union : public Aggregate
{
	public:
	Union (const string &n) throw () : Aggregate (n, 0) {}
	~Union () throw () {}

	void AddMember (const Variable *s) throw ();
};

// Defines instantiations of aggregators, like structures and unions
class AggregateInstance : public Variable
{
	const Aggregate *Father;

	public:
	AggregateInstance (const string &n, Dword off, const Aggregate *ag, unsigned int len) throw ()
		: Variable (n, off, ag->GetSize(), len) {}
	~AggregateInstance () {}

	const Aggregate *GetFather() const throw () {return Father;}
};

class Segment : public BasicSymbol
{
	Word Address;
	bool AddressSpecified;

	public:
	Segment (const string &n) : BasicSymbol (n), AddressSpecified(false) {}
	Segment (Word at) : BasicSymbol (""), Address(at), AddressSpecified(true) {}
	Segment (const string &n, Word at) : BasicSymbol (n), Address(at), AddressSpecified(true) {}
	~Segment () {}
};

/*
class Constant : public BasicSymbol
{
	Number Value;

	public:
	Constant (const string &n, const Number &val) : BasicSymbol (n), Value(val) {}
	~Constant () {}

	const Number &GetValue () const throw () {return Value;}
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

class HashFunctor
{
	public:
	unsigned int operator() (const BasicSymbol * const &sd);
};

#endif
