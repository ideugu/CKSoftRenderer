#pragma once

namespace CK
{

struct Matrix3x3
{
public:
	// 생성자 
	FORCEINLINE Matrix3x3();
	FORCEINLINE explicit Matrix3x3(const Vector3& InCol0, const Vector3& InCol1, const Vector3& InCol2);
	FORCEINLINE explicit Matrix3x3(float In00, float In01, float In02, float In10, float In11, float In12, float In20, float In21, float In22);

	// 연산자 
	FORCEINLINE const Vector3& operator[](BYTE InIndex) const;
	FORCEINLINE Vector3& operator[](BYTE InIndex);

	FORCEINLINE Matrix3x3 operator*(float InScalar) const;
	FORCEINLINE Matrix3x3 operator*(const Matrix3x3& InMatrix) const;
	FORCEINLINE Vector3 operator*(const Vector3& InVector) const;
	FORCEINLINE friend Vector3 operator*=(Vector3& InVector, const Matrix3x3& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}
	FORCEINLINE Vector2 operator*(const Vector2& InVector) const;
	FORCEINLINE friend Vector2 operator*=(Vector2& InVector, const Matrix3x3& InMatrix)
	{
		InVector = InMatrix * InVector;
		return InVector;
	}

	// 멤버함수 
	FORCEINLINE Matrix2x2 ToMatrix2x2() const;
	FORCEINLINE void SetIdentity();
	FORCEINLINE Matrix3x3 Tranpose() const;

	std::vector<std::string> ToStrings() const;

	// 정적멤버변수 
	static const Matrix3x3 Identity;
	enum { Rank = 3 };

	// 멤버변수 
	Vector3 Cols[3] = { Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ };
};

FORCEINLINE Matrix3x3::Matrix3x3()
{
}

FORCEINLINE Matrix3x3::Matrix3x3(const Vector3& InCol0, const Vector3& InCol1, const Vector3& InCol2)
{
	Cols[0] = InCol0;
	Cols[1] = InCol1;
	Cols[2] = InCol2;
}

FORCEINLINE Matrix3x3::Matrix3x3(float In00, float In01, float In02, float In10, float In11, float In12, float In20, float In21, float In22)
{
	Cols[0][0] = In00;
	Cols[0][1] = In01;
	Cols[0][2] = In02;

	Cols[1][0] = In10;
	Cols[1][1] = In11;
	Cols[1][2] = In12;

	Cols[2][0] = In20;
	Cols[2][1] = In21;
	Cols[2][2] = In22;
}

FORCEINLINE void Matrix3x3::SetIdentity()
{
	*this = Matrix3x3::Identity;
}

FORCEINLINE Matrix3x3 Matrix3x3::Tranpose() const
{
	return Matrix3x3(
		Vector3(Cols[0].X, Cols[1].X, Cols[2].X),
		Vector3(Cols[0].Y, Cols[1].Y, Cols[2].Y),
		Vector3(Cols[0].Z, Cols[1].Z, Cols[2].Z)
	);
}

FORCEINLINE const Vector3& Matrix3x3::operator[](BYTE InIndex) const
{
	return (InIndex < Rank) ? Cols[InIndex] : Cols[0];
}

FORCEINLINE Vector3& Matrix3x3::operator[](BYTE InIndex)
{
	return (InIndex < Rank) ? Cols[InIndex] : Cols[0];
}

FORCEINLINE Matrix3x3 Matrix3x3::operator*(float InScalar) const
{
	return Matrix3x3(
		Cols[0] * InScalar,
		Cols[1] * InScalar,
		Cols[2] * InScalar
	);
}

FORCEINLINE Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &InMatrix) const
{
	Matrix3x3 tpMat = Tranpose();
	return Matrix3x3(
		Vector3(tpMat[0].Dot(InMatrix[0]), tpMat[1].Dot(InMatrix[0]), tpMat[2].Dot(InMatrix[0])),
		Vector3(tpMat[0].Dot(InMatrix[1]), tpMat[1].Dot(InMatrix[1]), tpMat[2].Dot(InMatrix[1])),
		Vector3(tpMat[0].Dot(InMatrix[2]), tpMat[1].Dot(InMatrix[2]), tpMat[2].Dot(InMatrix[2]))
	);

}

FORCEINLINE Vector3 Matrix3x3::operator*(const Vector3& InVector) const
{
	Matrix3x3 tpMat = Tranpose();
	return Vector3(
		tpMat[0].Dot(InVector),
		tpMat[1].Dot(InVector),
		tpMat[2].Dot(InVector)
	);
}

FORCEINLINE Vector2 Matrix3x3::operator*(const Vector2& InVector) const
{
	Vector3 v3(InVector);
	v3 *= *this;

	return v3.ToVector2();
}

FORCEINLINE Matrix2x2 Matrix3x3::ToMatrix2x2() const
{
	return Matrix2x2(Cols[0].ToVector2(), Cols[1].ToVector2());
}

}