
#include "Precompiled.h"
#include "lodepng.h"

Texture::Texture(std::string InFileName)
{
	LoadPNG(InFileName);
}

void Texture::LoadPNG(std::string InFileName)
{
	std::vector<BYTE> image;
	unsigned w, h;
	unsigned int error = lodepng::decode(image, w, h, InFileName);
	if (error != NULL)
	{
		Release();
		return;
	}

	_Width = static_cast<UINT32>(w);
	_Height = static_cast<UINT32>(h);

	size_t bufferLength = image.size() / 4;
	_Buffer.reserve(bufferLength);
	for (UINT32 j = 0; j < _Height; j++)
	{
		for (UINT32 i = 0; i < _Width; i++)
		{
			auto ix = (j * _Width + i) * 4;
			Color32 c(image[ix], image[ix + 1], image[ix + 2], image[ix + 3]);
			_Buffer.push_back(LinearColor(c));
		}
	}
}

void Texture::Release()
{
	_Width = 0;
	_Height = 0;
	_Buffer.clear();
}

LinearColor Texture::GetSample(Vector2 InUV) const
{
	if (!IsIntialized())
	{
		return LinearColor::Error;
	}

	int x = Math::FloorToInt(InUV.X * _Width) % _Width;
	int y = Math::FloorToInt(InUV.Y * _Height) % _Height;
	size_t index = static_cast<size_t>(_Width * (_Height - (1 + y)) + x);
	if (index >= _Buffer.size())
	{
		return LinearColor::Error;
	}

	return _Buffer[index];
}