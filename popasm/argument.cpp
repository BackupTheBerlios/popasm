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
#include <typeinfo>

#include "argument.h"
#include "memory.h"
#include "full_ptr.h"
#include "lexnum.h"
#include "immed.h"
#include "dup_arg.h"

TypeMismatch::TypeMismatch (const vector<Argument *> v) throw () : WhatString ("Type mismatch between")
{
	for (unsigned int i = 0; i != v.size(); i++)
	{
		if (i == 0)
			WhatString += " ";
		else
			if (i == v.size() - 1)
				WhatString += " and ";
			else
				WhatString += ", ";

		WhatString += v[i]->Print();
	}
}

BitOutOfBounds::BitOutOfBounds (const BasicArgument *arg, unsigned int n) : WhatString ("Bit ")
{
	WhatString += Print(n);
	WhatString += " lies outside ";
	WhatString += arg->Print();
}

void Argument::TypeCheck (const vector<Argument *> &args, CheckType ct) throw (TypeMismatch, BitOutOfBounds, UndefinedSize)
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
				if (Sizes[1] == 0) throw UndefinedSize();
				args[0]->SetSize (Sizes[1]);
				break;
			}

			if (Sizes[1] == 0)
			{
				args[1]->SetSize (Sizes[0]);
				break;
			}

			if ((Sizes[0] != Sizes[1]) && (!args[0]->IsUndefined()) && (!args[1]->IsUndefined()))
				throw TypeMismatch (args);
			break;

		case GREATER:
			// Accepts undefined arguments as ok.
			if ((args[0]->IsUndefined()) || (args[1]->IsUndefined()))
				break;

			// Both sizes MUST be known
			if ((Sizes[0] == 0) || (Sizes[1] == 0))
				throw OneUndefinedSize();
			if  (Sizes[0] <= Sizes[1])
				throw TypeMismatch (args);
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

			if ((Sizes[0] != (Sizes[1] / 2)) && (!args[0]->IsUndefined()) && (!args[1]->IsUndefined()))
				throw TypeMismatch (args);
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

			if ((Sizes[0] != Sizes[1] - 16) && (!args[0]->IsUndefined()) && (!args[1]->IsUndefined()))
				throw TypeMismatch (args);
			break;

		case BIT_NUMBER:
		{
			// If quantity of arguments is three, the two first ones must be checked for equality
			if (args.size() == 3)
			{
				// If either argument has no defined size, match them
				if (Sizes[0] == 0)
				{
					if (Sizes[1] == 0) throw UndefinedSize();
					args[0]->SetSize (Sizes[1]);
					Sizes[0] = Sizes[1];
				}

				if (Sizes[1] == 0)
				{
					args[1]->SetSize (Sizes[0]);
					Sizes[1] = Sizes[0];
				}

				if ((Sizes[0] != Sizes[1]) && (!args[0]->IsUndefined()) && (!args[1]->IsUndefined()))
					throw TypeMismatch (vector<Argument *> (args.begin(), args.begin()+2));
			}

			unsigned long int s = static_cast<const Immediate *> (args.back()->GetData())->GetLong();
			if ((Sizes[0] <= s) && (!args[0]->IsUndefined()))
				throw BitOutOfBounds (args[0]->GetData(), s);
			break;
		}

		case NONE:
			break;
	}
}

Argument *Argument::MakeArgument (const Expression &e) throw (InvalidArgument, exception)
{
	Argument *arg;

	arg = Register::MakeArgument (e);
	if (arg != 0)
		return arg;

	arg = Memory::MakeArgument (e);
	if (arg != 0)
		return arg;

	arg = Immediate::MakeArgument (e);
	if (arg != 0)
		return arg;

	arg = FullPointer::MakeArgument (e);
	if (arg != 0)
		return arg;

	arg = DupArgument::MakeArgument (e);
	if (arg != 0)
		return arg;

	return new Argument (0, false, true);
};

/*
	const SimpleExpression *se = dynamic_cast<const SimpleExpression *> (&e.GetConstData());
	const DupExpression *de = dynamic_cast<const DupExpression *> (&e.GetConstData());
	const SimpleExpression *Prefix = 0;
	const Number *num;
	const Symbol *sym;

	if (se != 0)
	{
		Prefix = se->GetSegmentPrefix();
		num = se->GetNumber();
		sym = se->GetSymbol();
	}

	if (de != 0)
		return new Argument (new DupArgument (e), false);

	switch (se->GetCurrentType())
	{
		case Type::SCALAR:
			if (

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
						if (seg > 0xFFFF) throw SegmentOverflow();
						return new Argument (new FullPointer (p->first->GetSize(), static_cast<Word> (seg), off), true);
					}

					throw InvalidArgument (e);
				}
			}
			else throw InvalidArgument (e);
			break;

		case Type::WEAK_MEMORY:
		{
			Expression temp(e);

			temp.SetCurrentType (CurrentAssembler->TranslateWeakMemory());
			return MakeArgument (temp);
		}

		case Type::STRONG_MEMORY:
			return MakeMemory (e);

		case Type::UNKNOWN:
			return new Argument (0, false, true);
	}

	return 0;
}
*/

const char UndefinedSize::WhatString[] = "Operation size undefined. At least one argument must be explicitly sized.";
const char OneUndefinedSize::WhatString[] = "Operation size undefined. Both arguments must be explicitly sized.";
