/***************************************************************************
                          command.cpp  -  description
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

#include "command.h"
#include "instruct.h"
#include "directiv.h"

BasicSymbol *Command::Read (const string &str, InputFile &inp)
{
	BasicSymbol *s;
	string UppercaseName (str);

	// Gets a copy of the original string in uppercase
	for (string::iterator i = UppercaseName.begin(); i < UppercaseName.end(); i++)
		if ((*i <= 'z') && (*i >= 'a')) *i -= 32;

	s = Instruction::Read (UppercaseName, inp);
	if (s != 0) return s;

	s = Directive::Read (UppercaseName, inp);
	return s;
}
