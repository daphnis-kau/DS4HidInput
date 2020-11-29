#include "DS4Manager.h"

DS4Manager::DS4Manager() = default;

void DS4Manager::GetDevice()
{
	connectionNum = 0;
	GUID hidGuid;
	HidD_GetHidGuid(&hidGuid);
	const auto deviInfo = SetupDiGetClassDevs(&hidGuid, nullptr, nullptr, (DIGCF_PRESENT | DIGCF_INTERFACEDEVICE));
	SP_DEVICE_INTERFACE_DATA data;
	data.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
	for (auto i = 0; SetupDiEnumDeviceInterfaces(deviInfo, nullptr, &hidGuid, i, &data); i++)
	{
		DWORD size = 0;

		//�f�o�C�X�C���^�t�F�[�X�ڍ׏��̃������T�C�Y���擾
		SetupDiGetDeviceInterfaceDetail(deviInfo, &data, nullptr, 0, &size, nullptr);

		//�f�o�C�X�C���^�[�t�F�[�X�ڍ׏��̗̈���m�ۂ���
		auto detail = new SP_INTERFACE_DEVICE_DETAIL_DATA[size];
		if (detail)
		{
			DWORD len = 0;
			memset(detail, 0, size);

			//�f�o�C�X�C���^�[�t�F�[�X�̏ڍ׏���Ǎ���
			detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			if (SetupDiGetInterfaceDeviceDetail(deviInfo, &data, detail, size, &len, NULL))
			{
				//Hid�f�o�C�X�̍쐬
				HidDevice device = device.Create(detail->DevicePath, 0);

				if (device.GetVendorID() == 0x54c && (device.GetProductID() == 0x5c4 || device.GetProductID() == 0x9CC))
				{
					//PS4�R���g���[���[�Ƃ��Đݒ�
					ds4Device[connectionNum] = ds4Device[connectionNum].Create(device, connectionNum);
					connectionNum++;
				}
				else
				{
					//�f�o�C�X�̔j��
					device.Destroy();
				}
			}
		}
		delete[] detail;
	}
	SetupDestroyDiskSpaceList(deviInfo);
}

bool DS4Manager::IsDevice(const int id) const
{
	return ds4Device[id].IsDS4Device();
}

void DS4Manager::ChangeColor(const int id, const UCHAR r, const UCHAR g, const UCHAR b) const
{
	ds4Device[id].ChangeLedColor(LED(r, g, b));
}

void DS4Manager::ChangeVibration(const int id, const UCHAR right, const UCHAR left) const
{
	ds4Device[id].ChangeVibration(right, left);
}

void DS4Manager::SendOutput(const int id)
{
	ds4Device[id].SendOutputReport();
}

void DS4Manager::InputUpdate()
{
	for (auto i = 0; i < 4; i++)
	{
		if (!ds4Device[i].IsDS4Device()) { continue; }
		ds4Device[i].GetInputReport();
	}
}

bool DS4Manager::GetButton(const int id, const DS4KeyType key) const
{
	return ds4Device[id].GetButton(key);
}

bool DS4Manager::GetButtonDown(const int id, const DS4KeyType key) const
{
	return ds4Device[id].GetButtonDown(key);
}

bool DS4Manager::GetButtonUp(const int id, const DS4KeyType key) const
{
	return ds4Device[id].GetButtonUp(key);
}

float DS4Manager::GetAxis(const int id, const DS4AxisType axis) const
{
	return ds4Device[id].GetAxis(axis);
}

void DS4Manager::Destroy()
{
	for (int i = 0; i < connectionNum; i++)
	{
		ds4Device[i].Destroy();
	}
}
