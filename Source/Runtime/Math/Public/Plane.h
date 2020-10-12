#pragma once

namespace CK
{

struct Plane
{
public:
	Plane() = default;
	Plane(const Vector3& InNormal, float InDistance); // 정규화된 요소로 평면 생성하기
	Plane(const Vector3& InNormal, Vector3 InPlanePoint); // 정규화된 요소로 평면 생성하기
	Plane(const Vector3& InPoint1, const Vector3& InPoint2, const Vector3& InPoint3);
	Plane(const Vector4& InVector4); // 정규화안된 네 개의 요소로 평면 생성하기

	FORCEINLINE float Distance(const Vector3& InPoint) const;
	FORCEINLINE bool IsOutside(const Vector3& InPoint) const;

private:
	void Normalize();

public:
	Vector3 Normal = Vector3::UnitY;
	float D = 0.f; 
};

FORCEINLINE float Plane::Distance(const Vector3& InPoint) const
{
	return Normal.Dot(InPoint) + D;
}

FORCEINLINE bool Plane::IsOutside(const Vector3& InPoint) const
{
	return Distance(InPoint) > 0.f;
}

}
