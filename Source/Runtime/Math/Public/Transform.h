#pragma once

namespace CK
{

struct Transform
{
public:
	Transform() = default;
	Transform(const Vector3& InPosition) : Position(InPosition) { }
	Transform(const Vector3& InPosition, const Quaternion& InRotation) : Position(InPosition), Rotation(InRotation) { }
	Transform(const Vector3& InPosition, const Quaternion& InRotation, const Vector3& InScale) : Position(InPosition), Rotation(InRotation), Scale(InScale) { }
	Transform(const Matrix4x4& InMatrix)
	{ 
		// 스케일 회전 행렬만 분리
		Matrix3x3 rotScaleMatrix = InMatrix.ToMatrix3x3();
		Position = InMatrix[3].ToVector3();

		// 크기 행렬부터 구한다. 
		Scale = Vector3::Zero;
		const float squareSumX = rotScaleMatrix[0].SizeSquared();
		const float squareSumY = rotScaleMatrix[1].SizeSquared();
		const float squareSumZ = rotScaleMatrix[2].SizeSquared();
		if (squareSumX > SMALL_NUMBER) { Scale.X = sqrtf(squareSumX); }
		if (squareSumY > SMALL_NUMBER) { Scale.Y = sqrtf(squareSumY); }
		if (squareSumZ > SMALL_NUMBER) { Scale.Z = sqrtf(squareSumZ); }

		// 크기 요소를 나눠 직교 정방 행렬을 구한다.
		rotScaleMatrix[0] /= squareSumX;
		rotScaleMatrix[1] /= squareSumY;
		rotScaleMatrix[2] /= squareSumZ;

		// 사원수로 변환한다.
		Rotation = Quaternion(rotScaleMatrix);
	}

public: // 트랜스폼 설정함수
	void SetPosition(const Vector3& InPosition) { Position = InPosition; }
	void AddPosition(const Vector3& InDeltaPosition) { Position += InDeltaPosition; }
	void AddYawRotation(float InDegree) 
	{
		Rotator r = Rotation.ToRotator();
		r.Yaw += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}
	void AddRollRotation(float InDegree) 
	{ 
		Rotator r = Rotation.ToRotator();
		r.Roll += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}
	void AddPitchRotation(float InDegree) 
	{ 
		Rotator r = Rotation.ToRotator();
		r.Pitch += InDegree;
		r.Clamp();
		Rotation = Quaternion(r);
	}

	void SetRotation(const Rotator& InRotator) { Rotation = Quaternion(InRotator); }
	void SetRotation(const Matrix3x3& InMatrix) { Rotation = Quaternion(InMatrix); }
	void SetRotation(const Quaternion& InQuaternion) { Rotation = InQuaternion; }
	void SetScale(const Vector3& InScale) { Scale = InScale; }

	FORCEINLINE Vector3 GetXAxis() const { return Rotation * Vector3::UnitX; }
	FORCEINLINE Vector3 GetYAxis() const { return Rotation * Vector3::UnitY; }
	FORCEINLINE Vector3 GetZAxis() const { return Rotation * Vector3::UnitZ; }
	Matrix4x4 GetMatrix() const;

	FORCEINLINE Vector3 GetPosition() const { return Position; }
	FORCEINLINE Quaternion GetRotation() const { return Rotation; }
	FORCEINLINE Vector3 GetScale() const { return Scale; }

	// 트랜스폼 변환
	FORCEINLINE Transform Inverse() const;
	FORCEINLINE Transform LocalToWorld(const Transform& InParentWorldTransform) const;
	FORCEINLINE Transform WorldToLocal(const Transform& InParentWorldTransform) const;
	//// 연산자
	//FORCEINLINE Transform operator*(const Transform& InTransform) const;

private: // 트랜스폼에 관련된 변수
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scale = Vector3::One;

};

FORCEINLINE Matrix4x4 Transform::GetMatrix() const
{
	return Matrix4x4(
		Vector4(GetXAxis() * Scale.X, false),
		Vector4(GetYAxis() * Scale.Y, false),
		Vector4(GetZAxis() * Scale.Z, false),
		Vector4(Position, true)
	);
}

FORCEINLINE Transform Transform::Inverse() const
{
	// 로컬 정보만 남기기 위한 트랜스폼 ( 역행렬 )
	Vector3 reciprocalScale = Vector3::Zero;
	if (!Math::EqualsInTolerance(Scale.X, 0.f)) reciprocalScale.X = 1.f / Scale.X;
	if (!Math::EqualsInTolerance(Scale.Y, 0.f)) reciprocalScale.Y = 1.f / Scale.Y;
	if (!Math::EqualsInTolerance(Scale.Z, 0.f)) reciprocalScale.Z = 1.f / Scale.Z;

	Transform result;
	result.Rotation = Rotation.Inverse();
	result.Scale = reciprocalScale;
	result.Position = result.Rotation * (result.Scale * -Position);
	return result;
}


FORCEINLINE Transform Transform::LocalToWorld(const Transform& InParentWorldTransform) const
{
	// 현재 트랜스폼 정보가 로컬인 경우
	Transform result;
	result.Rotation = InParentWorldTransform.Rotation * Rotation;
	result.Scale = InParentWorldTransform.Scale * Scale;
	result.Position = InParentWorldTransform.Rotation.RotateVector(InParentWorldTransform.Scale * Position) + InParentWorldTransform.Position;
	return result;
}

FORCEINLINE Transform Transform::WorldToLocal(const Transform& InParentWorldTransform) const
{
	Transform invParent = InParentWorldTransform.Inverse();

	// 현재 트랜스폼 정보가 월드인 경우
	Transform result;
	result.Scale = invParent.GetScale() * Scale;
	result.Rotation = invParent.GetRotation() * Rotation;
	result.Position = invParent.GetPosition() + Position;
	return result;
}

}
