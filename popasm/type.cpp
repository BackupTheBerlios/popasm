/***************************************************************************
                            type.cpp  -  description
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

#include "type.h"

Type &Type::operator+= (const Type &t) throw ()
{
	switch (t.CurrentType)
	{
		case SCALAR:
			// Adding a scalar to a number does not change its original type
			break;

		case WEAK_MEMORY:
			switch (CurrentType)
			{
				case SCALAR:
					CurrentType = WEAK_MEMORY;	// SCALAR + WEAK_MEMORY = WEAK_MEMORY
					break;

				case WEAK_MEMORY:
					CurrentType = SCALAR;		// WEAK_MEMORY + WEAK_MEMORY = SCALAR
					break;

				case STRONG_MEMORY:
					break;							// STRONG_MEMORY + Anything = STRONG_MEMORY
			}

			break;

		case STRONG_MEMORY:
			CurrentType = STRONG_MEMORY;		// Anything + STRONG_MEMORY = STRONG_MEMORY
	}

	return *this;
}

Type &Type::operator-= (const Type &t) throw ()
{
	// += and -= share the same behavior
	return *this += t;
}

Type &Type::operator*= (const Type &t) throw (IncompatibleTypes)
{
	// Strong memory cannot multiply or divide.
	if ((CurrentType == STRONG_MEMORY) || (t.CurrentType == STRONG_MEMORY)) throw IncompatibleTypes();

	// The result of any valid operation will be scalar
	CurrentType = SCALAR;
	return *this;
}

Type &Type::operator/= (const Type &t) throw (IncompatibleTypes)
{
	// *= and /= share the same behavior
	return *this *= t;
}

Type &Type::operator%= (const Type &t) throw (IncompatibleTypes)
{
	// *= and %= share the same behavior
	return *this *= t;
}

Type &Type::operator&= (const Type &t) throw (IncompatibleTypes)
{
	// *= and &= share the same behavior
	return *this *= t;
}

Type &Type::operator|= (const Type &t) throw (IncompatibleTypes)
{
	// *= and |= share the same behavior
	return *this *= t;
}

Type &Type::operator^= (const Type &t) throw (IncompatibleTypes)
{
	// *= and ^= share the same behavior
	return *this *= t;
}

Type &Type::operator<<= (const Type &t) throw (IncompatibleTypes)
{
	// *= and <<= share the same behavior
	return *this *= t;
}

Type &Type::operator>>= (const Type &t) throw (IncompatibleTypes)
{
	// *= and >>= share the same behavior
	return *this *= t;
}

Type Type::operator~ () const throw (IncompatibleTypes)
{
	// Strong memory cannot negate or complement.
	if (CurrentType == STRONG_MEMORY) throw IncompatibleTypes();
	return *this;
}

Type Type::operator- () const throw (IncompatibleTypes)
{
	// - and ~ share the same behavior
	return ~*this;
}
