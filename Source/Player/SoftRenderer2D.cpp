
#include "Precompiled.h"
#include "SoftRenderer.h"
using namespace CK::DD;

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
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

	// 월드의 원점
	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

#include <random>

// 실습을 위한 변수
static std::random_device rd;
static std::mt19937 mt(rd());
static float fovAngle = 60.f; // 시야각 60도
Vector2 playerStart(100.f, 100.f);
Vector2 playerDestination(-100.f, 20.f);
Vector2 playerPosition = playerStart;
Vector2 guardPosition = Vector2::Zero;
LinearColor playerColor = LinearColor::Gray;
LinearColor guardColor = LinearColor::Blue;

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	static float duration = 3.f;
	static float elapsedTime = 0.f;

	// 보초의 시야 벡터 ( Y축 )
	static Vector2 f = Vector2::UnitY;
	// 보초 시야각의 절반 코사인 값
	static float cos = cosf(Math::Deg2Rad(fovAngle) * 0.5f);

	elapsedTime = Math::Clamp(elapsedTime + InDeltaSeconds, 0.f, duration);

	// 지정한 시간이 경과하면 새로운 목표를 랜덤으로 설정
	if (elapsedTime == duration)
	{
		playerStart = playerDestination;
		playerPosition = playerStart;

		std::uniform_real_distribution<float>  randomPosX(-200.f, 200.f);
		std::uniform_real_distribution<float>  randomPosY(20.f, 200.f);
		playerDestination = Vector2(randomPosX(mt), randomPosY(mt));

		elapsedTime = 0.f;
	}
	else // 그렇지 않으면 시간 비율에 따라 선형보간하면서 이동
	{
		float ratio = elapsedTime / duration;
		playerPosition = Vector2(
			Math::Lerp(playerStart.X, playerDestination.X, ratio),
			Math::Lerp(playerStart.Y, playerDestination.Y, ratio)
		);
	}

	// 시야각 판별 진행
	Vector2 v = (playerPosition - guardPosition).Normalize(); // 보초에서 플레이어로 향하는 정규화된 벡터

	if (v.Dot(f) >= cos) // 시야에 감지되면 붉은 색으로 변경
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

// 렌더링 로직
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 격자 그리기
	DrawGrid2D();

	// 플레이어와 보초를 표현하기 위한 구체
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

	// 플레이어의 위치 출력
	r.PushStatisticText(std::string("Player Position : ") + playerPosition.ToString());
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
