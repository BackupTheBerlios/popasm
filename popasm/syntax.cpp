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

#include <typeinfo>

#include "syntax.h"
#include "register.h"
#include "memory.h"
#include "immed.h"
#include "full_ptr.h"

Syntax::~Syntax () throw ()
{
	for (vector<const BasicArgument::IdFunctor *>::const_iterator i = ArgumentTypes.begin(); i != ArgumentTypes.end(); i++)
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

		case FULL_POINTER:
			Argument *arg = Arguments[0];
			unsigned int CurrentMode = CurrentAssembler->GetCurrentMode();

			if (((CurrentMode == 16) && (arg->GetSize() == 48)) ||
			    ((CurrentMode == 32) && (arg->GetSize() == 32))) break;

			return;
	}

	// Writes the operand size prefix
	Output.push_back (0x66);
}

bool Syntax::Match (vector<Argument *> &Arguments) const
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

const char JumpOutOfRange::WhatString[] = "Transfer to an out of range destination attempted.";
const char InvalidSegmentOverride::WhatString[] = "Cannot override default destination segment.";
const char AddressSizeMix::WhatString[] = "Cannot mix 16-bits and 32-bits memory access modes.";

bool ZerarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	// Writes the basic encoding and return success.
	Encoding.Write(Output);
	return true;
}

bool RetSyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	if (!Arguments.empty())
		return false;

	// Verifies the type of procedure
	Argument a;
	if (!(*ArgumentTypes.back())(a))
		return false;

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	// Writes the basic encoding and return success.
	Encoding.Write(Output);
	return true;
}

UnarySyntax::UnarySyntax (const Opcode &op, OperandSizeDependsOn dep, BasicArgument::IdFunctor *arg, Byte dwm = 0)
	throw () : Syntax (op, dep), dw_mask(dwm)
{
	ArgumentTypes.push_back (arg);
}

UnarySyntax::UnarySyntax (const Opcode &op, OperandSizeDependsOn dep, BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2, Byte dwm = 0)
	throw () : Syntax (op, dep), dw_mask(dwm)
{
	ArgumentTypes.push_back (arg1);
	ArgumentTypes.push_back (arg2);
}

bool UnarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	Byte dw = 0, mod_reg_rm = 0, SegmentPrefix = 0;
	vector <Byte> TrailerOpcode;
	const Register *reg = 0;
	const Memory *mem = 0;
	const Immediate *immed = 0;
	const FullPointer *fptr = 0;

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	const BasicArgument *arg = Arguments[(OperandSizePrefixUsage == SECOND_ARGUMENT) ? 1 : 0]->GetData();
	if (arg->GetSize() > 8) dw = 1 & dw_mask;

	// Checks for an immediate
	immed = dynamic_cast<const Immediate *> (Arguments[0]->GetData());
	if (immed != 0)
	{
		// If it is, just write it to the output
		(Encoding | Opcode (dw)).Write (Output);
		immed->Write (Output);
	}
	else
	{
		// Checks for full pointers
		fptr = dynamic_cast<const FullPointer *> (Arguments[0]->GetData());
		if (fptr != 0)
		{
			Encoding.Write (Output);
			fptr->Write (Output);
		}
		else
		{
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
				if (mem == 0) throw UnknownArgument (arg);

				// Checks for the need of prefix
				if (mem->UseAddressSizePrefix()) Output.push_back (0x67);
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
	}

	return true;
}

bool AdditiveUnarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	const Register *reg = 0;

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	// Writes the operand size prefix if necessary
	WriteOperandSizePrefix (Arguments, Output);

	// Gets the relevant argument for type specification
	switch (OperandSizePrefixUsage)
	{
		case SECOND_ARGUMENT:
			reg = dynamic_cast<const Register *> (Arguments[1]->GetData());
			break;

		default:
			reg = dynamic_cast<const Register *> (Arguments[0]->GetData());
			break;
	}

	// Writes the encoding, added by the register code
	(Encoding | Opcode (reg->GetCode())).Write (Output);

	if (Arguments.size() > 1)
	{
		const Immediate *immed = dynamic_cast<const Immediate *> (Arguments[1]->GetData());
		if (immed != 0) immed->Write (Output);
	}

	return true;
}

bool RelativeUnarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	vector<Byte> temp;

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	// Writes the operand size prefix (if necessary) and the encoding to a temporary location
	WriteOperandSizePrefix (Arguments, temp);
	Encoding.Write (temp);

	// Calculates the offset of the byte following the opcode
	unsigned long int FinalOffset = temp.size() + CurrentAssembler->GetCurrentOffset();

	// Calculates the relative distance between the end of the instruction and the address it targets
	const Immediate *immed = dynamic_cast <const Immediate *> (Arguments[0]->GetData());
	unsigned long int Target = immed->GetUnsignedLong();
	long int RelativeDistance = Target - FinalOffset;

	// Checks for SHORT or NEAR
	if (static_cast<RelativeArgument *> (ArgumentTypes[0])->GetRelativeDistance() == SHORT)
	{
		RelativeDistance--;
		if ((RelativeDistance < -128) || (RelativeDistance > 127))
		{
			if (immed->GetDistanceType() == SHORT)
				throw JumpOutOfRange();
			else
				return false;
		}

		temp.push_back (static_cast<Byte> (RelativeDistance & 0xFF));
	}
	else
	{
		unsigned int size = (immed->GetSize() == 0) ? CurrentAssembler->GetCurrentMode() : immed->GetSize();
		RelativeDistance -= size / 8;

		if (size == 16)
		{
			if ((RelativeDistance < -65535) || (RelativeDistance > 65535)) throw JumpOutOfRange();
			RelativeDistance &= 0xFFFF;
		}

		NaturalNumber (RelativeDistance).Write (temp, size / 8, 0);
	}

	Output.insert (Output.end(), temp.begin(), temp.end());
	return true;
}

BinarySyntax::BinarySyntax (const Opcode &op, OperandSizeDependsOn dep, bool i, Argument::CheckType chk, Byte dwm, ModRegRM_Usage usage,
	BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2, BasicArgument::IdFunctor *arg3 = 0) throw () : Syntax (op, dep, i)
{
	Check = chk;
	dw_mask = dwm;
	mrr_usage = usage;

	ArgumentTypes.push_back (arg1);
	ArgumentTypes.push_back (arg2);
	if (arg3 != 0) ArgumentTypes.push_back (arg3);
}

inline bool SpecialReg (const Register *reg)
{
	if (reg == 0) return false;

	return (dynamic_cast<const ControlRegister *> (reg) != 0) ||
		(dynamic_cast<const DebugRegister *> (reg) != 0) ||
		(dynamic_cast<const TestRegister *> (reg) != 0);
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

	// Verifies whether the combination of arguments is valid and their types match
	if (!Match (Arguments)) return false;
	if (Arguments.size() >= 2) Argument::TypeCheck (Arguments, Check);

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
			switch (i)
			{
				// The first argument is a register. Place it in the r/m field and set mod to 11B
				case 0:
					mod_reg_rm |= regs[0]->GetCode() | 0xC0;
					break;

				// The second argument is a register.
				case 1:
					// The code for this register must go in the reg field if:
					// 1. The previous argument is not a register or
					// 2. The previous argument is a register and ...
					if (regs[0] == 0)
					{
						mod_reg_rm |= regs[i]->GetCode() << 3;
					}
					else
					{
						// If Mod_Reg_r/m usage is EXCHANGED_REGS,
						// place first argument in reg field and the second one in r/m field
						if ((mrr_usage == EXCHANGED_REGS) || (SpecialReg (regs[0])))
						{
							mod_reg_rm <<= 3;
							mod_reg_rm |= 0xC0 | regs[1]->GetCode();
						}
						else
						{
							// Leaves the first argument in r/m field and places the second one into the reg field.
							mod_reg_rm |= regs[i]->GetCode() << 3;
						}
					}

					break;
			}

			continue;
		}

		// Checks if the argument is a memory reference
		mems[i] = dynamic_cast<const Memory *> (Arguments[i]->GetData());
		if (mems[i] != 0)
		{
			if (i == 2) continue;

			// Checks for the need of prefixes
			AddressSizePrefix = mems[i]->UseAddressSizePrefix();
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

		throw UnknownArgument(Arguments[i]->GetData());
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
      case EXCHANGED_REGS:
			(Encoding | Opcode (dw)).Write (Output);
			Output.push_back (mod_reg_rm);
			break;
	}

	// writes the rest of the encoding
	Output.insert (Output.end(), TrailerOpcode.begin(), TrailerOpcode.end());

	return true;
}

SuffixedBinarySyntax::SuffixedBinarySyntax (const Opcode &op,
	OperandSizeDependsOn dep, bool i, Argument::CheckType chk, Byte dwm, ModRegRM_Usage usage,
	BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2, Byte suf) throw ()
	: BinarySyntax (op, dep, i, chk, dwm, usage, arg1, arg2), Suffix(suf) {}

bool SuffixedBinarySyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	bool Result = BinarySyntax::Assemble (Arguments, Output);
	if (Result) Output.push_back (Suffix);
	return Result;
}

FPUBinarySyntax::FPUBinarySyntax (const Opcode &op, BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2) throw ()
	: Syntax (op, NOTHING, false)
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

StringSyntax::StringSyntax (const Opcode &op, OperandSizeDependsOn dep,
	Argument::CheckType chk, BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2, unsigned int ovr) throw ()
	: Syntax (op, dep, false), Overrideable (ovr), Check (chk)
{
	ArgumentTypes.push_back (arg1);
	if (arg2 != 0)	ArgumentTypes.push_back (arg2);
}

bool StringSyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	Byte SegmentPrefix = 0;
	bool AddressSizePrefix = false;
	const Memory *mem, *mem2;

	// Verifies the type of each argument
	if (!Match (Arguments)) return false;

	// Performs type checking
	if (Arguments.size() == 2)
	{
		Argument::TypeCheck (Arguments, Check);

		mem = dynamic_cast<const Memory *> (Arguments[0]->GetData());
		mem2 = dynamic_cast<const Memory *> (Arguments[1]->GetData());
		if ((mem != 0) && (mem2 != 0))
			if ((mem->GetAddressSize() != mem2->GetAddressSize()) && (mem->GetAddressSize() != 0) && (mem2->GetAddressSize() != 0))
				throw AddressSizeMix();
	}

	// Gets the relevant argument for type specification
	switch (OperandSizePrefixUsage)
	{
		case SECOND_ARGUMENT:
			mem = dynamic_cast<const Memory *> (Arguments[1]->GetData());
			break;

		default:
			mem = dynamic_cast<const Memory *> (Arguments[0]->GetData());
			break;
	}

	// Checks for the need for prefixes
	AddressSizePrefix = mem->UseAddressSizePrefix();

	for (unsigned int i = 0; i < Arguments.size(); i++)
	{
		Byte temp;
		mem = dynamic_cast<const Memory *> (Arguments[i]->GetData());
		if (mem == 0) continue;

		temp = mem->GetSegmentPrefix ();
		if (i != Overrideable)
		{
			switch (temp)
			{
				case 0:
					break;

				case 0x26:
//					Warning (new OverrideIgnored (0x26));
					cout << "Ignoring segment prefix." << endl;
					break;

				default:
					// If he attempts to override it with other segment, throw exception
					throw InvalidSegmentOverride();
			}
		}
		else
			SegmentPrefix = temp;
	}

	// Writes prefixes
	WriteOperandSizePrefix (Arguments, Output);
	if (AddressSizePrefix) Output.push_back (0x67);
	if (SegmentPrefix != 0)
		Output.push_back (SegmentPrefix);

	Encoding.Write (Output);

	return true;
}
