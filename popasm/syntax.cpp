/***************************************************************************
                          syntax.cpp  -  description
                             -------------------
    begin                : Tue Jun 18 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "syntax.h"
#include "register.h"
#include "memory.h"

Syntax::Syntax (unsigned int p, const Opcode &op, Byte dwm, int szarg = -1, unsigned int dm = 16) throw () : Encoding (op)
{
	Precedence = p;				// Precedence of this syntax

	ArgumentTypes[0] = 0;		// Empty argument list
	Interchangeable = false;	// Do not exchange argument positions
	Check = Argument::NONE;		// Do not perform type check
	dw_mask = dwm;
	mrr_usage = ABSENT;			// No mod_reg_r/m byte
	SizeArgument = szarg;
	DefaultMode = dm;
}

Syntax::Syntax (unsigned int p, const Opcode &op, const BasicArgument *t0, Byte dwm, ModRegRM_Usage usage, int szarg = 0) throw () : Encoding (op)
{
	Precedence = p;				// Precedence of this syntax

	ArgumentTypes[0] = t0;		// t0 is the only argument
	ArgumentTypes[1] = 0;		//

	Interchangeable = false;	// Do not exchange argument positions
	Check = Argument::NONE;		// Do not perform type check
	dw_mask = dwm;
	mrr_usage = usage;
	SizeArgument = szarg;
}

Syntax::Syntax (unsigned int p, const Opcode &op, const BasicArgument *t0, const BasicArgument *t1,
	Argument::CheckType ct, bool i, Byte dwm, ModRegRM_Usage usage, int szarg = 0) throw () : Encoding (op)
{
	Precedence = p;				// Precedence of this syntax

	ArgumentTypes[0] = t0;		// t0 and t1 are the only arguments
	ArgumentTypes[1] = t1;		//
	ArgumentTypes[2] = 0;		//

	Check = ct;
	Interchangeable = i;
	dw_mask = dwm;
	mrr_usage = usage;
	SizeArgument = szarg;
}

unsigned int Syntax::GetArity () const throw ()
{
	unsigned int i, count;

	for (i = count = 0; i < MaximumArity; i++)
	{
		if (ArgumentTypes[i] != 0) count++;
		else break;
	}

	return count;
}

bool Syntax::Match (vector<Argument *> &Arguments) const throw ()
{
	// Checks arity
	if (GetArity() != Arguments.size()) return false;

	bool Result = true;
	unsigned int i = 0;
	vector<Argument *>::const_iterator j = Arguments.begin();

	// Checks the types of each argument
	for (; j != Arguments.end(); i++, j++)
	{
		if (!(*j)->Match (ArgumentTypes[i]))
		{
			Result = false;
			break;
		}
	}

	// If the argument types of the syntax are interchangeable, try matching in reverse order
	if (Interchangeable && (Result == false) && (Arguments.size() >= 2))
		return (Arguments[0]->Match (ArgumentTypes[1])) && (Arguments[1]->Match (ArgumentTypes[0]));

	return Result;
}

bool Syntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output, unsigned int CurrentMode) const
{
	unsigned int i;
	Byte SegmentPrefix = 0;
	Byte dw = 0, mod_reg_rm = 0;
	bool OperandSizePrefix = false, AddressSizePrefix = false;
	vector <Byte> TrailerOpcode;
	const Register *regs[3] = {0, 0, 0};
	const Memory *mems[3] = {0, 0, 0};
	const Immediate *immeds[3] = {0, 0, 0};

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	switch (Arguments.size())
	{
		case 0:
			// Command with no arguments. Checks if the current mode affects the operand size prefix
			if (SizeArgument != -1)
				OperandSizePrefix = (DefaultMode == 16) && (CurrentMode == 32);

			// Copy its opcode to output
			Encoding.Write (Output);
			break;

		case 1:
			// Command with single argument. Calculates dw bits and checks for the operand size prefix
			i = Arguments[0]->GetData()->GetSize();
			if (i > 8) dw = 1 & dw_mask;

			if (SizeArgument == 0)
				OperandSizePrefix = ((CurrentMode == 16) && (i == 32)) || ((CurrentMode == 32) && (i == 16));

			// Checks for an immediate
			immeds[0] = dynamic_cast<const Immediate *> (Arguments[0]->GetData());
			if (immeds[0] != 0)
			{
				// If it is, just write it to the output
				Encoding.Write (Output);
				immeds[0]->Write (Output);
				break;
			}

			regs[0] = dynamic_cast<const Register *> (Arguments[0]->GetData());
			if (regs[0] != 0)
			{
				mod_reg_rm = 0xC0 + regs[0]->GetCode();
			}
			else
			{
				mems[0] = dynamic_cast<const Memory *> (Arguments[0]->GetData());
				if (mems[0] == 0) throw 0;

				// Checks for the need of prefixes
				AddressSizePrefix = (CurrentMode == 16) != (mems[0]->GetAddressSize() == 16);
				mod_reg_rm = mems[0]->GetCode();
				SegmentPrefix = mems[0]->GetSegmentPrefix ();

				// Schedule the SIB and displacement for writing
				mems[0]->WriteSIB (TrailerOpcode);
				mems[0]->WriteDisplacement (TrailerOpcode);
			}

			// Writes prefixes
			if (OperandSizePrefix) Output.push_back (0x66);
			if (AddressSizePrefix) Output.push_back (0x67);
			if (SegmentPrefix != 0) Output.push_back (SegmentPrefix);

			// Writes the encoding, the mod_reg_rm byte and the rest of the encoding
			(Encoding | Opcode (dw, mod_reg_rm)).Write (Output);
			Output.insert (Output.end(), TrailerOpcode.begin(), TrailerOpcode.end());
			break;

		case 2:
		case 3:
			if (!Arguments[0]->TypeCheck (*Arguments[1], Check))
			{
				// If type check fails, decides whether to throw an exception or return false.
				// Optimized syntaxes like add eax,byte 5 should not throw exceptions, because
				// they have alternate long forms.
				if (Precedence >= OptimizedPrecedence) throw 0;
				return false;
			}

			// Command with two or three arguments. Calculates dw bits and checks for the operand size prefix
			i = Arguments[0]->GetData()->GetSize();
			if (i > 8) dw = 1 & dw_mask;

			// Checks which argument (if any) affects the operand size prefix
			if (SizeArgument >= 0)
			{
				i = Arguments[SizeArgument]->GetData()->GetSize();
				OperandSizePrefix = ((CurrentMode == 16) && (i == 32)) || ((CurrentMode == 32) && (i == 16));
			}

			for (i = 0; i < Arguments.size(); i++)
			{
				// Checks if the argument is a register
				regs[i] = dynamic_cast <const Register *> (Arguments[i]->GetData());
				if (regs[i] != 0)
				{
					// If the register is the
					mod_reg_rm |= (i == 0) ? (regs[i]->GetCode() | 0xC0) : (regs[i]->GetCode() << 3);
					continue;
				}

				// Checks if the argument is a memory reference
				mems[i] = dynamic_cast<const Memory *> (Arguments[i]->GetData());
				if (mems[i] != 0)
				{
					// Checks for the need of prefixes
					AddressSizePrefix = (CurrentMode == 16) != (mems[i]->GetAddressSize() == 16);
					mod_reg_rm <<= 3;
					mod_reg_rm |= mems[i]->GetCode();
					SegmentPrefix = mems[i]->GetSegmentPrefix ();

					// Schedule the SIB and displacement for writing
					mems[i]->WriteSIB (TrailerOpcode);
					mems[i]->WriteDisplacement (TrailerOpcode);
					continue;
				}

				// Checks if the argument is an immediate value
				immeds[i] = dynamic_cast<const Immediate *> (Arguments[i]->GetData());
				if (immeds[i] != 0)
				{
					immeds[i]->Write (TrailerOpcode);
					continue;
				}

				throw 0;
			}

		// Writes prefixes
		if (OperandSizePrefix) Output.push_back (0x66);
		if (AddressSizePrefix) Output.push_back (0x67);
		if (SegmentPrefix != 0) Output.push_back (SegmentPrefix);

		// Sets the direction bit if the first argument is a register and the second one is a memory reference.
		if ((regs[0] != 0) && (mems[1] != 0)) dw |= (2 & dw_mask);

		// Writes the encoding, and the mod_reg_rm byte (if any)
		switch (mrr_usage)
		{
			case ABSENT:
				(Encoding | Opcode (dw)).Write (Output);
				break;

			case PARTIAL:
				(Encoding | Opcode (dw, mod_reg_rm)).Write (Output);
				break;				

			case PRESENT:
				(Encoding | Opcode (dw)).Write (Output);
				Output.push_back (mod_reg_rm);
				break;
		}

		// writes the rest of the encoding
		Output.insert (Output.end(), TrailerOpcode.begin(), TrailerOpcode.end());
		break;
	}

	return true;
}
