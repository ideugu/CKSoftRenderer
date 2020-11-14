
#include "Precompiled.h"
using namespace CK::DDD;

#include "PMXReader/EncodingHelper.h"
#include "PMXReader/Pmx.h"

bool PMXLoader::LoadPMX(GameEngine& InGameEngine, Mesh& InMesh, std::string InFileName)
{
	oguna::EncodingConverter converter = oguna::EncodingConverter();

	pmx::PmxModel x;
	std::filebuf fb;
	if (!fb.open(InFileName, std::ios::in | std::ios::binary))
	{
		return false;
	}

	std::istream is(&fb);
	x.Read(&is);

	InMesh.SetMeshType(MeshType::Skinned);

	auto& v = InMesh.GetVertices();
	auto& i = InMesh.GetIndices();
	auto& uv = InMesh.GetUVs();
	auto& ti = InMesh.GetTextureIndices();
	auto& bones = InMesh.GetBones();


	std::vector<std::string> boneBuffer;
	for (size_t ix = 0; ix < x.bone_count; ++ix)
	{
		auto& boneData = x.bones[ix];
		std::wstring wbone = x.bones[ix].bone_name;		
		std::string boneName;
		converter.Utf16ToUtf8(wbone.data(), (int)wbone.length(), &boneName);
		UINT16 f = boneData.bone_flag;
		Vector3 bonePosition = Vector3(x.bones[ix].position[0], x.bones[ix].position[1], x.bones[ix].position[2]);
		bones.insert({ boneName, Bone(boneName, Transform(bonePosition)) });

		boneBuffer.push_back(boneName);
		size_t index = static_cast<size_t>(x.bones[ix].parent_index);
		if (boneBuffer.size() <= index)
		{
			// 부모 인덱스 정보가 잘못됨
			continue;
		}
		std::string parentBoneName = boneBuffer[index];
		InMesh.GetBone(boneName).SetParent(InMesh.GetBone(parentBoneName));
	}


	for (size_t ix = 0; ix < x.vertex_count; ++ix)
	{
		v.emplace_back(Vector3(x.vertices[ix].positon[0], x.vertices[ix].positon[1], x.vertices[ix].positon[2]));
	}

	for (size_t ix = 0; ix < x.index_count; ++ix)
	{
		i.emplace_back(x.indices[ix]);
	}

	for (size_t ix = 0; ix < x.vertex_count; ++ix)
	{
		uv.emplace_back(Vector2(x.vertices[ix].uv[0], 1.f - x.vertices[ix].uv[1]));
	}

	size_t textureId = 0;
	for (size_t ix = 0; ix < x.texture_count; ++ix)
	{
		std::wstring path = x.textures[ix];
		path = L".\\" + path;
		std::string texturePath;
		converter.Utf16ToUtf8(path.data(), (int)path.size(), &texturePath);

		Texture& texture = InGameEngine.CreateTexture(textureId++, texturePath);
		assert(texture.IsIntialized());
	}

	size_t cnt = 0;
	for (size_t ix = 0; ix < x.material_count; ++ix)
	{
		auto& m = x.materials[ix];

		size_t c = m.index_count;
		ti.emplace_back(TexturesIndice(m.diffuse_texture_index, cnt, cnt + c));
		cnt += c;
	}


	return true;
}
