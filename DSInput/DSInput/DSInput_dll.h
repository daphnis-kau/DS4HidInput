#pragma once
#include "DSManager.h"
#define EXPORT_API __declspec(dllexport)

extern "C" 
{
	//开始管理
	EXPORT_API bool StartDS4();

	//获取控制器
	EXPORT_API bool GetController();

	//检查控制器
	EXPORT_API bool ControllerCheck(int id);

	//颜色变更
	EXPORT_API bool ChangeColor(int id, const UCHAR r, const UCHAR g, const UCHAR b);

	//玩家灯的变更
	EXPORT_API bool ChangePlayerLight(int id, const UCHAR val);

	//振动的变更
	EXPORT_API void ChangeVibration(int id, UCHAR right, UCHAR left);

	//触发锁定
	EXPORT_API void ChangeTriggerLock(int id, UCHAR rMode, UCHAR right, UCHAR lMode, UCHAR left);

	//输出报告的发送
	EXPORT_API void SendOutput(int id);

	//报告的更新
	EXPORT_API bool UpdateInputReport();

	//按钮的取得
	EXPORT_API bool GetButton(int id, UCHAR keyType);

	//按钮下降的取得
	EXPORT_API bool GetButtonDown(int id, UCHAR keyType);

	//按钮上升的取得
	EXPORT_API bool GetButtonUp(int id, UCHAR keyType);

	//取得轴
	EXPORT_API float GetAxis(int id, UCHAR keyType);

	//管理结束
	EXPORT_API bool EndDS4();
}
