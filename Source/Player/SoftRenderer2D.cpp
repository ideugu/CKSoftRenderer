
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

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직에만 사용하는 변수
	static float duration = 3.f;
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

	// 격자 그리기
	DrawGrid2D();

	// 렌더링 변수
	static Vector3 sp(-600.f, 0.f, 1.f);
	static Vector3 ep(600.f, 0.f, 1.f);

	// 아핀 변환 행렬 ( 회전 ) 
	float sin, cos;
	Math::GetSinCos(sin, cos, currentDegree);
	Vector3 rBasis1(cos, sin, 0.f);
	Vector3 rBasis2(-sin, cos, 0.f);
	Vector3 rBasis3 = Vector3::UnitZ;
	Matrix3x3 rMatrix(rBasis1, rBasis2, rBasis3);

	Vector2 s = (rMatrix * sp).ToVector2();
	Vector2 e = (rMatrix * ep).ToVector2();
	HSVColor hsv(0.f, 1.f, 0.85f); // 잘 보이도록 채도를 조금만 줄였음. 
	hsv.H = currentDegree / 360.f;
	r.DrawLine(s, e, hsv.ToLinearColor());

	r.PushStatisticText(std::string("Rotation : ") + std::to_string(currentDegree));
}

void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
