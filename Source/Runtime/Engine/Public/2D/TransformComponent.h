#pragma once

namespace CK
{
namespace DD
{ 

class TransformComponent
{
public:
	TransformComponent() = default;

public:
	// 게임 로직에서 사용할 멤버 함수
	void SetPosition(const Vector2& InPosition) { Position = InPosition; }
	void AddPosition(const Vector2& InDeltaPosition) { Position += InDeltaPosition; }
	void SetScale(const Vector2& InScale) { Scale = InScale; }
	void SetRotation(float InDegree) { Rotation = InDegree; Update(); }
	void AddRotation(float InDegree) { Rotation += InDegree; Update(); }
	Vector2 GetPosition() const { return Position; }
	Vector2 GetScale() const { return Scale; }
	float GetRotation() const { return Rotation; }
	const Vector2& GetLocalX() const { return Right; }
	const Vector2& GetLocalY() const { return Up; }

	// 행렬 생성 함수
	FORCEINLINE Matrix3x3 GetModelingMatrix() const;

private:
	FORCEINLINE void Update();

private:
	Vector2 Position = Vector2::Zero;
	float Rotation = 0.f;
	Vector2 Scale = Vector2::One;

	Vector2 Right = Vector2::UnitX;
	Vector2 Up = Vector2::UnitY;
};

FORCEINLINE Matrix3x3 TransformComponent::GetModelingMatrix() const
{
	return Matrix3x3(
		Vector3(Scale.X * Right.X, Scale.X * Right.Y, 0.f),
		Vector3(Scale.Y * Up.X, Scale.Y * Up.Y, 0.f),
		Vector3(Position.X, Position.Y, 1.f)
	);
}

FORCEINLINE void TransformComponent::Update()
{
	float sin, cos;
	Math::GetSinCos(sin, cos, Rotation);

	Right = Vector2(cos, sin);
	Up = Vector2(-sin, cos);
}

}
}