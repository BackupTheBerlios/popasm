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

// Thrown when types cannot be operated. E.g.: [5] + 1 = STRONG_MEMORY + SCALAR = error!
class IncompatibleTypes : public exception
{
	public:
	IncompatibleTypes () {}
	~IncompatibleTypes () {}
};

// Thrown if user attempts to use brackets twice. E.g.: [[bx]]
class MultipleBrackets : public exception
{
	static string WhatString;

	public:
	MultipleBrackets () throw () {}
	~MultipleBrackets () throw () {}

	const char *what() const throw() {return WhatString.c_str();}
};

class Type
{
	public:
	enum TypeName {SCALAR, WEAK_MEMORY, STRONG_MEMORY};
	enum Distance {NONE, SHORT, NEAR, FAR};

	private:
	unsigned int Size;
	TypeName CurrentType;
	Distance DistanceType;

	public:
	Type (unsigned int sz = 0, TypeName t = SCALAR, Distance dist = NONE) throw () : Size (sz), CurrentType (t), DistanceType(dist) {}
	~Type () throw () {}

	// Methods for reading and writing each member
	unsigned int GetSize () const throw () {return Size;}
	void SetSize (unsigned int sz) throw () {Size = sz;}

	TypeName GetCurrentType () const throw () {return CurrentType;}
	void SetCurrentType (TypeName NewType) throw () {CurrentType = NewType;}

	Distance GetDistanceType () const throw () {return DistanceType;}
	void SetDistanceType (Distance dist) throw () {DistanceType = dist;}

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

	void operator[] (int) throw (MultipleBrackets);
};

#endif
