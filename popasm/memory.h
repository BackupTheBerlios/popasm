/***************************************************************************
                          memory.h  -  description
                             -------------------
    begin                : Sat Jun 15 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sf.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Handles the way memory references are encoded
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MEMORY_H
#define MEMORY_H

#include <exception>
#include <utility>
#include <string>
#include <vector>
#include <typeinfo>

#include "argument.h"
#include "register.h"
#include "type.h"
#include "defs.h"

class MemoryException : public exception
{
	string WhatString;

	public:
	MemoryException (const string &s) : WhatString (s) {}
	~MemoryException () {}

	const char *what() const throw () {return WhatString.c_str();}
};

class MisusedDistanceQualifier : public MemoryException
{
	public:
	MisusedDistanceQualifier () throw () : MemoryException ("Cannot use distance qualifiers here.") {}
	~MisusedDistanceQualifier () throw () {}
};

class InvalidScale : public MemoryException
{
	public:
	InvalidScale (int) : MemoryException ("Invalid scaling factor.") {}
	~InvalidScale () {}
};

class InvalidIndex : public MemoryException
{
	public:
	InvalidIndex (const Register *r) : MemoryException (string ("Invalid index register: ") + r->Print()) {}
	~InvalidIndex () {}
};

class SegmentPrefixExpected : public MemoryException
{
	public:
	SegmentPrefixExpected (const Symbol *s)
		: MemoryException (string ("Expected segment prefix, got ") + s->Print()) {}
	~SegmentPrefixExpected () {}
};

class GPRegisterExpected : public MemoryException
{
	public:
	GPRegisterExpected (const Symbol *s)
		: MemoryException (string ("Expected 16 or 32 bit register, got ") + s->Print()) {}
	~GPRegisterExpected () {}
};

class Scaling16Bits : public MemoryException
{
	public:
	Scaling16Bits () : MemoryException ("Scaling not allowed in 16-bit mode") {}
	~Scaling16Bits () {}
};

class MixedAddressSize : public MemoryException
{
	public:
	MixedAddressSize () : MemoryException ("Mixing 16-bit and 32-bit terms not allowed.") {}
	~MixedAddressSize () {}
};

class MultipleBase : public MemoryException
{
	public:
	MultipleBase (const BaseRegister *b1, const BaseRegister *b2)
		: MemoryException (string ("At most one base register may be used in 16-bit mode; found two: ") + b1->Print() + " and " + b2->Print()) {}
	~MultipleBase () {}
};

class MultipleIndex : public MemoryException
{
	public:
	MultipleIndex (const IndexRegister *i1, const IndexRegister *i2)
		: MemoryException (string ("At most one index register may be used in 16-bit mode; found two: ") + i1->Print() + " and " + i2->Print()) {}
	~MultipleIndex () {}
};

class ImproperRegister : public MemoryException
{
	public:
	ImproperRegister (const Register *reg)
		: MemoryException (reg->Print() + " cannot be used inside brackets.") {}
	~ImproperRegister () {}
};

class TooManyRegisters : public MemoryException
{
	public:
	TooManyRegisters ()
		: MemoryException ("At most one base and one index register may be used in 32-bit memory references.") {}
	~TooManyRegisters () {}
};

class InvalidDisplacement : public MemoryException
{
	public:
	InvalidDisplacement ()
		: MemoryException ("Memory displacements must be integer constants not larger than the current address size.") {}
	~InvalidDisplacement () {}
};

class NegativeAddress : public MemoryException
{
	public:
	NegativeAddress () : MemoryException ("Negative memory addresses are not allowed.") {}
	~NegativeAddress () {}
};

class InvalidAddressSize : public MemoryException
{
	public:
	InvalidAddressSize ()
		: MemoryException ("Direct memory references displacements must be either 16 or 32 bits wide.") {}
	~InvalidAddressSize () {}
};

class DisplacementOverflow : public MemoryException
{
	public:
	DisplacementOverflow () : MemoryException ("Memory displacement too large for current mode.") {}
	~DisplacementOverflow () {}
};

class ShortDisplacement : public MemoryException
{
	public:
	ShortDisplacement () : MemoryException ("Cannot use a shorter displacement here.") {}
	~ShortDisplacement () {}
};

class UndefinedMemorySize : public MemoryException
{
	public:
	UndefinedMemorySize () : MemoryException ("Memory reference size undefined.") {}
	~UndefinedMemorySize () {}
};

class Memory : public BasicArgument
{
	Byte SegmentPrefix;			// Segment overide prefix (zero if none)
	Byte SIB;						// SIB (Scaled Index Base)
	Byte Code;						// mod___r/m byte
	Number Displacement;			// Displacement (including any size casts)
	unsigned int AddressSize;	// Address size: 16 or 32 bits, or zero if undecided

	bool SIBUsed () const throw () {return (AddressSize == 32) && ((Code & 7) == 4);}

	void MakeMemory16Bits (const BaseRegister *Base16, const IndexRegister *Index16);
	void MakeMemory32Bits (const GPRegister *Base32, const GPRegister *Index32, long int scale);

	public:
	Memory (const Type &t = Type ()) throw () : BasicArgument (t), SegmentPrefix(0), SIB(0), Code(0), AddressSize(0) {}
	~Memory () throw () {}

	Byte GetSegmentPrefix () const throw () {return SegmentPrefix;}
	void SetSegmentPrefix (Byte s) throw () {SegmentPrefix = s;}
	unsigned int GetAddressSize () const throw () {return AddressSize;}
	void SetAddressSize (unsigned int as) throw () {AddressSize = as;}
	bool UseAddressSizePrefix () const throw ();

	Byte &GetCode () throw () {return Code;}
	const Byte &GetCode () const throw () {return Code;}
	Number &GetDisplacement() throw () {return Displacement;}

	void WriteSIB (vector<Byte> &Output) const throw () {if (SIBUsed()) Output.push_back (SIB);}
	void WriteDisplacement (vector<Byte> &Output) const throw () {Displacement.Write (Output);}

	static Argument *MakeArgument (const Expression &e) throw (InvalidArgument, exception);

	template <int size, int dist, int type>
	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (Argument &arg)
		{
			const Memory *mem = dynamic_cast<const Memory *> (arg.GetData());

			if (mem == 0)
			{
				if (arg.IsUndefined())
				{
					arg.SetData (new Memory (Type (GetFirstSize(size), Type::WEAK_MEMORY, GetFirst(dist), GetFirst(type))));
					return true;
				}

				return false;
			}

			if (MatchSize (size, mem->GetSize()))
			{
				if (!Match (type, mem->GetNumericalType()))
				{
					cout << "Expected " << PrintNumerical(type & -2) << " got " << PrintNumerical(mem->GetNumericalType()) << " variable" << endl;
					return false;
//					throw 0;
				}

				return Match (dist, mem->GetDistanceType());
			}
			else
				if (mem->GetSize() == 0) throw UndefinedMemorySize();

			return false;
		}
	};

	class FarMemory : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (Argument &arg)
		{
			const Memory *mem = dynamic_cast<const Memory *> (arg.GetData());

			if (mem == 0)
			{
				if (arg.IsUndefined())
				{
					arg.SetData (new Memory (Type (0, Type::WEAK_MEMORY, FAR, UNDEFINED)));
					return true;
				}

				return false;
			}

			if (!Match (UNDEFINED | INTEGER, mem->GetNumericalType()))
			{
				cout << "Must be integer variable";
				return false;
			}

			if (mem->GetDistanceType() == NEAR) return false;
			return (mem->GetSize() == 0) || (mem->GetSize() == 32) || (mem->GetSize() == 48);
		}
	};

	class DirectMemory : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (Argument &arg)
		{
			const Memory *mem = dynamic_cast<const Memory *> (arg.GetData());

			if (mem == 0)
			{
				if (arg.IsUndefined())
				{
					arg.SetData (new Memory());
					return true;
				}

				return false;
			}

			if (mem->GetDistanceType() != UNDEFINED) throw MisusedDistanceQualifier();

			switch (mem->GetAddressSize())
			{
				case 16:
					return mem->GetCode() == 6;

				case 32:
					return mem->GetCode() == 5;
			}

			return false;
		}
	};

	string Print () const throw ();
};

#endif
