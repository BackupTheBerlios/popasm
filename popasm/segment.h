/***************************************************************************
                          segment.h  -  description
                             -------------------
    begin                : Wed Feb 5 2003
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

#ifndef SEGMENT_H
#define SEGMENT_H

#include <string>

#include "symbol.h"
#include "defs.h"

class Segment : public BasicSymbol
{
	bool Open;
	vector<Byte> Contents;

	public:
	Segment (const string &n = "") : BasicSymbol (n), Open(true) {}
	~Segment () {}

	void Close () {if (!Open) throw 0; Open = false;}
	bool IsOpen () const throw () {return Open;}
	void AddContents (const vector<Byte> &v) throw () {Contents.insert(Contents.end(), v.begin(), v.end());}
};

#endif