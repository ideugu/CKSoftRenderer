
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

// �޽�
const std::size_t GameEngine::CubeMesh = std::hash<std::string>()("SM_Cube");

// ���� ������Ʈ
const std::string GameEngine::SunGo("Sun");
const std::string GameEngine::EarthGo("Earth");
const std::string GameEngine::MoonGo("Moon");

// �ؽ���
const std::size_t GameEngine::DiffuseTexture = std::hash<std::string>()("Diffuse");
const std::string GameEngine::SteveTexturePath("Steve.png");

struct GameObjectCompare
{
	bool operator()(const std::unique_ptr<GameObject>& lhs, std::size_t rhs)
	{
		return lhs->GetHash() < rhs;
	}
};

void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize)
{
	// ȭ�� ũ���� ����
	_ScreenSize = InScreenSize;
	_MainCamera.SetViewportSize(_ScreenSize);
}

bool GameEngine::Init()
{
	// �̹� �ʱ�ȭ�Ǿ� ������ �ʱ�ȭ �������� ����.
	if (_IsInitialized)
	{
		return true;
	}

	// ȭ�� ũ�Ⱑ �ùٷ� �����Ǿ� �ִ��� Ȯ��
	if (_ScreenSize.HasZero())
	{
		return false;
	}

	if (!_InputManager.IsInputReady())
	{
		return false;
	}

	if (!LoadResources())
	{
		return false;
	}

	if (!LoadScene())
	{
		return false;
	}

	_IsInitialized = true;
	return _IsInitialized;
}

bool GameEngine::LoadResources()
{
	// ť�� �޽�
	Mesh& cubeMesh = CreateMesh(GameEngine::CubeMesh);
	auto& v = cubeMesh.GetVertices();
	auto& i = cubeMesh.GetIndices();

	static const float halfSize = 0.5f;
	std::transform(cubeMeshPositions.begin(), cubeMeshPositions.end(), std::back_inserter(v), [&](auto& p) { return p * halfSize; });
	std::transform(cubeMeshIndice.begin(), cubeMeshIndice.end(), std::back_inserter(i), [&](auto& p) { return p; });

	auto& uv = cubeMesh.GetUVs();
	uv = {
		// Right
		Vector2(0.f, 48.f) / 64.f, Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(0.f, 56.f) / 64.f,
		// Front
		Vector2(8.f, 48.f) / 64.f, Vector2(8.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(16.f, 48.f) / 64.f,
		// Back
		Vector2(32.f, 48.f) / 64.f, Vector2(32.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(24.f, 48.f) / 64.f,
		// Left
		Vector2(24.f, 48.f) / 64.f, Vector2(16.f, 48.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(24.f, 56.f) / 64.f,
		// Top
		Vector2(8.f, 64.f) / 64.f, Vector2(16.f, 64.f) / 64.f, Vector2(16.f, 56.f) / 64.f, Vector2(8.f, 56.f) / 64.f,
		// Bottom
		Vector2(16.f, 64.f) / 64.f, Vector2(24.f, 64.f) / 64.f, Vector2(24.f, 56.f) / 64.f, Vector2(16.f, 56.f) / 64.f
	};

	// �޽��� �ٿ�� ���� ����
	cubeMesh.CalculateBounds();

	// �ؽ��� �ε�
	Texture& diffuseTexture = CreateTexture(GameEngine::DiffuseTexture, GameEngine::SteveTexturePath);
	assert(diffuseTexture.IsIntialized());

	return true;
}

bool GameEngine::LoadScene()
{
	static const float sunScale = 100.f;
	static const float earthScale = 40.f;
	static const float moonScale = 30.f;
	static const Vector3 earthOffset(500.f, 0.0f, 0.f);
	static const Vector3 moonOffset(400.f, 0.0f, 0.f);

	// �¾��
	GameObject& goSun = CreateNewGameObject(GameEngine::SunGo);
	goSun.SetMesh(GameEngine::CubeMesh);
	goSun.GetTransform().SetWorldScale(Vector3::One * sunScale);

	GameObject& goEarth = CreateNewGameObject(GameEngine::EarthGo);
	goEarth.SetMesh(GameEngine::CubeMesh);
	goEarth.GetTransform().SetWorldPosition(earthOffset);
	goEarth.GetTransform().SetWorldScale(Vector3::One * earthScale);
	goEarth.SetParent(goSun);

	GameObject& goMoon = CreateNewGameObject(GameEngine::MoonGo);
	goMoon.SetMesh(GameEngine::CubeMesh);
	goMoon.GetTransform().SetWorldPosition(moonOffset);
	goMoon.GetTransform().SetWorldScale(Vector3::One * moonScale);
	goMoon.SetParent(goEarth);

	// ī�޶� ����
	CameraObject& mainCamera = GetMainCamera();
	mainCamera.GetTransform().SetWorldPosition(Vector3(700.f, 700.f, 700.f));

	return true;
}

Mesh& GameEngine::CreateMesh(const std::size_t& InKey)
{
	auto meshPtr = std::make_unique<Mesh>();
	_Meshes.insert({ InKey, std::move(meshPtr) });
	return *_Meshes.at(InKey).get();
}

Texture& GameEngine::CreateTexture(const std::size_t& InKey, const std::string& InTexturePath)
{
	auto texturePtr = std::make_unique<Texture>(InTexturePath);
	_Textures.insert({ InKey, std::move(texturePtr) });
	return *_Textures.at(InKey).get();
}

GameObject& GameEngine::CreateNewGameObject(const std::string& InName)
{
	std::size_t inHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), inHash, GameObjectCompare());

	auto newGameObject = std::make_unique<GameObject>(InName);
	if (it != _Scene.end())
	{
		std::size_t targetHash = (*it)->GetHash();
		if (targetHash == inHash)
		{
			// �ߺ��� Ű �߻�. ����.
			assert(false);
			return GameObject::Invalid;
		}
		else if (targetHash < inHash)
		{
			_Scene.insert(it + 1, std::move(newGameObject));
		}
		else
		{
			_Scene.insert(it, std::move(newGameObject));
		}
	}
	else
	{
		_Scene.push_back(std::move(newGameObject));
	}

	return GetGameObject(InName);
}

GameObject& GameEngine::GetGameObject(const std::string& InName)
{
	std::size_t targetHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash, GameObjectCompare());

	return (it != _Scene.end()) ? *(*it).get() : GameObject::Invalid;
}
