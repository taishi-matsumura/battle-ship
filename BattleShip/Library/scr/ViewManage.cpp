#include "SceneManage.h"
#include "graphic.h"


// 描画後処理
void EndRender ();

//----------------------------------------------------

//	描画の管理
//	引数：3Dモデルを扱う時はtrueに
void CSceneManager::ViewManage ( bool b3dView )
{
	
	if ( b3dView == true )
	{
		RenderManage();
	}
	DrawManage( );
}

//	レンダリング管理
void CSceneManager::RenderManage ( )
{
	
	
	m_sceneObj -> Render();

	EndRender();
}

//	描画管理
void CSceneManager::DrawManage (  )
{

	m_sceneObj -> Draw();

	EndRender();
}


