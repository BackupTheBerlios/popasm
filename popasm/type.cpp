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

const char MultipleBrackets::WhatString[] = "Illegal: Multiple brackets.";
const char DistanceSizedMemory::WhatString[] = "Distance size qualifiers cannot be used inside brackets.";
const char IncompatibleTypes::WhatString[] = "Error: Incompatible types that cannot be operated between them.";

Type &Type::operator+= (const Type &t) throw (IncompatibleTypes)
{
	// Cannot operate on SHORT, NEAR or FAR expressions
	if ((DistanceType != UNDEFINED) && (t.DistanceType != UNDEFINED)) throw IncompatibleTypes();

	switch (t.CurrentType)
	{
		case SCALAR:
			switch (CurrentType)
			{
				case SCALAR:
					Size = 0;
					break;							// SCALAR + SCALAR = SCALAR

				case WEAK_MEMORY:
					CurrentType = WEAK_MEMORY; // WEAK_MEMORY + SCALAR = WEAK_MEMORY
					break;

				case STRONG_MEMORY:
					throw IncompatibleTypes();	// STRONG_MEMORY + SCALAR = error!
					break;
			}

			break;

		case WEAK_MEMORY:
			switch (CurrentType)
			{
				case SCALAR:
					CurrentType = WEAK_MEMORY;	// SCALAR + WEAK_MEMORY = WEAK_MEMORY
					Size = t.Size;
					break;

				case WEAK_MEMORY:
					CurrentType = SCALAR;		// WEAK_MEMORY + WEAK_MEMORY = SCALAR
					Size = 0;
					break;

				case STRONG_MEMORY:
					throw IncompatibleTypes();	// STRONG_MEMORY + WEAK_MEMORY = error!
					break;
			}

			break;

		case STRONG_MEMORY:
			throw IncompatibleTypes ();		// Anything + STRONG_MEMORY = error!
	}

	return *this;
}

Type &Type::operator-= (const Type &t) throw (IncompatibleTypes)
{
	// += and -= share the same behavior
	return *this += t;
}

Type &Type::operator*= (const Type &t) throw (IncompatibleTypes)
{
	// Strong memory cannot multiply or divide.
	if ((CurrentType == STRONG_MEMORY) || (t.CurrentType == STRONG_MEMORY)) throw IncompatibleTypes();
	// Cannot operate on SHORT, NEAR or FAR expressions
	if ((DistanceType != UNDEFINED) && (t.DistanceType != UNDEFINED)) throw IncompatibleTypes();

	// The result of any valid operation will be scalar
	CurrentType = SCALAR;
	Size = 0;
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

Type Type::operator~ () throw (IncompatibleTypes)
{
	// Strong memory cannot negate or complement.
	if (CurrentType == STRONG_MEMORY) throw IncompatibleTypes();
	// Cannot operate on SHORT, NEAR or FAR expressions
	if (DistanceType != UNDEFINED) throw IncompatibleTypes();

	Size = 0;
	return *this;
}

Type Type::operator- () throw (IncompatibleTypes)
{
	// - and ~ share the same behavior
	return ~*this;
}

void Type::operator[] (int) throw (MultipleBrackets, DistanceSizedMemory)
{
	// Cannot operate on SHORT, NEAR or FAR expressions
	if (DistanceType != UNDEFINED) throw DistanceSizedMemory();
	if (CurrentType == STRONG_MEMORY) throw MultipleBrackets();

	CurrentType = STRONG_MEMORY;
}

bool Match (int Restriction, int i) throw ()
{
	return (Restriction & i) != 0;
}

bool MatchSize (int Restriction, int i)
{
	static const char Sizes[] = {0, 8, 16, 32, 48, 64, 80, 128};

	for (int i = 0; i < 8; i++)
	{
		if (Sizes[i] == rest)
		{
			return (Restriction & (1 << i)) != 0;
		}
	}

	return false;
}
