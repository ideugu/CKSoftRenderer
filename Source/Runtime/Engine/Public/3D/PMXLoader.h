#pragma once

namespace CK
{
namespace DDD
{

struct PMXLoader
{
	static bool LoadPMX(GameEngine& InGameEngine, Mesh& InMesh, std::wstring fileName);
};

}
}