/***************************************************************************
                          asmer.cpp  -  description
                             -------------------
    begin                : Tue May 28 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//---------------------------------------------------------------------------
// Encapsulates differences among relevant assemblers
//---------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <vector>
#include <iostream>

#include "asmer.h"
#include "defs.h"
#include "parser.h"

void PrintVector (const vector<Byte> &v)
{
	for (vector<Byte>::const_iterator i = v.begin(); i != v.end(); i++)
	{
		Byte b = *i;
		if (b < 16) cout << "0";
		cout << hex << (unsigned int) b << " ";
	}

	cout << endl;
}

Assembler::~Assembler () throw () {}

void Assembler::AssembleFile (InputFile &File) throw ()
{
	bool NeedAnotherPass;

	do
	{
		NeedAnotherPass = PerformPass (File);
	} while (NeedAnotherPass);
}

bool Assembler::PerformPass (InputFile &File) throw ()
{
	vector<Byte> LineEncoding;
	Parser p (File);

	CurrentPass++;
	CurrentOffset = 0;
	File.ResetFile();

	while (File)
	{
		try
		{
			LineEncoding = p.ParseLine ();
			CurrentOffset += LineEncoding.size();
			PrintVector (LineEncoding);
		}
		catch (exception &e)
		{
			cerr << "Line " << File.GetCurrentLine() - 1 << ": Error - " << e.what() << endl;
		}
		catch (...)
		{
			cerr << "Line " << File.GetCurrentLine() - 1 << ": Internal error. Please send a bug report." << endl;
		}
	}

	return false;
}
