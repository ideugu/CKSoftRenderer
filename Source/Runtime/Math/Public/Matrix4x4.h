#pragma once

namespace CK
{

struct Matrix4x4
{
public:
	// 생성자 
	FORCEINLINE Matrix4x4();
	FORCEINLINE explicit Matrix4x4(const Vector4& InCol0, const Vector4& InCol1, const Vector4& InCol2, const Vector4& InCol3);

	// 연산자 
	FORCEINLINE const Vector4& operator[](BYTE InIndex) const;
	FORCEINLINE Vector4& operator[](BYTE InIndex);
	FORCEINLINE Matrix4x4 operator*(float InScalar) const;
	FORCEINLINE Matrix4x4 operator*(const Matrix4x4& InMatrix) const;
	FORCEINLINE Vector4 operator*(const Vector4& InVector) const;
	FORCEINLINE friend Vector4 operator*=(Vector4& InVector, const Matrix4x4& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}
	FORCEINLINE Vector3 operator*(const Vector3& InVector) const;
	FORCEINLINE friend Vector3 operator*=(Vector3& InVector, const Matrix4x4& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}

	// 멤버함수 
	FORCEINLINE Matrix3x3 ToMatrix3x3() const;
	FORCEINLINE void SetIdentity();
	FORCEINLINE Matrix4x4 Tranpose() const;

	std::vector<std::string> ToStrings() const;

	// 정적멤버변수 
	static const Matrix4x4 Identity;
	enum { Rank = 4 };

	// 멤버변수 
	Vector4 Cols[4] = { Vector4::UnitX, Vector4::UnitY, Vector4::UnitZ, Vector4::UnitW };
};

FORCEINLINE Matrix4x4::Matrix4x4()
{
}

FORCEINLINE Matrix4x4::Matrix4x4(const Vector4& InCol0, const Vector4& InCol1, const Vector4& InCol2, const Vector4& InCol3)
{
	Cols[0] = InCol0;
	Cols[1] = InCol1;
	Cols[2] = InCol2;
	Cols[3] = InCol3;
}

FORCEINLINE void Matrix4x4::SetIdentity()
{
	*this = Matrix4x4::Identity;
}

FORCEINLINE Matrix4x4 Matrix4x4::Tranpose() const
{
	return Matrix4x4(
		Vector4(Cols[0].X, Cols[1].X, Cols[2].X, Cols[3].X),
		Vector4(Cols[0].Y, Cols[1].Y, Cols[2].Y, Cols[3].Y),
		Vector4(Cols[0].Z, Cols[1].Z, Cols[2].Z, Cols[3].Z),
		Vector4(Cols[0].W, Cols[1].W, Cols[2].W, Cols[3].W)
	);
}

FORCEINLINE const Vector4& Matrix4x4::operator[](BYTE InIndex) const
{
	return (InIndex < Rank) ? Cols[InIndex] : Cols[0];
}

FORCEINLINE Vector4& Matrix4x4::operator[](BYTE InIndex)
{
	return (InIndex < Rank) ? Cols[InIndex] : Cols[0];
}

FORCEINLINE Matrix4x4 Matrix4x4::operator*(float InScalar) const
{
	return Matrix4x4(
		Cols[0] * InScalar,
		Cols[1] * InScalar,
		Cols[2] * InScalar,
		Cols[3] * InScalar
	);
}

FORCEINLINE Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &InMatrix) const
{
	Matrix4x4 tpMat = Tranpose();
	return Matrix4x4(
		Vector4(tpMat[0].Dot(InMatrix[0]), tpMat[1].Dot(InMatrix[0]), tpMat[2].Dot(InMatrix[0]), tpMat[3].Dot(InMatrix[0])),
		Vector4(tpMat[0].Dot(InMatrix[1]), tpMat[1].Dot(InMatrix[1]), tpMat[2].Dot(InMatrix[1]), tpMat[3].Dot(InMatrix[1])),
		Vector4(tpMat[0].Dot(InMatrix[2]), tpMat[1].Dot(InMatrix[2]), tpMat[2].Dot(InMatrix[2]), tpMat[3].Dot(InMatrix[2])),
		Vector4(tpMat[0].Dot(InMatrix[3]), tpMat[1].Dot(InMatrix[3]), tpMat[2].Dot(InMatrix[3]), tpMat[3].Dot(InMatrix[3]))
	);
}

FORCEINLINE Vector4 Matrix4x4::operator*(const Vector4& InVector) const
{
	Matrix4x4 tpMat = Tranpose();
	return Vector4(
		tpMat[0].Dot(InVector),
		tpMat[1].Dot(InVector),
		tpMat[2].Dot(InVector),
		tpMat[3].Dot(InVector)
	);
}

FORCEINLINE Vector3 Matrix4x4::operator*(const Vector3& InVector) const
{
	Vector4 v4(InVector);
	v4 *= *this;

	return v4.ToVector3();
}

FORCEINLINE Matrix3x3 Matrix4x4::ToMatrix3x3() const
{
	return Matrix3x3(Cols[0].ToVector3(), Cols[1].ToVector3(), Cols[2].ToVector3());
}

}