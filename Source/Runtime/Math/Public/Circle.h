#pragma once

namespace CK
{

struct Circle
{
public:
	FORCEINLINE Circle() = default;
	FORCEINLINE Circle(const Circle& InCircle) : Center(InCircle.Center), Radius(InCircle.Radius) {};
	Circle(const std::vector<Vector2> InVertices);

	FORCEINLINE bool IsInside(const Vector2& InVector) const;
	FORCEINLINE bool Intersect(const Circle& InCircle) const;

public:
	Vector2 Center = Vector2::Zero;
	float Radius = 0.f;
};

FORCEINLINE bool Circle::IsInside(const Vector2& InVector) const
{
	return ((Center - InVector).SizeSquared() <= (Radius * Radius));
}

FORCEINLINE bool Circle::Intersect(const Circle& InCircle) const
{
	float twoRadiusSum = Radius + InCircle.Radius;
	return (Center - InCircle.Center).SizeSquared() < (twoRadiusSum * twoRadiusSum);
}

}