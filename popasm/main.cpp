/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue May 28 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//---------------------------------------------------------------------------
// Main source file
//---------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "inp_file.h"
#include "asmer.h"
#include "popasm.h"

#include "number.h"
#include "defs.h"
#include <vector>
#include <cstdlib>

Assembler *CurrentAssembler;

int main (int argc, char **argv)
{
	if (argc == 1)
	{
		cerr << "Error: expected at least one source file as argument." << endl;
		return 1;
	}

	CurrentAssembler = new PopAsm(16);

	for (int i = 1; i < argc; i++)
	{
		InputFile f (argv[i]);

		if (!f)
		{
			cerr << "Unable to open file " << argv[i] << "." << endl;
			return 2;
		}

		CurrentAssembler->AssembleFile (f);
	}

	delete CurrentAssembler;
	return 0;
}
