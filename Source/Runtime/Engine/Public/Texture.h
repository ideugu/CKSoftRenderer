#pragma once

namespace CK
{

class Texture
{
public:
	Texture() = default;
	Texture(std::string InFileName);
	~Texture() {}

public:
	void LoadPNG(std::string InFileName);
	void Release();
	bool IsIntialized() const { return (_Buffer.size() > 0); }
	UINT32 GetWidth() const { return _Width; }
	UINT32 GetHeight() const { return _Height; }
	UINT32 GetSize() const { return _Width * _Height; }
	LinearColor GetSample(Vector2 InUV) const;

private:
	std::vector<LinearColor> _Buffer;
	UINT32 _Width = 0;
	UINT32 _Height = 0;
};

}