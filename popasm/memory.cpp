/***************************************************************************
                          memory.cpp  -  description
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

#include "memory.h"
#include "lexsym.h"

void Memory::MakeMemory16Bits (const BaseRegister *Base16, const IndexRegister *Index16)
{
	if ((Base16 != 0) && (Index16 != 0))
	{
		// Handles [base16 + index16 + displacement]
		Code += Base16->BaseIndexCode + Index16->IndexCode;
	}
	else
	{
		if (Base16 != 0)
		{
			// Handles [base16]
			Code += Base16->BaseCode;
		}
		else
		{
			// Handles [index16]. Note, however, that index16 is working like a base in this case
			Code += Index16->BaseCode;
		}
	}

	// Expands [bp] to [bp + byte 0]
	if (Code == 6)
	{
		Code = 0x46;
		Displacement.SetSize (8);
	}
}

void Memory::MakeMemory32Bits (const GPRegister *Base32, const GPRegister *Index32, long int scale)
{
	if ((Base32 != 0) && (Index32 == 0))
	{
		// [Base + displacement]
		Byte rm = Base32->GetCode ();
		Code |= rm;

		switch (rm)
		{
			case 4:	// [ESP+displacement]
				// 0x24 = 00_100_100_B = [ESP + 0]
				SIB = 0x24;
				break;

			case 5:	//	[EBP+displacement]
				// Expands [ebp} to [ebp + byte 0]
				if ((Code & 0xC0) == 0)
				{
					Code = 0x45;
					Displacement.SetSize (8, Number::SIGNED);
				}
				break;

			default:
				break;
		}
	}
	else
	{
		if (Base32 != 0)
		{
			// [Base + scale * Index + displacement]

			// Signals the presence of the SIB
			Code |= 4;

			// Encodes the scaling factor into the SIB
			switch (scale)
			{
				case 1: break;
				case 2: SIB |= 0x40; break;
				case 4: SIB |= 0x80; break;
				case 8: SIB |= 0xC0; break;
				default: throw InvalidScale (scale);
			}

			// Checks if ESP is being used as an index register
			if (Index32->GetCode() == 4)
			{
				if (scale != 1) throw InvalidIndex (Index32);
				swap<const GPRegister *> (Base32, Index32);
			}

			// Checks if EBP is being used as base with zero displacement
			if ((Base32->GetCode() == 5) && (Code == 4))
/*
			{
				// If scaling factor is 1, base and index may be exchanged.
				// NOTE THIS WILL (ALMOST) ALWAYS CHANGE THE DEFAULT SEGMENT PREFIX
				if (scale == 1)
				{
					swap<const GPRegister *> (Base32, Index32);
				}
				else
*/
				{
					// If not, the only solution is to encode the zero displacement using a byte
					Code |= 0x40;
					Displacement.SetSize (8, Number::SIGNED);
				}
/*			}*/

			// Fills Base and Index fields of the SIB
			SIB |= (Index32->GetCode() << 3) + Base32->GetCode();
		}
		else
		{
			// [scale * Index + displacement]
			// Checks if ESP is being used as an index register
			if (Index32->GetCode() == 4)
			{
				if (scale != 1) throw InvalidIndex (Index32);
				MakeMemory32Bits (Index32, Base32, 1);
			}
			else
			{
				switch (scale)
				{
					case 1:
						MakeMemory32Bits (Index32, 0, 0);
						return;

					case 4:
						SIB = 0x80;
						break;

					case 8:
						SIB = 0xC0;
						break;

					case 2:
					case 3:
					case 5:
					case 9:
						// Splits [IndexRegister * n] into [IndexRegister + IndexRegister * (n - 1)]
						MakeMemory32Bits (Index32, Index32, scale-1);
						return;
				}

				// Encodes the displacement in 32-bit form and builds the SIB
				Code = 4;
				Displacement.SetSize (32, Number::RAW);
				SIB |= (Index32->GetCode() << 3) + 5;
			}
		}
	}
}

// Returns the minimum size of the given number as a memory displacement of a non-direct memory acess
unsigned int GetMinimumSize (Number &n, bool set)
{
	long int i = n.GetInteger(false);

	if (i == 0)
	{
		if (set) n.SetSize (0, Number::SIGNED);
		return 0;
	}

	if ((i >= -128) && (i <= 127))
	{
		if (set) n.SetSize (8, Number::SIGNED);
		return 8;
	}

	if ((i >= -65535) && (i <= 65535))
	{
		if (set) n.SetSize (16, Number::RAW);
		return 16;
	}

	if (set) n.SetSize (32, Number::RAW);
	return 32;
}

Argument *Argument::MakeMemory (const Expression &e)
{
	Memory *mem = new Memory (e.GetSize(), e.GetDistanceType());

	// Checks for segment override prefixes
	if (e.GetSegmentPrefix() != 0)
	{
		Symbol *s = static_cast<Symbol *> (*e.GetSegmentPrefix());
		const SegmentRegister *segreg = dynamic_cast <const SegmentRegister *> (s->GetData());
		if (segreg == 0) throw SegmentPrefixExpected (s);
		mem->SetSegmentPrefix(segreg->GetPrefixCode());
	}

	const GPRegister *Base32 = 0;
	const GPRegister *Index32 = 0;
	Number *Scale = 0;

	const BaseRegister *Base16 = 0, *NewBase = 0;
	const IndexRegister *Index16 = 0, *NewIndex = 0;

	for (unsigned int i = 0; i < e.QuantityOfTerms(); i++)
	{
		const pair<Number *, Symbol *> *p = e.TermAt(i);

		if (p->second != 0)
		{
			// Gets the register. Must be a general-purpose one
			const GPRegister *reg = dynamic_cast<const GPRegister *> (p->second->GetData());
			if (reg == 0) throw GPRegisterExpected (p->second);

			switch (reg->GetSize())
			{
				case 16:
					// Checks if the user mixed 16-bit and 32-bit terms or multiplied the register by a number
					if (mem->GetAddressSize() == 32) throw MixedAddressSize();
					if (p->first != 0) throw Scaling16Bits();

					// Checks for a base register
					NewBase = dynamic_cast<const BaseRegister *> (reg);
					if (NewBase != 0)
					{
						// Checks for multiple bases. Eg. [bx + bp]
						if (Base16 != 0) throw MultipleBase(Base16, NewBase);

						Base16 = NewBase;
						mem->SetAddressSize(16);
						break;
					}

					NewIndex = dynamic_cast<const IndexRegister *> (reg);
					// Checks for an index register
					if (NewIndex != 0)
					{
						// Checks for multiple indices. Eg. [si + di]
						if (Index16 != 0) throw MultipleIndex (Index16, NewIndex);

						Index16 = NewIndex;
						mem->SetAddressSize(16);
						break;
					}

					// Improper register inside brackets. Eg. [cx]
					throw ImproperRegister (reg);

				case 32:
					// Checks if the user mixed 16-bit and 32-bit terms
					if (mem->GetAddressSize() == 16) throw MixedAddressSize();

					// Checks for Base register
					if (p->first == 0)
					{
						if (Base32 == 0)
						{
							Base32 = reg;
							mem->SetAddressSize(32);
							break;
						}

						// If Base slot is already occupied, use it as an index multiplied by 1
						if (Index32 == 0)
						{
							Index32 = reg;
							mem->SetAddressSize(32);
							break;
						}
					}
					else
					{
						if (Index32 == 0)
						{
							Index32 = reg;
							Scale = p->first;
							mem->SetAddressSize(32);
							break;
						}
					}

					throw TooManyRegisters();

				default:
					// 8-bit registers not allowed inside brackets!
					throw ImproperRegister (reg);
			}
		}
		else
		{
			// Displacement
			if (p->first == 0) throw InvalidDisplacement();
			// Displacements must be integer numbers.
			if (!p->first->IsInteger()) throw InvalidDisplacement();

			mem->GetDisplacement() = *(p->first);
		}
	}

	// Direct memory references...
	if (mem->GetAddressSize() == 0)
	{
		// ... cannot have negative displacements
		if (mem->GetDisplacement().GetValue() < 0) throw NegativeAddress();

		switch (mem->GetDisplacement().GetSize())
		{
			case 0:
				// Direct memory operands defaults to be of the same size as the CurrentAddressSize
				mem->GetDisplacement().SetSize (CurrentAssembler->GetCurrentMode(), Number::UNSIGNED);
				mem->SetAddressSize(CurrentAssembler->GetCurrentMode());
				break;

			case 16:
				mem->SetAddressSize (16);
				break;

			case 32:
				mem->SetAddressSize (32);
				break;

			default:
				// Direct memory references must be either 16-bit or 32-bit wide
				throw InvalidAddressSize ();
		}

		// Builds the direct memory reference in binary form (mod_r/m) byte
		switch (mem->GetAddressSize())
		{
			case 16:
				mem->GetCode() = 6;
				break;

			case 32:
				mem->GetCode() = 5;
				break;
		}

		return new Argument (mem, true);
	}

	// If displacement had no forced size, get the minimum one
	if (mem->GetDisplacement().GetSize() == 0)
	{
		unsigned int i = GetMinimumSize (mem->GetDisplacement(), true);

		switch (i)
		{
			case 0:
			case 8:
				// zero or 8-bit displacements are always OK.
				break;

			case 16:
				// If the address size is already 32, adjust the displacement size
				if (mem->GetAddressSize() == 32) mem->GetDisplacement().SetSize (32, Number::RAW);
				break;

			default:
				// If the displacement requires 32 bits and the user wants a 16-bit mode address, sorry.
				if (mem->GetAddressSize() == 16) throw DisplacementOverflow();
				mem->GetDisplacement().SetSize (32, Number::RAW);
				break;
		}
	}

	// Converts the size of the displacement to the mod field of the mod_r/m byte
	switch (mem->GetDisplacement().GetSize())
	{
		case 0:
			// Zero length displacement. Leave mod field zero
			break;

		case 8:
			// 1-byte displacement. mod = 01
			mem->GetCode() |= 0x40;
			break;

		case 16:
			// It is impossible to force a 16-bit displacement in 32-bit mode
			if (mem->GetAddressSize() == 32) throw ShortDisplacement();
			mem->GetCode() |= 0x80;
			break;

		case 32:
			if (mem->GetAddressSize() == 16) throw DisplacementOverflow();
			mem->GetCode() |= 0x80;
			break;

		default:
			// Displacement is of higher size, like qword, tbyte, etc.
			throw DisplacementOverflow();
	}

	// Now, performs adjustments like calculating the SIB, r/m for direct memory acesses, etc.
	switch (mem->GetAddressSize())
	{
		case 16:
			mem->MakeMemory16Bits (Base16, Index16);
			break;

		case 32:
			mem->MakeMemory32Bits (Base32, Index32, (Scale == 0) ? 1 : Scale->GetInteger());
			break;
	}

	return new Argument (mem, true);
}
