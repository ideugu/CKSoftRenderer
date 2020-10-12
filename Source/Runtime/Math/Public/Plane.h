#pragma once

namespace CK
{

struct Plane
{
public:
	Plane() = default;
	Plane(const Vector3& InNormal, float InDistance); // ����ȭ�� ��ҷ� ��� �����ϱ�
	Plane(const Vector3& InNormal, Vector3 InPlanePoint); // ����ȭ�� ��ҷ� ��� �����ϱ�
	Plane(const Vector3& InPoint1, const Vector3& InPoint2, const Vector3& InPoint3);
	Plane(const Vector4& InVector4); // ����ȭ�ȵ� �� ���� ��ҷ� ��� �����ϱ�

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
