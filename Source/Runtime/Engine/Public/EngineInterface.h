#pragma once

namespace CK
{

enum class GameObjectType : UINT32
{
	Normal = 0,
	Gizmo
};

enum class MeshType : UINT32
{
	Normal = 0,
	Skinned
};

class EngineInterface
{
public:
	virtual bool Init() = 0;
	virtual bool IsInitialized() = 0;
	virtual void OnScreenResize(const ScreenPoint& InScreenSize) = 0;
	virtual InputManager& GetInputManager() = 0;
};

}