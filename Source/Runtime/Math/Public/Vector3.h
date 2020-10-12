#pragma once

namespace CK
{

struct Vector3
{
public:
	// 생성자 
	FORCEINLINE Vector3() = default;
	FORCEINLINE explicit Vector3(const Vector2& InV, bool IsPoint = true) : X(InV.X), Y(InV.Y) { Z = IsPoint ? 1.f : 0.f; }
	FORCEINLINE explicit Vector3(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) { }

	// 연산자 
	FORCEINLINE float operator[](BYTE InIndex) const;
	FORCEINLINE float& operator[](BYTE InIndex);
	FORCEINLINE Vector3 operator-() const;
	FORCEINLINE Vector3 operator*(float InScale) const;
	FORCEINLINE Vector3 operator/(float InScale) const;
	FORCEINLINE Vector3 operator*(const Vector3& InVector) const;
	FORCEINLINE Vector3 operator+(const Vector3& InVector) const;
	FORCEINLINE Vector3 operator-(const Vector3& InVector) const;
	FORCEINLINE Vector3& operator*=(float InScale);
	FORCEINLINE Vector3& operator/=(float InScale);
	FORCEINLINE Vector3& operator+=(const Vector3& InVector);
	FORCEINLINE Vector3& operator-=(const Vector3& InVector);

	// 멤버함수 
	FORCEINLINE Vector2 ToVector2() const;
	FORCEINLINE float Size() const;
	FORCEINLINE float SizeSquared() const;
	FORCEINLINE Vector3 Normalize() const;
	FORCEINLINE bool EqualsInTolerance(const Vector3& InVector, float InTolerance = SMALL_NUMBER) const;
	FORCEINLINE float Max() const;
	FORCEINLINE float Dot(const Vector3& InVector) const;
	FORCEINLINE Vector3 Cross(const Vector3& InVector) const;

	// 정적멤버변수 
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
	static const Vector3 One;
	static const Vector3 Zero;
	static const Vector3 Infinity;
	static const Vector3 InfinityNeg;

	std::string ToString() const;

	// 멤버변수 
	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;
};

FORCEINLINE Vector2 Vector3::ToVector2() const
{
	return Vector2(X, Y);
}

FORCEINLINE float Vector3::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE float Vector3::SizeSquared() const
{
	return X * X + Y * Y + Z * Z;
}

FORCEINLINE Vector3 Vector3::Normalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if(squareSum == 0.f)
	{
		return Vector3::Zero;
	}

	float invLength = Math::InvSqrt(squareSum);
	return Vector3(X * invLength, Y * invLength, Z * invLength);
}

FORCEINLINE float Vector3::operator[](BYTE InIndex) const
{
	if (InIndex < 0 || InIndex > 2) InIndex = 0;
	return ((float *)this)[InIndex];
}

FORCEINLINE float &Vector3::operator[](BYTE InIndex)
{
	if (InIndex < 0 || InIndex > 2) InIndex = 0;
	return ((float *)this)[InIndex];
}

FORCEINLINE Vector3 Vector3::operator-() const
{
	return Vector3(-X, -Y, -Z);
}

FORCEINLINE Vector3 Vector3::operator*(float InScale) const
{
	return Vector3(X * InScale, Y * InScale, Z * InScale);
}

FORCEINLINE Vector3 Vector3::operator/(float InScale) const
{
	return Vector3(X / InScale, Y / InScale, Z / InScale);
}

FORCEINLINE Vector3 Vector3::operator*(const Vector3& InVector) const
{
	return Vector3(X * InVector.X, Y * InVector.Y, Z * InVector.Z);
}

FORCEINLINE Vector3 Vector3::operator+(const Vector3& InVector) const
{
	return Vector3(X + InVector.X, Y + InVector.Y, Z + InVector.Z);
}

FORCEINLINE Vector3 Vector3::operator-(const Vector3& InVector) const
{
	return Vector3(X - InVector.X, Y - InVector.Y, Z - InVector.Z);
}

FORCEINLINE Vector3& Vector3::operator*=(float InScale)
{
	X *= InScale;
	Y *= InScale;
	Z *= InScale;
	return *this;
}

FORCEINLINE Vector3& Vector3::operator/=(float InScale)
{
	X /= InScale;
	Y /= InScale;
	Z /= InScale;
	return *this;
}

FORCEINLINE Vector3& Vector3::operator+=(const Vector3& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	Z += InVector.Z;
	return *this;
}

FORCEINLINE Vector3& Vector3::operator-=(const Vector3& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	Z -= InVector.Z;
	return *this;
}

FORCEINLINE bool Vector3::EqualsInTolerance(const Vector3& InVector, float InTolerance) const
{
	return (Math::Abs(this->X - InVector.X) <= InTolerance) &&
		(Math::Abs(this->Y - InVector.Y) < InTolerance) &&
		(Math::Abs(this->Z - InVector.Z) < InTolerance);
}

FORCEINLINE float Vector3::Max() const
{
	float max = Math::Max(X, Y);
	return Math::Max(max, Z);
}

FORCEINLINE float Vector3::Dot(const Vector3& InVector) const
{
	return X * InVector.X + Y * InVector.Y + Z * InVector.Z;
}

FORCEINLINE Vector3 Vector3::Cross(const Vector3& InVector) const
{
	return Vector3(
		Y * InVector.Z - Z * InVector.Y,
		Z * InVector.X - X * InVector.Z,
		X * InVector.Y - Y * InVector.X);
}

}