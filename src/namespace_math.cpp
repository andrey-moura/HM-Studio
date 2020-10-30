#include "namespace_math.hpp"

bool MathUtil::IsInsideBlock(int index, int min, int count)
{
	return index >= min && index < (min + count);
}