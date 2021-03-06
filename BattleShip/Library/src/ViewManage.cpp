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
	DrawManage( b3dView );
}

//	レンダリング管理
void CSceneManager::RenderManage ( )
{
	m_pRenderManager->BeginRender();
	
	m_sceneObj -> Render();

	EndRender();
}

//	描画管理
void CSceneManager::DrawManage ( bool _b3dView )
{
	m_pDrawManager->BeginDraw( _b3dView );

	m_sceneObj -> Draw();

	EndRender();
}


