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

Immediate::Immediate (const Number &n) throw () : BasicArgument(n.GetSize()), Value(n.GetValue())
{
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
