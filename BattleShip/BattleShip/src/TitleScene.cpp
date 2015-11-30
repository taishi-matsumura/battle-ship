/**
*@file	TitleScene.cpp
*@author	亀田
*/

#include "TitleScene.h"

bool TitleScene::Init()
{
	m_background.Init();
	m_screenMask.Init();
	m_button.Init( _BUTTON_POS_X_, _BUTTON_POS_Y_,
					_BUTTON_WIDTH_, _BUTTON_HEIGHT_ );
	m_screenMask.SetColor( 255, 255, 255, 255);//フェードインさせたいのでアルファ値は255で

	m_changeSceneFlag = false;
	
	m_connectSetting.Init( m_pMouse, m_pKey );

	m_stopFadeFlag = false;
	//	タイトルBGM再生
	m_pAudio->SoundPlay( Audio::_TITLE_BGM_, true );
	return true;
}


int TitleScene::Control()
{
	float tempX = (float)m_pMouse->GetCursorPosX();	///<	マウス座標の更新
	float tempY = (float)m_pMouse->GetCursorPosY();	///<	マウス座標の更新
	int	result = 0;

	//	シーンが代わる時
	if( m_changeSceneFlag )
	{
		if( m_stopFadeFlag == false)
		{
			m_stopFadeFlag = m_screenMask.FadeOut(_FADE_OUT_TIME_);
		}

		//	シーン変更
		if( m_stopFadeFlag )
		{
			result = 1;
		}
	}
	else
	{
		if( m_screenMask.FadeIn(_FADE_IN_TIME_) )
		{
			char inputState = 0;
			inputState = m_pMouse->GetMouseSt( MOUSE_L );

			//	ゲームスタートボタン上にカーソルがあったら

			if( m_button.Contorl( tempX, tempY, inputState ) )
			{
				if( m_button.GetState() == Button::STATE_SELECT ){
					m_changeSceneFlag = true;
					m_pAudio->SoundPlay( Audio::_CLICK_SE_ );
				}
			}
			m_connectSetting.Control();
		}
	}
	return result;
}


void TitleScene::Draw()
{
	float tempX, tempY;
	int tempA, tempR, tempG, tempB;

	//	背景描画
	m_background.GetPosition( &tempX, &tempY);
	m_pDrawManager->VertexDraw( _TEX_TITLE_BACK_, tempX, tempY, 
		m_background.GetWidth(),  m_background.GetHeight(),
		0.f, 0.f, 1.f, 1.f);
	
	//	ゲームスタートボタン描画
	m_button.GetPosition( &tempX, &tempY);
	m_button.GetColor( tempA, tempR, tempG, tempB );
	m_pDrawManager->VertexDraw( _TEX_STARTBUTTON_, tempX, tempY, 
		m_button.GetWidth(),  m_button.GetHeight(),
		0.f, 0.f, 1.f, 1.f, tempA, tempR, tempG, tempB);

	//	フェード用のマスク描画
	m_screenMask.GetPosition( &tempX, &tempY);
	m_screenMask.GetColor( tempA, tempR, tempG, tempB );
	m_pDrawManager->VertexDraw( _TEX_TITLEMASK_, tempX, tempY, 
		m_screenMask.GetWidth(),  m_screenMask.GetHeight(),
		0.f, 0.f, 1.f, 1.f,	tempA, tempR, tempG, tempB);
	
	m_connectSetting.Draw( m_pDrawManager );	
}
