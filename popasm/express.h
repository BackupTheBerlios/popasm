/***************************************************************************
                          express.h  -  description
                             -------------------
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Performs arithmetics on first degree polynomials
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EXPRESS_H
#define EXPRESS_H

#include <vector>
#include <exception>

// Thrown when terms with different variables are summed or subtracted. Multiplication between two variables
// also cause this exception (e.g. AX * BX)
class IncompatibleTerms : public exception
{
	static const char WhatString[];

	public:
	IncompatibleTerms () {}
	~IncompatibleTerms () {}

	const char *what() const {return WhatString;}
};

// Thrown if one attempts to build a BasicTerm whose numeric pointer is null
class InvalidTerm : public exception
{
	static const char WhatString[];

	public:
	InvalidTerm () {}
	~InvalidTerm () {}

	const char *what() const {return WhatString;}
};

// Thrown when an operation expected a constant term as argument, but found a variable
class ConstantExpected : public exception
{
	static const char WhatString[];

	public:
	ConstantExpected () {}
	~ConstantExpected () {}

	const char *what() const {return WhatString;}
};

class ExpressionException : public exception
{
	public:
	ExpressionException () {}
	virtual ~ExpressionException () {}

	virtual const char *what() const = 0;
};

// Thrown when one attempts to divide by an empty, zero or non-constant expression
class DivisionException : public ExpressionException
{
	static const char WhatString[];

	public:
	DivisionException () {}
	~DivisionException () {}

	const char *what() const {return WhatString;}
};

// A BasicTerm is a pair whose first element is a pointer to a numeric constant and the second element is
// a pointer to the variable being multiplied by that constant. Only the pointer that points to the variable
// may be null (0). If it is, the term is a numeric-constant. The BasicTerm is supposed to own these variables,
// so it frees the memory allocated by those elements.
//
// Additionally, the numeric and variable classes must have the following members implemented as virtual public:
//
// Clone()			required by both VariableClass and NumberClass, it is a sort of polymorphic copy constructor.
//						Takes no arguments and return a pointer to a copy of the object it was applyed on.
// operator==()	Required by both VariableClass and NumberClass. Takes	a reference to the object being compared
//						and returns true if they are equal.
// Zero()			Required by NumberClass only. Takes no argument and returns true if the number is zero.
//
template <class NumberClass, class VariableClass>
class BasicTerm : pair <NumberClass *, VariableClass *>
{
	public:
	BasicTerm (NumberClass *n, VariableClass *v = 0) throw (InvalidTerm);
	BasicTerm (const BasicTerm &t);
	~BasicTerm () throw () {delete first; delete second;}

	bool Zero () const;
	bool Compatible (const BasicTerm &t) const;
	bool Constant () const throw () {return second == 0;}

	BasicTerm &operator= (const BasicTerm &t);

	// Note that this unary operator overwrites its original term
	BasicTerm &operator- ();
	// Note that this unary operator overwrites its original term
	BasicTerm &operator~ ();

	BasicTerm &operator+= (const BasicTerm &t);
	BasicTerm &operator-= (const BasicTerm &t);
	BasicTerm &operator*= (const BasicTerm &t);
	BasicTerm &operator/= (const BasicTerm &t);
	BasicTerm &operator%= (const BasicTerm &t);

	BasicTerm &operator&= (const BasicTerm &t);
	BasicTerm &operator|= (const BasicTerm &t);
	BasicTerm &operator^= (const BasicTerm &t);
	BasicTerm &operator<<= (const BasicTerm &t);
	BasicTerm &operator>>= (const BasicTerm &t);

	// For debugging purposes only
	void Print () const {cout << "("; if (first != 0) first->Print(); cout << ", "; if (second != 0) second->Print(); cout << ")";}
};

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass>::BasicTerm (NumberClass *n, VariableClass *v = 0) throw (InvalidTerm)
	: pair<NumberClass *, VariableClass *> (n, v)
{
	// Numeric part of the term must not be null (but its value may be zero, as in
	// BasicTerm<int, char> (new int(0), 0);
	if (n == 0) throw InvalidTerm ();
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass>::BasicTerm (const BasicTerm &t)
{
	// Clones each member
	first = t.first->Clone();
	second = (t.second == 0) ? 0 : t.second->Clone();
}

template <class NumberClass, class VariableClass>
bool BasicTerm<NumberClass, VariableClass>::Zero () const
{
	// Term is zero only if its numerical part is zero too
	return first->Zero();
}

// Terms are compatible if either they are both constants or their variables are the same
template <class NumberClass, class VariableClass>
bool BasicTerm<NumberClass, VariableClass>::Compatible (const BasicTerm &t) const
{
	// First term is constant. Compatible if second is constant too.
	if (Constant()) return (t.Constant());

	// First term is NOT constant. If the second one is, they are not compatible.
	if (t.Constant()) return false;

	// Compare their variables
	return (*second)==(*t.second);
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Prevents self-assignment
	if (this == &t) return *this;

	// If pointers are equal, do not destroy the original one
	if (first != t.first) delete first;
	first = t.first->Clone();

	// If pointers are equal, do not destroy the original one
	if (second != t.second) delete second;
	if (t.second != 0)
		second = t.second->Clone();
	else
		second = 0;

	return *this;
}

// Note that this unary operator overwrites its original term
template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator- ()
{
	*first = -*first;
	return *this;
}

// Note that this unary operator overwrites its original term
template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator~ ()
{
	// Only constant terms can be complemented
	if (Constant())
	{
		*first = ~*first;
	}
	else
		throw ConstantExpected();

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator+= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Terms can only be added if they own the same variable
	if (!Compatible (t)) throw IncompatibleTerms ();

	// Performs operation. If numeric part becomes zero, convert the result to a constant term
	*first += *t.first;
	if (first->Zero())
	{
		delete second;
		second = 0;
	}

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator-= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Terms can only be subtracted if they own the same variable
	if (!Compatible (t)) throw IncompatibleTerms ();

	// Performs operation. If numeric part becomes zero, convert the result to a constant term
	*first -= *t.first;
	if (first->Zero())
	{
		delete second;
		second = 0;
	}

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator*= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// At least one of them MUST be a pure constant
	if (Constant() || t.Constant())
	{
		// Performs operation. If numeric part becomes zero, convert the result to a constant term
		*first *= *t.first;
		if (first->Zero())
		{
			delete second;
			second = 0;
		}
		else
			// If the first term is the pure constant, clone the variable to the first term
			if (Constant()) second = (t.second == 0) ? 0 : t.second->Clone();
	}
	else throw IncompatibleTerms();

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator/= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// The second term MUST be a pure constant
	if (!t.Constant()) throw ConstantExpected();
	*first /= *t.first;
	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator%= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*first %= *t.first;
	}
	else throw ConstantExpected();

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator&= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*first &= *t.first;
	}
	else throw ConstantExpected();

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator|= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*first |= *t.first;
	}
	else throw ConstantExpected();

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator^= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*first ^= *t.first;
	}
	else throw ConstantExpected();

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator<<= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*first <<= *t.first;
	}
	else throw ConstantExpected();

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator>>= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*first >>= *t.first;
	}
	else throw ConstantExpected();

	return *this;
}

//------- BasicExpression

// Performs arithmetics on expressions
template <class NumberClass, class VariableClass>
class BasicExpression
{
	protected:
	typedef BasicTerm <NumberClass, VariableClass> Term;

	// The value of the expression is the sum of all its terms
	vector <Term *> Terms;

	BasicExpression () throw () {}

	public:
	BasicExpression (NumberClass *n, VariableClass *v = 0) throw () {Terms.push_back(new Term(n, v));}
	BasicExpression (const BasicExpression &t);

	// Adds a term to the expression. Duplicates it if necessary.
	void AddTerm (const Term &t);
	// Deletes all terms and removes them from the expression
	void Clear ();
	// Checks if expression is zero
	bool Zero () const {if (Terms.size() == 1) return Terms.back()->Zero(); else return false;}
	// Checks if expression is a single constant number
	bool Constant () const {if (Terms.size() == 1) return Terms.back()->Constant(); else return false;}
	~BasicExpression () {Clear();}

	BasicExpression &operator=  (const BasicExpression &t);
	BasicExpression &operator+= (const BasicExpression &t);
	BasicExpression &operator-= (const BasicExpression &t);
	BasicExpression &operator*= (const BasicExpression &t);
	BasicExpression &operator/= (const BasicExpression &t);
	BasicExpression &operator%= (const BasicExpression &t);

	BasicExpression &operator- ();
	BasicExpression &operator~ ();

	BasicExpression &operator&= (const BasicExpression &t);
	BasicExpression &operator|= (const BasicExpression &t);
	BasicExpression &operator^= (const BasicExpression &t);
	BasicExpression &operator<<= (const BasicExpression &t);
	BasicExpression &operator>>= (const BasicExpression &t);

	// For debugging purposes only
	void Print () const
	{
		for (vector<Term *>::const_iterator i = Terms.begin(); i != Terms.end(); i++)
		{
			(*i)->Print();
			cout << endl;
		}
	}
};

template <class NumberClass, class VariableClass>
void BasicExpression<NumberClass, VariableClass>::Clear ()
{
	for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++) delete *i;
	Terms.clear();
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass>::BasicExpression (const BasicExpression<NumberClass, VariableClass> &t)
{
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
	{
		Terms.push_back (new Term (**i));
	}
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator= (const BasicExpression &t)
{
	// Prevents self assignment
	if (this == &t) return *this;

	Clear();
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
	{
		Terms.push_back (new Term (**i));
	}

	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator+= (const BasicExpression &t)
{
	// There's no problem if adding to itself because the vector's size would remain constant during the operation
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
		AddTerm (**i);

	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator-= (const BasicExpression &t)
{
	if (this == &t)
	{
		// Subtraction between empty expressions will lead to an empty expression as well
		if (Terms.empty()) return *this;

		// Self-subtracting will lead to zero
		Clear();
		Terms.push_back (new Term (new NumberClass (), 0));
	}
	else
	{	// Adds each term with its sign inverted
		for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
		{
			// aux is necessary because unary operator- would change **i, which is constant
			Term aux (**i);
			AddTerm (-aux);
		}
	}

	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator*= (const BasicExpression &t)
{
	BasicExpression <NumberClass, VariableClass> Result;

	for (vector<Term *>::const_iterator i = Terms.begin(); i != Terms.end(); i++)
	{
		for (vector<Term *>::const_iterator j = t.Terms.begin(); j != t.Terms.end(); j++)
		{
			// Multiplies each term of each expression and put them in Result.
			Term NewTerm (**i);
			NewTerm *= **j;
			Result.AddTerm (NewTerm);
		}
	}

	*this = Result;
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator/= (const BasicExpression &t)
{
	// Prevents division by zero or non-constant
	if (!t.Constant()) throw ConstantExpected ();
	if (t.Zero()) throw DivisionException ();

	// The divisor must have only one term in this version of the module
	Term *x = t.Terms.back();

	// Divides each term from the dividend by the one of the divisor
	for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++) **i /= *x;
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator%= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();
	if (t.Zero()) throw DivisionException ();

	// Performs modulus operation
	*Terms.back() %= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator&= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	// Performs operation
	*Terms.back() &= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator|= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	// Performs operation
	*Terms.back() |= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator^= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	// Performs operation
	*Terms.back() ^= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator<<= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	// Performs operation
	*Terms.back() <<= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator>>= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	// Performs operation
	*Terms.back() >>= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator- ()
{
	// Performs operation
	-*Terms.back();
	return *this;
}

template <class NumberClass, class VariableClass>
BasicExpression<NumberClass, VariableClass> &BasicExpression<NumberClass, VariableClass>::operator~ ()
{
	// Only constants may be used in this operation
	if (!Constant()) throw ConstantExpected ();

	// Performs operation
	~*Terms.back();
	return *this;
}

template <class NumberClass, class VariableClass>
void BasicExpression<NumberClass, VariableClass>::AddTerm (const Term &t)
{
	// Searches for one compatible term
	for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++)
	{
		if ((*i)->Compatible (t))
		{
			// Found one. Add them together.
			(**i) += t;
			return;
		}
	}

	// No compatible term found. Just include the new term.
	Terms.push_back (new Term (t));
}

#endif
