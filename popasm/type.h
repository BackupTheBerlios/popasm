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
enum {INTEGER = 2, FLOAT = 4};
bool Match (int Restriction, int i) throw ();
bool MatchSize (int SizeRestriction, int sz);
int GetFirstSize(int sz_mask) throw ();
int GetFirst(int mask) throw ();


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

class CastConflict : public exception
{
	string WhatString;

	public:
	CastConflict (const string &s1, const string &s2) throw ();
	~CastConflict () throw () {}

	const char *what() const throw() {return WhatString.c_str();}
};

class Type
{
	public:
	enum TypeName {SCALAR, WEAK_MEMORY, STRONG_MEMORY, UNKNOWN};

	private:
	unsigned int Size;
	TypeName CurrentType;
	int DistanceType;
	int NumericalType;

	public:
	Type (unsigned int sz = 0, TypeName t = SCALAR, int dist = UNDEFINED, int num = UNDEFINED) throw ()
		: Size (sz), CurrentType (t), DistanceType(dist), NumericalType (num) {}
	Type (const Type &t) throw ()
	{
		Size = t.Size; CurrentType = t.CurrentType; DistanceType = t.DistanceType; NumericalType = t.NumericalType;
	}

	virtual ~Type () throw () {}

	// Methods for reading and writing each member
	unsigned int GetSize () const throw () {return Size;}
	virtual void SetSize (unsigned int sz) throw () {Size = sz;}
	static string PrintSize (unsigned int sz) throw ();

	TypeName GetCurrentType () const throw () {return CurrentType;}
	virtual void SetCurrentType (TypeName NewType) throw () {CurrentType = NewType;}

	int GetDistanceType () const throw () {return DistanceType;}
	virtual void SetDistanceType (int dist) throw () {DistanceType = dist;}
	static string PrintDistance (unsigned int dist) throw ();

	int GetNumericalType () const throw () {return NumericalType;}
	virtual void SetNumericalType (int num) throw () {NumericalType = num;}
	static string PrintNumerical (unsigned int num) throw ();

	// Check whether it makes sense for a type to be sz sizes and
	// dist as distance qualifier at the same time
	static bool CombineSD (unsigned int sz, unsigned int dist) throw ();

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

	Type &operator- () throw (IncompatibleTypes);
	Type &operator~ () throw (IncompatibleTypes);
	Type &operator= (const Type &t) throw ()
	{
		Size = t.Size; CurrentType = t.CurrentType; DistanceType = t.DistanceType; NumericalType = t.NumericalType;
		return *this;
	}

	bool operator== (const Type &t) const throw ()
		{return (Size == t.Size) && (CurrentType == t.CurrentType) && (DistanceType == t.DistanceType) && (NumericalType == t.NumericalType);}
	bool operator!= (const Type &t) const throw ()
		{return (Size != t.Size) || (CurrentType != t.CurrentType) || (DistanceType != t.DistanceType) || (NumericalType != t.NumericalType);}

	void operator[] (int) throw (MultipleBrackets, DistanceSizedMemory);
};

#endif
