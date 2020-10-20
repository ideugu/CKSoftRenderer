
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
Vector2 deltaPosition;
float deltaDegree = 0.f;
float currentScale = 10.f;

// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직에만 사용하는 변수
	static float moveSpeed = 100.f;
	static float rotateSpeed = 180.f;

	// 입력 값으로 데이터 변경
	deltaPosition = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)) * moveSpeed * InDeltaSeconds;
	deltaDegree = input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 격자 그리기
	DrawGrid2D();

	// 렌더링 관련 변수
	static Vector2 currentPosition;
	static float currentDegree = 0.f;
	currentPosition += deltaPosition;
	currentDegree += deltaDegree;

	// 하트를 구성하는 점 생성
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

	// 각 값을 초기화한 후 동일하게 증가시키면서 색상 값을 지정
	rad = 0.f;
	HSVColor hsv(0.f, 1.f, 0.85f);  
	for (auto const& v : hearts)
	{
		float sin, cos;
		Math::GetSinCos(sin, cos, currentDegree);
		float halfScreenX = _ScreenSize.X * 0.5f;
		float halfScreenY = _ScreenSize.Y * 0.5f;
		Vector2 target = v * currentScale;

		// 화면의 중심을 기준으로 현재 벡터를 정규화
		Vector2 ndc = Vector2(target.X / halfScreenX, target.Y / halfScreenY);
		float len = Vector2(ndc.X * _ScreenSize.Y / _ScreenSize.X, ndc.Y).Size();

		// 극좌표계로 변경한 후 원점에서 멀어질 수록 더욱 회전 가중치를 부여.
		Vector2 polarNdc = ndc.ToPolarCoordinate();
		float w = Math::Lerp(0.f, 5.f, len);
		polarNdc.Y += Math::Deg2Rad(currentDegree) * w;

		// 변환된 결과를 다시 데카르트 좌표계로 변환
		target = polarNdc.ToCartesianCoordinate();
		target = Vector2(target.X * halfScreenX, target.Y * halfScreenY);

		hsv.H = rad / Math::TwoPI;
		r.DrawPoint(target + currentPosition, hsv.ToLinearColor());
		rad += increment;
	}

	// 현재 위치, 스케일, 회전각을 화면에 출력
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
