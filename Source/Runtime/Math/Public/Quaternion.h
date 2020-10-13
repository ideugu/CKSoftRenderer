#pragma once

namespace CK
{

struct Quaternion
{
public:
	FORCEINLINE constexpr Quaternion() = default;
	FORCEINLINE explicit constexpr Quaternion(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) { }
	FORCEINLINE explicit constexpr Quaternion(const Vector3& InAxis, float InAngleDegree)
	{
		FromAxisAngle(InAxis, InAngleDegree);
	}

	FORCEINLINE explicit constexpr Quaternion(const Rotator& InRotator)
	{
		FromRotator(InRotator);
	}

	FORCEINLINE Quaternion(const Vector3& InVector, const Vector3& InUp = Vector3::UnitY)
	{
		FromVector(InVector, InUp);
	}

	FORCEINLINE Quaternion(const Matrix3x3& InMatrix)
	{
		FromMatrix(InMatrix);
	}

	// 생성
	FORCEINLINE void FromMatrix(const Matrix3x3& InMatrix);
	FORCEINLINE void FromVector(const Vector3& InVector, const Vector3& InUp = Vector3::UnitY);
	FORCEINLINE constexpr void FromAxisAngle(const Vector3& InAxis, float InAngleDegree);
	FORCEINLINE constexpr void FromRotator(const Rotator& InRotator);


	FORCEINLINE constexpr Quaternion operator*(const Quaternion& InQuaternion) const;
	FORCEINLINE constexpr Quaternion operator*=(const Quaternion& InQuaternion);
	FORCEINLINE constexpr Vector3 operator*(const Vector3& InVector) const;
	static Quaternion Slerp(const Quaternion &InQuaternion1, const Quaternion &InQuaternion2, float InRatio);

	FORCEINLINE constexpr Vector3 RotateVector(const Vector3& InVector) const;
	FORCEINLINE constexpr Quaternion Inverse() const { return Quaternion(-X, -Y, -Z, W); }
	FORCEINLINE void Normalize();
	FORCEINLINE Rotator ToRotator() const;
	FORCEINLINE bool IsUnitQuaternion() const
	{
		float size = sqrtf(X * X + Y * Y + Z * Z + W * W);
		if (Math::EqualsInTolerance(size, 1.f))
		{
			return true;
		}
		
		return false;
	}

	FORCEINLINE constexpr float RealPart() const { return W; }
	FORCEINLINE constexpr Vector3 ImaginaryPart() const { return Vector3(X, Y, Z); }
	std::string ToString() const;

	static const Quaternion Identity;

public:
	float X = 0.f;
	float Y = 0.f;
	float Z = 0.f;
	float W = 1.f;
};

FORCEINLINE void Quaternion::FromMatrix(const Matrix3x3& InMatrix)
{
	float root = 0.f;
	float trace = InMatrix[0][0] + InMatrix[1][1] + InMatrix[2][2];

	if (!Math::EqualsInTolerance(InMatrix[0].SizeSquared(), 1.f) || !Math::EqualsInTolerance(InMatrix[1].SizeSquared(), 1.f) || !Math::EqualsInTolerance(InMatrix[2].SizeSquared(), 1.f))
	{
		*this = Quaternion::Identity;
	}

	if (trace > 0.f)
	{
		root = sqrtf(trace + 1.f);
		W = 0.5f * root;
		root = 0.5f / root;

		X = (InMatrix[1][2] - InMatrix[2][1]) * root;
		Y = (InMatrix[2][0] - InMatrix[0][2]) * root;
		Z = (InMatrix[0][1] - InMatrix[1][0]) * root;
	}
	else
	{
		BYTE i = 0;

		if (InMatrix[1][1] > InMatrix[0][0]) { i = 1; }
		if (InMatrix[2][2] > InMatrix[i][i]) { i = 2; }

		// i, j, k 의 순서 지정
		static const BYTE next[3] = { 1, 2, 0 };
		const BYTE j = next[i];
		const BYTE k = next[j];

		root = sqrtf(InMatrix[i][i] - InMatrix[j][j] - InMatrix[k][k] + 1.f);

		float* qt[3] = { &X, &Y, &Z };
		*qt[i] = 0.5f * root;

		root = 0.5f / root;
		*qt[j] = (InMatrix[i][j] + InMatrix[j][i]) * root;
		*qt[k] = (InMatrix[i][k] + InMatrix[k][i]) * root;
		W = (InMatrix[j][k] - InMatrix[k][j]) * root;
	}
}

FORCEINLINE void Quaternion::FromVector(const Vector3& InVector, const Vector3& InUp)
{
	Vector3 viewX, viewY, viewZ;

	// 단위 Z축 생성
	viewZ = InVector.Normalize();
	// 시선 방향과 월드 Y축이 평행한 경우
	if (Math::Abs(viewZ.Y) >= (1.f - SMALL_NUMBER))
	{
		// 특이 상황에서 로컬 X 좌표 값을 임의로 지정.
		viewX = Vector3::UnitX;
	}
	else
	{
		viewX = InUp.Cross(viewZ).Normalize();
	}
	viewY = viewZ.Cross(viewX).Normalize();

	FromMatrix(Matrix3x3(viewX, viewY, viewZ));
}

FORCEINLINE constexpr void Quaternion::FromAxisAngle(const Vector3& InAxis, float InAngleDegree)
{
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, InAngleDegree * 0.5f);
	W = cos;
	X = sin * InAxis.X;
	Y = sin * InAxis.Y;
	Z = sin * InAxis.Z;
}

FORCEINLINE constexpr void Quaternion::FromRotator(const Rotator& InRotator)
{
	float sp = 0.f, sy = 0.f, sr = 0.f;
	float cp = 0.f, cy = 0.f, cr = 0.f;

	Math::GetSinCos(sp, cp, InRotator.Pitch * 0.5f);
	Math::GetSinCos(sy, cy, InRotator.Yaw * 0.5f);
	Math::GetSinCos(sr, cr, InRotator.Roll * 0.5f);

	W = sy * sp * sr + cy * cp * cr;
	X = sy * sr * cp + sp * cy * cr;
	Y = sy * cp * cr - sp * sr * cy;
	Z = -sy * sp * cr + sr * cy * cp;
}

FORCEINLINE constexpr Quaternion Quaternion::operator*(const Quaternion & InQuaternion) const
{
	Quaternion result;
	Vector3 v1(X, Y, Z), v2(InQuaternion.X, InQuaternion.Y, InQuaternion.Z);
	result.W = W * InQuaternion.W - v1.Dot(v2);
	Vector3 v = v2 * W + v1 * InQuaternion.W + v1.Cross(v2);
	result.X = v.X;
	result.Y = v.Y;
	result.Z = v.Z;

	return result;
}

FORCEINLINE constexpr Quaternion Quaternion::operator*=(const Quaternion & InQuaternion)
{
	Vector3 v1(X, Y, Z), v2(InQuaternion.X, InQuaternion.Y, InQuaternion.Z);
	W = W * InQuaternion.W - v1.Dot(v2);
	Vector3 v = v2 * W + v1 * InQuaternion.W + v1.Cross(v2);
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	return *this;
}

FORCEINLINE constexpr Vector3 Quaternion::operator*(const Vector3& InVector) const
{
	return RotateVector(InVector);
}

FORCEINLINE Quaternion Quaternion::Slerp(const Quaternion & InQuaternion1, const Quaternion & InQuaternion2, float InRatio)
{
	float rawCosValue =
		InQuaternion1.X * InQuaternion2.X +
		InQuaternion1.Y * InQuaternion2.Y +
		InQuaternion1.Z * InQuaternion2.Z +
		InQuaternion1.W * InQuaternion2.W;

	float cosValue = (rawCosValue >= 0.f) ? rawCosValue : -rawCosValue;
	float alpha, beta;
	if (cosValue < 0.9999f)
	{
		const float omega = acosf((cosValue < -1.f) ? -1.f : ((cosValue < 1.f) ? cosValue : 1.f));
		const float invSin = 1.f / sinf(omega);
		alpha = sinf((1.f - InRatio) * omega) * invSin;
		beta = sinf(InRatio * omega) * invSin;
	}
	else
	{
		// 사이각이 극히 작은 경우 계산이 빠른 선형 보간을 사용한다.  
		alpha = 1.0f - InRatio;
		beta = InRatio;
	}

	beta = (rawCosValue >= 0.f) ? beta : -beta;
	Quaternion result;

	result.X = alpha * InQuaternion1.X + beta * InQuaternion2.X;
	result.Y = alpha * InQuaternion1.Y + beta * InQuaternion2.Y;
	result.Z = alpha * InQuaternion1.Z + beta * InQuaternion2.Z;
	result.W = alpha * InQuaternion1.W + beta * InQuaternion2.W;

	return result;
}

FORCEINLINE constexpr Vector3 Quaternion::RotateVector(const Vector3& InVector) const
{
	Vector3 q(X, Y, Z);
	Vector3 t = q.Cross(InVector) * 2.f;
	Vector3 result = InVector + (t * W) + q.Cross(t);
	return result;
}

FORCEINLINE void Quaternion::Normalize()
{
	const float squareSum = X * X + Y * Y + Z * Z + W * W;

	if (squareSum >= SMALL_NUMBER)
	{
		const float scale = 1.f / sqrtf(squareSum);

		X *= scale;
		Y *= scale;
		Z *= scale;
		W *= scale;
	}
	else
	{
		*this = Quaternion::Identity;
	}
}

FORCEINLINE Rotator Quaternion::ToRotator() const
{
	Rotator result;
	float sinrCosp = 2 * (W * Z + X * Y);
	float cosrCosp = 1 - 2 * (Z * Z + X * X);
	result.Roll = Math::Rad2Deg(atan2f(sinrCosp, cosrCosp));

	float pitchTest = W * X - Y * Z;
	float asinThreshold = 0.4999995f;
	float sinp = 2 * pitchTest;
	if (pitchTest < -asinThreshold)
	{
		result.Pitch = -90.f;
	}
	else if (pitchTest > asinThreshold)
	{
		result.Pitch = 90.f;
	}
	else
	{
		result.Pitch = Math::Rad2Deg(asinf(sinp));
	}

	float sinyCosp = 2 * (W * Y + X * Z);
	float cosyCosp = 1.f - 2 * (X * X + Y * Y);
	result.Yaw = Math::Rad2Deg(atan2f(sinyCosp, cosyCosp));
	
	return result;
}

}