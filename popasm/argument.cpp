/***************************************************************************
                          argument.cpp  -  description
                             -------------------
    begin                : Tue Jun 4 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Holds information about arguments passed for machine instructions
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "argument.h"
#include "memory.h"
#include "lexnum.h"

UnsignedByte::UnsignedByte (const RealNumber &n) : Immediate (8)
{
	// Unsigned bytes must be integers
	if (!n.GetInteger()) throw IntegerExpected(n);

	long int x = static_cast<IntegerNumber> (n).GetValue(true);
	if ((x < 0) || (x > 255)) throw CastFailed (n, 8);

	Value = static_cast<Byte> (x);
}

SignedByte::SignedByte (const RealNumber &n) : Immediate (8)
{
	// Signed bytes must be integers
	if (!n.GetInteger()) throw IntegerExpected(n);

	long int x = static_cast<IntegerNumber> (n).GetValue(true);
	if ((x < -128) || (x > 127)) throw CastFailed (n, 8);

	Value = static_cast<Byte> (x);
}

bool Argument::Match (const type_info &ti)
{
	const UnknownImmediate *immed = dynamic_cast<const UnknownImmediate *> (Data);

	// If the argument is not an immediate, it cannot be adjusted, so compare typeinfos
	if (immed == 0)
		return typeid(*Data) == ti;
	else
	{
		// Checks if the given typeinfo is an immediate
		if (!ti.before (typeid(Immediate))) return false;
		const RealNumber &n = immed->GetValue();

		if (ti == typeid(SignedByte))
		{
			// Checks size and bounds constraints
			if ((immed->GetSize() != 0) && (immed->GetSize() != 8)) return false;

			// Replaces the old number with a signed byte
			SignedByte *sb = new SignedByte (n);
			delete Data;
			Data = sb;

			return true;
		}
		else if (ti == typeid(UnsignedByte))
		{
			// Checks size and bounds constraints
			if ((immed->GetSize() != 0) && (immed->GetSize() != 8)) return false;

			// Replaces the old number with a signed byte
			UnsignedByte *ub = new UnsignedByte (n);
			delete Data;
			Data = ub;

			return true;
		}

		return false;
	}
}

Argument *Argument::MakeArgument (const Expression &e, unsigned int CurrentAddressSize)
{
	switch (e.GetType())
	{
		case Type::SCALAR:
			if (e.QuantityOfTerms() == 1)
			{
				// Get a reference for the only term of the expression
				const pair<Number *, Symbol *> *p = e.TermAt(0);

				// Checks for a symbol
				if ((p->first == 0) && (p->second != 0))
				{
					const BasicSymbol *data = p->second->GetData();

					// Checks for registers
					const Register *reg = dynamic_cast<const Register *> (data);
					if (reg != 0) return new Argument (reg, false);
					throw InvalidArgument (e);
				}
				else
				{
					// Checks for immediate arguments
					if ((p->first != 0) && (p->second == 0))
						return new Argument (new UnknownImmediate (*p->first), true);

					throw InvalidArgument (e);
				}
			}
			else throw InvalidArgument (e);
			break;

		case Type::WEAK_MEMORY:
			break;

		case Type::STRONG_MEMORY:
			return MakeMemory (e, CurrentAddressSize);
	}

	return 0;
}
