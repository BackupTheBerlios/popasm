/***************************************************************************
                          parser.h  -  description
                             -------------------
    begin                : Sun May 26 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//---------------------------------------------------------------------------
// Checks syntax and evaluates expressions respecting precedence
//---------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "lexical.h"

class Parser
{
	public:
	static Expression *EvaluateExpression (vector<Token *> &v);

	Parser () {}
	~Parser () {}
};

#endif
