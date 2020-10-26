
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DDD;

Rotator axisRotator;
Vector3 rotationAxis;
Vector3 rotationOrthoAxis;
float rotationDegree = 0.f;

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

	// ȸ�� �� �׸���
	axisRotator.Clamp();
	float cy = 0.f, sy = 0.f, cp = 0.f, sp = 0.f, cr = 0.f, sr = 0.f;
	Math::GetSinCos(sy, cy, axisRotator.Yaw);
	Math::GetSinCos(sp, cp, axisRotator.Pitch);
	Math::GetSinCos(sr, cr, axisRotator.Roll);
	rotationAxis = Vector3(-cy * sr + sy * sp * cr, cp * cr, sy * sr + cy * sp * cr);
	rotationOrthoAxis = Vector3(cy * cr + sy * sp * sr, cp * sr, -sy * cr + cy * sp * sr);

	static float axisLength = 100.f;
	Vector2 axisTo = (viewMatRotationOnly * rotationAxis).ToVector2() * axisLength;
	Vector2 axisFrom = -axisTo;

	// ������ �� �׸���
	Vector2 orthoAxisTo = (viewMatRotationOnly * rotationOrthoAxis).ToVector2() * axisLength * 0.5f;
	Vector2 orthoAxisFrom = -orthoAxisTo;

	r.DrawLine(orthoAxisFrom, orthoAxisTo, LinearColor::Blue);
	r.DrawLine(axisFrom, axisTo, LinearColor::Red);
}

// ���� ����
void SoftRenderer::Update3D(float InDeltaSeconds)
{
	// �⺻ ���۷���
	GameEngine& g = Get3DGameEngine();
	const InputManager& input = g.GetInputManager();

	// �⺻ ���� ����
	static float rotateSpeed = 180.f;

	// �Է¿� ���� ȸ������ ����
	axisRotator.Yaw += -input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds;
	axisRotator.Pitch += -input.GetAxis(InputAxis::YAxis) * rotateSpeed * InDeltaSeconds;
	axisRotator.Roll += input.GetAxis(InputAxis::ZAxis) * rotateSpeed * InDeltaSeconds;
	rotationDegree = Math::FMod(rotationDegree + input.GetAxis(InputAxis::XAxis) * rotateSpeed * InDeltaSeconds, 360.f);
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

	const CameraObject& mainCamera = g.GetMainCamera();
	const Matrix4x4 vMatrix = mainCamera.GetViewMatrix();
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
		Matrix4x4 finalMatrix = vMatrix * transform.GetModelingMatrix();
		DrawMesh3D(mesh, vMatrix, gameObject.GetTransform().GetScale(), gameObject.GetColor());
	}
}

void SoftRenderer::DrawMesh3D(const Mesh& InMesh, const Matrix4x4& InMatrix, const Vector3& InScale, const LinearColor& InColor)
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
	for (Vertex3D& v : vertices)
	{
		float sin = 0.f, cos = 0.f;
		Math::GetSinCos(sin, cos, rotationDegree);
		Vector3 u = v.Position.ToVector3();
		float udotn = u.Dot(rotationAxis);
		Vector3 ncrossu = rotationAxis.Cross(u);
		Vector3 result = Vector3(u * cos + rotationAxis * ((1.f - cos) * udotn) + ncrossu * sin) * InScale;
		v.Position = InMatrix * Vector4(result);
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
			DrawTriangle3D(sub, InColor, FillMode::Color);
		}
	}
}

void SoftRenderer::DrawTriangle3D(std::vector<Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
	auto& r = GetRenderer();
	const GameEngine& g = Get3DGameEngine();

	// �����̽� �ø� ( �޸��̸� �׸��� ���� )
	Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
	Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
	float z = edge1.Cross(edge2).Z;
	if (z <= 0.f)
	{
		return;
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

