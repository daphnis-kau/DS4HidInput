#include "DS4Button.h"

DS4Button::DS4Button()
{
	status = DSButtonStatus::None;
}

DS4Button::DS4Button(bool isButtonDown)
{
	ChangeStatus(isButtonDown);
}

void DS4Button::ChangeStatus(bool isButtonDown)
{
	if (isButtonDown == true)
	{
		if (status == DSButtonStatus::None)
		{
			status = DSButtonStatus::Push;
			return;
		}

		if (status == DSButtonStatus::Push)
		{
			status = DSButtonStatus::Pushing;
			return;
		}

		if (status == DSButtonStatus::UnPush)
		{
			status = DSButtonStatus::Push;
		}
		return;
	}

	if (isButtonDown == false)
	{
		if (status == DSButtonStatus::Push)
		{
			status = DSButtonStatus::UnPush;
			return;
		}

		if (status == DSButtonStatus::Pushing)
		{
			status = DSButtonStatus::UnPush;
			return;
		}

		if (status == DSButtonStatus::UnPush)
		{
			status = DSButtonStatus::None;
		}
		return;
	}
}
