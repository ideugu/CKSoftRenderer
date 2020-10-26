
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DDD;

// 그리드 그리기
void SoftRenderer::DrawGizmo3D()
{
	auto& r = GetRenderer();
	const GameEngine& g = Get3DGameEngine();

	// 뷰 기즈모 그리기
	std::vector<Vertex3D> viewGizmo = { 
		Vertex3D(Vector4(Vector3::Zero)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
	};

	Matrix4x4 viewMatRotationOnly = g.GetMainCamera().GetViewMatrixRotationOnly();
	VertexShader3D(viewGizmo, viewMatRotationOnly);

	// 축 그리기
	Vector2 v0 = viewGizmo[0].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v1 = viewGizmo[1].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v2 = viewGizmo[2].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v3 = viewGizmo[3].Position.ToVector2() + _GizmoPositionOffset;
	r.DrawLine(v0, v1, LinearColor::Red);
	r.DrawLine(v0, v2, LinearColor::Green);
	r.DrawLine(v0, v3, LinearColor::Blue);

}

// 게임 로직
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	// 기본 레퍼런스
	GameEngine& g = Get3DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 기본 설정 변수
	static float moveSpeed = 500.f;
	static float rotateSpeed = 180.f;

	// 게임 로직에서 사용할 게임 오브젝트 레퍼런스
	GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);
	goPlayer.GetTransform().AddPosition(Vector3::UnitZ * input.GetAxis(InputAxis::ZAxis) * moveSpeed * InDeltaSeconds);
	goPlayer.GetTransform().AddPitchRotation(-input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds);

	CameraObject& camera = g.GetMainCamera();
	camera.GetTransform().AddYawRotation(-input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds);
	camera.GetTransform().AddPitchRotation(-input.GetAxis(InputAxis::YAxis) * rotateSpeed * InDeltaSeconds);



	// 플레이어 게임 오브젝트 트랜스폼의 변경
	//GameObject& player = _GameEngine3.FindGameObject(GameEngine::PlayerKey);
	//if (!player.IsNotFound())
	//{
	//	Transform& playerTransform = player.GetTransform();
	//	playerTransform.AddPosition(Vector3::UnitZ * input.GetZAxis() * moveSpeed * InDeltaSeconds);
	//	playerTransform.AddPitchRotation(-input.GetWAxis() * rotateSpeed * InDeltaSeconds);
	//}

	//// 카메라 회전에는 좌우 반전을 적용
	//_GameEngine3.GetMainCamera().GetTransform().AddYawRotation(-input.GetXAxis() * rotateSpeed * InDeltaSeconds);
	//_GameEngine3.GetMainCamera().GetTransform().AddPitchRotation(-input.GetYAxis() * rotateSpeed * InDeltaSeconds);

	//CameraObject& camera = g.GetMainCamera();

	//// 기본 설정 변수
	//static float fovSpeed = 100.f;
	//static float rotateSpeed = 180.f;
	//static float moveSpeed = 500.f;

	//// 게임 로직에서 사용할 게임 오브젝트 레퍼런스
	//GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);
	//GameObject& goCameraRig = g.GetGameObject(GameEngine::CameraRigGo);

	//goPlayer.GetTransform().AddLocalYawRotation(-input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds);
	//goPlayer.GetTransform().AddLocalPosition(goPlayer.GetTransform().GetLocalZ() * input.GetAxis(InputAxis::YAxis) * moveSpeed * InDeltaSeconds);

	//// 카메라 화각 설정
	//float newFOV = Math::Clamp(camera.GetFOV() + input.GetAxis(InputAxis::ZAxis) * fovSpeed * InDeltaSeconds, 5.f, 179.f);
	//camera.SetFOV(newFOV);
}

// 캐릭터 애니메이션 로직
void SoftRenderer::LateUpdate3D(float InDeltaSeconds)
{
	return;
}

// 렌더링 로직
void SoftRenderer::Render3D()
{
	const GameEngine& g = Get3DGameEngine();

	const CameraObject& mainCamera = g.GetMainCamera();
	const Matrix4x4 vMatrix = mainCamera.GetViewMatrix();
	const ScreenPoint viewportSize = mainCamera.GetViewportSize();

	// 기즈모 그리기
	DrawGizmo3D();

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		const TransformComponent& transform = gameObject.GetTransform();
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		// 렌더링 시작
		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());

		// 최종 변환 행렬
		Matrix4x4 finalMatrix = vMatrix * transform.GetModelingMatrix();
		DrawMesh3D(mesh, finalMatrix, gameObject.GetColor());
	}
}

void SoftRenderer::DrawMesh3D(const Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor)
{
	size_t vertexCount = InMesh.GetVertices().size();
	size_t indexCount = InMesh.GetIndices().size();
	size_t triangleCount = indexCount / 3;

	// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
	std::vector<Vertex3D> vertices(vertexCount);
	std::vector<size_t> indice(InMesh.GetIndices());
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = Vector4(InMesh.GetVertices()[vi]);

		if (InMesh.HasColor())
		{
			vertices[vi].Color = InMesh.GetColors()[vi];
		}

		if (InMesh.HasUV())
		{
			vertices[vi].UV = InMesh.GetUVs()[vi];
		}
	}

	// 정점 변환 진행
	VertexShader3D(vertices, InMatrix);

	// 삼각형 별로 그리기
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
		std::vector<Vertex3D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };

		size_t triangles = tvs.size() / 3;
		for (size_t ti = 0; ti < triangles; ++ti)
		{
			size_t si = ti * 3;
			std::vector<Vertex3D> sub(tvs.begin() + si, tvs.begin() + si + 3);
			DrawTriangle3D(sub, InColor, FillMode::Color);
		}
	}
}

void SoftRenderer::DrawTriangle3D(std::vector<Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
	auto& r = GetRenderer();
	const GameEngine& g = Get3DGameEngine();

	LinearColor finalColor = _WireframeColor;
	if (InColor != LinearColor::Error)
	{
		finalColor = InColor;
	}

	r.DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
	r.DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
	r.DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
}

