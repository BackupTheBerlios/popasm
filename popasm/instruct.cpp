/***************************************************************************
                          instruct.cpp  -  description
                             -------------------
    begin                : Wed Jun 19 2002
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

#include "instruct.h"
#include <iostream>

BasicSymbol *Instruction::Read (const string &str, InputFile &inp)
{
	Instruction s (str);
	Instruction * const *i = InstructionTable.Find (&s);

	return (i == 0) ? 0 : *i;
}

void Instruction::Assemble (const BasicSymbol *sym, vector<Argument *> &Arguments, vector<Byte> &Encoding, unsigned int CurrentMode) const
{
	// Include in the symbol table the creation of the new label
	if (sym != 0)
	{
		cout << "Label definition not yet implemented." << endl;
		throw 0;
	}

	// Checks all syntaxes avaiable
	for (ContainerType::const_reverse_iterator i = Syntaxes.rbegin(); i != Syntaxes.rend(); i++)
	{
		if ((*i)->Assemble (Arguments, Encoding, CurrentMode)) return;
	}

	cout << "Syntax not found" << endl;
	// None of them fits, so throw exception
	throw 0;
}

HashTable <Instruction *, HashFunctor, PointerComparator<BasicSymbol> > Instruction::InstructionTable =
	HashTable <Instruction *, HashFunctor, PointerComparator<BasicSymbol> > ();

void Instruction::SetupInstructionTable () throw ()
{
	static Instruction *Instructions[] =
	{
		new ZeraryInstruction ("AAA", Opcode (0x37)),
		new ZeraryUnaryInstruction ("AAD", Opcode (0xD5, 0x0A),
		                                   Opcode (0xD5), UnsignedByte::ClassInstance),
		new ZeraryUnaryInstruction ("AAM", Opcode (0xD4, 0x0A),
		                                   Opcode (0xD4), UnsignedByte::ClassInstance),
		new ZeraryInstruction ("AAS", Opcode (0x3F)),
		new OptimizedBinaryInstruction ("ADC", Opcode (0x14), Opcode(0x80), Opcode(0x83), Opcode (0x10), 2),
		new OptimizedBinaryInstruction ("ADD", Opcode (0x04), Opcode(0x80), Opcode(0x83), Opcode (0x00), 0),
		new OptimizedBinaryInstruction ("AND", Opcode (0x24), Opcode(0x80), Opcode(0x83), Opcode (0x20), 4),
		new ARPLInstruction ("ARPL", 0x63)
	};

	for (unsigned int i = 0; i < sizeof (Instructions) / sizeof (Instruction *); i++)
		InstructionTable.Insert (Instructions[i]);
}

Instruction::~Instruction () throw ()
{
	for (ContainerType::const_iterator i = Syntaxes.begin(); i != Syntaxes.end(); i++)
		delete *i;
}

ZeraryInstruction::ZeraryInstruction (const string &nm, const Opcode &op) throw () : Instruction (nm)
{
	AddSyntax (new Syntax (100, op, 0));
}

ZeraryUnaryInstruction::ZeraryUnaryInstruction (const string &nm, const Opcode &op0, const Opcode &op1,
	const BasicArgument * const t1) throw () : Instruction (nm)
{
	AddSyntax (new Syntax (100, op0, 0));
	AddSyntax (new Syntax (200, op1, t1, 0, Syntax::ABSENT));
}

ARPLInstruction::ARPLInstruction (const string &nm, const Opcode &op) throw () : Instruction (nm)
{
	// GPRegister16/Memory16, GPRegister16, not affected by operand size prefix
	AddSyntax (new Syntax (100,   op,
		GPRegister16Bits::ClassInstance,  GPRegister16Bits::ClassInstance, Argument::NONE,  false, 0, Syntax::PRESENT, -1));
	AddSyntax (new Syntax (110,   op,
		Memory::ClassInstance,            GPRegister16Bits::ClassInstance, Argument::EQUAL, false, 0, Syntax::PRESENT, -1));
}

OptimizedBinaryInstruction::OptimizedBinaryInstruction (const string &nm, const Opcode &Accum,
	const Opcode &Immed, const Opcode &Immed8, const Opcode &RegMem, Byte ConstReg) throw () : Instruction (nm)
{
	// GPRegister/Memory, GPRegister/Memory 			(except Memory to Memory)
	AddSyntax (new Syntax (100,   RegMem,
		GPRegister::ClassInstance,  GPRegister::ClassInstance,      Argument::EQUAL,   false, 3, Syntax::PRESENT));
	AddSyntax (new Syntax (110,   RegMem,
		GPRegister::ClassInstance,  Memory::ClassInstance,          Argument::EQUAL,   true,  3, Syntax::PRESENT));

	//	GPRegister/Memory, Immed
	AddSyntax (new Syntax (120,   Immed + (ConstReg << 3),
		GPRegister::ClassInstance,  Immediate::ClassInstance,       Argument::EQUAL,   false, 1, Syntax::PARTIAL));
	AddSyntax (new Syntax (130,   Immed + (ConstReg << 3),
		Memory::ClassInstance,      Immediate::ClassInstance,       Argument::EQUAL,   false, 1, Syntax::PARTIAL));

	//	Accum, Immed
	AddSyntax (new Syntax (200,   Accum,
		Accumulator::ClassInstance, Immediate::ClassInstance,       Argument::EQUAL,   false, 1, Syntax::ABSENT));

	// GPRegister/Memory, SignExtend (Byte Immed)
	AddSyntax (new Syntax (10300, Immed8 + (ConstReg << 3),
		GPRegister16Bits::ClassInstance, SignedByte::ClassInstance, Argument::GREATER, false, 1, Syntax::PARTIAL));
	AddSyntax (new Syntax (10305, Immed8 + (ConstReg << 3),
		GPRegister32Bits::ClassInstance, SignedByte::ClassInstance, Argument::GREATER, false, 1, Syntax::PARTIAL));
	AddSyntax (new Syntax (10310, Immed8 + (ConstReg << 3),
		Memory::ClassInstance,           SignedByte::ClassInstance, Argument::GREATER, false, 1, Syntax::PARTIAL));
}
