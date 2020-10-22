
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

// 실습을 위한 변수
float currentDegree = 0.f;
Vector2 spherePosition = Vector2::Zero;

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직에만 사용하는 변수
	static float duration = 20.f;
	static float elapsedTime = 0.f;

	// 경과 시간에 따른 현재 각과 이를 사용한 [0,1]값의 생성
	elapsedTime += InDeltaSeconds;
	elapsedTime = Math::FMod(elapsedTime, duration);
	float currentRad = (elapsedTime / duration) * Math::TwoPI;
	float alpha = (sinf(currentRad) + 1) * 0.5f;

	// [0,1]을 활용해 주기적으로 크기를 반복하기
	currentDegree = Math::Lerp(0.f, 360.f, alpha);
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 조명 구체
	static std::vector<Vector2> light;
	if (light.empty())
	{
		float lightRadius = 10.f;
		for (float x = -lightRadius; x <= lightRadius; ++x)
		{
			for (float y = -lightRadius; y <= lightRadius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = lightRadius * lightRadius;
				if (sizeSquared < rr)
				{
					light.push_back(target);
				}
			}
		}
	}

	// 조명을 적용할 구체
	static std::vector<Vector2> sphere;
	static float sphereRadius = 50.f;
	if (sphere.empty())
	{
		for (float x = -sphereRadius; x <= sphereRadius; ++x)
		{
			for (float y = -sphereRadius; y <= sphereRadius; ++y)
			{
				Vector2 target(x, y);
				float sizeSquared = target.SizeSquared();
				float rr = sphereRadius * sphereRadius;
				if (sizeSquared < rr)
				{
					sphere.push_back(target);
				}
			}
		}
	}

	// 조명의 좌표 지정
	float sin = 0.f, cos = 0.f;
	static float lightLength = 200.f;
	Math::GetSinCos(sin, cos, currentDegree);
	Vector2 lightPosition = Vector2(cos, sin) * lightLength;
	static HSVColor hsv(0.f, 1.f, 1.f);

	// 각에 따른 색상의 변화
	hsv.H = Math::Deg2Rad(currentDegree) * Math::InvPI * 0.5f;

	// 조명 그리기
	LinearColor lightColor = hsv.ToLinearColor();
	r.DrawLine(lightPosition, lightPosition - lightPosition.Normalize() * 50.f, lightColor);
	for (auto const& v : light)
	{
		r.DrawPoint(v + lightPosition, lightColor);
	}

	// 원을 구성하는 모든 픽셀에 NdotL을 계산해 음영을 산출하고 이를 최종 색상에 반영
	for (auto const& v : sphere)
	{
		Vector2 n = (v - spherePosition).Normalize();
		Vector2 l = (lightPosition - v).Normalize();
		float shading = Math::Clamp(n.Dot(l), 0.f, 1.f);
		r.DrawPoint(v, lightColor * shading);
	}

	// 현재 조명의 각 출력
	r.PushStatisticText(std::string("Light Degree : ") + std::to_string(currentDegree));
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
