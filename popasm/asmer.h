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

#include "lexical.h"
#include "operator.h"

class Assembler
{
	protected:
	virtual void SetupOperatorTable () throw () {};

	public:
	Assembler () throw () {}
	virtual ~Assembler () throw () {}
};

class PopAsm : public Assembler
{
	protected:
	void SetupOperatorTable () throw ();
	void SetupEncloserTable () throw ();

	static const OperatorData<Expression> *OperatorTable[];
	static const EncloserData<Expression> *EncloserTable[];

	public:
	PopAsm () throw ();
	~PopAsm () throw ();
};

#endif
