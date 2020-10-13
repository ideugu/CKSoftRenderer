
#include "Precompiled.h"
using namespace CK;

const Vector3 Vector3::UnitX(1.f, 0.f, 0.f);
const Vector3 Vector3::UnitY(0.f, 1.f, 0.f);
const Vector3 Vector3::UnitZ(0.f, 0.f, 1.f);
const Vector3 Vector3::Zero(0.f, 0.f, 0.f);
const Vector3 Vector3::One(1.f, 1.f, 1.f);

std::string Vector3::ToString() const
{
	char result[64];
	std::snprintf(result, sizeof(result), "(%.3f, %.3f, %.3f)", X, Y, Z);
	return result;
}