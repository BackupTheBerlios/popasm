/***************************************************************************
                          command.h  -  description
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

#ifndef COMMAND_H
#define COMMAND_H

#include <string>

#include "argument.h"
#include "defs.h"
#include "parser.h"
#include "symbol.h"
#include "lexical.h"

// General assembler commands
class Command : public BasicSymbol
{
	public:
	Command (const string &n) throw () : BasicSymbol (n) {}
	~Command () throw () {}

	virtual void Assemble (const Symbol *sym, Parser &p, vector<Byte> &Encoding) const = 0;
	static BasicSymbol *Read (const string &str, InputFile &inp);
};

#endif
