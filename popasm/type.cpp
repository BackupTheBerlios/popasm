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

string Type::PrintSize (unsigned int sz) throw ()
{
	switch (sz)
	{
		case 8: return "byte";
		case 16: return "word";
		case 32: return "dword";
		case 48: return "pword";
		case 64: return "qword";
		case 80: return "tbyte";
	}

	return "unknown size";
}

string Type::PrintDistance (unsigned int dist) throw ()
{
	switch (dist)
	{
		case SHORT: return "short";
		case NEAR: return "near";
		case FAR: return "far";
	}

	return "unknown distance";
}

string Type::PrintNumerical (unsigned int num) throw ()
{
	switch (num)
	{
		case INTEGER: return "integer";
		case FLOAT: return "float";
	}

	return "unknown numerical type";
}

CastConflict::CastConflict (const string &s1, const string &s2) throw () : WhatString ("Cannot cast a ")
{
	WhatString += s1 + " expression to be " + s2;
}

bool Type::CombineSD (unsigned int sz, unsigned int dist) throw ()
{
	switch (dist)
	{
		case SHORT:
			return (sz == 0) || (sz == 8);

		case NEAR:
			return (sz == 0) || (sz == 16) || (sz == 32);

		case FAR:
			return (sz == 0) || (sz == 32) || (sz == 48);

		default:
			break;
	}

	return true;
}

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

				case UNKNOWN:						// UNKNOWN + anything = UNKNOWN
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

				case UNKNOWN:						// UNKNOWN + anything = UNKNOWN
					break;
			}

			break;

		case STRONG_MEMORY:
			throw IncompatibleTypes ();		// Anything + STRONG_MEMORY = error!

		case UNKNOWN:
			CurrentType = UNKNOWN;				// Anything + UNKNOWN = UNKNOWN
			break;
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
	// Returns UNKNOWN if either object is unknown
	if ((CurrentType == UNKNOWN) || (t.CurrentType == UNKNOWN))
	{
		CurrentType = UNKNOWN;
		return *this;
	}

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

Type &Type::operator~ () throw (IncompatibleTypes)
{
	// Strong memory cannot negate or complement.
	if (CurrentType == STRONG_MEMORY) throw IncompatibleTypes();
	// Cannot operate on SHORT, NEAR or FAR expressions
	if (DistanceType != UNDEFINED) throw IncompatibleTypes();

	if (CurrentType != UNKNOWN)
		CurrentType = SCALAR;

	Size = 0;
	return *this;
}

Type &Type::operator- () throw (IncompatibleTypes)
{
	// - and ~ share the same behavior
	return ~*this;
}

void Type::operator[] (int) throw (MultipleBrackets, DistanceSizedMemory)
{
	// Cannot operate on SHORT, NEAR or FAR expressions
	if (DistanceType != UNDEFINED) throw DistanceSizedMemory();
	if (CurrentType == STRONG_MEMORY) throw MultipleBrackets();

	if (CurrentType != UNKNOWN)
		CurrentType = STRONG_MEMORY;
}

bool Match (int Restriction, int i) throw ()
{
	return (Restriction & i) != 0;
}

static const int Sizes[] = {0, 8, 16, 32, 48, 64, 80, 128};

bool MatchSize (int Restriction, int x)
{
	for (unsigned int i = 0; i < sizeof (Sizes); i++)
	{
		if (Sizes[i] == x)
		{
			return (Restriction & (1 << i)) != 0;
		}
	}

	return false;
}

int GetFirstSize(int sz_mask) throw ()
{
	for (unsigned int i = 1; i < sizeof (Sizes); i++)
	{
		if ((sz_mask & (1 << i)) != 0)
		{
			return Sizes[i];
		}
	}

	return 0;
}

int GetFirst(int mask) throw ()
{
	for (int i = 1;; i <<= 1)
	{
		if ((i & mask) != 0)
			return i;
	}

	return 0;
}
