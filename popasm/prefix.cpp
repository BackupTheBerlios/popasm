/***************************************************************************
                          prefix.cpp  -  description
                             -------------------
    begin                : Mon Jan 27 2003
    copyright            : (C) 2003 by Helcio Mello
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

#include "prefix.h"
#include "instruct.h"

InstructionExpected::InstructionExpected (const Token *tt) throw () : t(tt)
{
	WhatString = "Instruction expected after prefix. Got ";
	WhatString += t->Print();
}


HashTable<PrefixInstruction *, HashFunctor, PointerComparator<PrefixInstruction> > PrefixInstruction::PrefixTable;

void PrefixInstruction::SetupPrefixTable ()
{
	static PrefixInstruction PrefixInstructions[] =
	{
		PrefixInstruction ("LOCK",  Opcode (0xF0)),
		PrefixInstruction ("REP",   Opcode (0xF3)),
		PrefixInstruction ("REPE",  Opcode (0xF3)),
		PrefixInstruction ("REPNE", Opcode (0xF2)),
		PrefixInstruction ("REPNZ", Opcode (0xF2)),
		PrefixInstruction ("REPZ",  Opcode (0xF3))
	};

	unsigned int n = sizeof (PrefixInstructions) / sizeof (PrefixInstruction);
	for (unsigned int i = 0; i < n; i++)
		PrefixTable.Insert (&(PrefixInstructions[i]));
}

void PrefixInstruction::Assemble (const BasicSymbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j,
		vector<Byte> &Output) const
{
	vector<Byte> temp;

	// Checks if the prefix has been used alone with no instruction following it.
	if (i != j)
	{
		// Checks if the next token is really an instruction
		const Symbol *s = dynamic_cast<const Symbol *> (*i);
		if (s == 0)
			throw InstructionExpected (*i);

		const Instruction *inst = dynamic_cast<const Instruction *> (s->GetData());
		if (inst == 0)
			throw InstructionExpected (*i);

		// Attempts to assemble the instruction to a temporary vector
		inst->Assemble (sym, i + 1, j, temp);
	}

	Encoding.Write (Output);
	Output.insert (Output.end(), temp.begin(), temp.end());
}

BasicSymbol *PrefixInstruction::Read (const string &str, InputFile &inp)
{
	PrefixInstruction s (str, Opcode (0));
	PrefixInstruction * const *i = PrefixTable.Find (&s);

	return (i == 0) ? 0 : *i;
}
