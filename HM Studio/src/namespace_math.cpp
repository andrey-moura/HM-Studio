#include "namespace_math.hpp"

bool MathUtil::IsInsideBlock(int offset, int min, int size)
{
	return !((offset - min) <= (min - (min + size)));
}