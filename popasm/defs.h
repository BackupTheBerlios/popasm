//--------------------------------------------------------------------------
//                          defs.h  -  description
//                             -------------------
//    begin                : Mon Apr 23 2001
//    copyright            : (C) 2001 by Helcio Mello
//    email                : helcio@users.sourceforge.net
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// This file contains definitions and functions used throughout the project
//---------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DEFS_H
#define DEFS_H

#include <string>
#include <vector>
#include <algorithm>

// typedefs used here to save some typing and increase portability
typedef unsigned char Byte;
typedef unsigned short int Word;
typedef unsigned long int Dword;

Dword ZeroExtend (Byte b) throw ();
Dword ZeroExtend (Word w) throw ();

Dword SignExtend (Byte b) throw ();
Dword SignExtend (Word w) throw ();

// SHR and SAR stand for SHift Right and Shift Arithmetic Right, respectivelly
Dword SHR (Dword d, int n) throw ();
Dword SAR (Dword d, int n) throw ();

union BasicDataTypes
{
	private:
	Byte b[4];
	Dword d;

	public:
	void Write (vector<Byte> &v, Dword dd, unsigned int n) throw ();
};

string Print (unsigned long int i) throw ();
void UpperCase (string &s) throw ();

#include <iostream>

template <class T>
class ReferenceCount
{
	T *Data;
	mutable int count;

	// Prevents copy
	ReferenceCount (const ReferenceCount &) {}

	public:
	ReferenceCount (T *d) {Data = d; count = 1;}
	~ReferenceCount ()
	{
		#ifdef DEBUG
		if (count != 0)
			throw 0;
		#endif

		delete Data;
	}

	T &GetData () throw () {return *Data;}
	const T &GetConstData () const throw () {return *Data;}
	friend void kill<T> (const ReferenceCount<T> *&ref);
	ReferenceCount *Clone () throw () {count++; return this;}
	bool operator== (const ReferenceCount &ref) const throw ();
};

template <class T>
bool ReferenceCount<T>::operator== (const ReferenceCount<T> &ref) const throw ()
{
	if (Data != ref.Data)
		return false;

	if (Data != 0)
		return *Data == *ref.Data;

	return true;
}

template <class T>
void kill (const ReferenceCount<T> *&ref)
{
	if (ref == 0)
		return;

	if (ref->count != 0)
		ref->count--;
	#ifdef DEBUG
	else
		throw 0;
	#endif

	if (ref->count == 0)
	{
		delete ref;
		ref = 0;
	}
}

class Assembler;
extern Assembler *CurrentAssembler;

#endif
