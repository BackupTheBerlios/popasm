/***************************************************************************
                          register.h  -  description
                             -------------------
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Gathers data necessary to model registers
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef REGISTER_H
#define REGISTER_H

#include <string>
#include "defs.h"

class RegisterData
{
	string Name;
	Byte Size;		// Register size in bits
	Byte Code;		// Register code

	public:
	RegisterData (const string &n, Byte s, Byte c) throw () : Name (n), Size (s), Code (c) {}
	~RegisterData () throw () {}

	const string &GetName () const throw () {return Name;}
};

#endif
