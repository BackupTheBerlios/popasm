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

#include "asmer.h"

const OperatorData<Expression> *PopAsm::OperatorTable[] =
{
	new PlusEqual<Expression> ("+", 600, 400),
	new MinusEqual<Expression> ("-", 600, 400),
	new TimesEqual<Expression> ("*", 0, 500),
	new DividesEqual<Expression> ("/", 0, 500),
	new ModEqual<Expression> ("MOD", 0, 500),
	new Period<Expression> (".", 0, 1000),
	new And<Expression> ("AND", 0, 200),
	new Or<Expression> ("OR", 0, 100),
	new Xor<Expression> ("XOR", 0, 100),
	new Not<Expression> ("NOT", 600, 0),
	new ShiftLeft<Expression> ("SHL", 0, 500),
	new ShiftRight<Expression> ("SHR", 0, 500),
	0	// End of list
};

const EncloserData<Expression> *PopAsm::EncloserTable[] =
{
	new OpenBracket<Expression> ("[", "]"),
	new CloseBracket<Expression> ("]"),
	new OpenParenthesis<Expression> ("(", ")"),
	new CloseParenthesis<Expression> (")"),
	0	// End of list
};

PopAsm::PopAsm () throw ()
{
	SetupOperatorTable ();
	SetupEncloserTable ();
}

PopAsm::~PopAsm () throw ()
{
	for (unsigned int i = 0; OperatorTable[i] != 0; i++)
		delete OperatorTable[i];

	for (unsigned int i = 0; EncloserTable[i] != 0; i++)
		delete EncloserTable[i];
}

void PopAsm::SetupOperatorTable () throw ()
{
	Operator::SetupOperatorTable (OperatorTable);
}

void PopAsm::SetupEncloserTable () throw ()
{
	Encloser::SetupEncloserTable (EncloserTable);
}
