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

	private:
	TypeName CurrentType;

	public:
	Type (TypeName t = SCALAR) throw () : CurrentType (t) {}
	~Type () throw () {}

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

	Type operator- () const throw (IncompatibleTypes);
	Type operator~ () const throw (IncompatibleTypes);

	bool operator== (const Type &t) const throw () {return CurrentType == t.CurrentType;}
	bool operator!= (const Type &t) const throw () {return CurrentType != t.CurrentType;}

	void operator[] (int) throw (MultipleBrackets);
};

#endif
