#include "lexical.h"

Token::~Token ()
{
}

Token *GetToken (InputFile &inp)
{
	// Gets the next string from file. Returns zero if EOF is reached
	string *s = inp.GetString();
	if (s == 0) return 0;

	Token *NextToken;

	// Tests for numbers. If successful, s will no longer be necessary
	NextToken = Number::Read(*s, inp);
	if (NextToken != 0)
	{
		delete s;
		return NextToken;
	}

	// Tests for registers. If successful, s will no longer be necessary
	NextToken = Register::Read (*s, inp);
	if (NextToken != 0)
	{
		delete s;
		return NextToken;
	}

	// Tests for operators. If successful, s will no longer be necessary
	NextToken = Operator::Read (*s, inp);
	if (NextToken != 0)
	{
		delete s;
		return NextToken;
	}

	// None of the above...
	NextToken = new UnknownToken (*s);
	delete s;
	return NextToken;
}

Number *Number::Read (const string &str, InputFile &inp)
{
	if (str.size() == 0) return 0;
	char c = str[0];

	// if the first char is a '$', the string is a number only if the second character is a digit
	if (c == '$')
	{
		if (str.size() == 1) return 0;
		c = str[1];
	}

	// The first character must be a digit (0-9)
	if (!InputFile::Digit (c)) return 0;

	string s(str);

	// Checks for exceptional conditions
	switch (inp.GetNextChar())
	{
		// If immediatelly followed by a decimal point, concatenate the strings
		case '.':
			// Appends the decimal point to the original string.
			delete inp.GetString();
			s += '.';

			// If there's something just after the point, append it.
			if (InputFile::Space (inp.GetNextChar()))
			{
				// User has typed a number followed by a decimal point. Eg.: 123. = 123.0
				break;
			}
			else
			{
				string *t = inp.GetString();

				// Watches out for premature end-of-line
				if (*t != "\n")
				{
					// Appends fractional part of float point number
					s += *t;
					delete t;
					if ((inp.GetNextChar() != '+') && (inp.GetNextChar() != '-')) break;
				}
				else
				{
					inp.UngetString (t);
					break;
				}
			}

		case '+':
		case '-':
		{
			// Numbers like 6.02e-23 are only allowed if default base is 10.
			Word Base;
			string aux (SimplifyString (s, Base));
			if (Base != 10) break;

			// Checks if last character is an 'e'
			char c = *(aux.rbegin());
			if ((c == 'e') || (c == 'E'))
			{
				// t contains either '+' or '-'
				string *t = inp.GetString();

				// Watches out for premature end-of-line
				if ((inp.GetNextChar() != '\n') && (!InputFile::Space (inp.GetNextChar())))
				{
					string *u = inp.GetString();

					if (*u == "\n")
					{
						// User has broken line just after exponent sign. Unget what was got...
						inp.UngetString (u);
						inp.UngetString (t, (*u)[0]);
					}
					else
					{
						// Now s holds the whole numeric string
						s += *t + *u;
						delete t;
						delete u;
					}
				}
				else inp.UngetString (t);
			}

			break;
		}

		default:
			break;
	}

	Number *n = new Number (s);
	return n;
}

void Number::CheckSemantics () const
{
	// Checks if a real number is being used to acess memory.
	if ((!n.GetInteger()) && (t != Type::SCALAR)) throw 0;
}

Number &Number::operator/= (const Number &x)
{
	t /= x.t;

	// Both numbers are integers?
	if (n.GetInteger() && x.n.GetInteger())
	{
		// Integer division
		n = RealNumber (static_cast<IntegerNumber> (n) / static_cast<IntegerNumber> (x.n));
	}
	else
	{
		// Real division
		n /= x.n;
	}

	CheckSemantics();
	return *this;
}

Number &Number::operator%= (const Number &x)
{
	// Both numbers are integers?
	if (n.GetInteger() && x.n.GetInteger())
	{
		t /= x.t;
		n = RealNumber (static_cast<IntegerNumber> (n) % static_cast<IntegerNumber> (x.n));
		CheckSemantics();
	}
	else
		throw 0;

	return *this;
}

Number &Number::operator&= (const Number &x)
{
	// Both numbers are integers?
	if (n.GetInteger() && x.n.GetInteger())
	{
		t &= x.t;
		n = RealNumber (static_cast<IntegerNumber> (n) & static_cast<IntegerNumber> (x.n));
		CheckSemantics();
	}
	else
		throw 0;

	return *this;
}

Number &Number::operator|= (const Number &x)
{
	// Both numbers are integers?
	if (n.GetInteger() && x.n.GetInteger())
	{
		t |= x.t;
		n = RealNumber (static_cast<IntegerNumber> (n) | static_cast<IntegerNumber> (x.n));
		CheckSemantics();
	}
	else
		throw 0;

	return *this;
}

Number &Number::operator^= (const Number &x)
{
	// Both numbers are integers?
	if (n.GetInteger() && x.n.GetInteger())
	{
		t ^= x.t;
		n = RealNumber (static_cast<IntegerNumber> (n) ^ static_cast<IntegerNumber> (x.n));
		CheckSemantics();
	}
	else
		throw 0;

	return *this;
}

Number &Number::operator<<= (const Number &x)
{
	// Both numbers are integers?
	if (n.GetInteger() && x.n.GetInteger())
	{
		t <<= x.t;
		n = RealNumber (static_cast<IntegerNumber> (n) << static_cast<IntegerNumber> (x.n));
		CheckSemantics();
	}
	else
		throw 0;

	return *this;
}

Number &Number::operator>>= (const Number &x)
{
	// Both numbers are integers?
	if (n.GetInteger() && x.n.GetInteger())
	{
		t >>= x.t;
		n = RealNumber (static_cast<IntegerNumber> (n) >> static_cast<IntegerNumber> (x.n));
		CheckSemantics();
	}
	else
		throw 0;

	return *this;
}

Number &Number::operator~ ()
{
	// Number must be integer for One's Complement
	if (n.GetInteger())
	{
		t = ~t;
		n = RealNumber (~static_cast<IntegerNumber> (n));
		CheckSemantics();
	}
	else
		throw 0;

	return *this;
}

OperatorData<Expression> Operator::OperatorTable[] =
{
	PlusEqual<Expression> (),
	MinusEqual<Expression> (),
	TimesEqual<Expression> (),
	DividesEqual<Expression> (),
	ModEqual<Expression> (),
	Period<Expression> (),
	OpenBracket<Expression> (),
	CloseBracket<Expression> (),
	OpenParenthesis<Expression> (),
	CloseParenthesis<Expression> (),
	And<Expression> (),
	Or<Expression> (),
	Xor<Expression> (),
	Not<Expression> (),
	ShiftLeft<Expression> (),
	ShiftRight<Expression> ()
};

Operator *Operator::Read (const string &str, InputFile &inp)
{
	string UppercaseName(str);

	// Gets a copy of the original string in uppercase
	for (string::iterator i = UppercaseName.begin(); i < UppercaseName.end(); i++)
		if ((*i <= 'z') && (*i >= 'a')) *i -= 32;

	for (unsigned int i = 0; i < sizeof (OperatorTable) / sizeof (OperatorData<Expression>); i++)
		if (OperatorTable[i].GetName() == UppercaseName) return new Operator (OperatorTable + i);

	return 0;
}

Register::~Register ()
{
}

RegisterData SegmentRegister:: RegisterTable[] =
{
	RegisterData ("ES", 16, 0),
	RegisterData ("CS", 16, 1),
	RegisterData ("SS", 16, 2),
	RegisterData ("DS", 16, 3),
	RegisterData ("FS", 16, 4),
	RegisterData ("GS", 16, 5)
};

SegmentRegister *SegmentRegister::Read (const string &s, InputFile &inp)
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (RegisterData); i++)
		if (RegisterTable[i].GetName() == s) return new SegmentRegister (RegisterTable + i);

	return 0;
}

RegisterData GPRegister:: RegisterTable[] =
{
	RegisterData ("EAX", 32, 0),
	RegisterData ("ECX", 32, 1),
	RegisterData ("EDX", 32, 2),
	RegisterData ("EBX", 32, 3),
	RegisterData ("ESP", 32, 4),
	RegisterData ("EBP", 32, 5),
	RegisterData ("ESI", 32, 6),
	RegisterData ("EDI", 32, 7),

	RegisterData ("AX", 16, 0),
	RegisterData ("CX", 16, 1),
	RegisterData ("DX", 16, 2),
	RegisterData ("BX", 16, 3),
	RegisterData ("SP", 16, 4),
	RegisterData ("BP", 16, 5),
	RegisterData ("SI", 16, 6),
	RegisterData ("DI", 16, 7),

	RegisterData ("AL", 8, 0),
	RegisterData ("CL", 8, 1),
	RegisterData ("DL", 8, 2),
	RegisterData ("BL", 8, 3),
	RegisterData ("AH", 8, 4),
	RegisterData ("CH", 8, 5),
	RegisterData ("DH", 8, 6),
	RegisterData ("BH", 8, 7)
};

GPRegister *GPRegister::Read (const string &s, InputFile &inp)
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (RegisterData); i++)
		if (RegisterTable[i].GetName() == s) return new GPRegister (RegisterTable + i);

	return 0;
}

RegisterData ControlRegister:: RegisterTable[] =
{
	RegisterData ("CR0", 32, 0),
	RegisterData ("CR1", 32, 1),
	RegisterData ("CR2", 32, 2),
	RegisterData ("CR3", 32, 3),
	RegisterData ("CR4", 32, 4),
	RegisterData ("CR5", 32, 5),
	RegisterData ("CR6", 32, 6),
	RegisterData ("CR7", 32, 7),
};

ControlRegister *ControlRegister::Read (const string &s, InputFile &inp)
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (RegisterData); i++)
		if (RegisterTable[i].GetName() == s) return new ControlRegister (RegisterTable + i);

	return 0;
}

RegisterData TestRegister:: RegisterTable[] =
{
	RegisterData ("TR0", 32, 0),
	RegisterData ("TR1", 32, 1),
	RegisterData ("TR2", 32, 2),
	RegisterData ("TR3", 32, 3),
	RegisterData ("TR4", 32, 4),
	RegisterData ("TR5", 32, 5),
	RegisterData ("TR6", 32, 6),
	RegisterData ("TR7", 32, 7),
};

TestRegister *TestRegister::Read (const string &s, InputFile &inp)
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (RegisterData); i++)
		if (RegisterTable[i].GetName() == s) return new TestRegister (RegisterTable + i);

	return 0;
}

RegisterData DebugRegister:: RegisterTable[] =
{
	RegisterData ("DR0", 32, 0),
	RegisterData ("DR1", 32, 1),
	RegisterData ("DR2", 32, 2),
	RegisterData ("DR3", 32, 3),
	RegisterData ("DR4", 32, 4),
	RegisterData ("DR5", 32, 5),
	RegisterData ("DR6", 32, 6),
	RegisterData ("DR7", 32, 7),
};

DebugRegister *DebugRegister::Read (const string &s, InputFile &inp)
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (RegisterData); i++)
		if (RegisterTable[i].GetName() == s) return new DebugRegister (RegisterTable + i);

	return 0;
}

RegisterData MMXRegister:: RegisterTable[] =
{
	RegisterData ("MM",  64, 0),
	RegisterData ("MM0", 64, 0),
	RegisterData ("MM1", 64, 1),
	RegisterData ("MM2", 64, 2),
	RegisterData ("MM3", 64, 3),
	RegisterData ("MM4", 64, 4),
	RegisterData ("MM5", 64, 5),
	RegisterData ("MM6", 64, 6),
	RegisterData ("MM7", 64, 7),
};

MMXRegister *MMXRegister::Read (const string &s, InputFile &inp)
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (RegisterData); i++)
		if (RegisterTable[i].GetName() == s) return new MMXRegister (RegisterTable + i);

	return 0;
}

RegisterData XMMRegister:: RegisterTable[] =
{
	RegisterData ("XMM",  128, 0),
	RegisterData ("XMM0", 128, 0),
	RegisterData ("XMM1", 128, 1),
	RegisterData ("XMM2", 128, 2),
	RegisterData ("XMM3", 128, 3),
	RegisterData ("XMM4", 128, 4),
	RegisterData ("XMM5", 128, 5),
	RegisterData ("XMM6", 128, 6),
	RegisterData ("XMM7", 128, 7),
};

XMMRegister *XMMRegister::Read (const string &s, InputFile &inp)
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (RegisterData); i++)
		if (RegisterTable[i].GetName() == s) return new XMMRegister (RegisterTable + i);

	return 0;
}

RegisterData FPURegister:: RegisterTable[] =
{
	RegisterData ("ST0", 80, 0),
	RegisterData ("ST1", 80, 1),
	RegisterData ("ST2", 80, 2),
	RegisterData ("ST3", 80, 3),
	RegisterData ("ST4", 80, 4),
	RegisterData ("ST5", 80, 5),
	RegisterData ("ST6", 80, 6),
	RegisterData ("ST7", 80, 7),
};

FPURegister *FPURegister::Read (const string &s, InputFile &inp)
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (RegisterData); i++)
		if (RegisterTable[i].GetName() == s) return new FPURegister (RegisterTable + i);

	if (s == "ST")
	{
		string *t, *u, *v;
		char c, d, x;

		// Checks if user has said ST(n)
		t = inp.GetString();
		c = inp.GetNextChar();
		if (*t != "(") goto abort1;

		// Checks if there is a digit between parentheses
		u = inp.GetString();
		d = inp.GetNextChar();
		if (u->size() != 1) goto abort2;

		// Must be between ST(0) and ST(7)
		x = (*u)[0];
		if ((x < '0') || (x > '7')) goto abort2;

		// Checks if user has closed parentheses
		v = inp.GetString();
		if (*v != ")") goto abort3;

		// Returns STn instead of ST(n)
		delete t;
		delete u;
		delete v;
		return new FPURegister (RegisterTable + x - '0');

		// If not a valid register, unget everything and return ST
		abort3:	inp.UngetString (v);
		abort2:	inp.UngetString (u, d);
		abort1:	inp.UngetString (t, c);
					return new FPURegister (RegisterTable);
	}

	return 0;
}

Register *Register::Read (const string &str, InputFile &inp)
{
	Register *answer;
	string UppercaseName(str);

	// Gets a copy of the original string in uppercase
	for (string::iterator i = UppercaseName.begin(); i < UppercaseName.end(); i++)
		if ((*i <= 'z') && (*i >= 'a')) *i -= 32;

	answer = SegmentRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = GPRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = ControlRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = TestRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = DebugRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = MMXRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = XMMRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = FPURegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	return 0;
}

Expression &Expression::operator- ()
{
	BasicExpression <Register, Number>::operator- ();
	return *this;
}

Expression &Expression::operator~ ()
{
	BasicExpression <Register, Number>::operator~ ();
	return *this;
}

Expression &Expression::operator+= (const Expression &e)
{
	BasicExpression <Register, Number>::operator+= (e);
	return *this;
}

Expression &Expression::operator-= (const Expression &e)
{
	BasicExpression <Register, Number>::operator-= (e);
	return *this;
}

Expression &Expression::operator*= (const Expression &e)
{
	BasicExpression <Register, Number>::operator*= (e);
	return *this;
}

Expression &Expression::operator/= (const Expression &e)
{
	BasicExpression <Register, Number>::operator/= (e);
	return *this;
}

Expression &Expression::operator&= (const Expression &e)
{
	BasicExpression <Register, Number>::operator&= (e);
	return *this;
}

Expression &Expression::operator|= (const Expression &e)
{
	BasicExpression <Register, Number>::operator|= (e);
	return *this;
}

Expression &Expression::operator^= (const Expression &e)
{
	BasicExpression <Register, Number>::operator^= (e);
	return *this;
}

Expression &Expression::operator<<= (const Expression &e)
{
	BasicExpression <Register, Number>::operator<<= (e);
	return *this;
}

Expression &Expression::operator>>= (const Expression &e)
{
	BasicExpression <Register, Number>::operator>>= (e);
	return *this;
}

Expression &Expression::operator%= (const Expression &e)
{
	BasicExpression <Register, Number>::operator%= (e);
	return *this;
}

