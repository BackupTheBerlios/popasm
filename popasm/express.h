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

class ExpressionException : public exception
{
	public:
	ExpressionException () {}
	~ExpressionException () {}

	virtual const char *what() const = 0;
};

// Thrown when one attempts to divide to an empty, zero or non-constant expression
class DivisionException : public ExpressionException
{
	static const char WhatString[];

	public:
	DivisionException () {}
	~DivisionException () {}

	const char *what() const {return WhatString;}
};

// A BasicTerm is a pair whose first element is a pointer to a variable. The second element is a pointer to
// the numeric constant that multiplies it. The BasicTerm is supposed to own these variables, so it frees
// the memory allocated to those elements.
template <class VariableClass, class NumberClass>
class BasicTerm : pair <VariableClass *, NumberClass *>
{
	public:
	BasicTerm (VariableClass *v = 0, NumberClass *n = 0) : pair<VariableClass *, NumberClass *> (v, n) {}
	BasicTerm (const BasicTerm &t);
	~BasicTerm () {delete first; delete second;}

	bool Zero () const {return (*second) == NumberClass (0);}
	bool Compatible (const BasicTerm &t) const {return *first == *t.first;}
	bool Constant () const {return first == 0;}

	BasicTerm &operator= (const BasicTerm &t);
	BasicTerm &operator- ();
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

	void Print () const {cout << "("; if (first != 0) first->Print(); cout << ", "; second->Print(); cout << ")";}
};

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass>::BasicTerm (const BasicTerm &t)
{
	first = (t.first == 0) ? 0 : t.first->Clone();
	second = new NumberClass (*(t.second));
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Prevents self-assignment
	if (this == &t) return *this;

	// If pointers are equal, do not destroy the original one
	if (first != t.first) delete first;
	first = new VariableClass (*(t.first));

	if (second != t.second) delete second;
	second = new NumberClass (*(t.second));

	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator- ()
{
	*second = -*second;
	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator~ ()
{
	// Only constant terms can be negated
	if (Constant())
	{
		*second = -*second;
	}
	else
		throw 0;

	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator+= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Terms can only be added if they own the same variable
	if (!Compatible (t)) throw IncompatibleTerms ();

	*second += *t.second;
	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator-= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Terms can only be subtracted if they own the same variable
	if (!Compatible (t)) throw IncompatibleTerms ();

	*second -= *t.second;
	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator*= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// At least one of them MUST be a pure constant
	if (Constant() || t.Constant())
	{
		// If the first term is a pure constant, clone the variable to the first term
		if (Constant()) first = (t.first == 0) ? 0 : t.first->Clone();
		*second *= *t.second;
	}
	else throw IncompatibleTerms ();

	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator/= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// The second term MUST be a pure constant or have the same variable
	if (!t.Constant())
	{
		// If the same variable is being divided the result will be the quotient of their coefficients
		if (Compatible (t))
		{
			if (*t.second == NumberClass (0)) throw DivisionException();

			delete first;
			first = 0;
			*second /= *t.second;
			return *this;
		}

		throw IncompatibleTerms ();
	}

	*second /= *t.second;
	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator%= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*second %= *t.second;
	}
	else throw IncompatibleTerms ();

	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator&= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*second &= *t.second;
	}
	else throw IncompatibleTerms ();

	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator|= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*second |= *t.second;
	}
	else throw IncompatibleTerms ();

	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator^= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*second ^= *t.second;
	}
	else throw IncompatibleTerms ();

	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator<<= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*second <<= *t.second;
	}
	else throw IncompatibleTerms ();

	return *this;
}

template <class VariableClass, class NumberClass>
BasicTerm<VariableClass, NumberClass> &BasicTerm<VariableClass, NumberClass>::operator>>= (const BasicTerm <VariableClass, NumberClass> &t)
{
	// Both terms MUST be pure constants
	if (Constant() && t.Constant())
	{
		*second >>= *t.second;
	}
	else throw IncompatibleTerms ();

	return *this;
}

// Performs arithmetics on expressions
template <class VariableClass, class NumberClass>
class BasicExpression
{
	protected:
	typedef BasicTerm <VariableClass, NumberClass> Term;

	// The value of the expression is the sum of all its terms
	vector <Term *> Terms;

	public:
	BasicExpression () {}
	BasicExpression (const BasicExpression &t);

	// Adds a term to the expression. Duplicates it if necessary.
	void AddTerm (const Term &t);
	// Deletes all terms and removes them from the expression
	void Clear ();
	// Checks if expression is zero (NOT EMPTY)
	bool Zero () const {if (Terms.size() == 1) return Terms.back()->Zero(); else return false;}

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

	void Print () const
	{
		for (vector<Term *>::const_iterator i = Terms.begin(); i != Terms.end(); i++)
		{
			(*i)->Print();
			cout << endl;
		}
	}
};

template <class VariableClass, class NumberClass>
void BasicExpression<VariableClass, NumberClass>::Clear ()
{
	for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++) delete *i;
	Terms.clear();
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass>::BasicExpression (const BasicExpression<VariableClass, NumberClass> &t)
{
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
	{
		Terms.push_back (new Term (**i));
	}
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator= (const BasicExpression &t)
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

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator+= (const BasicExpression &t)
{
	// There's no problem if adding to itself because the vector's size would remain constant during the operation
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++) AddTerm (**i);
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator-= (const BasicExpression &t)
{
	// Self-subtracting will lead to zero
	if ((this == &t) && (!Terms.empty()))
	{
		Clear();
		Terms.push_back (new Term (0, new NumberClass (0)));
	}

	// Adds each term with its sign inverted
	for (vector<Term *>::const_iterator i = t.Terms.begin(); i != t.Terms.end(); i++)
	{
		Term aux (**i);
		AddTerm (-aux);
	}

	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator*= (const BasicExpression &t)
{
	BasicExpression <VariableClass, NumberClass> Result;

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

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator/= (const BasicExpression &t)
{
	// Watch out for self division
	if (this == &t)
	{
		// Empty expressions cannot divide themselves
		if (Terms.empty()) throw DivisionException ();

		// Prevents division by zero
		if (Zero()) throw DivisionException ();

		// Makes the expression be equal to 1
		Clear();
		Terms.push_back (new Term (0, new NumberClass (1)));
		return *this;
	}

	// The divisor must have only one term in this version of the module
	if (t.Terms.size() != 1) throw DivisionException ();
	Term *x = t.Terms.back();

	// Divides each term from the dividend by the one of the divisor
	for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++) **i /= *x;
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator%= (const BasicExpression &t)
{
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator&= (const BasicExpression &t)
{
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator|= (const BasicExpression &t)
{
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator^= (const BasicExpression &t)
{
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator<<= (const BasicExpression &t)
{
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator>>= (const BasicExpression &t)
{
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator- ()
{
	return *this;
}

template <class VariableClass, class NumberClass>
BasicExpression<VariableClass, NumberClass> &BasicExpression<VariableClass, NumberClass>::operator~ ()
{
	return *this;
}

template <class VariableClass, class NumberClass>
void BasicExpression<VariableClass, NumberClass>::AddTerm (const Term &t)
{
	for (vector<Term *>::iterator i = Terms.begin(); i != Terms.end(); i++)
	{
		if ((*i)->Compatible (t))
		{
			(**i) += t;
			return;
		}
	}

	Terms.push_back (new Term (t));
}

#endif
