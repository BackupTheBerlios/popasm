/***************************************************************************
                          variable.h  -  description
                             -------------------
    begin                : Fri Feb 7 2003
    copyright            : (C) 2003 by Helcio Mello
    email                : helcio@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VARIABLE_H
#define VARIABLE_H

#include "symbol.h"
#include "type.h"

class UserDefined : public BasicSymbol, public Type
{
	Dword Line;             // Line where symbol was defined
	Dword Offset;				// Offset within its segment

	public:
 	UserDefined (const string &n, const Type &t) throw ();
	UserDefined (const string &n, Dword off, const Type &t) throw ();
	~UserDefined () throw () {}

	Dword GetOffset() const throw () {return Offset;}
	Dword GetLine() const throw () {return Line;}
	bool Changed (const BasicSymbol *s) throw ();
};

class Variable : public UserDefined
{
	unsigned int Length;		// Quantity of data items.

// Example:
// VAR_NAME		DD		58, 47, 12
//
// Size = 32 (because doublewords are 32-bits wide)
// Length = 3 (three doublewords defined in the same line)

	public:
	Variable (const string &n, const Type &t, unsigned int len = 1) throw () :
		UserDefined(n, t), Length(len) {}
	Variable (const string &n, Dword off, const Type &t, unsigned int len = 1) throw () :
		UserDefined(n, off, t), Length(len) {}
	~Variable () {}

	unsigned int GetLength() const throw () {return Length;}
	void SetLength(unsigned int NewLength) throw () {Length = NewLength;}
};

class Label : public UserDefined
{
	public:
	Label (const string &n, int dist = UNDEFINED) throw () : UserDefined(n, Type (0, SCALAR, dist)) {}
	~Label () throw () {}
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
		: Variable (n, Type (ag->GetSize(), WEAK_MEMORY), len) {}
	~AggregateInstance () {}

	const Aggregate *GetFather() const throw () {return Father;}
};

#endif
