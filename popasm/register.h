#ifndef REGISTER_H
#define REGISTER_H

#include <string>
#include "defs.h"

class RegisterData
{
	string Name;
	Byte Size;		// Register size in bits
	Byte Code;		// Register code

	public:
	RegisterData (const string &n, Byte s, Byte c) : Name (n), Size (s), Code (c) {}
	~RegisterData () {}

	const string &GetName () const {return Name;}
};

#endif
