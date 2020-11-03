
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DDD;

// �׸��� �׸���
void SoftRenderer::DrawGizmo3D()
{
	auto& r = GetRenderer();
	const GameEngine& g = Get3DGameEngine();

	// �� ����� �׸���
	std::vector<Vertex3D> viewGizmo = { 
		Vertex3D(Vector4(Vector3::Zero)),
		Vertex3D(Vector4(Vector3::UnitX * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitY * _GizmoUnitLength)),
		Vertex3D(Vector4(Vector3::UnitZ * _GizmoUnitLength)),
	};

	Matrix4x4 viewMatRotationOnly = g.GetMainCamera().GetViewMatrixRotationOnly();
	Matrix4x4 pvMatrix = g.GetMainCamera().GetPerspectiveViewMatrix();
	VertexShader3D(viewGizmo, viewMatRotationOnly);

	// �� �׸���
	Vector2 v0 = viewGizmo[0].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v1 = viewGizmo[1].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v2 = viewGizmo[2].Position.ToVector2() + _GizmoPositionOffset;
	Vector2 v3 = viewGizmo[3].Position.ToVector2() + _GizmoPositionOffset;
	r.DrawLine(v0, v1, LinearColor::Red);
	r.DrawLine(v0, v2, LinearColor::Green);
	r.DrawLine(v0, v3, LinearColor::Blue);

}

bool useLinearVisualization = false;

// ���� ����
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	// �⺻ ���۷���
	GameEngine& g = Get3DGameEngine();
	const InputManager& input = g.GetInputManager();

	// �⺻ ���� ����
	static float moveSpeed = 500.f;
	static float rotateSpeed = 180.f;
	static float fovSpeed = 100.f;

	// ���� �������� ����� ���� ������Ʈ ���۷���
	GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);
	TransformComponent& playerTransform = goPlayer.GetTransform();
	Vector3 inputVector = Vector3(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis), input.GetAxis(InputAxis::ZAxis));
	playerTransform.AddPosition(inputVector * moveSpeed * InDeltaSeconds);

	// ī�޶�� �׻� �÷��̾ �ٶ󺸱�
	CameraObject& camera = g.GetMainCamera();
	camera.SetLookAtRotation(playerTransform.GetPosition());
	float deltaFOV = input.GetAxis(InputAxis::WAxis)* moveSpeed* InDeltaSeconds;
	camera.SetFOV(Math::Clamp(camera.GetFOV() + deltaFOV, 15.f, 150.f));

	if (input.IsReleased(InputButton::Space))
	{
		useLinearVisualization = !useLinearVisualization;
	}
}

// ĳ���� �ִϸ��̼� ����
void SoftRenderer::LateUpdate3D(float InDeltaSeconds)
{
	return;
}

// ������ ����
void SoftRenderer::Render3D()
{
	const GameEngine& g = Get3DGameEngine();
	auto& r = GetRenderer();

	const CameraObject& mainCamera = g.GetMainCamera();
	const Matrix4x4 vMatrix = mainCamera.GetViewMatrix();
	const Matrix4x4 pMatrix = mainCamera.GetPerspectiveMatrix();
	const Matrix4x4 pvMatrix = mainCamera.GetPerspectiveViewMatrix();
	const ScreenPoint viewportSize = mainCamera.GetViewportSize();

	// ����� �׸���
	DrawGizmo3D();

	// ����ü ������ ���� ���� ����� ����
	Matrix4x4 ptMatrix = pMatrix.Tranpose();

	// ����ü �ø� �׽�Ʈ�� ���� ��� ����
	size_t totalObjects = g.GetScene().size();
	size_t culledObjects = 0;
	size_t intersectedObjects = 0;
	size_t renderedObjects = 0;

	// ����ü�� �����ϴ� ����� ������
	std::array<Plane, 6> frustumPlanes = {
		Plane(-(ptMatrix[3] - ptMatrix[1])), // +Y
		Plane(-(ptMatrix[3] + ptMatrix[1])), // -Y
		Plane(-(ptMatrix[3] - ptMatrix[0])), // +X
		Plane(-(ptMatrix[3] + ptMatrix[0])), // -X
		Plane(-(ptMatrix[3] - ptMatrix[2])),  // +Z
		Plane(-(ptMatrix[3] + ptMatrix[2])), // -Z
	};

	// ����ü ����
	Frustum frustumFromMatrix(frustumPlanes);

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		const TransformComponent& transform = gameObject.GetTransform();
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
		LinearColor finalColor = gameObject.GetColor();

		// �ٿ�� ������ ũ�⸦ Ʈ�������� �°� ����
		Sphere sphereBound = mesh.GetSphereBound();
		sphereBound.Radius *= transform.GetScale().Max();
		sphereBound.Center = (vMatrix * Vector4(transform.GetPosition())).ToVector3();

		auto checkResult = frustumFromMatrix.CheckBound(sphereBound);
		if (checkResult == BoundCheckResult::Outside)
		{
			culledObjects++;
			continue;
		}
		else if (checkResult == BoundCheckResult::Intersect)
		{
			// ��ģ ���� ������Ʈ�� ��迡 ����
			intersectedObjects++;
			finalColor = LinearColor::Red;
		}

		// ���� ��ȯ ���
		Matrix4x4 finalMatrix = pvMatrix * transform.GetModelingMatrix();
		DrawMesh3D(mesh, finalMatrix, finalColor);

		// �׸� ��ü�� ��迡 ����
		renderedObjects++;
	}
	
	r.PushStatisticText("Total GameObjects : " + std::to_string(totalObjects));
	r.PushStatisticText("Culled GameObjects : " + std::to_string(culledObjects));
	r.PushStatisticText("Intersected GameObjects : " + std::to_string(intersectedObjects));
	r.PushStatisticText("Rendered GameObjects : " + std::to_string(renderedObjects));
}

void SoftRenderer::DrawMesh3D(const Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor)
{
	size_t vertexCount = InMesh.GetVertices().size();
	size_t indexCount = InMesh.GetIndices().size();
	size_t triangleCount = indexCount / 3;

	// �������� ����� ���� ���ۿ� �ε��� ���۷� ��ȯ
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

	// ���� ��ȯ ����
	VertexShader3D(vertices, InMatrix);

	// �ﰢ�� ���� �׸���
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
		// ���� ������ ���, �ణ �������ش�.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
	}

	// �����̽� �ø� ( �޸��̸� �׸��� ���� )
	Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
	Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
	Vector3 faceNormal = edge1.Cross(edge2);
	Vector3 viewDirection = -Vector3::UnitZ;
	if (faceNormal.Dot(viewDirection) <= 0.f)
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

		// �ﰢ�� ĥ�ϱ�
		// �ﰢ���� ���� ����
		Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
		Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

		// �����߽���ǥ�� ���� ���� ���ͷ� ��ȯ
		Vector2 u = InVertices[1].Position.ToVector2() - InVertices[0].Position.ToVector2();
		Vector2 v = InVertices[2].Position.ToVector2() - InVertices[0].Position.ToVector2();

		// ���� �и� �� ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;

		// ��ȭ �ﰢ�� ����.
		if (Math::EqualsInTolerance(denominator, 0.f))
		{
			return;
		}

		float invDenominator = 1.f / denominator;

		// ȭ����� �� ���ϱ�
		minPos.X *= _ScreenSize.X * 0.5f;
		minPos.Y *= _ScreenSize.Y * 0.5f;
		maxPos.X *= _ScreenSize.X * 0.5f;
		maxPos.Y *= _ScreenSize.Y * 0.5f;

		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// �� ���� ȭ�� ���� ����� ��� Ŭ���� ó��
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// �ﰢ�� ���� �� ��� ���� �����ϰ� ��ĥ
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
					// �� ������ ������ �� ������ z��
					float invZ0 = 1.f / InVertices[0].Position.W;
					float invZ1 = 1.f / InVertices[1].Position.W;
					float invZ2 = 1.f / InVertices[2].Position.W;

					// ���� ���������� ����� ���� �и�
					float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
					float invZ = 1.f / z;

					// ���� ���� �׽���
					float newDepth = (InVertices[0].Position.Z * oneMinusST * invZ0 + InVertices[1].Position.Z * s * invZ1 + InVertices[2].Position.Z * t * invZ2) * invZ;
					float prevDepth = r.GetDepthBufferValue(fragment);
					if (newDepth < prevDepth)
					{
						// �ȼ��� ó���ϱ� �� ���� ���� ���ۿ� ����
						r.SetDepthBufferValue(fragment, newDepth);
					}
					else
					{
						// �̹� �տ� ���� �׷��������Ƿ� �ȼ��׸���� ����
						continue;
					}

					if (IsDepthBufferDrawing())
					{
						float grayScale = (newDepth + 1.f) * 0.5f;
						if (useLinearVisualization)
						{
							float n = g.GetMainCamera().GetNearZ();
							float f = g.GetMainCamera().GetFarZ();

							// �ð�ȭ�� ���� ����ȭ�� ��� ������ ��ȯ
							grayScale = (invZ - n) / (f - n);
						}

						// ���� ���� �׸���
						r.DrawPoint(fragment, LinearColor::White * grayScale);
					}
					else
					{
						// ���� ���������� UV ��ǥ
						Vector2 targetUV = (InVertices[0].UV * oneMinusST * invZ0 + InVertices[1].UV * s * invZ1 + InVertices[2].UV * t * invZ2) * invZ;
						r.DrawPoint(fragment, FragmentShader3D(mainTexture.GetSample(targetUV), InColor));
					}
				}
			}
		}
	}
}

