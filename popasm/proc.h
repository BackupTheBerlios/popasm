/***************************************************************************
                          proc.h  -  description
                             -------------------
    begin                : Sun Feb 9 2003
    copyright            : (C) 2003 by Helcio Mello
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

#ifndef PROC_H
#define PROC_H

#include <string>
#include "variable.h"

class Procedure : public UserDefined
{
	bool Open;

	public:
	Procedure (const string &n, int dist = NEAR) throw ()
		: UserDefined(n, Type (0, SCALAR, dist)), Open(true) {}
	~Procedure () throw () {}

	void Close () {if (!Open) throw 0; Open = false;}
	bool IsOpen () const throw () {return Open;}
};

#endif
