
#include "Precompiled.h"
#include <random>
using namespace CK::DDD;

// �޽�
const std::size_t GameEngine::CubeMesh = std::hash<std::string>()("SM_Cube");

// ���� ������Ʈ
const std::string GameEngine::PlayerGo("Player");

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

	// �ؽ��� �ε�
	Texture& diffuseTexture = CreateTexture(GameEngine::DiffuseTexture, GameEngine::SteveTexturePath);
	assert(diffuseTexture.IsIntialized());

	return true;
}

bool GameEngine::LoadScene()
{
	// �÷��̾�
	constexpr float cubeScale = 100.f;

	// �÷��̾� ����
	GameObject& goPlayer = CreateNewGameObject(GameEngine::PlayerGo);
	goPlayer.SetMesh(GameEngine::CubeMesh);
	goPlayer.GetTransform().SetPosition(Vector3::Zero);
	goPlayer.GetTransform().SetScale(Vector3::One * cubeScale);

	// ���� �õ�� �����ϰ� ����
	std::mt19937 generator(0);
	std::uniform_real_distribution<float> distZ(-1500.f, 0.f);
	std::uniform_real_distribution<float> distXY(-1000.f, 1000.f);

	// 100���� ť�� ���� ������Ʈ ����
	for (int i = 0; i < 100; ++i)
	{
		char name[64];
		std::snprintf(name, sizeof(name), "GameObject%d", i);
		GameObject& newGo = CreateNewGameObject(name);
		newGo.GetTransform().SetPosition(Vector3(distXY(generator), distXY(generator), distZ(generator)));
		newGo.GetTransform().SetScale(Vector3::One * cubeScale);
		newGo.SetMesh(GameEngine::CubeMesh);
	}

	// ī�޶� ����
	CameraObject& mainCamera = GetMainCamera();
	mainCamera.GetTransform().SetPosition(Vector3(0.f, 0.f, 400.f));
	mainCamera.GetTransform().SetRotation(Rotator(180.f, 0.f, 0.f));
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
