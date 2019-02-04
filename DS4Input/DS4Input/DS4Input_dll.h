#pragma once
#include "DS4Manager.h"
#define EXPORT_API __declspec(dllexport)

extern "C" 
{
	//�Ǘ��X�^�[�g
	EXPORT_API bool StartDS4();

	//�R���g���[���̎擾
	EXPORT_API bool GetController();

	//�R���g���[���̃`�F�b�N
	EXPORT_API bool ControllerChack(int id);

	//�F�̕ύX
	EXPORT_API bool ChangeColor(int id, const UCHAR r, const UCHAR g, const UCHAR b);

	//�U���̕ύX
	EXPORT_API void ChangeVibration(int id, UCHAR right, UCHAR left);

	//�A�E�g�v�b�g���|�[�g�̑��M
	EXPORT_API void SendOutput(int id);

	//���|�[�g�̍X�V
	EXPORT_API bool UpdateInputReport();

	//�{�^���̎擾
	EXPORT_API bool GetButton(int id, DS4KeyType keyType);

	//�{�^���_�E���̎擾
	EXPORT_API bool GetButtonDown(int id, DS4KeyType keyType);

	//�{�^���A�b�v�̎擾
	EXPORT_API bool GetButtonUp(int id, DS4KeyType keyType);

	//���̎擾
	EXPORT_API float GetAxis(int id, DS4AxisType keyType);

	//�Ǘ��I��
	EXPORT_API bool EndDS4();
}
