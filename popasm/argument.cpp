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

#include <iostream>

#include "argument.h"
#include "memory.h"
#include "lexnum.h"

const Immediate * const Immediate::ClassInstance = new Immediate ();

void Immediate::SetSize(unsigned int sz)
{
	Number temp(Value);
	temp.SetSize(sz);
	BasicArgument::SetSize(sz);
}

const UnsignedByte * const UnsignedByte::ClassInstance = new UnsignedByte ();

bool UnsignedByte::Identify (const BasicArgument * const ptr) const
{
	const Immediate * const immed = dynamic_cast<const Immediate * const> (ptr);
	if (immed == 0) return false;

	if ((ptr->GetSize() != 0) && (ptr->GetSize() != 8)) return false;

	// Unsigned bytes must be integers
	const RealNumber &n = immed->GetValue();
	if (!n.GetInteger()) return false;

	long int x = static_cast<IntegerNumber> (n).GetValue(true);
	if ((x >= 0) && (x <= 255))
	{
		ptr->BasicArgument::SetSize (8);
		return true;
	}

	return false;
}

const SignedByte * const SignedByte::ClassInstance = new SignedByte ();

bool SignedByte::Identify (const BasicArgument * const ptr) const
{
	const Immediate * const immed = dynamic_cast<const Immediate * const> (ptr);
	if (immed == 0) return false;

	if ((ptr->GetSize() != 0) && (ptr->GetSize() != 8)) return false;

	// Unsigned bytes must be integers
	const RealNumber &n = immed->GetValue();
	if (!n.GetInteger()) return false;

	long int x = static_cast<IntegerNumber> (n).GetValue(true);
	if ((x >= -128) && (x <= 127))
	{
		ptr->BasicArgument::SetSize (8);
		return true;
	}

	return false;
}

bool Argument::TypeCheck (Argument &arg, CheckType ct)
{
	if (ct == NONE) return true;

	unsigned int s1 = Data->GetSize();
	unsigned int s2 = arg.Data->GetSize();

	// Checks if either argument has a non-specified size
	if ((s1 == 0) || (s2 == 0))
	{
		// Only the EQUAL check type can overide an unknown size, such as in "mov eax,[var]"
		if (ct != EQUAL) return false;

		// Make sizes equal
		if (s1 == 0)
			SetSize (s2);
		else
			arg.SetSize (s1);

		return true;
	}

	// Performs the check according to the desired way
	switch (ct)
	{
		case EQUAL:
			return s1 == s2;

		case GREATER:
			return s1 > s2;

		case NONE:
			break;
	}

	return true;
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
						return new Argument (new Immediate (*p->first), true);

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
