
#include "Precompiled.h"
using namespace CK::DDD;

#include "PMXReader/EncodingHelper.h"
#include "PMXReader/Pmx.h"

bool PMXLoader::LoadPMX(GameEngine& InGameEngine, Mesh& InMesh, std::wstring InFileName)
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
	auto& cb = InMesh.GetConnectedBones();
	auto& w = InMesh.GetWeights();

	std::vector<std::wstring> boneBuffer;
	for (size_t ix = 0; ix < x.bone_count; ++ix)
	{
		auto& boneData = x.bones[ix];
		std::wstring boneName = x.bones[ix].bone_name;		
		Vector3 bonePosition = Vector3(x.bones[ix].position[0], x.bones[ix].position[1], x.bones[ix].position[2]);
		bones.insert({ boneName, Bone(boneName, Transform(bonePosition)) });

		boneBuffer.push_back(boneName);
		size_t index = static_cast<size_t>(x.bones[ix].parent_index);
		if (boneBuffer.size() <= index)
		{
			// 부모 인덱스 정보가 잘못됨
			continue;
		}
		std::wstring parentBoneName = boneBuffer[index];
		Bone& b = InMesh.GetBone(boneName);
		b.SetParent(InMesh.GetBone(parentBoneName));
		b.SetProperty(boneData.bone_flag);
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

	for (size_t ix = 0; ix < x.vertex_count; ++ix)
	{
		//x.vertices[ix].skinning;
		pmx::PmxVertexSkinningType type = x.vertices[ix].skinning_type;
		switch (type)
		{
		case pmx::PmxVertexSkinningType::BDEF1:
		{
			cb.push_back(1);
			pmx::PmxVertexSkinningBDEF1* s = static_cast<pmx::PmxVertexSkinningBDEF1*>(x.vertices[ix].skinning.get());
			std::wstring boneKey = boneBuffer[s->bone_index];
			Weight newWeight;
			newWeight.Bones.emplace_back(boneKey);
			newWeight.Values.emplace_back(1.f);
			w.emplace_back(newWeight);
			break;
		}
		case pmx::PmxVertexSkinningType::BDEF2:
		{
			cb.push_back(0);
			Weight newWeight;
			w.emplace_back(newWeight);
			//cb.push_back(2);
			//pmx::PmxVertexSkinningBDEF2* s = static_cast<pmx::PmxVertexSkinningBDEF2*>(x.vertices[ix].skinning.get());
			//std::wstring boneKey1 = boneBuffer[s->bone_index1];
			//std::wstring boneKey2 = boneBuffer[s->bone_index2];
			//float boneWeight1 = s->bone_weight;
			//Weight newWeight;
			//newWeight.Bones.emplace_back(boneKey1);
			//newWeight.Values.emplace_back(boneWeight1);
			//newWeight.Bones.emplace_back(boneKey2);
			//newWeight.Values.emplace_back(1.f - boneWeight1);
			//w.emplace_back(newWeight);
			break;
		}
		case pmx::PmxVertexSkinningType::BDEF4:
		{
			cb.push_back(0);
			Weight newWeight;
			w.emplace_back(newWeight);
			//pmx::PmxVertexSkinningBDEF4* s = static_cast<pmx::PmxVertexSkinningBDEF4*>(x.vertices[ix].skinning.get());
			//std::wstring boneKey1 = boneBuffer[s->bone_index1];
			//std::wstring boneKey2 = boneBuffer[s->bone_index2];
			//std::wstring boneKey3 = boneBuffer[s->bone_index3];
			//std::wstring boneKey4 = boneBuffer[s->bone_index4];
			//float boneWeight1 = s->bone_weight1;
			//float boneWeight2 = s->bone_weight2;
			//float boneWeight3 = s->bone_weight3;
			//float boneWeight4 = s->bone_weight4;
			//Weight newWeight;
			//newWeight.Bones.emplace_back(boneKey1);
			//newWeight.Values.emplace_back(boneWeight1);
			//newWeight.Bones.emplace_back(boneKey2);
			//newWeight.Values.emplace_back(boneWeight2);
			//newWeight.Bones.emplace_back(boneKey3);
			//newWeight.Values.emplace_back(boneWeight3);
			//newWeight.Bones.emplace_back(boneKey4);
			//newWeight.Values.emplace_back(boneWeight4);
			//w.emplace_back(newWeight);
			break;
		}
		default:
		{
			cb.push_back(0);
			Weight newWeight;
			w.emplace_back(newWeight);
			break;
		}
		}
		//w.push_back(Weight());
		//.emplace_back(Vector2(x.vertices[ix].uv[0], 1.f - x.vertices[ix].uv[1]));
	}


	size_t textureId = 0;
	for (size_t ix = 0; ix < x.texture_count; ++ix)
	{
		std::wstring path = x.textures[ix];
		path = L".\\" + path;
		Texture& texture = InGameEngine.CreateTexture(textureId++, path);
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
