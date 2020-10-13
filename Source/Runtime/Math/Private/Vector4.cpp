
#include "Precompiled.h"
using namespace CK;

const Vector4 Vector4::UnitX(1.f, 0.f, 0.f, 0.f);
const Vector4 Vector4::UnitY(0.f, 1.f, 0.f, 0.f);
const Vector4 Vector4::UnitZ(0.f, 0.f, 1.f, 0.f);
const Vector4 Vector4::UnitW(0.f, 0.f, 0.f, 1.f);
const Vector4 Vector4::Zero(0.f, 0.f, 0.f, 0.f);
const Vector4 Vector4::One(1.f, 1.f, 1.f, 1.f);

std::string Vector4::ToString() const
{
	char result[64];
	std::snprintf(result, sizeof(result), "(%.3f, %.3f, %.3f, %.3f)", X, Y, Z, W);
	return result;
}