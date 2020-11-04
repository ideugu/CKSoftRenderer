
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DDD;

Vector3 n;
Vector3 right, forward;
float thetaDegree = 0.f;

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

	// 회전 축 그리기
	static float axisLength = 150.f;
	static float planeLength = 30.f;

	Vector2 axisTo = (viewMatRotationOnly * n).ToVector2() * axisLength;
	Vector2 axisFrom = -axisTo;
	Vector2 rightTo = (viewMatRotationOnly * right).ToVector2() * planeLength;
	Vector2 rightFrom = -rightTo;
	Vector2 forwardTo = (viewMatRotationOnly * forward).ToVector2() * planeLength;
	Vector2 forwardFrom = -forwardTo;

	r.DrawLine(axisFrom, axisTo, LinearColor::Red);
	r.DrawLine(rightFrom, rightTo, LinearColor::DimGray);
	r.DrawLine(forwardFrom, forwardTo, LinearColor::DimGray);
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
	static float fovSpeed = 100.f;

	// 게임 로직에서 사용할 게임 오브젝트 레퍼런스
	GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);
	TransformComponent& playerTransform = goPlayer.GetTransform();
	Vector3 inputVector = Vector3(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis), input.GetAxis(InputAxis::ZAxis));
	playerTransform.AddPosition(inputVector * moveSpeed * InDeltaSeconds);

	// 카메라는 항상 플레이어를 바라보기
	CameraObject& camera = g.GetMainCamera();
	camera.SetLookAtRotation(playerTransform.GetPosition());
	float deltaFOV = input.GetAxis(InputAxis::WAxis)* moveSpeed* InDeltaSeconds;
	camera.SetFOV(Math::Clamp(camera.GetFOV() + deltaFOV, 15.f, 150.f));
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
	auto& r = GetRenderer();

	const CameraObject& mainCamera = g.GetMainCamera();
	const Matrix4x4 vMatrix = mainCamera.GetViewMatrix();
	const Matrix4x4 pMatrix = mainCamera.GetPerspectiveMatrix();
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
		Matrix4x4 finalMatrix = pMatrix * vMatrix * transform.GetModelingMatrix();
		DrawMesh3D(mesh, finalMatrix, gameObject.GetColor());
	}
	
	r.PushStatisticText("FOV : " + std::to_string(mainCamera.GetFOV()));
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

	// 클립 좌표를 NDC 좌표로 변경
	for (auto& v : InVertices)
	{
		// 무한 원점인 경우, 약간 보정해준다.
		if (v.Position.Z == 0.f) v.Position.Z = SMALL_NUMBER;

		float invZ = 1.f / v.Position.Z;
		v.Position.X *= invZ;
		v.Position.Y *= invZ;
		v.Position.Z *= invZ;
	}

	// 백페이스 컬링 ( 뒷면이면 그리기 생략 )
	Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
	Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
	Vector3 faceNormal = edge1.Cross(edge2);
	Vector3 viewDirection = -Vector3::UnitZ;
	if (faceNormal.Dot(viewDirection) >= 0.f)
	{
		return;
	}

	// NDC 좌표를 화면 좌표로 늘리기
	for (auto& v : InVertices)
	{
		v.Position.X *= _ScreenSize.X * 0.5f;
		v.Position.Y *= _ScreenSize.Y * 0.5f;
	}

	LinearColor finalColor = _WireframeColor;
	if (InColor != LinearColor::Error)
	{
		finalColor = InColor;
	}

	r.DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
	r.DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
	r.DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
}

