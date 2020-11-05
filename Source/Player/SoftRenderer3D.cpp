
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
	Matrix4x4 pvMatrix = g.GetMainCamera().GetPerspectiveViewMatrix();
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
	const Matrix4x4 pvMatrix = mainCamera.GetPerspectiveViewMatrix();
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
		Matrix4x4 finalMatrix = pvMatrix * transform.GetModelingMatrix();
		DrawMesh3D(mesh, finalMatrix, gameObject.GetColor());

		if (gameObject == GameEngine::PlayerGo)
		{
			// 플레이어 관련 깊이와 카메라로부터의 거리
			Vector4 clippedPos = pvMatrix * Vector4(transform.GetPosition());
			float cameraDepth = clippedPos.W;
			if (cameraDepth == 0) cameraDepth = SMALL_NUMBER;
			float ndcZ = clippedPos.Z / cameraDepth;

			r.PushStatisticText("Player: " + transform.GetPosition().ToString());
			r.PushStatisticText("Depth: " + std::to_string(ndcZ));
			r.PushStatisticText("Distance: " + std::to_string(clippedPos.W));
		}
	}
	
	r.PushStatisticText("Camera: " + mainCamera.GetTransform().GetPosition().ToString());
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

	for (auto& v : InVertices)
	{
		// 무한 원점인 경우, 약간 보정해준다.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
	}

	// 백페이스 컬링 ( 뒷면이면 그리기 생략 )
	Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
	Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
	// 왼손 좌표계를 사용하므로 반대 방향으로 설정
	Vector3 faceNormal = -edge1.Cross(edge2);
	Vector3 viewDirection = Vector3::UnitZ;
	if (faceNormal.Dot(viewDirection) >= 0.f)
	{
		return;
	}

	if (IsWireframeDrawing())
	{
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
	else
	{
		const Texture& mainTexture = g.GetTexture(GameEngine::DiffuseTexture);

		// 삼각형 칠하기
		// 삼각형의 영역 설정
		Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
		Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

		// 무게중심좌표를 위해 점을 벡터로 변환
		Vector2 u = InVertices[1].Position.ToVector2() - InVertices[0].Position.ToVector2();
		Vector2 v = InVertices[2].Position.ToVector2() - InVertices[0].Position.ToVector2();

		// 공통 분모 값 ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;

		// 퇴화 삼각형 판정.
		if (Math::EqualsInTolerance(denominator, 0.f))
		{
			return;
		}

		float invDenominator = 1.f / denominator;

		// 화면상의 점 구하기
		minPos.X *= _ScreenSize.X * 0.5f;
		minPos.Y *= _ScreenSize.Y * 0.5f;
		maxPos.X *= _ScreenSize.X * 0.5f;
		maxPos.Y *= _ScreenSize.Y * 0.5f;

		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// 삼각형 영역 내 모든 점을 점검하고 색칠
		for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
		{
			for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
			{
				ScreenPoint fragment = ScreenPoint(x, y);
				Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
				pointToTest.X *= (2.f / _ScreenSize.X);
				pointToTest.Y *= (2.f / _ScreenSize.Y);
				Vector2 w = pointToTest - InVertices[0].Position.ToVector2();
				float wdotu = w.Dot(u);
				float wdotv = w.Dot(v);

				float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
				float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
				float oneMinusST = 1.f - s - t;
				if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
				{
					Vector2 targetUV = InVertices[0].UV * oneMinusST + InVertices[1].UV * s + InVertices[2].UV * t;
					r.DrawPoint(fragment, FragmentShader3D(mainTexture.GetSample(targetUV), LinearColor::White));
				}
			}
		}
	}
}

