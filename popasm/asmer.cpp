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

void Assembler::SetCurrentMode (unsigned int n)
{
	if ((n != 16) && (n != 32))
	{
		cout << "Modes must be 16 or 32 bits." << endl;
		return;
	}

	CurrentMode = n;
}

void Assembler::AssembleFile (InputFile &File) throw ()
{
	delete CurrentParser;
	CurrentParser = new Parser (File);

	do
	{
		NeedAnotherPass = false;
		PerformPass ();
	} while (NeedAnotherPass);
}

unsigned long int Assembler::GetCurrentLine () const throw ()
{
	if (CurrentParser == 0) return 0;
	return CurrentParser->GetCurrentLine();
}

void Assembler::AddContents (const vector<Byte> &v)
{
	// Creates a default segment if none exists
	if (Segments.empty())
		AddSegment(new Segment ());

	if (!Segments.back()->IsOpen())
	{
		cout << "Statement outside segment." << endl;
	}

	Segments.back()->AddContents (v);
}

void Assembler::PerformPass () throw ()
{
	vector<Byte> LineEncoding;

	CurrentPass++;
	CurrentOffset = 0;
	CurrentMode = InitialMode;
	CurrentParser->Reset();

	cout << "*** Performing pass " << CurrentPass << " ***" << endl;

	while (*CurrentParser)
	{
		try
		{
			LineEncoding = CurrentParser->ParseLine ();

			if (LineEncoding.size() != 0)
			{
				CurrentOffset += LineEncoding.size();
				AddContents (LineEncoding);
			}

			PrintVector (LineEncoding);
		}
		catch (exception &e)
		{
			cerr << "Line " << CurrentParser->GetCurrentLine() - 1 << ": Error - " << e.what() << endl;
		}
		catch (...)
		{
			cerr << "Line " << CurrentParser->GetCurrentLine() - 1 << ": Internal error. Please send a bug report." << endl;
		}
	}

	if (!Segments.empty())
	{
		if ((Segments.back()->IsOpen()) && (Segments.back()->GetName() != ""))
		{
			cout << "Segment not ended - " << Segments.back()->GetName() << endl;
		}
	}
}

void Assembler::AddSegment (Segment *seg)
{
	//! Search the table for multiple segment definitions
	Segments.push_back(seg);
}

void Assembler::CloseSegment (const string &s)
{
	if (Segments.empty ())
	{
		cout << "Segment not found " << s << endl;
		throw 0;
	}

	if (Segments.back()->GetName() != s)
	{
		cout << "Segment not found " << s << endl;
		throw 0;
	}

	Segments.back()->Close();
}

void Assembler::AddProcedure (Procedure *proc)
{
	DefineSymbol(proc);
	CurrentProcedure = proc;
}

void Assembler::CloseProcedure (const string &s)
{
	if (CurrentProcedure == 0)
	{
		cout << "No open procedure at this point." << endl;
		return;
	}

	if (CurrentProcedure->GetName() != s)
	{
		cout << "Mismatch between PROC and ENDP." << endl;
		return;
	}

	CurrentProcedure->Close();
	CurrentProcedure = 0;
}

const Procedure *Assembler::GetCurrentProcedure() const throw ()
{
	if (CurrentProcedure != 0)
	{
		if (!CurrentProcedure->IsOpen())
			return 0;
	}

	return CurrentProcedure;
}

Token *Assembler::Read (const string &str, InputFile &inp) throw ()
{
	BasicSymbol *temp = Find (str);
	if (temp != 0)
		return new Symbol (temp, false);

	return 0;
}

void Assembler::DefineSymbol (BasicSymbol *s) throw (MultidefinedSymbol)
{
	if (Segments.empty())
		AddSegment (new Segment());

	if (Segments.back()->IsOpen())
		Segments.back()->DefineSymbol(s);
	else
	{
		cout << "Symbol defined outside segment" << endl;
	}
}

BasicSymbol *Assembler::Find (const string &name)
{
	if (Segments.empty())
		return 0;

	// If segment is still open, search its symbol table
	if (Segments.back()->IsOpen())
	{
		BasicSymbol *temp = Segments.back()->Find (name);
		if (temp != 0) return temp;
	}

	// If segment is closed or the symbol does not belong its symbol table, search through the segment table
	for (vector<Segment *>::iterator i = Segments.begin(); i != Segments.end(); i++)
	{
		if ((*i)->GetName() == name)
			return *i;
	}

	return 0;
}
