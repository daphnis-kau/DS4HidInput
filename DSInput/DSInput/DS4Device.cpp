#include "DS4Device.h"
#include "DSAxisType.h"

const int BT_OUTPUT_REPORT_LENGTH = 78;
const int BT_INPUT_REPORT_LENGTH = 547;

DS4Device::DS4Device(HidDevice device, int controllerId) noexcept
{
	this->device = device;
	outputDataLength = device.GetCapabilities().OutputReportByteLength;
	inputDataLength = device.GetCapabilities().InputReportByteLength;

	if (outputDataLength == 547)
	{
		outputData = new UCHAR[BT_OUTPUT_REPORT_LENGTH];
		inputData = new UCHAR[BT_INPUT_REPORT_LENGTH];
		outputDataLength = BT_OUTPUT_REPORT_LENGTH;
		inputDataLength = BT_INPUT_REPORT_LENGTH;
	}
	else
	{
		outputData = new UCHAR[outputDataLength];
		inputData = new UCHAR[inputDataLength];
	}

	for (int i = 0; i < inputDataLength; i++)
	{
		inputData[i] = 0;
	}
	for (int i = 0; i < outputDataLength; i++)
	{
		outputData[i] = 1;
	}

	if (inputDataLength == 547)
	{
		outputData[0] = 0x11;//报告
		outputData[1] = 0x80;//常数
		outputData[3] = 0xff;//常数
		outputData[6] = 0x00;//R马达
		outputData[7] = 0x00;//L马达
		outputData[8] = 0x00;//Red
		outputData[9] = 0x00;//G
		outputData[10] = 0x00;//B
		outputData[11] = 0x00;//闪光点
		outputData[12] = 0x00;//闪光点
	}
	else
	{
		outputData[0] = 0x05;//报告ID
		outputData[1] = 0x03;//模式（0：关断，1：仅晃动，2：光，3：两者）
		outputData[2] = 0x00;//LED模式（最下位0:一直发行，1:一瞬）
		outputData[3] = 0x00;//改行
		outputData[4] = 0x00;//R马达
		outputData[5] = 0x00;//L马达
	}

	switch (controllerId)
	{
	case 0:
		ChangeLedColor(LED::Blue());
		break;
	case 1:
		ChangeLedColor(LED::Red());
		break;
	case 2:
		ChangeLedColor(LED::Green());
		break;
	case 3:
		ChangeLedColor(LED::Purple());
		break;
	}

	SendOutputReport();
}

bool DS4Device::SendOutputReport()
{
	BOOL result;
	DWORD sizet = 0;
	OVERLAPPED overlapped;
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	if (inputDataLength == BT_INPUT_REPORT_LENGTH)
	{
		result = HidD_SetOutputReport(device.GetHandle(), outputData, outputDataLength);
	}
	else
	{
		result = WriteFile(device.GetHandle(), outputData, outputDataLength, &sizet, &overlapped);
	}
	device.isDevice = result;
	return result;
}

bool DS4Device::ChangeLedColor(const LED& led) noexcept
{
	if (inputDataLength == BT_INPUT_REPORT_LENGTH)
	{
		outputData[8] = led.red;
		outputData[9] = led.green;
		outputData[10] = led.blue;
	}
	else
	{
		outputData[6] = led.red;
		outputData[7] = led.green;
		outputData[8] = led.blue;
	}
	return true;
}

void DS4Device::ChangeVibration(UCHAR right, UCHAR left)
{
	if (inputDataLength == BT_INPUT_REPORT_LENGTH)
	{
		outputData[6] = right;
		outputData[7] = left;
	}
	else
	{
		outputData[4] = right;
		outputData[5] = left;
	}
}

bool DS4Device::GetInputReport()
{
	DWORD size = 0;
	OVERLAPPED overlapped;
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	const BOOL result = ReadFile(device.GetHandle(), inputData, inputDataLength, &size, &overlapped);
	device.isDevice = result;

	if (!result) return result;

	if (inputDataLength == BT_INPUT_REPORT_LENGTH)
	{
		auto data = inputData[1];
		if (data == 0x40) { return result; }
		data = inputData[7];
		status.square.ChangeStatus((data & 0x10) == 0x10);
		status.cross.ChangeStatus((data & 0x20) == 0x20);
		status.circle.ChangeStatus((data & 0x40) == 0x40);
		status.triangle.ChangeStatus((data & 0x80) == 0x80);
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

		data = inputData[8];
		status.l1.ChangeStatus((data & 0x01) == 0x01);
		status.r1.ChangeStatus((data & 0x02) == 0x02);
		status.share.ChangeStatus((data & 0x10) == 0x10);
		status.option.ChangeStatus((data & 0x20) == 0x20);
		status.l3.ChangeStatus((data & 0x40) == 0x40);
		status.r3.ChangeStatus((data & 0x80) == 0x80);
		status.l2 = (data & 0x04) == 0x04 ? 255.0f : 0.0f;
		status.r2 = (data & 0x08) == 0x08 ? 255.0f : 0.0f;

		//保存左棒输入
		data = inputData[3];
		status.leftStickX = (data - 127) / 127.0f;
		data = inputData[4];
		status.leftStickY = (data - 127) / 127.0f;

		//保存右棒输入
		data = inputData[5];
		status.rightStickX = (data - 127) / 127.0f;
		data = inputData[6];
		status.rightStickY = (data - 127) / 127.0f;

	}
	else
	{
		//保存按钮输入
		auto data = inputData[5];
		status.square.ChangeStatus((data & 0x10) == 0x10);
		status.cross.ChangeStatus((data & 0x20) == 0x20);
		status.circle.ChangeStatus((data & 0x40) == 0x40);
		status.triangle.ChangeStatus((data & 0x80) == 0x80);
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

		data = inputData[6];
		status.l1.ChangeStatus((data & 0x01) == 0x01);
		status.r1.ChangeStatus((data & 0x02) == 0x02);
		status.share.ChangeStatus((data & 0x10) == 0x10);
		status.option.ChangeStatus((data & 0x20) == 0x20);
		status.l3.ChangeStatus((data & 0x40) == 0x40);
		status.r3.ChangeStatus((data & 0x80) == 0x80);

		//保存左棒输入
		data = inputData[1];
		status.leftStickX = (data - 127) / 127.0f;
		data = inputData[2];
		status.leftStickY = (data - 127) / 127.0f;

		//保存右棒输入
		data = inputData[3];
		status.rightStickX = (data - 127) / 127.0f;
		data = inputData[4];
		status.rightStickY = (data - 127) / 127.0f;

		//保存L2和R2的输入
		data = inputData[8];
		status.l2 = data / 255.0f;
		data = inputData[9];
		status.r2 = data / 255.0f;
	}

	return result;
}

bool DS4Device::Destroy()
{
	ChangeLedColor(LED(0, 0, 0));
	SendOutputReport();
	delete[] inputData;
	inputData = nullptr;
	delete[] outputData;
	outputData = nullptr;
	device.Destroy();
	return true;
}

bool DS4Device::GetButton(UCHAR keyType)
{
	const DSButtonStatus &statusData = status.data[keyType].status;
	return statusData == DSButtonStatus::Pushing || statusData == DSButtonStatus::Push;
}

bool DS4Device::GetButtonDown(UCHAR keyType)
{
	const DSButtonStatus &statusData = status.data[keyType].status;
	return statusData == DSButtonStatus::Push;
}

bool DS4Device::GetButtonUp(UCHAR keyType)
{
	const DSButtonStatus &statusData = status.data[keyType].status;
	return statusData == DSButtonStatus::UnPush;
}

float DS4Device::GetAxis(UCHAR axisType)
{
	if (axisType < 0 || axisType >= NUM) return 0.0f;
	return status.axisVals[axisType];
}

void DS4Device::Reset() noexcept
{
	switch (controllerId)
	{
	case 0:
		ChangeLedColor(LED::Blue());
		break;
	case 1:
		ChangeLedColor(LED::Red());
		break;
	case 2:
		ChangeLedColor(LED::Green());
		break;
	case 3:
		ChangeLedColor(LED::Purple());
		break;
	}
	ChangeVibration(0, 0);
	SendOutputReport();
}
