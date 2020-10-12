
#include "Precompiled.h"

float InputManager::GetAxis(InputAxis InInputAxis) const
{
	if (InInputAxis >= InputAxis::LastAxis)
	{
		return 0.f;
	}

	return AxisMap[static_cast<size_t>(InInputAxis)]();
}

bool InputManager::IsPressed(InputButton InInputButton) const
{
	if (InInputButton >= InputButton::LastButton)
	{
		return false;
	}

	size_t targetIndex = static_cast<size_t>(InInputButton);
	return (!PrevButtonStatus[targetIndex] && PressedButtonMap[targetIndex]());
}

bool CK::InputManager::IsPressing(InputButton InInputButton) const
{
	if (InInputButton >= InputButton::LastButton)
	{
		return false;
	}

	size_t targetIndex = static_cast<size_t>(InInputButton);
	return (PrevButtonStatus[targetIndex] && PressedButtonMap[targetIndex]());
}

bool InputManager::IsReleased(InputButton InInputButton) const
{
	if (InInputButton >= InputButton::LastButton)
	{
		return false;
	}

	size_t targetIndex = static_cast<size_t>(InInputButton);
	return (PrevButtonStatus[targetIndex] && !PressedButtonMap[targetIndex]());
}

bool InputManager::IsInputSystemReady() const
{
	if (AxisMap.size() != static_cast<size_t>(InputAxis::LastAxis))
	{
		return false;
	}

	if (PressedButtonMap.size() != static_cast<size_t>(InputButton::LastButton))
	{
		return false;
	}

	return true;
}

void InputManager::SetInputAxis(InputAxis InInputAxis, std::function<float()> InAxisFn)
{
	if (InInputAxis >= InputAxis::LastAxis)
	{
		return;
	}

	AxisMap[static_cast<size_t>(InInputAxis)] = InAxisFn;
}

void InputManager::SetInputButton(InputButton InInputButton, std::function<bool()> InPressedFn)
{
	if (InInputButton >= InputButton::LastButton)
	{
		return;
	}

	PressedButtonMap[static_cast<size_t>(InInputButton)] = InPressedFn;
}

void CK::InputManager::UpdateInput()
{
	size_t lastIndex = static_cast<size_t>(InputButton::LastButton);
	for (size_t bi = 0; bi < lastIndex; ++bi)
	{
		PrevButtonStatus[bi] = PressedButtonMap[bi]();
	}
}
