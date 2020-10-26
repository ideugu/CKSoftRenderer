
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
static float fovAngle = 60.f; // �þ߰� 60��
Vector2 playerStart(100.f, 100.f);
Vector2 playerDestination(-100.f, 20.f);
Vector2 playerPosition = playerStart;
Vector2 guardPosition = Vector2::Zero;
LinearColor playerColor = LinearColor::Gray;
LinearColor guardColor = LinearColor::Blue;

// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float duration = 3.f;
	static float elapsedTime = 0.f;

	// ������ �þ� ���� ( Y�� )
	static Vector2 f = Vector2::UnitY;
	// ���� �þ߰��� ���� �ڻ��� ��
	static float cos = cosf(Math::Deg2Rad(fovAngle) * 0.5f);

	elapsedTime = Math::Clamp(elapsedTime + InDeltaSeconds, 0.f, duration);

	// ������ �ð��� ����ϸ� ���ο� ��ǥ�� �������� ����
	if (elapsedTime == duration)
	{
		playerStart = playerDestination;
		playerPosition = playerStart;

		std::uniform_real_distribution<float>  randomPosX(-200.f, 200.f);
		std::uniform_real_distribution<float>  randomPosY(20.f, 200.f);
		playerDestination = Vector2(randomPosX(mt), randomPosY(mt));

		elapsedTime = 0.f;
	}
	else // �׷��� ������ �ð� ������ ���� ���������ϸ鼭 �̵�
	{
		float ratio = elapsedTime / duration;
		playerPosition = Vector2(
			Math::Lerp(playerStart.X, playerDestination.X, ratio),
			Math::Lerp(playerStart.Y, playerDestination.Y, ratio)
		);
	}

	// �þ߰� �Ǻ� ����
	Vector2 v = (playerPosition - guardPosition).Normalize(); // ���ʿ��� �÷��̾�� ���ϴ� ����ȭ�� ����

	if (v.Dot(f) >= cos) // �þ߿� �����Ǹ� ���� ������ ����
	{
		playerColor = LinearColor::Red;
		guardColor = LinearColor::Red;
	}
	else
	{
		playerColor = LinearColor::Gray;
		guardColor = LinearColor::Blue;
	}
}

// ������ ����
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// ���� �׸���
	DrawGrid2D();

	// �÷��̾�� ���ʸ� ǥ���ϱ� ���� ��ü
	static float radius = 10.f;
	static std::vector<Vector2> sphere;
	if (sphere.empty())
	{
		for (float x = -radius; x <= radius; ++x)
		{
			for (float y = -radius; y <= radius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = radius * radius;
				if (sizeSquared < rr)
				{
					sphere.push_back(target);
				}
			}
		}
	}

	static float length = 300.f;
	float halfAngle = Math::HalfPI - Math::Deg2Rad(fovAngle) * 0.5f;
	static float sin = sinf(halfAngle);
	static float cos = cosf(halfAngle);

	r.DrawLine(guardPosition, guardPosition + Vector2(length * cos, length * sin), guardColor);
	r.DrawLine(guardPosition, guardPosition + Vector2(-length * cos, length * sin), guardColor);
	r.DrawLine(guardPosition, Vector2(0.f, 50.f), LinearColor::Black);

	for (auto const& v : sphere)
	{
		r.DrawPoint(v + guardPosition, guardColor);
		r.DrawPoint(v + playerPosition, playerColor);
	}

	// �÷��̾��� ��ġ ���
	r.PushStatisticText(std::string("Player Position : ") + playerPosition.ToString());
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
