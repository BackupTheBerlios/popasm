/***************************************************************************
                          lexsym.cpp  -  description
                             -------------------
    begin                : Tue Jun 4 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "asmer.h"
#include "lexsym.h"
#include "register.h"
#include "command.h"

Token *Symbol::Read (const string &str, InputFile &inp) throw ()
{
	// Tests for registers
	BasicSymbol *t = Register::Read (str, inp);
	if (t != 0) return new Symbol (t, false);

	// Tests for commands
	t = Command::Read (str, inp);
	if (t != 0) return new Symbol (t, false);

	// Tests for symbols defined in the symbol table
	BasicSymbol *bs = CurrentAssembler->Find (str);
	if (bs != 0) return new Symbol (bs, false);

	// Returns zero if not found
	return 0;
}

void Symbol::SetData (BasicSymbol *bs, bool own) throw ()
{
	if (Owner) delete s;
	s = bs;
	Owner = own;
}
