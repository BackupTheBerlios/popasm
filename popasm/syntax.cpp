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
#include "immed.h"

Syntax::~Syntax () throw ()
{
	for (vector<const BasicIdFunctor *>::const_iterator i = ArgumentTypes.begin(); i != ArgumentTypes.end(); i++)
		delete *i;
}

void Syntax::WriteOperandSizePrefix (Argument *arg, vector<Byte> &Output) const throw ()
{
	unsigned int CurrentMode = CurrentAssembler->GetCurrentMode();

	if (((CurrentMode == 16) && (arg->GetSize() == 32)) ||
	    ((CurrentMode == 32) && (arg->GetSize() == 16))) Output.push_back (0x66);
}

void Syntax::WriteOperandSizePrefix (vector<Argument *> &Arguments, vector<Byte> &Output) const throw ()
{
	switch (OperandSizePrefixUsage)
	{
		case FIRST_ARGUMENT:
			WriteOperandSizePrefix (Arguments[0], Output);
			return;

		case SECOND_ARGUMENT:
			WriteOperandSizePrefix (Arguments[1], Output);
			return;

		case THIRD_ARGUMENT:
			WriteOperandSizePrefix (Arguments[2], Output);
			return;

		case NOTHING:
			return;

		case MODE_16BITS:
			if (CurrentAssembler->GetCurrentMode() == 16) return;
			break;

		case MODE_32BITS:
			if (CurrentAssembler->GetCurrentMode() == 32) return;
			break;
	}

	// Writes the operand size prefix
	Output.push_back (0x66);
}

bool Syntax::Match (vector<Argument *> &Arguments) const throw ()
{
	bool Result = true;

	// Checks arity
	if (ArgumentTypes.size() != Arguments.size()) return false;

	// Checks the types of each argument
	for (unsigned int i = 0; i != ArgumentTypes.size(); i++)
	{
		if (!Arguments[i]->Match (ArgumentTypes[i]))
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

bool ZerarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const throw ()
{
	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	// Writes the basic encoding and return success.
	Encoding.Write(Output);
	return true;
}

UnarySyntax::UnarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg, Byte dwm = 0)
	throw () : Syntax (p, op, dep), dw_mask(dwm)
{
	ArgumentTypes.push_back (arg);
}

UnarySyntax::UnarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg1, BasicIdFunctor *arg2, Byte dwm = 0)
	throw () : Syntax (p, op, dep), dw_mask(dwm)
{
	ArgumentTypes.push_back (arg1);
	ArgumentTypes.push_back (arg2);
}

bool UnarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const throw ()
{
	Byte dw = 0, mod_reg_rm = 0, SegmentPrefix = 0;
	vector <Byte> TrailerOpcode;
	const Register *reg = 0;
	const Memory *mem = 0;
	const Immediate *immed = 0;

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	// Checks for an immediate
	immed = dynamic_cast<const Immediate *> (Arguments[0]->GetData());
	if (immed != 0)
	{
		// If it is, just write it to the output
		Encoding.Write (Output);
		immed->Write (Output);
	}
	else
	{
		const BasicArgument *arg = Arguments[0]->GetData();
		if (arg->GetSize() > 8) dw = 1 & dw_mask;

		// Checks for register
		reg = dynamic_cast<const Register *> (arg);
		if (reg != 0)
		{
			mod_reg_rm = 0xC0 + reg->GetCode();
		}
		else
		{
			// If neither immediate nor register, try memory
			mem = dynamic_cast<const Memory *> (arg);
			if (mem == 0) throw 0;

			// Checks for the need of prefix
			if ((CurrentAssembler->GetCurrentMode() == 16) != (mem->GetAddressSize() == 16)) Output.push_back (0x67);
			mod_reg_rm = mem->GetCode();
			SegmentPrefix = mem->GetSegmentPrefix ();
			if (SegmentPrefix != 0) Output.push_back (SegmentPrefix);

			// Schedule the SIB and displacement for writing
			mem->WriteSIB (TrailerOpcode);
			mem->WriteDisplacement (TrailerOpcode);
		}

		// Writes the encoding, the mod_reg_rm byte and the rest of the encoding
		((Encoding << 3) | Opcode (dw, mod_reg_rm)).Write (Output);
		Output.insert (Output.end(), TrailerOpcode.begin(), TrailerOpcode.end());
	}

	return true;
}

bool AdditiveUnarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const throw ()
{
	const Register *reg = 0;

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	// Gets the register
	reg = dynamic_cast<const Register *> (Arguments[0]->GetData());

	// Writes the encoding, added by the register code
	(Encoding | Opcode (reg->GetCode())).Write (Output);

	if (Arguments.size() > 1)
	{
		const Immediate *immed = dynamic_cast<const Immediate *> (Arguments[1]->GetData());
		if (immed != 0) immed->Write (Output);
	}

	return true;
}

BinarySyntax::BinarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, bool i, Argument::CheckType chk,
	Byte dwm, ModRegRM_Usage usage, BasicIdFunctor *arg1, BasicIdFunctor *arg2, BasicIdFunctor *arg3 = 0) throw () : Syntax (p, op, dep, i)
{
	Check = chk;
	dw_mask = dwm;
	mrr_usage = usage;

	ArgumentTypes.push_back (arg1);
	ArgumentTypes.push_back (arg2);
	if (arg3 != 0) ArgumentTypes.push_back (arg3);
}

bool BinarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	unsigned int i;
	Byte SegmentPrefix = 0;
	Byte dw = 0, mod_reg_rm = 0;
	bool AddressSizePrefix = false;
	vector <Byte> TrailerOpcode;
	const Register *regs[3] = {0, 0, 0};
	const Memory *mems[3] = {0, 0, 0};
	const Immediate *immeds[3] = {0, 0, 0};

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	if (!Arguments[0]->TypeCheck (*Arguments[1], Check))
	{
		// If type check fails, decides whether to throw an exception or return false.
		// Optimized syntaxes like add eax,byte 5 should not throw exceptions, because
		// they have alternate long forms.
		if (Precedence >= OptimizedPrecedence) throw 0;
			return false;
	}

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	// Command with two or three arguments. Calculates dw bits and checks for the operand size prefix
	i = Arguments[0]->GetData()->GetSize();
	if (i > 8) dw = 1 & dw_mask;

	for (i = 0; i < Arguments.size(); i++)
	{
		// Checks if the argument is a register
		regs[i] = dynamic_cast <const Register *> (Arguments[i]->GetData());
		if (regs[i] != 0)
		{
			if (i == 2) continue;

			// If the register is special (segment, control, test or debug) or the second one, write it in reg field
			// If the register is the first argument, write it in the r/m field
			if ((dynamic_cast <const GPRegister *>  (Arguments[i]->GetData()) == 0) &&
			    (dynamic_cast <const FPURegister *> (Arguments[i]->GetData()) == 0))
			{
				mod_reg_rm |= regs[i]->GetCode() << 3;
				if (i == 0) mod_reg_rm |= 0xC0;
			}
			else
			{
				mod_reg_rm |= (i == 0) ? (regs[i]->GetCode() | 0xC0) : (regs[i]->GetCode() << 3);
			}

			continue;
		}

		// Checks if the argument is a memory reference
		mems[i] = dynamic_cast<const Memory *> (Arguments[i]->GetData());
		if (mems[i] != 0)
		{
			if (i == 2) continue;

			// Checks for the need of prefixes
			AddressSizePrefix = (CurrentAssembler->GetCurrentMode() == 16) != (mems[i]->GetAddressSize() == 16);
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
	if (AddressSizePrefix) Output.push_back (0x67);
	if (SegmentPrefix != 0) Output.push_back (SegmentPrefix);

	// Sets the direction bit if the first argument is a register and the second one is a memory reference.
	if (regs[0] != 0)
	{
		if ((mems[1] != 0) || (dynamic_cast<const GPRegister *> (regs[0]) == 0)) dw |= (2 & dw_mask);
	}

	// Writes the encoding, and the mod_reg_rm byte (if any)
	switch (mrr_usage)
	{
		case ABSENT:
			(Encoding | Opcode (dw)).Write (Output);
			break;

		case PARTIAL:
			((Encoding << 3) | Opcode (dw, mod_reg_rm)).Write (Output);
			break;				

		case REPEATED:
			mod_reg_rm |= (mod_reg_rm & 7) << 3;

		case PRESENT:
			(Encoding | Opcode (dw)).Write (Output);
			Output.push_back (mod_reg_rm);
			break;
	}

	// writes the rest of the encoding
	Output.insert (Output.end(), TrailerOpcode.begin(), TrailerOpcode.end());

	return true;
}

FPUBinarySyntax::FPUBinarySyntax (unsigned int p, const Opcode &op, BasicIdFunctor *arg1, BasicIdFunctor *arg2) throw ()
	: Syntax (p, op, NOTHING, false)
{
	ArgumentTypes.push_back (arg1);
	ArgumentTypes.push_back (arg2);
}

bool FPUBinarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	const FPURegister *FPUReg1 = dynamic_cast<const FPURegister *> (Arguments[0]->GetData());
	const FPURegister *FPUReg2 = dynamic_cast<const FPURegister *> (Arguments[1]->GetData());

	// Adds the code of the non-ST argument
	(Encoding | ((FPUReg1->GetCode() == 0) ? FPUReg2->GetCode() : FPUReg1->GetCode())).Write (Output);
	return true;
}

StringSyntax::StringSyntax (unsigned int p, const Opcode &op, BasicIdFunctor *arg1, bool ovr) throw ()
	: Syntax (p, op, FIRST_ARGUMENT, false), Overrideable (ovr), Check (Argument::NONE)
{
	ArgumentTypes.push_back (arg1);
}

StringSyntax::StringSyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, Argument::CheckType chk,
	BasicIdFunctor *arg1, BasicIdFunctor *arg2) throw () : Syntax (p, op, dep, false), Overrideable (false), Check(chk)
{
	ArgumentTypes.push_back (arg1);
	ArgumentTypes.push_back (arg2);
}

bool StringSyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	Byte SegmentPrefix = 0;
	bool AddressSizePrefix = false;
	const Memory *mem;

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	if (Arguments.size() == 2)
	{
		if (!Arguments[0]->TypeCheck (*Arguments[1], Check))
		{
			// If type check fails, decides whether to throw an exception or return false.
			// Optimized syntaxes like add eax,byte 5 should not throw exceptions, because
			// they have alternate long forms.
			if (Precedence >= OptimizedPrecedence) throw 0;
				return false;
		}
	}

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	switch (OperandSizePrefixUsage)
	{
		case FIRST_ARGUMENT:
			mem = dynamic_cast<const Memory *> (Arguments[0]->GetData());
			break;

		case SECOND_ARGUMENT:
			mem = dynamic_cast<const Memory *> (Arguments[1]->GetData());
			break;

		default:
			break;
	}

	// Checks if the argument is a memory reference
	if (mem == 0) throw 0;

	// Checks for the need of prefixes
	AddressSizePrefix = (CurrentAssembler->GetCurrentMode() == 16) != (mem->GetAddressSize() == 16);
	SegmentPrefix = mem->GetSegmentPrefix ();

	// Writes prefixes
	if (AddressSizePrefix) Output.push_back (0x67);
	if (SegmentPrefix != 0)
	{
		if (!Overrideable) throw 0;
		Output.push_back (SegmentPrefix);
	}

	Encoding.Write (Output);
	return true;
}
