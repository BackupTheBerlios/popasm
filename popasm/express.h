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
#include <string>

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

// Thrown when an operation expected a constant term as argument, but found a variable
class ConstantExpected : public exception
{
	static const char WhatString[];

	public:
	ConstantExpected () {}
	~ConstantExpected () {}

	const char *what() const {return WhatString;}
};

// Root of exception classes hierarchy
class ExpressionException : public exception
{
	protected:
	string WhatString;

	public:
	ExpressionException (const string &s) : WhatString(s) {}
	~ExpressionException () {}

	virtual const char *what() const throw () {return WhatString.c_str();}
};

// Thrown when one attempts to divide by an empty, zero or non-constant expression
class DivisionException : public ExpressionException
{
	public:
	DivisionException () : ExpressionException ("Division by zero or non-constant expression.") {}
	~DivisionException () {}
};

// A BasicTerm is a pair whose first element is a pointer to a numeric constant and the second element is
// a pointer to the variable being multiplied by that constant. If the pointer to the variable is null,
// the term is a numeric-constant. If the numeric pointer is null, the variable is being implicitly
// multiplied by one. If both pointers are null, the term represents the numeric constant "1". The
// BasicTerm is supposed to own these variables, so it frees the memory allocated by those elements.
//
// Additionally, the numeric and variable classes must have the following members implemented as virtual public:
//
// Clone()			required by both VariableClass and NumberClass, it is a sort of polymorphic copy constructor.
//						Takes no arguments and return a pointer to a copy of the object it was applyed on.
// operator==()	Required by both VariableClass and NumberClass. Takes	a reference to the object being compared
//						and returns true if they are equal.
// Zero()			Required by NumberClass only. Takes no argument and returns true if the number is zero.
//
// A last requirement is that NumberClass must be constructible from a signed int.
//
template <class NumberClass, class VariableClass>
class BasicTerm : public pair <NumberClass *, VariableClass *>
{
	public:
	BasicTerm (NumberClass *n, VariableClass *v) throw () : pair<NumberClass *, VariableClass *> (n, v) {}
	BasicTerm (const BasicTerm &t);
	~BasicTerm () throw () {delete first; delete second;}	// deleting null pointers causes no problems

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
	string Print () const throw ()
	{
		string s("(");
		if (first != 0) s += first->Print();
		s += ", ";
		if (second != 0) s += second->Print();
		s += ")";

		return s;
	}
};

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass>::BasicTerm (const BasicTerm &t)
{
	// Clones each member
	first = (t.first == 0) ? 0 : t.first->Clone();
	second = (t.second == 0) ? 0 : t.second->Clone();
}

template <class NumberClass, class VariableClass>
bool BasicTerm<NumberClass, VariableClass>::Zero () const
{
	// If numeric pointer is zero, the implicit "1" makes the term non-zero
	if (first == 0) return false;

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
	if (t.first != 0)
		first = t.first->Clone();
	else
		first = 0;

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
	// If there is an implicit "1", convert it to a "-1"
	if (first == 0)
		first = new NumberClass (-1);
	else
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
		// If the numeric part is implicit, replace it by a true number
		if (first == 0) first = new NumberClass (1);
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

	// If the numeric part is implicit, replace it by a true number
	if (first == 0) first = new NumberClass (1);

	// Performs operation.
	*first += (t.first != 0) ? *t.first : NumberClass (1);
	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator-= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Terms can only be subtracted if they own the same variable
	if (!Compatible (t)) throw IncompatibleTerms ();

	// If the numeric part is implicit, replace it by a true number
	if (first == 0) first = new NumberClass (1);

	// Performs operation.
	*first -= (t.first != 0) ? *t.first : NumberClass (1);
	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator*= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// At least one of them MUST be a pure constant
	if (Constant() || t.Constant())
	{
		// If the first term is the pure constant, clone the variable to the first term
		if (Constant()) second = (t.second == 0) ? 0 : t.second->Clone();

		// If the numeric part of the second term is implicit 1, do nothing else
		if (t.first != 0)
		{
			if (first != 0)
				*first *= *t.first;
			else
				first = t.first->Clone();
		}
	}
	else throw IncompatibleTerms();

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator/= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// The second term MUST be a pure constant
	if (!t.Constant()) throw ConstantExpected();

	// If the second term is an implicit one, there's no need to perform the operation at all
	if (t.first != 0)
	{
		// If the numeric part is implicit, replace it by a true number
		if (first == 0) first = new NumberClass (1);

		*first /= *t.first;
	}

	return *this;
}

template <class NumberClass, class VariableClass>
BasicTerm<NumberClass, VariableClass> &BasicTerm<NumberClass, VariableClass>::operator%= (const BasicTerm <NumberClass, VariableClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		// If the numeric part is implicit, replace it by a true number
		if (first == 0) first = new NumberClass (1);

		*first %= (t.first != 0) ? *t.first : NumberClass (1);
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
		// If the numeric part is implicit, replace it by a true number
		if (first == 0) first = new NumberClass (1);

		*first &= (t.first != 0) ? *t.first : NumberClass (1);
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
		// If the numeric part is implicit, replace it by a true number
		if (first == 0) first = new NumberClass (1);

		*first |= (t.first != 0) ? *t.first : NumberClass (1);
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
		// If the numeric part is implicit, replace it by a true number
		if (first == 0) first = new NumberClass (1);

		*first ^= (t.first != 0) ? *t.first : NumberClass (1);
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
		// If the numeric part is implicit, replace it by a true number
		if (first == 0) first = new NumberClass (1);

		*first <<= (t.first != 0) ? *t.first : NumberClass (1);
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
		// If the numeric part is implicit, replace it by a true number
		if (first == 0) first = new NumberClass (1);

		*first >>= (t.first != 0) ? *t.first : NumberClass (1);
	}
	else throw ConstantExpected();

	return *this;
}

//------- BasicExpression

// Performs arithmetics on expressions
template <class NumberClass, class VariableClass, bool Redundant=true>
class BasicExpression
{
	protected:
	typedef BasicTerm <NumberClass, VariableClass> Term;

	// The value of the expression is the sum of all its terms. An empty vector means ZERO, NOT 1.
	vector <Term *> Terms;

	public:
	BasicExpression () throw () {}
	BasicExpression (NumberClass *n, VariableClass *v) throw ();
	BasicExpression (const BasicExpression &t);

	// Adds a term to the expression. Duplicates it if necessary.
	void AddTerm (const Term &t);
	// Deletes all terms and removes them from the expression
	void Clear ();
	// Checks if expression is zero
	bool Zero () const;
	// Checks if expression is a single constant number or zero
	bool Constant () const {if (Terms.empty()) return true; return (Terms.size() == 1) && Terms.back()->Constant();}
	virtual ~BasicExpression () {Clear();}

	virtual BasicExpression &operator=  (const BasicExpression &t);
	virtual BasicExpression &operator+= (const BasicExpression &t);
	virtual BasicExpression &operator-= (const BasicExpression &t);
	virtual BasicExpression &operator*= (const BasicExpression &t);
	virtual BasicExpression &operator/= (const BasicExpression &t);
	virtual BasicExpression &operator%= (const BasicExpression &t);

	virtual BasicExpression &operator- ();
	virtual BasicExpression &operator~ ();

	virtual BasicExpression &operator&= (const BasicExpression &t);
	virtual BasicExpression &operator|= (const BasicExpression &t);
	virtual BasicExpression &operator^= (const BasicExpression &t);
	virtual BasicExpression &operator<<= (const BasicExpression &t);
	virtual BasicExpression &operator>>= (const BasicExpression &t);

	// For debugging purposes only
	virtual string Print () const
	{
		if (Terms.empty()) return string ("(0, )");

		string s;

		for (vector<Term *>::const_iterator i = Terms.begin(); i != Terms.end(); i++)
		{
			s += (*i)->Print();
			s += ' ';
		}

		return s;
	}
};

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant>::BasicExpression (NumberClass *n, VariableClass *v) throw ()
{
	// Prevents the creation of expressions from zero terms
	if (!Redundant && (n != 0))
		if (n->Zero()) return;

	Terms.push_back(new Term(n, v));
}

template <class NumberClass, class VariableClass, bool Redundant=true>
void BasicExpression<NumberClass, VariableClass, Redundant>::Clear ()
{
	for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++) delete *i;
	Terms.clear();
}

template <class NumberClass, class VariableClass, bool Redundant=true>
bool BasicExpression<NumberClass, VariableClass, Redundant>::Zero () const
{
	// An expression is zero only if it is empty or all of its terms are zero
	for (vector<Term *>::const_iterator i = Terms.begin(); i != Terms.end(); i++)
	{
		if (!(*i)->Zero()) return false;
	}

	return true;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant>::BasicExpression (const BasicExpression<NumberClass, VariableClass, Redundant> &t)
{
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
	{
		Terms.push_back (new Term (**i));
	}
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator= (const BasicExpression &t)
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

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator+= (const BasicExpression &t)
{
	// There's no problem if adding to itself because the vector's size would remain constant during the operation
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
		AddTerm (**i);

	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator-= (const BasicExpression &t)
{
	// Adds each term with its sign inverted
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
	{
		// aux is necessary because unary operator- would change **i, which is constant
		Term aux (**i);
		AddTerm (-aux);
	}

	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator*= (const BasicExpression &t)
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

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator/= (const BasicExpression &t)
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

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator%= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();
	if (t.Zero()) throw DivisionException ();

	// Zero divided by anything yields zero
	if (Zero()) return *this;

	// Performs modulus operation
	*Terms.back() %= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator&= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	if (Zero()) return *this;
	if (t.Zero())
	{
		Clear();
		return *this;
	}

	// Performs operation
	*Terms.back() &= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator|= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	if (t.Zero()) return *this;
	if (Zero())
	{
		*this = t;
		return *this;
	}

	// Performs operation
	*Terms.back() |= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator^= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	if (t.Zero()) return *this;
	if (Zero())
	{
		*this = t;
		return *this;
	}

	// Performs operation
	*Terms.back() ^= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator<<= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	if (Zero() || t.Zero())	return *this;

	// Performs operation
	*Terms.back() <<= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator>>= (const BasicExpression &t)
{
	// Only constants may be used in this operation
	if ((!Constant()) || (!t.Constant())) throw ConstantExpected ();

	if (Zero() || t.Zero())	return *this;

	// Performs operation
	*Terms.back() >>= *(t.Terms.back());
	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator- ()
{
	if (Zero()) return *this;

	// Performs operation
	-*Terms.back();
	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
BasicExpression<NumberClass, VariableClass, Redundant> &BasicExpression<NumberClass, VariableClass, Redundant>::operator~ ()
{
	// Only constants may be used in this operation
	if (!Constant()) throw ConstantExpected ();

	// Zero expressions can be complemented.
	if (Zero())	Terms.push_back (new Term (new NumberClass (0), 0));

	// Performs operation
	~*Terms.back();

	return *this;
}

template <class NumberClass, class VariableClass, bool Redundant=true>
void BasicExpression<NumberClass, VariableClass, Redundant>::AddTerm (const Term &t)
{
	// Searches for one compatible term
	for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++)
	{
		if ((*i)->Compatible (t))
		{
			// Found one. Add them together.
			(**i) += t;

			// If term got zero, take it out!
			if (!Redundant && (*i)->Zero()) Terms.erase(i);
			return;
		}
	}

	// No compatible term found. Just include the new term.
	if (Redundant || !t.Zero()) Terms.push_back (new Term (t));
}

#endif
