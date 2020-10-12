
#include "Precompiled.h"
using namespace CK;

Frustum::Frustum(const std::array<Plane, 6>& InPlanes)
{
	std::copy(InPlanes.begin(), InPlanes.begin() + 6, Planes.begin());
}




