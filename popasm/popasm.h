/***************************************************************************
                          popasm.h  -  description
                             -------------------
    begin                : Sun Jul 7 2002
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

#ifndef POPASM_H
#define POPASM_H

#include "asmer.h"
#include "lexical.h"
#include "express.h"
#include "lexop.h"

class PopAsm : public Assembler
{
	protected:
	void SetupOperatorTable () throw ();
	void SetupEncloserTable () throw ();

	static const OperatorData<Expression> *OperatorTable[];
	static const EncloserData<Expression> *EncloserTable[];

	Type::TypeName WeakMemoryTranslation;

	public:
	PopAsm (unsigned int InitialMode) throw ();
	~PopAsm () throw ();

	Type::TypeName TranslateWeakMemory() throw () {return WeakMemoryTranslation;}
};

#endif
