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

void Instruction::Assemble (const BasicSymbol *sym, vector<Argument *> &Arguments, vector<Byte> &Encoding) const
{
	// Include in the symbol table the creation of the new label
	if (sym != 0)
	{
		throw 0;
	}

	// Checks all syntaxes avaiable
	for (ContainerType::const_reverse_iterator i = Syntaxes.rbegin(); i != Syntaxes.rend(); i++)
	{
		if ((*i)->Assemble (Arguments, Encoding)) return;
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
		new ZeraryInstruction ("AAA", 1, 0x37, 0, 0),
		new ZeraryUnaryInstruction ("AAD", 2, 0xD5, 0x0A, 0,
		                                   1, 0xD5,    0, 0, &typeid(UnsignedByte)),
		new ZeraryUnaryInstruction ("AAM", 2, 0xD4, 0x0A, 0,
		                                   1, 0xD4,    0, 0, &typeid(UnsignedByte)),
		new ZeraryInstruction ("AAS", 1, 0x3F, 0, 0),
		new BinaryInstruction ("ADC", 0x14, 0x80, 0x83, 0x02, 0x10),
		new BinaryInstruction ("ADD", 0x04, 0x80, 0x83, 0x00, 0x00),
		new BinaryInstruction ("AND", 0x24, 0x80, 0x83, 0x04, 0x20)
	};

	for (unsigned int i = 0; i < sizeof (Instructions) / sizeof (Instruction *); i++)
		InstructionTable.Insert (Instructions[i]);
}

Instruction::~Instruction () throw ()
{
	for (ContainerType::const_iterator i = Syntaxes.begin(); i != Syntaxes.end(); i++)
		delete *i;
}

ZeraryInstruction::ZeraryInstruction (const string &nm, unsigned int n, Byte b0, Byte b1, Byte b2) throw () : Instruction (nm)
{
	AddSyntax (new Syntax (100, n, b0, b1, b2));
}

ZeraryUnaryInstruction::ZeraryUnaryInstruction (const string &nm,
	unsigned int n0, Byte b00, Byte b01, Byte b02,
	unsigned int n1, Byte b10, Byte b11, Byte b12, const type_info *t1) throw () : Instruction (nm)
{
	AddSyntax (new Syntax (100, n0, b00, b01, b02));
	AddSyntax (new Syntax (200, n1, b10, b11, b12, false, t1));
}

BinaryInstruction::BinaryInstruction (const string &nm, Byte Accum, Byte Immed, Byte Immed8, Byte ConstReg, Byte RegMem) throw () : Instruction (nm)
{
	AddSyntax (new Syntax (100, 1, Accum, 0, 0, false, &typeid(Accumulator), &typeid(Immediate)));
	AddSyntax (new Syntax (100, 2, Immed8, ConstReg, 0, false, &typeid(GPRegister), &typeid(SignedByte)));
	AddSyntax (new Syntax (100, 2, Immed8, ConstReg, 0, false, &typeid(Memory), &typeid(SignedByte)));
	AddSyntax (new Syntax (100, 2, Immed, ConstReg, 0, false, &typeid(GPRegister), &typeid(Immediate)));
	AddSyntax (new Syntax (100, 2, Immed, ConstReg, 0, false, &typeid(Memory), &typeid(Immediate)));
	AddSyntax (new Syntax (100, 1, RegMem, 0, 0, false, &typeid(GPRegister), &typeid(GPRegister)));
	AddSyntax (new Syntax (100, 1, RegMem, 0, 0, true, &typeid(GPRegister), &typeid(Memory)));
}
