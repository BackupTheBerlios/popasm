/***************************************************************************
                          popasm.cpp  -  description
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

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "inp_file.h"
#include "parser.h"
#include "asmer.h"

#include "instruct.h"

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

int main (int argc, char **argv)
{
	if (argc != 2) return 1;
	InputFile inp (argv[1]);

	if (!inp)
	{
		cout << "File not found" << endl;
		return 1;
	}

	Parser p (inp);
	Assembler *a = new PopAsm();
	vector <Byte> Encoding;

	while (inp)
	{
		try
		{
			Encoding = p.ParseLine (16);
			PrintVector (Encoding);
		} catch (exception &e) {cout << e.what() << endl;}
	}

	delete a;
	return 0;
}
