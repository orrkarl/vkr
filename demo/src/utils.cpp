#include "utils.h"

std::ostream& operator<<(std::ostream& os, const Triangle& self)
{
	return os << "Triangle [" << self.points[0] << ", " << self.points[1] << ", " << self.points[2] << "]";
}
