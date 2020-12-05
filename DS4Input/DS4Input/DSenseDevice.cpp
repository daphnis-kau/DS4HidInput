#include "DSenseDevice.h"
#include "DSKeyType.h"
#include "DSAxisType.h"

DSenseDevice::DSenseDevice(HidDevice device, int controllerId)
{
	this->device = device;
	this->controllerNum = controllerId;
	outputDataLength = device.GetCapabilities().OutputReportByteLength;
	inputDataLength = device.GetCapabilities().InputReportByteLength;

	for (int i = 0; i < inputDataLength; i++)
	{
		inputData[i] = 0;
	}
	for (int i = 0; i < outputDataLength; i++)
	{
		outputData[i] = 0x00;
	}

	outputData[0] = 0x2;//���|�[�gID
	outputData[1] = 0xff;//ff�Œ肩
	outputData[2] = 0x17;//���C�g�o�[ �v���C���[�����v�ݒ�  : 0x08 : �[�d 

	outputData[3] = 0x00;//R���[�^ 0�`FF
	outputData[4] = 0x00;//L���[�^ 0�`FF

	outputData[9] = 0x00;// �}�C�N�~���[�g���C�g 0 : ���� , 1 : �_��, 2 : �_��

	outputData[10] = 0x00;// LR�t���O

	outputData[11] = 0x0; // R�g���K�[ 1:��R 2:���b�N
	outputData[12] = 0x00; //R�g���K�[ ���bit�����b�N�̈ʒu�l���傫���Ȃ�ƈʒu�����ɂȂ�

	outputData[22] = 0x00; // L�g���K�[
	outputData[23] = 0x00; // L�g���K�[

	outputData[44] = 0x00; // �v���C���[���C�g1bit�Âœ_������A�ő�5bit

	outputData[45] = 0x00; // ��
	outputData[46] = 0x00; // ��
	outputData[47] = 0x00; // ��


	switch (controllerId)
	{
	case 0:
		ChangeLedColor(LED::Blue());
		ChangePlayerLight(4);
		break;
	case 1:
		ChangeLedColor(LED::Red());
		ChangePlayerLight(6);
		break;
	case 2:
		ChangeLedColor(LED::Green());
		ChangePlayerLight(14);
		break;
	case 3:
		ChangeLedColor(LED::Purple());
		ChangePlayerLight(0x1f);
		break;
	}

	SendOutputReport();
}

bool DSenseDevice::GetInputReport()
{
	DWORD sizet = 0;
	OVERLAPPED overlapped;
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	BOOL result = ReadFile(device.GetHandle(), inputData, inputDataLength, &sizet, &overlapped);
	device.isDevice = result;
	if (!result) return false;
	

#ifdef DEBUG
	for (int i = 0; i < 11; i++) {
		printf(" %d ", inputData[i]);
	}
	printf("\n");
#endif // DEBUG

	UCHAR data = inputData[8];
	status.square.ChangeStatus((data & 0x10) == 0x10 ? true : false);
	status.cross.ChangeStatus((data & 0x20) == 0x20 ? true : false);
	status.circle.ChangeStatus((data & 0x40) == 0x40 ? true : false);
	status.triangle.ChangeStatus((data & 0x80) == 0x80 ? true : false);
	data &= 0x0F;
	bool d[4] = { false, false, false, false };
	switch (data)
	{
	case 0:
		d[0] = true;
		break;
	case 1:
		d[0] = true;
		d[1] = true;
		break;
	case 2:
		d[1] = true;
		break;
	case 3:
		d[1] = true;
		d[2] = true;
		break;
	case 4:
		d[2] = true;
		break;
	case 5:
		d[2] = true;
		d[3] = true;
		break;
	case 6:
		d[3] = true;
		break;
	case 7:
		d[3] = true;
		d[0] = true;
		break;
	}
	status.up.ChangeStatus(d[0]);
	status.right.ChangeStatus(d[1]);
	status.down.ChangeStatus(d[2]);
	status.left.ChangeStatus(d[3]);

	data = inputData[9];
	status.l1.ChangeStatus((data & 0x01) == 0x01 ? true : false);
	status.r1.ChangeStatus((data & 0x02) == 0x02 ? true : false);
	status.share.ChangeStatus((data & 0x10) == 0x10 ? true : false);
	status.option.ChangeStatus((data & 0x20) == 0x20 ? true : false);
	status.l3.ChangeStatus((data & 0x40) == 0x40 ? true : false);
	status.r3.ChangeStatus((data & 0x80) == 0x80 ? true : false);
	data = inputData[10];
	status.home.ChangeStatus(data == 0x01 ? true : false);

	//���X�e�B�b�N�̓��͂̕ۑ�
	data = inputData[1];
	status.leftStickX = (data - 127) / 127.0f;
	data = inputData[2];
	status.leftStickY = (data - 127) / 127.0f;

	//�E�X�e�B�b�N�̓��͂̕ۑ�
	data = inputData[3];
	status.rightStickX = (data - 127) / 127.0f;
	data = inputData[4];
	status.rightStickY = (data - 127) / 127.0f;

	//L2��R2�̓��͂̕ۑ�
	data = inputData[5];
	status.l2 = data / 255.0f;
	data = inputData[6];
	status.r2 = data / 255.0f;

	return true;
}

bool DSenseDevice::Destroy()
{
	ChangeLedColor(LED(0, 0, 0));
	ChangeVibration(0, 0);
	ChangeTriggerLock(0, 0, 0, 0);
	ChangePlayerLight(0);
	SendOutputReport();
	device.Destroy();
	controllerNum = -1;
	return true;
}

bool DSenseDevice::ChangeLedColor(LED led)
{
	outputData[45] = led.red;
	outputData[46] = led.green;
	outputData[47] = led.blue;
	return true;
}

void DSenseDevice::ChangeVibration(UCHAR right, UCHAR left)
{
	outputData[3] = right;
	outputData[4] = left;
}

void DSenseDevice::ChangeTriggerLock(UCHAR rMode, UCHAR right, UCHAR lMode, UCHAR left)
{
	outputData[11] = rMode;
	outputData[12] = right; //R�g���K�[ ���bit�����b�N�̈ʒu�l���傫���Ȃ�ƈʒu�����ɂȂ�

	outputData[22] = lMode; // L�g���K�[
	outputData[23] = left; // L�g���K�[
}

void DSenseDevice::ChangePlayerLight(UCHAR val)
{
	outputData[44] = val & 0x1f;
}

bool DSenseDevice::SendOutputReport()
{
	BOOL result;
	DWORD sizet = 0;
	OVERLAPPED overlapped;
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	result = WriteFile(device.GetHandle(), outputData, outputDataLength, &sizet, &overlapped);
	device.isDevice = result;
	return result;
}

bool DSenseDevice::IsDSDevice()
{
	return device.isDevice;
}

bool DSenseDevice::GetButton(UCHAR keyType)
{
	bool isDown = false;
	DSButtonStatus statusData;
	switch ((DSKeyType)keyType)
	{
	case DSKeyType::Square:
		statusData = status.square.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Cross:
		statusData = status.cross.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Circle:
		statusData = status.circle.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Triangle:
		statusData = status.triangle.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Up:
		statusData = status.up.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Right:
		statusData = status.right.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Down:
		statusData = status.down.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Left:
		statusData = status.left.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::L1:
		statusData = status.l1.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::R1:
		statusData = status.r1.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::SHARE:
		statusData = status.share.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::OPTION:
		statusData = status.option.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::L3:
		statusData = status.l3.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::R3:
		statusData = status.r3.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::HOME:
		statusData = status.home.status;
		if (statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	}
	return isDown;
}

bool DSenseDevice::GetButtonDown(UCHAR keyType)
{
	bool isDown = false;
	DSButtonStatus statusData;
	switch ((DSKeyType)keyType)
	{
	case DSKeyType::Square:
		statusData = status.square.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Cross:
		statusData = status.cross.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Circle:
		statusData = status.circle.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Triangle:
		statusData = status.triangle.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Up:
		statusData = status.up.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Right:
		statusData = status.right.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Down:
		statusData = status.down.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::Left:
		statusData = status.left.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::L1:
		statusData = status.l1.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::R1:
		statusData = status.r1.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::SHARE:
		statusData = status.share.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::OPTION:
		statusData = status.option.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::L3:
		statusData = status.l3.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::R3:
		statusData = status.l3.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	case DSKeyType::HOME:
		statusData = status.home.status;
		if (statusData == DSButtonStatus::Push) { isDown = true; }
		break;
	}
	return isDown;
}

bool DSenseDevice::GetButtonUp(UCHAR keyType)
{
	bool isUp = false;
	DSButtonStatus statusData;
	switch ((DSKeyType)keyType)
	{
	case DSKeyType::Square:
		statusData = status.square.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::Cross:
		statusData = status.cross.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::Circle:
		statusData = status.circle.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::Triangle:
		statusData = status.triangle.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::Up:
		statusData = status.up.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::Right:
		statusData = status.right.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::Down:
		statusData = status.down.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::Left:
		statusData = status.left.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::L1:
		statusData = status.l1.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::R1:
		statusData = status.r1.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::SHARE:
		statusData = status.share.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::OPTION:
		statusData = status.option.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::L3:
		statusData = status.l3.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::R3:
		statusData = status.l3.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	case DSKeyType::HOME:
		statusData = status.option.status;
		if (statusData == DSButtonStatus::UnPush) { isUp = true; }
		break;
	}
	return isUp;
}

float DSenseDevice::GetAxis(UCHAR axisType)
{
	switch ((DSAxisType)axisType)
	{
	case RightStickX:
		return status.rightStickX;
	case RightStickY:
		return status.rightStickY;
	case LeftStickX:
		return status.leftStickX;
	case LeftStickY:
		return status.leftStickY;
	case L2:
		return status.l2;
	case R2:
		return status.r2;
	}
	return 0.0f;
}
