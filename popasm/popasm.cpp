/***************************************************************************
                          popasm.cpp  -  description
                             -------------------
    begin                : Tue May 28 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "popasm.h"

#include "asmer.h"
#include "instruct.h"
#include "directiv.h"
#include "type.h"

const OperatorData<Expression> *PopAsm::OperatorTable[] =
{
	new PlusEqual<Expression> ("+", 600, 400),
	new MinusEqual<Expression> ("-", 600, 400),
	new TimesEqual<Expression> ("*", 0, 500),
	new DividesEqual<Expression> ("/", 0, 500),
	new ModEqual<Expression> ("MOD", 0, 500),
	new ModEqual<Expression> ("%", 0, 500),
	new Colon<Expression> (":", 0, 900),
	new Period<Expression> (".", 0, 1000),
	new OperatorData<Expression> (",", 0, 0),
	new And<Expression> ("AND", 0, 200),
	new And<Expression> ("&", 0, 200),
	new Or<Expression> ("OR", 0, 100),
	new Or<Expression> ("|", 0, 100),
	new Xor<Expression> ("XOR", 0, 100),
	new Xor<Expression> ("^", 0, 100),
	new Not<Expression> ("NOT", 600, 0),
	new Not<Expression> ("~", 600, 0),
	new ShiftLeft<Expression> ("SHL", 0, 500),
	new ShiftLeft<Expression> ("<<", 0, 500),
	new ShiftRight<Expression> ("SHR", 0, 500),
	new ShiftRight<Expression> (">>", 0, 500),
	new SizeCast<Expression> ("BYTE", 50, 0, 8),
	new SizeCast<Expression> ("WORD", 50, 0, 16),
	new SizeCast<Expression> ("DWORD", 50, 0, 32),
	new SizeCast<Expression> ("PWORD", 50, 0, 48),
	new SizeCast<Expression> ("FWORD", 50, 0, 48),
	new SizeCast<Expression> ("QWORD", 50, 0, 64),
	new SizeCast<Expression> ("TBYTE", 50, 0, 80),
	new SizeCast<Expression> ("TWORD", 50, 0, 80),
	new SizeCast<Expression> ("OWORD", 50, 0, 128),
	new DistanceCast<Expression> ("SHORT", 50, 0, SHORT),
	new DistanceCast<Expression> ("NEAR", 50, 0, NEAR),
	new DistanceCast<Expression> ("FAR", 50, 0, FAR),
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

PopAsm::PopAsm (unsigned int InitialMode) throw () : Assembler (InitialMode)
{
	SetupOperatorTable ();
	SetupEncloserTable ();
	Instruction::SetupInstructionTable ();
	Directive::SetupDirectiveTable ();
}

PopAsm::~PopAsm () throw ()
{
	for (unsigned int i = 0; OperatorTable[i] != 0; i++)
		delete OperatorTable[i];

	for (unsigned int i = 0; EncloserTable[i] != 0; i++)
		delete EncloserTable[i];

	// Still pending: destroy instruction and directive tables.
}

void PopAsm::SetupOperatorTable () throw ()
{
	Operator::SetupOperatorTable (OperatorTable);
}

void PopAsm::SetupEncloserTable () throw ()
{
	Encloser::SetupEncloserTable (EncloserTable);
}
