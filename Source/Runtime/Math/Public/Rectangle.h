#pragma once

namespace CK
{

struct Rectangle
{
public:
	Rectangle() = default;
	Rectangle(const Rectangle& InRectangle) : Min(InRectangle.Min), Max(InRectangle.Max) { }
	Rectangle(const Vector2& InMinVector, const Vector2& InMaxVector) : Min(InMinVector), Max(InMaxVector) { }
	Rectangle(const std::vector<Vector2> InVertices);

	FORCEINLINE bool Intersect(const Rectangle& InRectangle) const;
	FORCEINLINE bool IsInside(const Rectangle& InRectangle) const;
	FORCEINLINE bool IsInside(const Vector2& InVector) const;

	FORCEINLINE Rectangle operator+=(const Vector2& InVector);
	FORCEINLINE Rectangle operator+=(const Rectangle& InRectangle);

	FORCEINLINE Vector2 GetSize() const;
	FORCEINLINE Vector2 GetExtent() const;
	FORCEINLINE void GetCenterAndExtent(Vector2& OutCenter, Vector2& OutExtent) const;

public:
	Vector2 Min;
	Vector2 Max;
};

FORCEINLINE bool Rectangle::Intersect(const Rectangle& InRectangle) const
{
	if ((Min.X > InRectangle.Max.X) || (InRectangle.Min.X > Max.X))
	{
		return false;
	}

	if ((Min.Y > InRectangle.Max.Y) || (InRectangle.Min.Y > Max.Y))
	{
		return false;
	}

	return true;
}

FORCEINLINE bool Rectangle::IsInside(const Rectangle& InRectangle) const
{
	return (IsInside(InRectangle.Min) && IsInside(InRectangle.Max));
}

FORCEINLINE bool Rectangle::IsInside(const Vector2& InVector) const
{
	return ((InVector.X >= Min.X) && (InVector.X <= Max.X) && (InVector.Y >= Min.Y) && (InVector.Y <= Max.Y));
}

FORCEINLINE Rectangle Rectangle::operator+=(const Vector2& InVector)
{
	Min.X = Math::Min(Min.X, InVector.X);
	Min.Y = Math::Min(Min.Y, InVector.Y);

	Max.X = Math::Max(Max.X, InVector.X);
	Max.Y = Math::Max(Max.Y, InVector.Y);
	return *this;
}

FORCEINLINE Rectangle Rectangle::operator+=(const Rectangle& InRectangle)
{
	Min.X = Math::Min(Min.X, InRectangle.Min.X);
	Min.Y = Math::Min(Min.Y, InRectangle.Min.Y);

	Max.X = Math::Max(Max.X, InRectangle.Max.X);
	Max.Y = Math::Max(Max.Y, InRectangle.Max.Y);
	return *this;
}

FORCEINLINE Vector2 Rectangle::GetSize() const
{
	return (Max - Min);
}

FORCEINLINE Vector2 Rectangle::GetExtent() const
{
	return GetSize() * 0.5f;
}

FORCEINLINE void Rectangle::GetCenterAndExtent(Vector2 & OutCenter, Vector2 & OutExtent) const
{
	OutExtent = GetExtent();
	OutCenter = Min + OutExtent;
}

}