/***************************************************************************
                          lexop.cpp  -  description
                             -------------------
    begin                : Mon Jun 3 2002
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

#include "lexop.h"
#include "defs.h"

const OperatorData<Expression> **Operator::OperatorTable = 0;

Operator *Operator::Read (const string &str, InputFile &inp) throw ()
{
	string UppercaseName(str);
	UpperCase (UppercaseName);

	// Scans the list of operators
	for (unsigned int i = 0; OperatorTable[i] != 0; i++)
	{
		if (OperatorTable[i]->GetName() == UppercaseName)
		{
			// Ignores PTR token (if found)
			if ((dynamic_cast<const SizeCast<Expression> *> (OperatorTable[i]) != 0)
				|| (dynamic_cast<const DistanceCast<Expression> *> (OperatorTable[i]) != 0))
			{
				string *s = inp.GetString();
				if (s != 0)
				{
					string upper (*s);
					UpperCase (upper);
					if (upper == "PTR")
						delete s;
					else
						inp.UngetString (s);
				}
			}

			return new Operator (OperatorTable[i]);
		}
	}

	return Encloser::Read (str, inp);
}

const EncloserData<Expression> **Encloser::EncloserTable = 0;

Encloser *Encloser::Read (const string &str, InputFile &inp) throw ()
{
	string UppercaseName(str);
	UpperCase (UppercaseName);

	for (unsigned int i = 0; EncloserTable[i] != 0; i++)
		if (EncloserTable[i]->GetName() == UppercaseName)
			return new Encloser (EncloserTable[i]);

	return 0;
}
