#include "type.h"

Type &Type::operator+= (const Type &t)
{
	switch (t.CurrentType)
	{
		case SCALAR:
			// Adding a scalar to a number does not change its original type
			break;

		case WEAK_MEMORY:
			switch (CurrentType)
			{
				case SCALAR:
					CurrentType = WEAK_MEMORY;	// SCALAR + WEAK_MEMORY = WEAK_MEMORY
					break;

				case WEAK_MEMORY:
					CurrentType = SCALAR;		// WEAK_MEMORY + WEAK_MEMORY = SCALAR
					break;

				case STRONG_MEMORY:
					break;							// STRONG_MEMORY + Anything = STRONG_MEMORY
			}

			break;

		case STRONG_MEMORY:
			CurrentType = STRONG_MEMORY;		// Anything + STRONG_MEMORY = STRONG_MEMORY
	}

	return *this;
}

Type &Type::operator-= (const Type &t)
{
	return *this += t;
}

Type &Type::operator*= (const Type &t)
{
	return *this += t;
}

Type &Type::operator/= (const Type &t)
{
	return *this += t;
}

Type &Type::operator%= (const Type &t)
{
	return *this += t;
}

Type &Type::operator&= (const Type &t)
{
	return *this += t;
}

Type &Type::operator|= (const Type &t)
{
	return *this += t;
}

Type &Type::operator^= (const Type &t)
{
	return *this += t;
}

Type &Type::operator<<= (const Type &t)
{
	return *this += t;
}

Type &Type::operator>>= (const Type &t)
{
	return *this += t;
}

Type Type::operator~ () const
{
	return *this;
}

Type Type::operator- () const
{
	return *this;
}
