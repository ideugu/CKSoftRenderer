#pragma once

namespace CK
{
namespace DDD
{

class TransformComponent
{
public:
	TransformComponent() = default;

public:
	void SetPosition(const Vector3 & InPosition) { _Position = InPosition; }
	void AddPosition(const Vector3 & InDeltaPosition) { _Position += InDeltaPosition; }
	void AddYawRotation(float InDegree) { _Rotation.Yaw += InDegree; _Rotation.Clamp(); CalculateLocalAxis(); }
	void AddRollRotation(float InDegree) { _Rotation.Roll += InDegree; _Rotation.Clamp(); CalculateLocalAxis(); }
	void AddPitchRotation(float InDegree) { _Rotation.Pitch += InDegree; _Rotation.Clamp(); CalculateLocalAxis(); }
	void SetRotation(const Rotator & InRotation) { _Rotation = InRotation; CalculateLocalAxis(); }
	void SetScale(const Vector3 & InScale) { _Scale = InScale; }

	Vector3 GetPosition() const { return _Position; }
	Rotator GetRotation() const { return _Rotation; }
	Vector3 GetScale() const { return _Scale; }

	FORCEINLINE Matrix4x4 GetModelingMatrix() const;
	const Vector3& GetLocalX() const { return _Right; }
	const Vector3& GetLocalY() const { return _Up; }
	const Vector3& GetLocalZ() const { return _Forward; }
	void SetLocalAxes(const Vector3 & InRight, const Vector3 & InUp, const Vector3 & InForward)
	{
		_Right = InRight;
		_Up = InUp;
		_Forward = InForward;
	}

private:
	FORCEINLINE void CalculateLocalAxis();

	Vector3 _Position = Vector3::Zero;
	Rotator _Rotation;
	Vector3 _Scale = Vector3::One;

	Vector3 _Right = Vector3::UnitX;
	Vector3 _Up = Vector3::UnitY;
	Vector3 _Forward = Vector3::UnitZ;
};

FORCEINLINE Matrix4x4 TransformComponent::GetModelingMatrix() const
{
	return Matrix4x4(
		Vector4(_Right * _Scale.X, false), 
		Vector4(_Up * _Scale.Y, false), 
		Vector4(_Forward * _Scale.Z, false),
		Vector4(_Position, true)
	);
}

FORCEINLINE void TransformComponent::CalculateLocalAxis()
{
	float cy, sy, cp, sp, cr, sr;
	Math::GetSinCos(sy, cy, _Rotation.Yaw);
	Math::GetSinCos(sp, cp, _Rotation.Pitch);
	Math::GetSinCos(sr, cr, _Rotation.Roll);

	_Right = Vector3(cy * cr + sy * sp * sr, cp * sr, -sy * cr + cy * sp * sr);
	_Up = Vector3(-cy * sr + sy * sp * cr, cp * cr, sy * sr + cy * sp * cr);
	_Forward = Vector3(sy * cp, -sp, cy * cp);
}

}
}
