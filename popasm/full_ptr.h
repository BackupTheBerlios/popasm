/***************************************************************************
                          full_ptr.h  -  description
                             -------------------
    begin                : Sun Aug 4 2002
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

#ifndef FULL_PTR_H
#define FULL_PTR_H

#include <string>
#include <exception>

#include "argument.h"
#include "defs.h"

class InvalidFullPointer : public exception
{
	static const char WhatString[];

	public:
	InvalidFullPointer () throw () {}
	~InvalidFullPointer () throw () {}

	const char *what() const throw() {return WhatString;}
};

class SegmentOverflow : public exception
{
	static const char WhatString[];

	public:
	SegmentOverflow () throw () {}
	~SegmentOverflow () throw () {}

	const char *what() const throw() {return WhatString;}
};

class OffsetOverflow : public exception
{
	static const char WhatString[];

	public:
	OffsetOverflow () throw () {}
	~OffsetOverflow () throw () {}

	const char *what() const throw() {return WhatString;}
};

class FullPointer : public BasicArgument
{
	Word Segment;
	Dword Offset;

	public:
	FullPointer (unsigned int sz, Word seg, Dword off) throw (InvalidFullPointer, OffsetOverflow);
	~FullPointer () throw () {}

	void Write (vector<Byte> &Output) const throw ();
	string Print () const throw () {return ::Print (Segment) + ':' + ::Print (Offset);}

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const FullPointer *> (arg) != 0;}
	};
};

#endif
