#include "graphic.h"
#include "SceneManage.h"
#include "AllInput.h"

//	ゲームメイン関数
int CSceneManager::GameMain ()
{
	Manage();

	SceneRoutingManage();

	ViewManage();

	return 0;
}