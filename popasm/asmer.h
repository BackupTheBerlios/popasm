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

#include "inp_file.h"

class Parser;

class Assembler
{
	// Operating mode the assembler is reset to at the beginning of each pass
	unsigned int InitialMode;

	unsigned int CurrentMode;
	unsigned int CurrentPass;
	unsigned long int CurrentOffset;
	Parser *CurrentParser;

	bool PerformPass (InputFile &File) throw ();

	public:
	Assembler (unsigned int im) throw () : InitialMode (im), CurrentPass(0), CurrentParser(0) {}
	virtual ~Assembler () throw () = 0;

	unsigned int GetCurrentMode () const throw () {return CurrentMode;}
	void SetCurrentMode (unsigned int n);
	unsigned long int GetCurrentOffset () const throw () {return CurrentOffset;}
	unsigned long int GetCurrentLine () const throw ();
	void AssembleFile (InputFile &File) throw ();
};

#endif
