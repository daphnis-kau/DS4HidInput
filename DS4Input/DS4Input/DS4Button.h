#pragma once
#include "DS4ButtonStatus.h"

struct DS4Button
{
	DSButtonStatus status;

	DS4Button();
	DS4Button(bool isButtonDown);
	void ChangeStatus(bool isButtonDown);
};