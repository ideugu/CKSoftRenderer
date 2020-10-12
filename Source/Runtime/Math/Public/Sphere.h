#pragma once

namespace CK
{

struct Sphere
{
public:
	FORCEINLINE Sphere() = default;
	FORCEINLINE Sphere(const Circle& InCircle) : Center(InCircle.Center), Radius(InCircle.Radius) {};
	Sphere(const std::vector<Vector3>& InVertices);

	FORCEINLINE bool IsInside(const Vector3& InVector) const;
	FORCEINLINE bool Intersect(const Sphere& InCircle) const;

public:
	Vector3 Center = Vector3::Zero;
	float Radius = 0.f;
};

FORCEINLINE bool Sphere::IsInside(const Vector3& InVector) const
{
	return ((Center - InVector).SizeSquared() <= (Radius * Radius));
}

FORCEINLINE bool Sphere::Intersect(const Sphere& InCircle) const
{
	float radiusSum = Radius + InCircle.Radius;
	return (Center - InCircle.Center).SizeSquared() < (radiusSum * radiusSum);
}

}