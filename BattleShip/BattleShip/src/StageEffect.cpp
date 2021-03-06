/**
*@file	StageEffect.cpp
*@author	亀田
*/

#include "StageEffect.h"
#include <cmath>
#include <cstdio>

#define _EFFECT_POS_TWEAK_	4.0f


bool StageEffect::Init()
{
	CheckOfMyShipPos();		///< 
	CheckSelectOfStage();	///< 
	
	return false;
}

void StageEffect::CheckSelectOfStage()
{
	for ( int iPlayer = 0; iPlayer < _PLAYER_NUM_; iPlayer++ )
	{
		for ( int iColumn = 0; iColumn < _STAGE_COLUMN_MAX_; iColumn++ )
		{
			for ( int iLine = 0; iLine < _STAGE_LINE_MAX_; iLine++ )
			{

				int selectType = StageObject::SelectOfData( m_pStage->m_stageArray[iPlayer][iColumn][iLine]);
				//	エフェクトは攻撃索敵のみ
				switch ( selectType )
				{
				case StageObject::_SEARCH_NOMAL_:
				case StageObject::_SEARCH_ALL_:
				case StageObject::_ACTION_NOMAL_:
				case StageObject::_ACTION_ALL_:
					if ( iPlayer == m_playerID/_PLAYER_NUM_ )	///<プレイヤー側のマス（敵による行動選択）
					{
						m_enemySelect.push_back(m_pStage->m_stageBlock[iPlayer][iColumn][iLine] );
						//	敵の選択した行動をプレイヤー側の選択されたマスから判断する
						switch (selectType)
						{
						case StageObject::_SEARCH_NOMAL_:
						case StageObject::_SEARCH_ALL_:
							m_enemySelectType = _SELECT_SEARCH_;
							break;
						case StageObject::_ACTION_NOMAL_:
						case StageObject::_ACTION_ALL_:
							m_enemySelectType = _SELECT_ACTION_;
								break;
						}
					}
					else	///<敵側のマス（プレイヤーによる行動マス）
					{
						m_playerSelect.push_back( m_pStage->m_stageBlock[iPlayer][iColumn][iLine] );						
					}
					break;
				}

			}
		}
	}
	D3DXVECTOR2 tempVec;	//正規化を行う際の仮保存ベクトル
	//	ちゃんとプレイヤーは攻撃or索敵をしているなら
	if ( !m_playerSelect.empty() )
	{
		//	エフェクトに必要な座標やベクトルをセットしてあげる。
		m_plTargetPointX = m_playerSelect[m_playerSelect.size()/2].GetPositionX();
		m_plTargetPointY = m_playerSelect[m_playerSelect.size()/2].GetPositionY();
		m_plTargetVector = D3DXVECTOR2( m_plTargetPointX-m_myShipBlock.GetPositionX(), m_plTargetPointY-m_myShipBlock.GetPositionY() );
		
		tempVec = m_plTargetVector;	///<一度ベクトルの正規化を行う
		D3DXVec2Normalize( &m_plTargetVector, &tempVec );

		//	移動速度を掛けてやる。
		//	描画時にこの処理を毎回するのは面倒。
		if ( m_ShipCount >= ShipObject::TYPE_DESTROYER )
		{
			float tempX =( m_plTargetPointX-m_myShipBlock.GetPositionX() ) / static_cast<float>( TIME_CHANGE_EFFECT );
			float tempY =( m_plTargetPointY-m_myShipBlock.GetPositionY() ) / static_cast<float>( TIME_CHANGE_EFFECT );
			m_plTargetVector.x = tempX;
			m_plTargetVector.y = tempY;
		}
		else
		{
			m_plTargetVector.x *= _MOVE_SPEED_RECON_;
			m_plTargetVector.y *= _MOVE_SPEED_RECON_;
		}
	}
	//	敵の場合も
	if ( !m_enemySelect.empty() )
	{
		//	エフェクトに必要な座標やベクトルをセットしてあげる。
		//	敵の場合はX軸のみを適応させて、正確な位置から飛んできた様に見えない様にさせる。
		//	ただし、エフェクト演出時にプレイヤー側に出てくるまで透過などをするつもり	
		for ( unsigned int i=0; i<m_enemySelect.size(); i++ )
		{
			m_enTargetPointX += m_enemySelect[i].GetPositionX();
			m_enTargetPointY += m_enemySelect[i].GetPositionY();
		}
		//	標的座標の平均座標を調べる
		m_enTargetPointX = m_enTargetPointX/m_enemySelect.size();
		m_enTargetPointY = m_enTargetPointY/m_enemySelect.size();

		//	敵側のX軸のポジションは画面端基準
		m_enTargetVector = D3DXVECTOR2( m_enTargetPointX-( (m_playerID%_PLAYER_NUM_) * WIDTH), m_enTargetPointY-m_enTargetPointY );
		
		tempVec = m_enTargetVector;	///<一度ベクトルの正規化を行う
		D3DXVec2Normalize( &m_enTargetVector, &tempVec );
		
		//	移動速度を掛けてやる。
		//	描画時にこの処理を毎回するのは面倒。
		if ( m_ShipCount >= ShipObject::TYPE_DESTROYER )
		{
			m_enTargetVector.x *= ( fabs(((m_playerID%_PLAYER_NUM_) * WIDTH )-m_enTargetPointX) ) / TIME_CHANGE_EFFECT;
			m_enTargetVector.y *= (m_enTargetPointY-m_enTargetPointY) / TIME_CHANGE_EFFECT;
		}
		else
		{
			m_enTargetVector.x *= _MOVE_SPEED_RECON_;
			m_enTargetVector.y *= _MOVE_SPEED_RECON_;
		}
	}
	
}


void StageEffect::CheckOfMyShipPos()
{
	ShipObject* tempShip = m_pPlayer[m_playerID/_PLAYER_NUM_]->GetShip( static_cast<ShipObject::_SHIP_TYPE_NUM_>(m_ShipCount) );
	tempShip->GetArrayPos( m_actionShipPosColumn, m_actionShipPosLine );
	m_myShipBlock = m_pStage->m_stageBlock[m_playerID/_PLAYER_NUM_][m_actionShipPosColumn][m_actionShipPosLine];
}


int StageEffect::Control()
{
	//	エフェクトを再生する際に基準にするカウンタ
	//	ここではログに追加などをする。
	//	一度だけ駒が両者とも死んでいないか確認する
	static bool destroyShipsCheck = false;
	if (!destroyShipsCheck)
	{
		bool checkFlag[2];
		checkFlag[0] = m_pPlayer[0]->CheckDestroy(static_cast<ShipObject::_SHIP_TYPE_NUM_>(m_ShipCount));
		checkFlag[1] = m_pPlayer[1]->CheckDestroy(static_cast<ShipObject::_SHIP_TYPE_NUM_>(m_ShipCount));
		//	どちらも駒が死んでいたのでエフェクトは終了させる
		if ( checkFlag[0] && checkFlag[1] )
			return 1;

		destroyShipsCheck = true;
	}

	if ( m_elapsedTimeFormStateInstance < TIME_END_RUSULT_EFFECT  )
	{
		EffectSoundControl();
	}
	else
	{
		destroyShipsCheck = false;	//シーン自体が終わったので初期化
		return 1;
	}

	return 0;
}


//	
void StageEffect::Draw()
{
	//	エフェクトを再生する際に基準にするカウンタ分岐
	if ( m_elapsedTimeFormStateInstance < TIME_END_ACTION_EFFECT  )
	{
		AttackStartControl();
		SearchStartControl();
		
	}
	else if ( m_elapsedTimeFormStateInstance < TIME_CHANGE_EFFECT )
	{
		//	駆逐艦などの場合は魚雷を徐々に目標マスに近づけたいので攻撃エフェクト描画を挟む
		if ( m_ShipCount >= ShipObject::TYPE_DESTROYER )
			AttackStartControl();
	}
	else if ( m_elapsedTimeFormStateInstance < TIME_END_RUSULT_EFFECT )
	{
		HitEffectControl();
		SearchResultControl();
	}

}


void StageEffect::AttackStartControl()
{
	//	プレイヤー側
	if ( m_plyaerSelectType == _SELECT_ACTION_ )
	{
		if ( m_ShipCount == static_cast<int>(ShipObject::TYPE_AIRCARRIER) )
		{
			AircraftEffect( m_myShipBlock, 0 );
		}
		else if ( m_ShipCount >= static_cast<int>(ShipObject::TYPE_DESTROYER) )
		{
			TorpedoEffect( m_myShipBlock );
		}
		else
		{

			FireEffect( m_myShipBlock );
		}
	}
	
	//	敵側
	if ( m_enemySelectType == _SELECT_ACTION_ )
	{
		if ( m_ShipCount == static_cast<int>(ShipObject::TYPE_AIRCARRIER) )
		{
			AircraftEffect( m_myShipBlock, 0, true );
		}
		else if ( m_ShipCount >= static_cast<int>(ShipObject::TYPE_DESTROYER) )
		{
			TorpedoEffect( m_myShipBlock, true );
		}
		
	}
}

void StageEffect::SearchStartControl()
{
	
	//	プレイヤー側
	if ( m_plyaerSelectType == _SELECT_SEARCH_ )
	{
		if ( m_ShipCount >= static_cast<int>(ShipObject::TYPE_DESTROYER) )
		{
			SonarEffect( m_myShipBlock );
		}
		else
		{
			AircraftEffect( m_myShipBlock, 1 );
		}
	}
	//	敵側
	if ( m_enemySelectType == _SELECT_SEARCH_ )
	{
		//	敵側は駆逐潜水以外は索敵機を飛ばす
		if ( m_ShipCount < static_cast<int>(ShipObject::TYPE_DESTROYER) )
		{
			AircraftEffect( m_myShipBlock, 1, true );
		}
	}
}

void StageEffect::HitEffectControl()
{
	/**
	*@details	選択されたマス分エフェクトを出す
	*/

	//	プレイヤー側
	if ( m_plyaerSelectType == _SELECT_ACTION_ )
	{
		for ( unsigned int i = 0; i< m_playerSelect.size(); i++ )
		{
			ExplosionEffect( m_playerSelect[i] );
		}
	}
	
	//	敵側
	if ( m_enemySelectType == _SELECT_ACTION_ )
	{
		for ( unsigned int i = 0; i< m_enemySelect.size(); i++ )
		{
			ExplosionEffect( m_enemySelect[i] );
		}
	}

}

void StageEffect::SearchResultControl()
{
	/**
	*@details	選択されたマス分エフェクトを出す
	*/

	if ( m_plyaerSelectType == _SELECT_SEARCH_ )
	{
		for ( unsigned int i = 0; i< m_playerSelect.size(); i++ )
		{
			SonarEffect( m_playerSelect[i] );
		}
	}
	if ( m_enemySelectType == _SELECT_SEARCH_ )
	{
		for ( unsigned int i = 0; i< m_enemySelect.size(); i++ )
		{
			SonarEffect( m_enemySelect[i] );
		}
	}

}


void StageEffect::FireEffect( BoardOfFrame& _block )
{
	float tempX, tempY;
	tempX = _block.GetPositionX()-_EFFECT_POS_TWEAK_;
	tempY = _block.GetPositionY()-_EFFECT_POS_TWEAK_;

	int wDiv = static_cast<int>(m_elapsedTimeFormStateInstance/_DIVISON_OF_EFFECT_TIME_);	///<シーンの経過時間から何コマ目のアニメーションをさせるかを計算（テスト実装）
	bool flipHorizontal = m_playerID/_PLAYER_NUM_ ? true : false;	///<プレイヤーによって画像の反転をさせる

	m_pDrawManager->AnimationDraw( _TEX_EFFECTMAP1_,tempX,tempY, 
								   _block.GetWidth()+_EFFECT_POS_TWEAK_*2, 
								   _block.GetHeight()+_EFFECT_POS_TWEAK_*2,
								   flipHorizontal,false,wDiv,0 );

	
}

void StageEffect::ExplosionEffect( BoardOfFrame& _block )
{
	
	float tempX, tempY;
	tempX = _block.GetPositionX();
	tempY = _block.GetPositionY();
	int wDiv = static_cast<int>(m_elapsedTimeFormStateInstance/_DIVISON_OF_EFFECT_TIME_);	///<シーンの経過時間から何コマ目のアニメーションをさせるかを計算（テスト実装）
	bool flipHorizontal = m_playerID/_PLAYER_NUM_ ? true : false;	///<プレイヤーによって画像の反転をさせる

	m_pDrawManager->AnimationDraw(_TEX_EFFECTMAP1_,tempX,tempY, 
								   _block.GetWidth(), 
								   _block.GetHeight(),
								   flipHorizontal,false,wDiv,1 );

	
}

void StageEffect::AircraftEffect( BoardOfFrame& _block, int _aircraftType, bool _appearanceInvisibility )
{
	float tempX, tempY;
	bool flipHorizontal = m_playerID/_PLAYER_NUM_ ? true : false;	///<プレイヤーによって画像の反転をさせる
	unsigned long color = 0xffffffff;
	static const int multiplOfAircraftAlpha = 8;	///< 航空機のアルファ値をtimeから乗算して出す際の数
	//int texNum;

	if ( !_appearanceInvisibility )
	{
		//	航空機の移動先の位置をゲームカウントで制御
		tempX = _block.GetPositionX()+(m_plTargetVector.x)*m_elapsedTimeFormStateInstance;
		tempY = _block.GetPositionY()+(m_plTargetVector.y)*m_elapsedTimeFormStateInstance;
	}
	//	出現位置を隠したい場合
	else 
	{
		int alpha = 0;
		/*
			どちらのプレイヤーが航空機を飛ばしているかも判断しないと
			どの場所から飛ばして可視化させれば良いか分からない。
			被攻撃対象側の陣地に入った場合、徐々にアルファ値を上げていく。
		*/
		//	出現位置は相手側の画面端
		tempX = ( (m_playerID%_PLAYER_NUM_) * WIDTH )+(m_enTargetVector.x)*m_elapsedTimeFormStateInstance;
		tempY = m_enTargetPointY+(m_enTargetVector.y)*m_elapsedTimeFormStateInstance;
									
		alpha = m_elapsedTimeFormStateInstance*multiplOfAircraftAlpha;
		
		if ( alpha >= 255 )
			alpha = 255;

		color = D3DCOLOR_ARGB( alpha, 255, 255, 255 );	///< アルファ値をカラー値に入れる
		flipHorizontal = m_playerID/_PLAYER_NUM_ ? false : true;	///< 消している方は今の時点では自身とは
	}
	////	航空機のテクスチャを設定
	//switch ( _aircraftType )
	//{
	//case 0:
	//	texNum = _TEX_AIRCRAFT_;
	//	break;
	//case 1:
	//	texNum = _TEX_RECON_;
	//	break;
	//}
	m_pDrawManager->AnimationDraw( _TEX_EFFECTMAP2_,tempX,tempY, 
								   _block.GetWidth(), 
								   _block.GetHeight(),
								   flipHorizontal,false,0,_aircraftType,color );
}


void StageEffect::TorpedoEffect( BoardOfFrame& _block, bool _appearanceInvisibility )
{
	float tempX, tempY;
	bool flipHorizontal = m_playerID/_PLAYER_NUM_ ? true : false;	///<プレイヤーによって画像の反転をさせる
	unsigned long color = 0xc8a0a0ff;	///< 魚雷のデフォルトカラー値（青っぽくすることで、水中を進んでいるように表現）
	static const int multiplOfTorpedoAlpha = 4;	///< 魚雷のアルファ値をtimeから乗算して出す際の数
	
	if ( !_appearanceInvisibility )
	{
		//	航空機の移動先の位置をゲームカウントで制御
		tempX = _block.GetPositionX()+((m_plTargetVector.x)*m_elapsedTimeFormStateInstance);
		tempY = _block.GetPositionY()+((m_plTargetVector.y)*m_elapsedTimeFormStateInstance);
	}
	//	出現位置を隠したい場合
	else 
	{
		int alpha = 0;
		/*
			どちらのプレイヤーが航空機を飛ばしているかも判断しないと
			どの場所から飛ばして可視化させれば良いか分からない。
			被攻撃対象側の陣地に入った場合、徐々にアルファ値を上げていく。
		*/
		//	出現位置は相手側の画面端
		tempX = ( (m_playerID%_PLAYER_NUM_) * WIDTH )+(m_enTargetVector.x)*m_elapsedTimeFormStateInstance;
		tempY = m_enTargetPointY+(m_enTargetVector.y)*m_elapsedTimeFormStateInstance;
									
		alpha = m_elapsedTimeFormStateInstance*multiplOfTorpedoAlpha;
		
		if ( alpha >= 200 )
			alpha = 200;

		color = D3DCOLOR_ARGB( alpha, 160, 160, 255 );	///< アルファ値をカラー値に入れる
		flipHorizontal = m_playerID/_PLAYER_NUM_ ? false : true;	///< 消している方は今の時点では自身とは
	}
	
	m_pDrawManager->AnimationDraw( _TEX_EFFECTMAP2_,tempX,tempY, 
								   _block.GetWidth(), 
								   _block.GetHeight(),
								   flipHorizontal,false,0,2,color );
}

void StageEffect::SonarEffect( BoardOfFrame& _block )
{
	/**
	*@details	0〜8秒 12〜20秒 4〜12秒 の３パターンの中で、索敵する座標の中心から、
				円の画像を経過時間%8/5（これが色々試した中で一番見た目が良かった）で
				縮小→拡大させていく。
				4〜12秒のパターンが経過時間%8+4/5なのは他の2パターンとの差をつけるため。
	*/

	const int xalpha = 20;
	const float blockW = _BLOCK_WIDTH_SIZE_, blockH = _BLOCK_HEIGHT_SIZE_;

	if ( m_elapsedTimeFormStateInstance <= TIME_CHANGE_EFFECT+8 || m_elapsedTimeFormStateInstance >= TIME_CHANGE_EFFECT+12 )
	{
		float tempX, tempY, tempW, tempH;
		unsigned int Alpha = 200;
		tempW = _block.GetWidth() *(m_elapsedTimeFormStateInstance%8)/5;
		tempH =	_block.GetHeight()*(m_elapsedTimeFormStateInstance%8)/5;
		tempX = _block.GetPositionX()-(tempW/2.0f)+(blockW/2);
		tempY = _block.GetPositionY()-(tempH/2.0f)+(blockW/2);
		Alpha -= (m_elapsedTimeFormStateInstance%8)*xalpha;
		
		m_pDrawManager->AnimationDraw( _TEX_EFFECTMAP2_,tempX,tempY, 
									   tempW,tempH,
									   false,false,0,3,
									   D3DCOLOR_ARGB(Alpha,255,255,255));
	}
	if ( m_elapsedTimeFormStateInstance >= TIME_CHANGE_EFFECT+4 || m_elapsedTimeFormStateInstance <= TIME_CHANGE_EFFECT+12 )
	{
		float tempX, tempY, tempW, tempH;
		unsigned int Alpha = 200;
		tempW = _block.GetWidth() *((m_elapsedTimeFormStateInstance%8)+4)/5;
		tempH =	_block.GetHeight()*((m_elapsedTimeFormStateInstance%8)+4)/5;
		tempX = _block.GetPositionX()-(tempW/2.0f)+(blockW/2);
		tempY = _block.GetPositionY()-(tempH/2.0f)+(blockW/2);
		Alpha -= (m_elapsedTimeFormStateInstance%8)*xalpha;

		m_pDrawManager->AnimationDraw( _TEX_EFFECTMAP2_,tempX,tempY, 
									   tempW,tempH,
									   false,false,0,3,
									   D3DCOLOR_ARGB(Alpha,255,255,255));

	}
}



void StageEffect::EffectSoundControl()
{
	/*
		不格好ですが取り敢えず時間に合わせて各艦別に発生SEを変えたりしています
	*/
	switch (m_plyaerSelectType)
	{
		//	攻撃
	case _SELECT_ACTION_:
		//	エフェクト開始
		if ( m_elapsedTimeFormStateInstance == 0 ){
			switch (m_ShipCount)
			{
			case ShipObject::TYPE_AIRCARRIER:
				m_pAudio->SoundPlay( Audio::_AIRCRAFT_SE_ );
				break;
			case ShipObject::TYPE_BATTLESHIP:
				m_pAudio->SoundPlay( Audio::_FIRE_H_SE_ );
				break;
			case ShipObject::TYPE_CRUISER:
				m_pAudio->SoundPlay( Audio::_FIRE_M_SE_ );
				break;
			case ShipObject::TYPE_DESTROYER:
			case ShipObject::TYPE_SUBMARINE:
				m_pAudio->SoundPlay( Audio::_TORPEDO_SE_ );

				break; 
			}
		
		}
		//	エフェクト中盤
		if ( m_elapsedTimeFormStateInstance == TIME_END_ACTION_EFFECT ){
			if ( m_ShipCount == ShipObject::TYPE_AIRCARRIER ) 
				m_pAudio->SoundPlay( Audio::_AC_ATTACK_SE_ );
		}
		//	エフェクト終盤
		if ( m_elapsedTimeFormStateInstance == TIME_CHANGE_EFFECT ){
			m_pAudio->SoundPlay( Audio::_WATER_SE_ );
			m_pAudio->SoundPlay( Audio::_EXPLOSION_SE_ );
		}

		break;

		//	索敵
	case _SELECT_SEARCH_:
		//	エフェクト開始
		if ( m_elapsedTimeFormStateInstance == 0 ){
			if ( m_ShipCount <= ShipObject::TYPE_CRUISER ){
				m_pAudio->SoundPlay( Audio::_AIRCRAFT_SE_ );
			}else{
				m_pAudio->SoundPlay( Audio::_SONAR_SE_ );
			}
		}
		//	エフェクト中盤
		if ( m_elapsedTimeFormStateInstance == TIME_END_ACTION_EFFECT ){
			
		}
		//	エフェクト終盤
		if ( m_elapsedTimeFormStateInstance == TIME_CHANGE_EFFECT ){
			m_pAudio->SoundPlay( Audio::_SONAR_SE_ );
		}
		break;
	}		 
	
	//	敵側
	switch (m_enemySelectType)
	{
		//	攻撃
	case _SELECT_ACTION_:
		//	エフェクト開始
		if ( m_elapsedTimeFormStateInstance == 0 ){
			switch (m_ShipCount)
			{
			case ShipObject::TYPE_AIRCARRIER:
				m_pAudio->SoundPlay( Audio::_AIRCRAFT_SE_ );
				break;
			case ShipObject::TYPE_BATTLESHIP:
				m_pAudio->SoundPlay( Audio::_FIRE_H_SE_ );
				break;
			case ShipObject::TYPE_CRUISER:
				m_pAudio->SoundPlay( Audio::_FIRE_M_SE_ );
				break;
			case ShipObject::TYPE_DESTROYER:
			case ShipObject::TYPE_SUBMARINE:
				m_pAudio->SoundPlay( Audio::_FIRE_L_SE_ );

				break; 
			}
		
		}
		//	エフェクト中盤
		if ( m_elapsedTimeFormStateInstance == TIME_END_ACTION_EFFECT ){
			if ( m_ShipCount == ShipObject::TYPE_AIRCARRIER ) 
				m_pAudio->SoundPlay( Audio::_AC_ATTACK_SE_ );
		}
		//	エフェクト終盤
		if ( m_elapsedTimeFormStateInstance == TIME_CHANGE_EFFECT ){
			m_pAudio->SoundPlay( Audio::_WATER_SE_ );
			m_pAudio->SoundPlay( Audio::_EXPLOSION_SE_ );
		}

		break;

		//	索敵
	case _SELECT_SEARCH_:
		//	エフェクト開始
		if ( m_elapsedTimeFormStateInstance == 0 ){
			if ( m_ShipCount <= ShipObject::TYPE_CRUISER ){
				m_pAudio->SoundPlay( Audio::_AIRCRAFT_SE_ );
			}else{
				m_pAudio->SoundPlay( Audio::_SONAR_SE_ );
			}
		}
		//	エフェクト中盤
		if ( m_elapsedTimeFormStateInstance == TIME_END_ACTION_EFFECT ){
			
		}
		//	エフェクト終盤
		if ( m_elapsedTimeFormStateInstance == TIME_CHANGE_EFFECT ){
			m_pAudio->SoundPlay( Audio::_SONAR_SE_ );
		}
		break;
	}

}