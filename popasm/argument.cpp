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
#include "immed.h"

bool Argument::TypeCheck (Argument &arg, CheckType ct)
{
	if (ct == NONE) return true;

	unsigned int s1 = Data->GetSize();
	unsigned int s2 = arg.Data->GetSize();

	// If both arguments have no defined size, throw exception
	if ((s1 == 0) && (s2 == 0)) throw 0;

	// Performs the check according to the desired way
	switch (ct)
	{
		case EQUAL:
			// If either argument has no defined size, match them
			if (s1 == 0)
			{
				SetSize (s2);
				return true;
			}

			if (s2 == 0)
			{
				arg.SetSize (s1);
				return true;
			}

			return s1 == s2;

		case GREATER:
			// Both sizes MUST be known
			if ((s1 == 0) || (s2 == 0)) return false;
			return s1 > s2;

		case HALF:
			// If either argument has no defined size, match them so the first is half of the second in size
			if (s1 == 0)
			{
				SetSize (s2 / 2);
				return true;
			}

			if (s2 == 0)
			{
				arg.SetSize (s1 * 2);
				return true;
			}

			return s1 == s2 / 2;

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
