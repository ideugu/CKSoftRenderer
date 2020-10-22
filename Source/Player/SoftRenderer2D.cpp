
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

#include <random>

// �ǽ��� ���� ����
static std::random_device rd;
static std::mt19937 mt(rd());
float currentDegree = 0.f;
Vector2 lineStart(-400.f, 10.f);
Vector2 lineEnd(400.f, -10.f);

// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// ���� �������� ����ϴ� ����
	static float duration = 6.f;
	static float elapsedTime = 0.f;
	static float rotateSpeed = 180.f;

	elapsedTime = Math::Clamp(elapsedTime + InDeltaSeconds, 0.f, duration);
	currentDegree = Math::FMod(currentDegree + rotateSpeed * InDeltaSeconds, 360.f);
	if (elapsedTime == duration)
	{
		std::uniform_real_distribution<float>  randomY(-200.f, 200.f);
		lineStart = Vector2(-400.f, randomY(mt));
		lineEnd = Vector2(400.f, randomY(mt));
		elapsedTime = 0.f;
		return;
	}
}

// ������ ����
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ��
	static std::vector<Vector2> point;
	if (point.empty())
	{
		float radius = 5.f;
		for (float x = -radius; x <= radius; ++x)
		{
			for (float y = -radius; y <= radius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = radius * radius;
				if (sizeSquared < rr)
				{
					point.push_back(target);
				}
			}
		}
	}

	// �� �׸���
	float sin = 0.f, cos = 0.f;
	static float pointOrbit = 250.f;
	Math::GetSinCos(sin, cos, currentDegree);
	Vector2 pointPos = Vector2(cos, sin) * pointOrbit;
	for (auto const& v : point)
	{
		r.DrawPoint(v + pointPos, LinearColor::Red);
	}

	// ������ ���� �׸���
	r.DrawLine(lineStart, lineEnd, LinearColor::Black);

	// ������ ��ġ �׸���
	Vector2 hatV = (lineEnd - lineStart).Normalize();
	Vector2 u = pointPos - lineStart;
	Vector2 projV = hatV * (u.Dot(hatV));
	Vector2 projectedPos = lineStart + projV;
	float distance = (projectedPos - pointPos).Size();
	for (auto const& v : point)
	{
		r.DrawPoint(v + projectedPos, LinearColor::Magenta);
	}

	// ���� ���� �׸���
	r.DrawLine(projectedPos, pointPos, LinearColor::Gray);

	// ���� ������ ȭ�� ���
	r.PushStatisticText(std::string("Point : ") + pointPos.ToString());
	r.PushStatisticText(std::string("Projection : ") + projectedPos.ToString());
	r.PushStatisticText(std::string("Distance : ") + std::to_string(distance));
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
