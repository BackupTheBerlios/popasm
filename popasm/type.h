/***************************************************************************
                            type.h  -  description
                             -------------------
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Control how types change among operations
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TYPE_H
#define TYPE_H

#include <exception>
#include <string>

enum {UNDEFINED = 1, ANY = 255};
enum {BYTE = 2, WORD = 4, DWORD = 8, PWORD = 16, QWORD = 32, TBYTE = 64, OWORD = 128};
enum {SHORT = 2, NEAR = 4, FAR = 8};
enum {INTEGER = 2, FLOAT = 4, BCD = 8};
bool Match (int Restriction, int i) throw ();
bool MatchSize (int SizeRestriction, int sz);

// Thrown when types cannot be operated. E.g.: [5] + 1 = STRONG_MEMORY + SCALAR = error!
class IncompatibleTypes : public exception
{
	static const char WhatString[];

	public:
	IncompatibleTypes () {}
	~IncompatibleTypes () {}

	const char *what() const throw() {return WhatString;}
};

// Thrown if user attempts to use brackets twice. E.g.: [[bx]]
class MultipleBrackets : public exception
{
	static const char WhatString[];

	public:
	MultipleBrackets () throw () {}
	~MultipleBrackets () throw () {}

	const char *what() const throw() {return WhatString;}
};

// Thrown if user says something like [near ebx] instead of near[ebx]
class DistanceSizedMemory : public exception
{
	static const char WhatString[];

	public:
	DistanceSizedMemory () throw () {}
	~DistanceSizedMemory () throw () {}

	const char *what() const throw() {return WhatString;}
};

class Type
{
	public:
	enum TypeName {SCALAR, WEAK_MEMORY, STRONG_MEMORY};

	private:
	unsigned int Size;
	TypeName CurrentType;
	int DistanceType;

	public:
	Type (unsigned int sz = 0, TypeName t = SCALAR, int dist = UNDEFINED) throw () : Size (sz), CurrentType (t), DistanceType(dist) {}
	virtual ~Type () throw () {}

	// Methods for reading and writing each member
	unsigned int GetSize () const throw () {return Size;}
	void SetSize (unsigned int sz) throw () {Size = sz;}

	TypeName GetCurrentType () const throw () {return CurrentType;}
	void SetCurrentType (TypeName NewType) throw () {CurrentType = NewType;}

	int GetDistanceType () const throw () {return DistanceType;}
	void SetDistanceType (int dist) throw () {DistanceType = dist;}

	Type &operator+= (const Type &t) throw (IncompatibleTypes);
	Type &operator-= (const Type &t) throw (IncompatibleTypes);
	Type &operator*= (const Type &t) throw (IncompatibleTypes);
	Type &operator/= (const Type &t) throw (IncompatibleTypes);
	Type &operator%= (const Type &t) throw (IncompatibleTypes);

	Type &operator&= (const Type &t) throw (IncompatibleTypes);
	Type &operator|= (const Type &t) throw (IncompatibleTypes);
	Type &operator^= (const Type &t) throw (IncompatibleTypes);
	Type &operator<<= (const Type &t) throw (IncompatibleTypes);
	Type &operator>>= (const Type &t) throw (IncompatibleTypes);

	Type operator- () throw (IncompatibleTypes);
	Type operator~ () throw (IncompatibleTypes);

	bool operator== (const Type &t) const throw ()
		{return (Size == t.Size) && (CurrentType == t.CurrentType) && (DistanceType == t.DistanceType);}
	bool operator!= (const Type &t) const throw ()
		{return (Size != t.Size) || (CurrentType != t.CurrentType) || (DistanceType != t.DistanceType);}

	void operator[] (int) throw (MultipleBrackets, DistanceSizedMemory);
};

#endif
