
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// ���� �ϴܿ������� ���� �׸���
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	// ������ ����
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// �ǽ��� ���� ����
Vector2 deltaPosition;
float deltaDegree = 0.f;
float currentScale = 10.f;

// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// ���� �������� ����ϴ� ����
	static float moveSpeed = 100.f;
	static float rotateSpeed = 180.f;

	// �Է� ������ ������ ����
	deltaPosition = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)) * moveSpeed * InDeltaSeconds;
	deltaDegree = input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds;
}

// ������ ����
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ���� �׸���
	DrawGrid2D();

	// ������ ���� ����
	static Vector2 currentPosition;
	static float currentDegree;
	currentPosition += deltaPosition;
	currentDegree += deltaDegree;

	// ��Ʈ�� �����ϴ� �� ����
	static float increment = 0.001f;
	float rad = 0.f;
	static std::vector<Vector2> hearts;
	if (hearts.empty())
	{
		for (rad = 0.f; rad < Math::TwoPI; rad += increment)
		{
			float sin = sinf(rad);
			float cos = cosf(rad);
			float cos2 = cosf(2 * rad);
			float cos3 = cosf(3 * rad);
			float cos4 = cosf(4 * rad);
			float x = 16.f * sin * sin * sin;
			float y = 13 * cos - 5 * cos2 - 2 * cos3 - cos4;
			hearts.push_back(Vector2(x, y));
		}
	}

	// �� ���� �ʱ�ȭ�� �� �����ϰ� ������Ű�鼭 ���� ���� ����
	rad = 0.f;
	HSVColor hsv(0.f, 1.f, 0.85f);  
	for (auto const& v : hearts)
	{
		// ������ �ش��ϴ� ���ΰ� �ڻ��� �Լ� ���
		float sin = 0.f, cos = 0.f;
		Math::GetSinCos(sin, cos, currentDegree);

		// 1. ������ ũ�⸦ ����
		Vector2 target = v * currentScale;
		// 2. ������ ������ ȸ���� ������ ���
		Vector2 rotatedTarget = Vector2(target.X * cos - target.Y * sin, target.X * sin + target.Y * cos);
		// 3. �̵��� ��ġ�� ����
		rotatedTarget += currentPosition;

		hsv.H = rad / Math::TwoPI;
		r.DrawPoint(rotatedTarget, hsv.ToLinearColor());
		rad += increment;
	}

	// ���� ��ġ, ������, ȸ������ ȭ�鿡 ���
	r.PushStatisticText(std::string("Position : ") + currentPosition.ToString());
	r.PushStatisticText(std::string("Scale : ") + std::to_string(currentScale));
	r.PushStatisticText(std::string("Degree : ") + std::to_string(currentDegree));
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
