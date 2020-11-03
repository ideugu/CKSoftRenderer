
#include "Precompiled.h"
using namespace CK::DDD;

void CameraObject::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	Vector3 localX, localY, localZ;

	// ���� Z�� ����
	localZ = (InTargetPosition - _Transform.GetPosition()).Normalize();
	// �ü� ����� ���� Y���� ������ ���
	if (Math::Abs(localZ.Y) >= (1.f - SMALL_NUMBER))
	{
		// Ư�� ��Ȳ���� ���� X ��ǥ ���� ���Ƿ� ����.
		localX = Vector3::UnitX;
	}
	else
	{
		localX = InUp.Cross(localZ).Normalize();
	}

	// �� ���Ͱ� �����ϰ� ũ�Ⱑ 1�̸� ����� 1�� ����
	localY = localZ.Cross(localX);

	_Transform.SetLocalAxes(localX, localY, localZ);
}