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

#include <iostream>
#include "instruct.h"
#include "immed.h"

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

typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           GPRegister::IdFunctor> 	   MemGPReg;
typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           GPRegister16Bits::IdFunctor> MemReg16;
typedef BinaryCompose <logical_or<bool>,  GPRegister16Bits::IdFunctor, GPRegister32Bits::IdFunctor> WordReg;
typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           WordReg>                     MemWordReg;

typedef BinaryCompose <logical_and<bool>, GPRegister::IdFunctor,       Register::CompareCodeFunctor<0> > Accumulator;
typedef BinaryCompose <logical_and<bool>, FPURegister::IdFunctor,      Register::CompareCodeFunctor<0> > ST;
typedef BinaryCompose <logical_and<bool>, GPRegister16Bits::IdFunctor, Register::CompareCodeFunctor<0> > AX;

void Instruction::SetupInstructionTable () throw ()
{
	static Instruction *Instructions[] =
	{
		new Instruction ("AAA",
			new ZerarySyntax        (100, Opcode (0x37),             Syntax::NOTHING)),

		new Instruction ("AAD",
			new UnarySyntax         (100, Opcode (0xD5),             Syntax::NOTHING, new UnsignedByte::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xD5, 0x0A),       Syntax::NOTHING)),

		new Instruction ("AAM",
			new UnarySyntax         (100, Opcode (0xD4),             Syntax::NOTHING, new UnsignedByte::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xD4, 0x0A),       Syntax::NOTHING)),

		new Instruction ("AAS",
			new ZerarySyntax        (100, Opcode (0x3F),             Syntax::NOTHING)),

		new OptimizedBinaryInstruction ("ADC", Opcode (0x14), Opcode(0x80, 0x02), Opcode(0x83, 0x02), Opcode (0x10)),
		new OptimizedBinaryInstruction ("ADD", Opcode (0x04), Opcode(0x80, 0x00), Opcode(0x83, 0x00), Opcode (0x00)),
		new OptimizedBinaryInstruction ("AND", Opcode (0x24), Opcode(0x80, 0x04), Opcode(0x83, 0x04), Opcode (0x20)),

		new Instruction ("ARPL",
			new BinarySyntax        (100, Opcode (0x63),             Syntax::NOTHING,        false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new MemReg16(),   new GPRegister16Bits::IdFunctor())),

		new Instruction ("BOUND",
			new BinarySyntax        (100, Opcode (0x62),             Syntax::FIRST_ARGUMENT, false, Argument::HALF,  0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		new Instruction ("BSF",
			new BinarySyntax        (100, Opcode (0x0F, 0xBC),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("BSR",
			new BinarySyntax        (100, Opcode (0x0F, 0xBD),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("BSWAP",
			new AdditiveUnarySyntax (100, Opcode (0x0F, 0xC8),       Syntax::FIRST_ARGUMENT,                                                   new GPRegister32Bits::IdFunctor())),

		new Instruction ("BT",
			new BinarySyntax        (100, Opcode (0x0F, 0xA3),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x04), Syntax::FIRST_ARGUMENT, false, Argument::NONE,  0, BinarySyntax::PARTIAL, new MemWordReg(), new UnsignedByte::IdFunctor())),

		new Instruction ("BTC",
			new BinarySyntax        (100, Opcode (0x0F, 0xBB),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x07), Syntax::FIRST_ARGUMENT, false, Argument::NONE,  0, BinarySyntax::PARTIAL, new MemWordReg(), new UnsignedByte::IdFunctor())),

		new Instruction ("BTR",
			new BinarySyntax        (100, Opcode (0x0F, 0xB3),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x06), Syntax::FIRST_ARGUMENT, false, Argument::NONE,  0, BinarySyntax::PARTIAL, new MemWordReg(), new UnsignedByte::IdFunctor())),

		new Instruction ("BTS",
			new BinarySyntax        (100, Opcode (0x0F, 0xAB),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x05), Syntax::FIRST_ARGUMENT, false, Argument::NONE,  0, BinarySyntax::PARTIAL, new MemWordReg(), new UnsignedByte::IdFunctor())),

		new Instruction ("CBW",
			new ZerarySyntax        (100, Opcode (0x98),             Syntax::MODE_16BITS)),

		new Instruction ("CDQ",
			new ZerarySyntax        (100, Opcode (0x99),             Syntax::MODE_32BITS)),

		new Instruction ("CLC",
			new ZerarySyntax        (100, Opcode (0xF8),             Syntax::NOTHING)),

		new Instruction ("CLD",
			new ZerarySyntax        (100, Opcode (0xFC),             Syntax::NOTHING)),

		new Instruction ("CLI",
			new ZerarySyntax        (100, Opcode (0xFA),             Syntax::NOTHING)),

		new Instruction ("CLTS",
			new ZerarySyntax        (100, Opcode (0x0F, 0X06),       Syntax::NOTHING)),

		new Instruction ("CMC",
			new ZerarySyntax        (100, Opcode (0xF5),             Syntax::NOTHING)),

		new Instruction ("CMOVA",
			new BinarySyntax        (100, Opcode (0x0F, 0x47),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVAE",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVB",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVBE",
			new BinarySyntax        (100, Opcode (0x0F, 0x46),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVC",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVE",
			new BinarySyntax        (100, Opcode (0x0F, 0x44),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVG",
			new BinarySyntax        (100, Opcode (0x0F, 0x4F),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVGE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4D),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVL",
			new BinarySyntax        (100, Opcode (0x0F, 0x4C),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVLE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4E),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNA",
			new BinarySyntax        (100, Opcode (0x0F, 0x46),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNAE",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNB",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNBE",
			new BinarySyntax        (100, Opcode (0x0F, 0x47),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNC",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNE",
			new BinarySyntax        (100, Opcode (0x0F, 0x45),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNG",
			new BinarySyntax        (100, Opcode (0x0F, 0x4E),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNGE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4C),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNL",
			new BinarySyntax        (100, Opcode (0x0F, 0x4D),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNLE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4F),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNO",
			new BinarySyntax        (100, Opcode (0x0F, 0x41),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNP",
			new BinarySyntax        (100, Opcode (0x0F, 0x4B),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNS",
			new BinarySyntax        (100, Opcode (0x0F, 0x49),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNZ",
			new BinarySyntax        (100, Opcode (0x0F, 0x45),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVO",
			new BinarySyntax        (100, Opcode (0x0F, 0x40),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVP",
			new BinarySyntax        (100, Opcode (0x0F, 0x4A),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVPE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4A),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVPO",
			new BinarySyntax        (100, Opcode (0x0F, 0x4B),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVS",
			new BinarySyntax        (100, Opcode (0x0F, 0x48),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVZ",
			new BinarySyntax        (100, Opcode (0x0F, 0x44),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new OptimizedBinaryInstruction ("CMP", Opcode (0x3C), Opcode(0x80, 0x07), Opcode(0x83, 0x07), Opcode (0x38)),

		new Instruction ("CMPXCHG",
			new BinarySyntax        (100, Opcode (0x0F, 0xB0),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL, 1, BinarySyntax::PRESENT, new MemGPReg(),   new GPRegister::IdFunctor())),

		new Instruction ("CMPXCHG8B",
			new UnarySyntax         (100, Opcode (0x0F, 0xC7, 0x01), Syntax::NOTHING,                                                          new Mem<64>())),

		new Instruction ("CPUID",
			new ZerarySyntax        (100, Opcode (0x0F, 0xA2),       Syntax::NOTHING)),

		new Instruction ("CWD",
			new ZerarySyntax        (100, Opcode (0x99),             Syntax::MODE_16BITS)),

		new Instruction ("CWDE",
			new ZerarySyntax        (100, Opcode (0x98),             Syntax::MODE_32BITS)),

		new Instruction ("DAA",
			new ZerarySyntax        (100, Opcode (0x27),             Syntax::NOTHING)),

		new Instruction ("DAS",
			new ZerarySyntax        (100, Opcode (0x2F),             Syntax::NOTHING)),

		new Instruction ("DEC",
			new UnarySyntax         (100, Opcode (0xFE, 0x01),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1),
			new AdditiveUnarySyntax (110, Opcode (0x48),             Syntax::FIRST_ARGUMENT,                                                   new WordReg())),

		new Instruction ("DIV",
			new UnarySyntax         (100, Opcode (0xF6, 0x06),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1)),

		new Instruction ("EMMS",
			new ZerarySyntax        (100, Opcode (0x0F, 0x77),       Syntax::NOTHING)),

		new Instruction ("ENTER",
			new BinarySyntax        (100, Opcode (0xC8),             Syntax::NOTHING,        false, Argument::NONE , 0, BinarySyntax::ABSENT,  new UnsignedWord::IdFunctor(), new UnsignedByte::IdFunctor())),

		new Instruction ("F2XM1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF0),       Syntax::NOTHING)),

		new Instruction ("FABS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE1),       Syntax::NOTHING)),

		new Instruction ("FADD",
			new UnarySyntax         (100, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x00),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xC0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FADDP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xC1),       Syntax::NOTHING)),

		new Instruction ("FIADD",
			new UnarySyntax         (100, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FBLD",
			new UnarySyntax         (100, Opcode (0xDF, 0x04),       Syntax::NOTHING,                                                          new Mem<80, Memory::BCD>())),

		new Instruction ("FBSTP",
			new UnarySyntax         (100, Opcode (0xDF, 0x06),       Syntax::NOTHING,                                                          new Mem<80, Memory::BCD>())),

		new Instruction ("FCHS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE0),       Syntax::NOTHING)),

		new Instruction ("FCLEX",
			new ZerarySyntax        (100, Opcode (0x9B, 0xDB, 0xE2), Syntax::NOTHING)),

		new Instruction ("FNCLEX",
			new ZerarySyntax        (100, Opcode (0xDB, 0xE2),       Syntax::NOTHING)),

		new Instruction ("FCMOVB",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVE",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVBE",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xD0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVU",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xD8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVNB",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVNE",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVNBE",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xD0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVNU",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xD8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCOM",
			new UnarySyntax         (100, Opcode (0xD8, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x02),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xD8, 0xD0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD8, 0xD1),       Syntax::NOTHING)),

		new Instruction ("FCOMP",
			new UnarySyntax         (100, Opcode (0xD8, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x03),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xD8, 0xD8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD8, 0xD9),       Syntax::NOTHING)),

		new Instruction ("FCOMPP",
			new ZerarySyntax        (100, Opcode (0xDE, 0xD9),       Syntax::NOTHING)),

		new Instruction ("FCOMI",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCOMIP",
			new FPUBinarySyntax     (100, Opcode (0xDF, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FUCOMI",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FUCOMIP",
			new FPUBinarySyntax     (100, Opcode (0xDF, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCOS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFF),       Syntax::NOTHING)),

		new Instruction ("FDECSTP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF6),       Syntax::NOTHING)),

		new Instruction ("FDIV",
			new UnarySyntax         (100, Opcode (0xD8, 0x06),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x06),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xF8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FDIVP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xF8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xF9),       Syntax::NOTHING)),

		new Instruction ("FIDIV",
			new UnarySyntax         (100, Opcode (0xDA, 0x06),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x06),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FDIVR",
			new UnarySyntax         (100, Opcode (0xD8, 0x07),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x07),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xF8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xF0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FDIVRP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xF1),       Syntax::NOTHING)),

		new Instruction ("FIDIVR",
			new UnarySyntax         (100, Opcode (0xDA, 0x07),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FFREE",
			new AdditiveUnarySyntax (100, Opcode (0xDD, 0xC0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		new Instruction ("FICOM",
			new UnarySyntax         (100, Opcode (0xDE, 0x02),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDA, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		new Instruction ("FICOMP",
			new UnarySyntax         (100, Opcode (0xDE, 0x03),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDA, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		new Instruction ("FILD",
			new UnarySyntax         (100, Opcode (0xDF, 0x00),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDB, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (120, Opcode (0xDF, 0x05),       Syntax::NOTHING,                                                          new Mem<64, Memory::INTEGER>())),

		new Instruction ("FINCSTP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF7),       Syntax::NOTHING)),

		new Instruction ("FINIT",
			new ZerarySyntax        (100, Opcode (0x9B, 0xDB, 0xE3), Syntax::NOTHING)),

		new Instruction ("FNINIT",
			new ZerarySyntax        (100, Opcode (0xDB, 0xE3),       Syntax::NOTHING)),

		new Instruction ("FIST",
			new UnarySyntax         (100, Opcode (0xDF, 0x02),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDB, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		new Instruction ("FISTP",
			new UnarySyntax         (100, Opcode (0xDF, 0x03),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDF, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (120, Opcode (0xDF, 0x07),       Syntax::NOTHING,                                                          new Mem<64, Memory::INTEGER>())),

		new Instruction ("FLD",
			new UnarySyntax         (100, Opcode (0xD9, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x00),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new UnarySyntax         (120, Opcode (0xDB, 0x05),       Syntax::NOTHING,                                                          new Mem<80, Memory::FLOAT>()),
			new AdditiveUnarySyntax (130, Opcode (0xD9, 0xC0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		new Instruction ("FLD1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE8),       Syntax::NOTHING)),

		new Instruction ("FLDL2T",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE9),       Syntax::NOTHING)),

		new Instruction ("FLDL2E",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEA),       Syntax::NOTHING)),

		new Instruction ("FLDPI",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEB),       Syntax::NOTHING)),

		new Instruction ("FLDLG2",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEC),       Syntax::NOTHING)),

		new Instruction ("FLDLN2",
			new ZerarySyntax        (100, Opcode (0xD9, 0xED),       Syntax::NOTHING)),

		new Instruction ("FLDZ",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEE),       Syntax::NOTHING)),

		new Instruction ("FLDCW",
			new UnarySyntax         (100, Opcode (0xD9, 0x05),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FLDENV",
			new UnarySyntax         (100, Opcode (0xD9, 0x04),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FMUL",
			new UnarySyntax         (100, Opcode (0xD8, 0x01),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x01),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xC8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FMULP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xC9),       Syntax::NOTHING)),

		new Instruction ("FIMUL",
			new UnarySyntax         (100, Opcode (0xDA, 0x01),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x01),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FNOP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xD0),       Syntax::NOTHING)),

		new Instruction ("FATAN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF3),       Syntax::NOTHING)),

		new Instruction ("FPREM",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF8),       Syntax::NOTHING)),

		new Instruction ("FPREM1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF5),       Syntax::NOTHING)),

		new Instruction ("FTAN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF2),       Syntax::NOTHING)),

		new Instruction ("FRDINT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFC),       Syntax::NOTHING)),

		new Instruction ("FRSTOR",
			new UnarySyntax         (100, Opcode (0xDD, 0x04),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FSAVE",
			new UnarySyntax         (100, Opcode (0x9B, 0xDD, 0x06), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FNSAVE",
			new UnarySyntax         (100, Opcode (0xDD, 0x06),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FSCALE",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFD),       Syntax::NOTHING)),

		new Instruction ("FSIN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFE),       Syntax::NOTHING)),

		new Instruction ("FSINCOS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFB),       Syntax::NOTHING)),

		new Instruction ("FSQRT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFA),       Syntax::NOTHING)),

		new Instruction ("FST",
			new UnarySyntax         (100, Opcode (0xD9, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x02),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xD0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		new Instruction ("FSTP",
			new UnarySyntax         (100, Opcode (0xD9, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x03),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new UnarySyntax         (120, Opcode (0xDB, 0x07),       Syntax::NOTHING,                                                          new Mem<80, Memory::FLOAT>()),
			new AdditiveUnarySyntax (130, Opcode (0xDD, 0xD8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		new Instruction ("FSTCW",
			new UnarySyntax         (100, Opcode (0x9B, 0xD9, 0x07), Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FNSTCW",
			new UnarySyntax         (100, Opcode (0xD9, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FSTENV",
			new UnarySyntax         (100, Opcode (0x9B, 0xD9, 0x06), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FNSTENV",
			new UnarySyntax         (100, Opcode (0xD9, 0x06),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FSTSW",
			new UnarySyntax         (100, Opcode (0x9B, 0xDD, 0x07), Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new ZerarySyntax        (110, Opcode (0x9B, 0xDF, 0XE0), Syntax::NOTHING)),

		new Instruction ("FNSTSW",
			new UnarySyntax         (100, Opcode (0xDD, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new ZerarySyntax        (110, Opcode (0xDF, 0XE0),       Syntax::NOTHING)),

		new Instruction ("FSUB",
			new UnarySyntax         (100, Opcode (0xD8, 0x04),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x04),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xE0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xE8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FSUBP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xE9),       Syntax::NOTHING)),

		new Instruction ("FISUB",
			new UnarySyntax         (100, Opcode (0xDA, 0x04),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x04),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FSUBR",
			new UnarySyntax         (100, Opcode (0xD8, 0x05),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x05),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xE0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FSUBRP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xE0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xE1),       Syntax::NOTHING)),

		new Instruction ("FISUBR",
			new UnarySyntax         (100, Opcode (0xDA, 0x05),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x05),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FTST",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE4),       Syntax::NOTHING)),

		new Instruction ("FUCOM",
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xE0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xDD, 0xE1),       Syntax::NOTHING)),

		new Instruction ("FUCOMP",
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xE8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xDD, 0xE9),       Syntax::NOTHING)),

		new Instruction ("FUCOMPP",
			new ZerarySyntax        (100, Opcode (0xDA, 0xE9),       Syntax::NOTHING)),

		new Instruction ("FWAIT",
			new ZerarySyntax        (100, Opcode (0x9B),             Syntax::NOTHING)),

		new Instruction ("FXAM",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE5),       Syntax::NOTHING)),

		new Instruction ("FXCH",
			new AdditiveUnarySyntax (120, Opcode (0xD9, 0xC8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD9, 0xc9),       Syntax::NOTHING)),

		new Instruction ("FXTRACT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF4),       Syntax::NOTHING)),

		new Instruction ("FYL2X",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF1),       Syntax::NOTHING)),

		new Instruction ("FYL2XP1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF9),       Syntax::NOTHING)),

		new Instruction ("HLT",
			new ZerarySyntax        (100, Opcode (0xF4),             Syntax::NOTHING)),

		new Instruction ("IDIV",
			new UnarySyntax         (100, Opcode (0xF6, 0x07),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1)),
	};

	unsigned int n = sizeof (Instructions) / sizeof (Instruction *);
	for (unsigned int i = 0; i < n; i++)
		InstructionTable.Insert (Instructions[i]);
}

Instruction::~Instruction () throw ()
{
	for (ContainerType::const_iterator i = Syntaxes.begin(); i != Syntaxes.end(); i++)
		delete *i;
}

OptimizedBinaryInstruction::OptimizedBinaryInstruction (const string &nm, const Opcode &Accum,
	const Opcode &Immed, const Opcode &Immed8, const Opcode &RegMem) throw () : Instruction (nm)
{
	// GPRegister/Memory, GPRegister/Memory 			(except Memory to Memory)
	AddSyntax (new BinarySyntax (110,   RegMem, Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,
		3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()));

	//	GPRegister/Memory, Immed
	AddSyntax (new BinarySyntax (120,   Immed,  Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,
		1, BinarySyntax::PARTIAL, new GPRegister::IdFunctor(),  new Immediate::IdFunctor()));

	//	Accum, Immed
	AddSyntax (new BinarySyntax (200,   Accum,  Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,
		1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()));

	// GPRegister/Memory, SignExtend (Byte Immed)
	AddSyntax (new BinarySyntax (10300, Immed8, Syntax::FIRST_ARGUMENT, false, Argument::GREATER,
		1, BinarySyntax::PARTIAL, new MemWordReg(),             new SignedByte::IdFunctor()));
}
