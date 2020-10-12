#pragma once

namespace CK
{

struct Vector2
{
public:
	// 생성자 
	FORCEINLINE Vector2() = default;
	FORCEINLINE explicit Vector2(int InX, int InY) : X((float)InX), Y((float)InY) { }
	FORCEINLINE explicit Vector2(float InX, float InY) : X(InX), Y(InY) { }

	// 연산자 
	FORCEINLINE float operator[](BYTE InIndex) const;
	FORCEINLINE float& operator[](BYTE InIndex);
	FORCEINLINE Vector2 operator-() const;
	FORCEINLINE Vector2 operator*(float InScalar) const;
	FORCEINLINE Vector2 operator/(float InScalar) const;
	FORCEINLINE Vector2 operator*(const Vector2& InVector) const;
	FORCEINLINE Vector2 operator+(const Vector2& InVector) const;
	FORCEINLINE Vector2 operator-(const Vector2& InVector) const;
	FORCEINLINE Vector2& operator*=(float InScale);
	FORCEINLINE Vector2& operator/=(float InScale);
	FORCEINLINE Vector2& operator+=(const Vector2& InVector);
	FORCEINLINE Vector2& operator-=(const Vector2& InVector);

	// 멤버함수 
	FORCEINLINE float Size() const;
	FORCEINLINE float SizeSquared() const;
	FORCEINLINE Vector2 Normalize() const;
	FORCEINLINE bool EqualsInTolerance(const Vector2& InVector, float InTolerance = SMALL_NUMBER) const;
	FORCEINLINE float Max() const;
	FORCEINLINE float Dot(const Vector2& InVector) const;
	FORCEINLINE float Angle() const;
	FORCEINLINE float AngleInDegree() const;

	std::string ToString() const;

	// 정적멤버변수 
	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 One;
	static const Vector2 Zero;
	static const Vector2 Infinity;
	static const Vector2 InfinityNeg;

	// 멤버변수 
	float X = 0.f;
	float Y = 0.f;
};

FORCEINLINE float Vector2::Size() const
{
	return sqrtf(SizeSquared());
}

FORCEINLINE float Vector2::SizeSquared() const
{
	return X * X + Y * Y;
}

FORCEINLINE Vector2 Vector2::Normalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if (squareSum == 0.f)
	{
		return Vector2::Zero;
	}

	float invLength = Math::InvSqrt(squareSum);
	return Vector2(X, Y) * invLength;
}

FORCEINLINE float Vector2::operator[](BYTE InIndex) const
{
	if (InIndex < 0 || InIndex > 1) InIndex = 0;
	return ((float *)this)[InIndex];
}

FORCEINLINE float &Vector2::operator[](BYTE InIndex)
{
	if (InIndex < 0 || InIndex > 1) InIndex = 0;
	return ((float *)this)[InIndex];
}

FORCEINLINE Vector2 Vector2::operator-() const
{
	return Vector2(-X, -Y);
}

FORCEINLINE Vector2 Vector2::operator*(float InScalar) const
{
	return Vector2(X * InScalar, Y * InScalar);
}

FORCEINLINE Vector2 Vector2::operator/(float InScalar) const
{
	return Vector2(X / InScalar, Y / InScalar);
}

FORCEINLINE Vector2 Vector2::operator+(const Vector2& InVector) const
{
	return Vector2(X + InVector.X, Y + InVector.Y);
}

FORCEINLINE Vector2 Vector2::operator-(const Vector2& InVector) const
{
	return Vector2(X - InVector.X, Y - InVector.Y);
}

FORCEINLINE Vector2 Vector2::operator*(const Vector2& InVector) const
{
	return Vector2(X * InVector.X, Y * InVector.Y);
}

FORCEINLINE Vector2& Vector2::operator*=(float InScale)
{
	X *= InScale;
	Y *= InScale;
	return *this;
}

FORCEINLINE Vector2& Vector2::operator/=(float InScale)
{
	X /= InScale;
	Y /= InScale;
	return *this;
}

FORCEINLINE Vector2& Vector2::operator+=(const Vector2& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	return *this;
}

FORCEINLINE Vector2& Vector2::operator-=(const Vector2& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	return *this;
}

FORCEINLINE bool Vector2::EqualsInTolerance(const Vector2& InVector, float InTolerance) const
{
	return (Math::Abs(this->X - InVector.X) <= InTolerance) &&
		(Math::Abs(this->Y - InVector.Y) < InTolerance);
}

FORCEINLINE float Vector2::Max() const
{
	return Math::Max(X, Y);
}

FORCEINLINE float Vector2::Dot(const Vector2& InVector) const
{
	return X * InVector.X + Y * InVector.Y;
}

FORCEINLINE float Vector2::Angle() const
{
	return atan2f(Y,X);
}

FORCEINLINE float Vector2::AngleInDegree() const
{
	return Math::Rad2Deg(atan2f(Y, X));
}

}