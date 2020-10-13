#pragma once

namespace CK
{

struct Rotator
{
public:
	FORCEINLINE constexpr Rotator() = default;
	FORCEINLINE constexpr Rotator(float InYaw, float InRoll, float InPitch) : Yaw(InYaw), Roll(InRoll), Pitch(InPitch) { }
	FORCEINLINE void Clamp()
	{
		Yaw = GetAxisClampedValue(Yaw);
		Roll = GetAxisClampedValue(Roll);
		Pitch = GetAxisClampedValue(Pitch);
	}

	FORCEINLINE float GetAxisClampedValue(float InRotatorValue)
	{
		float angle = Math::FMod(InRotatorValue, 360.f);
		if (angle < 0.f)
		{
			angle += 360.f;
		}

		return angle;
	}

	std::string ToString() const;
	static const Rotator Identity;

public:
	float Yaw = 0.f;
	float Roll = 0.f;
	float Pitch = 0.f;
};

}
