
#include "Precompiled.h"
using namespace CK::DDD;

void CameraObject::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	Vector3 viewX, viewY, viewZ;

	// ���� Z�� ����
	viewZ = (InTargetPosition - _Transform.GetPosition()).Normalize();
	// �ü� ����� ���� Y���� ������ ���
	if (Math::Abs(viewZ.Y) >= (1.f - SMALL_NUMBER))
	{
		// Ư�� ��Ȳ���� ���� X ��ǥ ���� ���Ƿ� ����.
		viewX = Vector3::UnitX;
	}
	else
	{
		viewX = InUp.Cross(viewZ).Normalize();
	}

	// �� ���Ͱ� �����ϰ� ũ�Ⱑ 1�̸� ����� 1�� ����
	viewY = viewZ.Cross(viewX);

	_Transform.SetLocalAxes(viewX, viewY, viewZ);
}