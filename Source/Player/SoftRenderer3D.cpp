
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

// ���� ����
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	// �⺻ ���۷���
	GameEngine& g = Get3DGameEngine();
	const InputManager& input = g.GetInputManager();

	// �⺻ ���� ����
	static float rotateSpeed = 180.f;

	// ���� �������� ����� ���� ������Ʈ ���۷���
	GameObject& goPlayer = g.GetGameObject(GameEngine::PlayerGo);
	Rotator r = goPlayer.GetTransform().GetRotation();
	if (input.IsPressed(InputButton::Space))
	{
		r.Pitch = -90.f;
	}
	else
	{
		r.Pitch += input.GetAxis(InputAxis::ZAxis)* rotateSpeed* InDeltaSeconds;
	}
	r.Roll += input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds;
	r.Yaw += input.GetAxis(InputAxis::YAxis) * rotateSpeed * InDeltaSeconds;
	goPlayer.GetTransform().SetRotation(r);
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
	const Matrix4x4 pvMatrix = mainCamera.GetPerspectiveViewMatrix();
	const ScreenPoint viewportSize = mainCamera.GetViewportSize();

	// ����� �׸���
	DrawGizmo3D();

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		const TransformComponent& transform = gameObject.GetTransform();
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		// ������ ����
		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());

		// ���� ��ȯ ���
		Matrix4x4 finalMatrix = pvMatrix * transform.GetModelingMatrix();
		DrawMesh3D(mesh, finalMatrix, gameObject.GetColor());

		if (gameObject == GameEngine::PlayerGo)
		{
			Rotator rotator = gameObject.GetTransform().GetRotation();
			r.PushStatisticText("Yaw : " + std::to_string(rotator.Yaw));
			r.PushStatisticText("Roll : " + std::to_string(rotator.Roll));
			r.PushStatisticText("Pitch : " + std::to_string(rotator.Pitch));
		}
	}
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

	// �׸����� ����
	FillMode fm = FillMode::None;
	if (InMesh.HasColor())
	{
		fm |= FillMode::Color;
	}
	if (InMesh.HasUV())
	{
		fm |= FillMode::Texture;
	}

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
			DrawTriangle3D(sub, InColor, fm);
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

					float newDepth = (InVertices[0].Position.Z * oneMinusST * invZ0 + InVertices[1].Position.Z * s * invZ1 + InVertices[2].Position.Z * t * invZ2) * invZ;
					float prevDepth = r.GetDepthBufferValue(fragment);
					if (newDepth < prevDepth)
					{
						r.SetDepthBufferValue(fragment, newDepth);
					}
					else
					{
						// �̹� �տ� ���� �׷��������Ƿ� �ȼ��׸���� ����
						continue;
					}

					// ���� �Ķ���Ͱ� ���� �ȵ� ��쿡�� ����� ���
					LinearColor paramColor = LinearColor::White;
					if (InColor != LinearColor::Error)
					{
						paramColor = InColor;
					}

					// ���ؽ� �÷� �Ǵ� �ؽ��� �������� ���� ������ ����
					LinearColor fragmentColor = LinearColor::White;
					if (InFillMode & FillMode::Color)
					{
						fragmentColor = (InVertices[0].Color * oneMinusST * invZ0 + InVertices[1].Color * s * invZ1 + InVertices[2].Color * t * invZ2) * invZ;
					}

					if (InFillMode & FillMode::Texture)
					{
						// ���������������� ������ �ش� �ȼ��� UV ��
						Vector2 targetUV = (InVertices[0].UV * oneMinusST * invZ0 + InVertices[1].UV * s * invZ1 + InVertices[2].UV * t * invZ2) * invZ;

						// �ؽ��� ���� ����
						LinearColor textureColor = g.GetTexture(GameEngine::DiffuseTexture).GetSample(targetUV);
						fragmentColor = fragmentColor * textureColor;
					}

					r.DrawPoint(fragment, FragmentShader3D(fragmentColor, paramColor));
				}
			}
		}
	}
}

