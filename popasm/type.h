#ifndef TYPE_H
#define TYPE_H

class Type
{
	public:
	enum TypeName {SCALAR, WEAK_MEMORY, STRONG_MEMORY};

	private:
	TypeName CurrentType;

	public:
	Type (TypeName t = SCALAR) : CurrentType (t) {}
	~Type () {}

	Type &operator+= (const Type &t);
	Type &operator-= (const Type &t);
	Type &operator*= (const Type &t);
	Type &operator/= (const Type &t);
	Type &operator%= (const Type &t);

	Type &operator&= (const Type &t);
	Type &operator|= (const Type &t);
	Type &operator^= (const Type &t);
	Type &operator<<= (const Type &t);
	Type &operator>>= (const Type &t);

	Type operator- () const;
	Type operator~ () const;

	bool operator== (const Type &t) const {return CurrentType == t.CurrentType;}
	bool operator!= (const Type &t) const {return CurrentType != t.CurrentType;}
};

#endif
