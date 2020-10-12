
#include "Precompiled.h"
#include "SoftRenderer.h"

SoftRenderer::SoftRenderer(GameEngineMode InGameEngineMode, RendererInterface* InRSI) : _GameEngineMode(InGameEngineMode), _RSIPtr(InRSI)
{
}

void SoftRenderer::OnTick()
{
	if (!_AllInitialized)
	{
		// �����ս� ī���� �ʱ�ȭ.
		if(_PerformanceInitFunc && _PerformanceMeasureFunc)
		{
			_CyclesPerMilliSeconds = _PerformanceInitFunc();
			_PerformanceCheckInitialized = true;
		}
		else
		{
			return;
		}

		// ��ũ�� ũ�� Ȯ��
		if (_ScreenSize.HasZero())
		{
			return;
		}

		// ����Ʈ ������ �ʱ�ȭ.
		if (!GetRenderer().Init(_ScreenSize))
		{
			return;
		}

		_RendererInitialized = true;

		// ���� ���� �ʱ�ȭ
		GetGameEngine().OnScreenResize(_ScreenSize);
		if (!GetGameEngine().Init())
		{
			return;
		}

		_GameEngineInitialized = true;

		_AllInitialized = _RendererInitialized && _PerformanceCheckInitialized && _GameEngineInitialized;
		if (_AllInitialized)
		{
			_TickEnabled = true;
		}
	}
	else
	{
		assert(_RSIPtr != nullptr && GetRenderer().IsInitialized() && !_ScreenSize.HasZero());

		if (_TickEnabled)
		{
			PreUpdate();

			if (_GameEngineMode == GameEngineMode::DD)
			{
				Update2D(_FrameTime / 1000.f);
				Render2D();
			}
			else
			{
				Update3D(_FrameTime / 1000.f);
				LateUpdate3D(_FrameTime / 1000.f);
				Render3D();
			}

			PostUpdate();
		}
	}
}

void SoftRenderer::OnResize(const ScreenPoint& InNewScreenSize)
{
	_ScreenSize = InNewScreenSize;

	// ũ�Ⱑ ����Ǹ� �������� ���� �ʱ�ȭ
	if (_RendererInitialized)
	{
		GetRenderer().Init(InNewScreenSize);
	}

	if (_GameEngineInitialized)
	{
		GetGameEngine().OnScreenResize(_ScreenSize);
	}	
}

void SoftRenderer::OnShutdown()
{
	GetRenderer().Shutdown();
}

void SoftRenderer::SetDefaultGameEngine(GameEngineMode InEngineMode)
{
}

void SoftRenderer::PreUpdate()
{
	// ���� ���� ����.
	_FrameTimeStamp = _PerformanceMeasureFunc();
	if (_FrameCount == 0)
	{
		_StartTimeStamp = _FrameTimeStamp;
	}

	// ��� �����.
	GetRenderer().Clear(_BackgroundColor);

	// ���� �ð�ȭ
	const InputManager& input = GetGameEngine().GetInputManager();

	if (input.IsReleased(InputButton::F1)) { _CurrentDrawMode = DrawMode::Normal; }
	if (input.IsReleased(InputButton::F2)) { _CurrentDrawMode = DrawMode::Wireframe; }
	if (input.IsReleased(InputButton::F3)) { _CurrentDrawMode = DrawMode::DepthBuffer; }
}

void SoftRenderer::PostUpdate()
{
	// ������ ������.
	GetRenderer().EndFrame();

	// �Է� ���� ������Ʈ
	GetGameEngine().GetInputManager().UpdateInput();

	// ���� ���� ������.
	_FrameCount++;
	INT64 currentTimeStamp = _PerformanceMeasureFunc();
	INT64 frameCycles = currentTimeStamp - _FrameTimeStamp;
	INT64 elapsedCycles = currentTimeStamp - _StartTimeStamp;
	_FrameTime = frameCycles / _CyclesPerMilliSeconds;
	_ElapsedTime = elapsedCycles / _CyclesPerMilliSeconds;
	_FrameFPS = _FrameTime == 0.f ? 0.f : 1000.f / _FrameTime;
	_AverageFPS = _ElapsedTime == 0.f ? 0.f : 1000.f / _ElapsedTime * _FrameCount;
}


