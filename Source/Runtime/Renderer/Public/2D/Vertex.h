
#pragma once

namespace CK
{
namespace DD
{

struct Vertex2D
{
public:
	Vertex2D() = default;
	Vertex2D(const Vector2& InPosition) : Position(InPosition) { }
	Vertex2D(const Vector2& InPosition, const LinearColor& InColor) : Position(InPosition), Color(InColor) { }
	Vertex2D(const Vector2& InPosition, const LinearColor& InColor, const Vector2& InUV) : Position(InPosition), Color(InColor), UV(InUV) { }

	Vertex2D operator*(float InScalar) const
	{
		return Vertex2D(
			Position * InScalar,
			Color * InScalar,
			UV * InScalar
		);
	}

	Vertex2D operator+(const Vertex2D& InVector) const
	{
		return Vertex2D(
			Position + InVector.Position,
			Color + InVector.Color,
			UV + InVector.UV
		);
	}

	Vector2 Position;
	LinearColor Color;
	Vector2 UV;
};

//struct Vertex2D
//{
//public:
//	Vertex2D() = default;
//	Vertex2D(const Vector2& InPosition, const Vector2& InUV) : Position(InPosition), UV(InUV) { }
//
//	Vector2 Position;
//	Vector2 UV;
//};

}
}