/***************************************************************************
                          lexop.h  -  description
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

#ifndef LEXOP_H
#define LEXOP_H

#include "lexical.h"
#include "operator.h"
#include "type_exp.h"

class Operator : public Token
{
	// Table of all valid operators
	static const OperatorData<Expression> **OperatorTable;

	protected:
	const OperatorData<Expression> *Data;

	public:
	Operator (const OperatorData<Expression> *d) throw () : Data(d) {}
	virtual ~Operator () throw () {}

	// Attempts to read an operator from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Operator *Read (const string &str, InputFile &inp) throw ();
	static void SetupOperatorTable (const OperatorData<Expression> **ot) throw () {OperatorTable = ot;}

	// Unary prefix
	virtual Expression &operator() (Expression &x) const {return (*Data)(x);}
	// Binary
	virtual Expression &operator() (Expression &x, Expression &y) const {return (*Data)(x, y);}

	// Returns the precedence for the given usage
	enum Usage {PREFIX = 0, BINARY = 1};
	unsigned int Precedence (Usage u) const throw (InvalidUsage) {return Data->Precedence ((OperatorData<Expression>::Usage) u);}

	const string &GetName () const throw () {return Data->GetName();}
	string Print () const throw () {return Data->GetName();}
};

class Encloser : public Operator
{
	// Table of all valid enclosers
	static const EncloserData<Expression> **EncloserTable;
	typedef EncloserData<Expression> EncData;

	public:
	Encloser (const EncloserData<Expression> *ed) throw () : Operator(ed) {}
	~Encloser () throw () {}

	static void SetupEncloserTable (const EncloserData<Expression> **et) throw () {EncloserTable = et;}
	static Encloser *Read (const string &str, InputFile &inp) throw ();

	// Checks if (*this) and op form an open-close enclosement
	bool Matches (const Encloser &op) const throw () {return dynamic_cast<const EncData *>(Data)->Matches (*op.Data);}

	// Checks if the operator is an opening encloser
	bool Opens () const throw () {return dynamic_cast<const EncData *>(Data)->Opens();}
};

#endif
