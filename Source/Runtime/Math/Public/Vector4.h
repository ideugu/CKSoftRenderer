#pragma once

namespace CK
{

struct Vector4
{
public:
	// 생성자 
	FORCEINLINE Vector4() = default;
	FORCEINLINE explicit Vector4(const Vector2& InV, bool IsPoint = true) : X(InV.X), Y(InV.Y), Z(0.f) { W = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit Vector4(const Vector3& InV, bool IsPoint = true) : X(InV.X), Y(InV.Y), Z(InV.Z) { W = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit Vector4(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) { }
	FORCEINLINE explicit Vector4(float InX, float InY, float InZ, bool IsPoint = true) : X(InX), Y(InY), Z(InZ) { W = IsPoint ? 1.f : 0.f; }

	// 연산자 
	FORCEINLINE float operator[](BYTE InIndex) const;
	FORCEINLINE float& operator[](BYTE InIndex);
	FORCEINLINE Vector4 operator-() const;
	FORCEINLINE Vector4 operator*(float InScale) const;
	FORCEINLINE Vector4 operator/(float InScale) const;
	FORCEINLINE Vector4 operator*(const Vector4& InVector) const;
	FORCEINLINE Vector4 operator+(const Vector4& InVector) const;
	FORCEINLINE Vector4 operator-(const Vector4& InVector) const;
	FORCEINLINE Vector4& operator*=(float InScale);
	FORCEINLINE Vector4& operator/=(float InScale);
	FORCEINLINE Vector4& operator+=(const Vector4& InVector);
	FORCEINLINE Vector4& operator-=(const Vector4& InVector);

	// 멤버함수 
	FORCEINLINE Vector2 ToVector2() const;
	FORCEINLINE Vector3 ToVector3() const;
	FORCEINLINE float Size() const;
	FORCEINLINE float SizeSquared() const;
	FORCEINLINE Vector4 Normalize() const;
	FORCEINLINE bool EqualsInTolerance(const Vector4& InVector, float InTolerance = SMALL_NUMBER) const;
	FORCEINLINE float Max() const;
	FORCEINLINE float Dot(const Vector4& InVector) const;

	// 정적멤버변수 
	static const Vector4 UnitX;
	static const Vector4 UnitY;
	static const Vector4 UnitZ;
	static const Vector4 UnitW;
	static const Vector4 One;
	static const Vector4 Zero;
	static const Vector4 Infinity;
	static const Vector4 InfinityNeg;

	std::string ToString() const;

	// 멤버변수 
	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;
	float W = 0.f;
};

FORCEINLINE Vector2 Vector4::ToVector2() const
{
	return Vector2(X, Y);
}

FORCEINLINE Vector3 Vector4::ToVector3() const
{
	return Vector3(X, Y, Z);
}

FORCEINLINE float Vector4::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE float Vector4::SizeSquared() const
{
	return X * X + Y * Y + Z * Z + W * W;
}

FORCEINLINE Vector4 Vector4::Normalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if (squareSum == 0.f)
	{
		return Vector4::Zero;
	}

	float invLength = Math::InvSqrt(squareSum);
	return Vector4(X * invLength, Y * invLength, Z * invLength, W * invLength);
}

FORCEINLINE float Vector4::operator[](BYTE InIndex) const
{
	if (InIndex < 0 || InIndex > 3) InIndex = 0;
	return ((float *)this)[InIndex];
}

FORCEINLINE float& Vector4::operator[](BYTE InIndex)
{
	if (InIndex < 0 || InIndex > 3) InIndex = 0;
	return ((float *)this)[InIndex];
}

FORCEINLINE Vector4 Vector4::operator-() const
{
	return Vector4(-X, -Y, -Z, -W);
}

FORCEINLINE Vector4 Vector4::operator*(float InScale) const
{
	return Vector4(X * InScale, Y * InScale, Z * InScale, W * InScale);
}

FORCEINLINE Vector4 Vector4::operator/(float InScale) const
{
	return Vector4(X / InScale, Y / InScale, Z / InScale, W / InScale);
}

FORCEINLINE Vector4 Vector4::operator*(const Vector4& InVector) const
{
	return Vector4(X * InVector.X, Y * InVector.Y, Z * InVector.Z, W * InVector.W);
}

FORCEINLINE Vector4 Vector4::operator+(const Vector4& InVector) const
{
	return Vector4(X + InVector.X, Y + InVector.Y, Z + InVector.Z, W + InVector.W);
}

FORCEINLINE Vector4 Vector4::operator-(const Vector4& InVector) const
{
	return Vector4(X - InVector.X, Y - InVector.Y, Z - InVector.Z, W - InVector.W);
}

FORCEINLINE Vector4& Vector4::operator*=(float InScale)
{
	X *= InScale;
	Y *= InScale;
	Z *= InScale;
	W *= InScale;
	return *this;
}

FORCEINLINE Vector4& Vector4::operator/=(float InScale)
{
	X /= InScale;
	Y /= InScale;
	Z /= InScale;
	W /= InScale;
	return *this;
}

FORCEINLINE Vector4& Vector4::operator+=(const Vector4& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	Z += InVector.Z;
	W += InVector.W;
	return *this;
}

FORCEINLINE Vector4& Vector4::operator-=(const Vector4& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	Z -= InVector.Z;
	W -= InVector.W;
	return *this;
}

FORCEINLINE bool Vector4::EqualsInTolerance(const Vector4& InVector, float InTolerance) const
{
	return (Math::Abs(this->X - InVector.X) <= InTolerance) &&
		(Math::Abs(this->Y - InVector.Y) < InTolerance) &&
		(Math::Abs(this->Z - InVector.Z) < InTolerance) &&
		(Math::Abs(this->W - InVector.W) < InTolerance);
}

FORCEINLINE float Vector4::Max() const
{
	float max = Math::Max(X, Y);
	max = Math::Max(max, Z);
	return Math::Max(max, W);
}


FORCEINLINE float Vector4::Dot(const Vector4& InVector) const
{
	return X * InVector.X + Y * InVector.Y + Z * InVector.Z + W * InVector.W;
}

}