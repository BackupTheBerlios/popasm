/***************************************************************************
                          asmer.h  -  description
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

#ifndef ASMER_H
#define ASMER_H

#include <string>

#include "symbol.h"
#include "proc.h"
#include "inp_file.h"
#include "type.h"
#include "segment.h"

class Parser;
class Token;

class Assembler
{
	// Operating mode the assembler is reset to at the beginning of each pass
	unsigned int InitialMode;

	unsigned int CurrentMode;
	unsigned int CurrentPass;
	unsigned long int CurrentOffset;
	Parser *CurrentParser;

	vector<Segment *> Segments;
	Procedure *CurrentProcedure;
	bool NeedAnotherPass;

	protected:
	void PerformPass () throw ();
	void AddContents (const vector<Byte> &v);

	public:
	Assembler (unsigned int im) throw ()
		: InitialMode (im), CurrentPass(0), CurrentParser(0), CurrentProcedure(0) {}
	virtual ~Assembler () throw () = 0;

	unsigned int GetCurrentMode () const throw () {return CurrentMode;}
	void SetCurrentMode (unsigned int n);
	unsigned long int GetCurrentOffset () const throw () {return CurrentOffset;}
	unsigned long int GetCurrentLine () const throw ();
	unsigned long int GetCurrentPass () const throw () {return CurrentPass;}
	const Procedure *GetCurrentProcedure() const throw ();

	void AddSegment (Segment *seg);
	void CloseSegment (const string &s);
	void AddProcedure (Procedure *proc);
	void CloseProcedure (const string &s);

	void RequestNewPass () throw () {NeedAnotherPass = true;}

	// Attempts to read a symbol from the given string.
	// Returns 0 if there's currently no symbol with that name defined in any segment
	Token *Read (const string &str, InputFile &inp) throw ();
	void DefineSymbol (BasicSymbol *s) throw (MultidefinedSymbol);
	BasicSymbol *Find (const string &name);

	void AssembleFile (InputFile &File) throw ();
	virtual Type::TypeName TranslateWeakMemory() throw () = 0;
};

#endif
