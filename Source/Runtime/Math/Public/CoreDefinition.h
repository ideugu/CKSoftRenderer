#pragma once

typedef unsigned char		BYTE;		// ��ȣ ���� 8��Ʈ
typedef unsigned int		UINT32;		// ��ȣ ���� 32��Ʈ
//typedef unsigned long long	UINT64;		// ��ȣ ���� 64��Ʈ.
typedef signed long long	INT64;	// ��ȣ �ִ� 64��Ʈ.

#define SMALL_NUMBER		(1.e-8f)
//#define KINDA_SMALL_NUMBER	(1.e-4f)

enum class BoundCheckResult : UINT32
{
	Outside = 0,
	Intersect,
	Inside
};