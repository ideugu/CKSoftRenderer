#pragma once

namespace CK
{
namespace DDD
{

class Bone
{
public:
	Bone() = default;
	Bone(const std::wstring& InName, const Transform& InTransform) : _Name(InName), _ParentName(InName)
	{
		_Property = 27;
		_Hash = std::hash<std::wstring>()(_Name);
		_BindPose = InTransform;
		_Transform.SetLocalTransform(InTransform);
	}

public:
	// Ʈ������
	TransformComponent& GetTransform() { return _Transform; }
	const TransformComponent& GetTransform() const { return _Transform; }
	const Transform& GetBindPose() const { return _BindPose; }
	void SetParent(Bone& InBone) 
	{
		_ParentName = InBone.GetName();
		_Transform.SetParent(InBone.GetTransform());
	}

	// Ű ����
	const std::wstring& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }
	bool HasParent() const { return _Name.compare(_ParentName) != 0; }
	const std::wstring& GetParentName() const { return _ParentName; }

	// �Ӽ� ����
	size_t GetProperty() const { return _Property; }
	void SetProperty(size_t InProperty) { _Property = InProperty; }

private:
	std::size_t _Hash = 0;
	std::wstring _Name;

	// ���� Ʈ������ ����
	TransformComponent _Transform;

	// ���� Ʈ������ ����
	Transform _BindPose;

	// �θ� ����
	std::wstring _ParentName;

	// �� �Ӽ�
	// 0x02 : Rotation , 0x04 : Movement , 0x08 : VIS , 0x10 : Enable , 0x20 : IK 
	std::size_t _Property = 27;
};

}
}