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
#include "full_ptr.h"
#include "lexnum.h"
#include "immed.h"

void Argument::TypeCheck (const vector<Argument *> &args, CheckType ct) throw (TypeMismatch)
{
	unsigned int Sizes[3];
	for (unsigned int i = 0; i != args.size(); i++)
		if (args[i] != 0)
			Sizes[i] = args[i]->GetSize();

	// Performs the check according to the desired way
	switch (ct)
	{
		case EQUAL:
			// If either argument has no defined size, match them
			if (Sizes[0] == 0)
			{
				args[0]->SetSize (Sizes[1]);
				break;
			}

			if (Sizes[1] == 0)
			{
				args[1]->SetSize (Sizes[0]);
				break;
			}

			if (Sizes[0] != Sizes[1]) throw TypeMismatch (args);
			break;

		case GREATER:
			// Both sizes MUST be known
			if ((Sizes[0] == 0) || (Sizes[1] == 0)) throw OneUndefinedSize();
			if  (Sizes[0] <= Sizes[1]) throw TypeMismatch (args);
			break;

		case HALF:
			// If either argument has no defined size, match them so the first is half of the second in size
			if (Sizes[0] == 0)
			{
				args[1]->SetSize (Sizes[1] / 2);
				break;
			}

			if (Sizes[1] == 0)
			{
				args[1]->SetSize (Sizes[0] * 2);
				break;
			}

			if (Sizes[0] != (Sizes[1] / 2)) throw TypeMismatch (args);
			break;

		case MINUS_16BITS:
			if (Sizes[0] == 0)
			{
				args[0]->SetSize (Sizes[1] - 16);
				break;
			}

			if (Sizes[1] == 0)
			{
				args[1]->SetSize (Sizes[0] + 16);
				break;
			}

			if (Sizes[0] != Sizes[1] - 16) throw TypeMismatch (args);
			break;

		case NONE:
			break;
	}
}

Argument *Argument::MakeArgument (const Expression &e) throw (InvalidArgument, exception)
{
	const Expression *Prefix = e.GetSegmentPrefix();

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
					if (reg != 0)
					{
						// Registers cannot be preceeded by prefixes like ES:
						if (Prefix != 0) throw InvalidFullPointer();
						if (e.GetDistanceType() != UNDEFINED) throw MisusedDistanceQualifier();
						return new Argument (reg, false);
					}

					throw InvalidArgument (e);
				}
				else
				{
					// Checks for immediate arguments
					if ((p->first != 0) && (p->second == 0))
					{
						if (Prefix == 0) return new Argument (new Immediate (*p->first, e.GetDistanceType()), true);

						// Checks for full-pointers. The prefix must be constant
						if (Prefix->QuantityOfTerms() != 1) throw InvalidFullPointer();
						const pair<Number *, Symbol *> *q = Prefix->TermAt(0);
						if ((q->first == 0) || (q->second != 0)) throw InvalidFullPointer();

						Dword seg = q->first->GetUnsignedLong ();
						Dword off = p->first->GetUnsignedLong ();
						if (seg > 0xFFFF) throw InvalidFullPointer();
						return new Argument (new FullPointer (p->first->GetSize(), static_cast<Word> (seg), off), true);
					}

					throw InvalidArgument (e);
				}
			}
			else throw InvalidArgument (e);
			break;

		case Type::WEAK_MEMORY:
			break;

		case Type::STRONG_MEMORY:
			return MakeMemory (e);
	}

	return 0;
}

const char UndefinedSize::WhatString[] = "Operation size undefined. At least one argument must be explicitly sized.";
const char OneUndefinedSize::WhatString[] = "Operation size undefined. Both arguments must be explicitly sized.";
