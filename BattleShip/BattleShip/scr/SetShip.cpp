/**
*@file	SetShip.cpp
*@author	亀田
*/

#include "SetShip.h"

bool SetShip::Init( ShipObject::_SHIP_TYPE_NUM_ _type )
{
	m_SetCompFlag = false;
	m_SetCount = _type;		///< 呼び出し元（StateManager）から初期艦種の値をもらうので
	return false;
}

//	
int SetShip::Control()
{
	m_pStage->ResetSelect();	///<	ステージの選択状態をリセット

	if( !m_SetCompFlag )
	{
		int iCheckResult = 0;
		//	駒が置ける置けない関係なく、右クリックで駒を回転させる
		if( m_pMouse->MouseStCheck( MOUSE_R, PUSH ) ) {
			ShipObject* tempShip = m_pPlayer[m_playerID-1]->GetShip( (ShipObject::_SHIP_TYPE_NUM_)m_SetCount );
			tempShip->RotationShip( 0, true );
		}
		iCheckResult = CheckBoard();

		if( iCheckResult == 2 )
			m_SetCount++;
	}

	if( m_SetCount >= ShipObject::TYPE_MAX ){
		m_SetCompFlag = true;
		m_pStage->ResetSelect();
	}
	return m_SetCount;
}

//
int SetShip::CheckBoard()
{
	bool tempFlag = false;	///<	ネストが深くなる事を防止するために、ブロックのクリック判定がtrue

	//	行
	for( int iColumn=0; iColumn<_STAGE_COLUMN_MAX_; iColumn++ ){	
		//	列
		for( int iLine=0; iLine<_STAGE_LINE_MAX_; iLine++ ){
			
			float tempX = m_pMouse->GetCursorPosX(), tempY = m_pMouse->GetCursorPosY();
			
			if( m_pStage->m_stageBlock[m_playerID-1][iColumn][iLine].HitBlockCheck( tempX, tempY ))
			{
				int iCheckResult=0;
				ShipObject* tempShip = m_pPlayer[m_playerID-1]->GetShip( (ShipObject::_SHIP_TYPE_NUM_)m_SetCount );
				//	ステージブロックのチェック
				iCheckResult = m_pStage->CheckStageBlock( m_playerID, iColumn, iLine, tempShip);
				
				if( iCheckResult != 0 )	///<駒を置けるマスじゃなかった。
				{	
					//	置けない範囲だった場合も、置けないという情報をステージにセットする
					m_pStage->SetRange( m_playerID, iColumn, iLine, tempShip->m_shipArray, 2 );
					return 1;
				}
				else ///<置けるマス。
				{
					m_pStage->SetRange( m_playerID, iColumn, iLine, tempShip->m_shipArray, 1);
					//	駒が置けるマスであり、左クリックを押した時
					if( m_pMouse->MouseStCheck( MOUSE_L, PUSH )) {
						m_pStage->SetShip( m_playerID, iColumn, iLine, tempShip );
						tempShip->SetArrayPos( iColumn, iLine );
						tempShip->SetDeadFlag( false );///<駒を設置したのでオブジェクトの死亡フラグを下げる
						return 2;
					}
				}
			}
		}
	}

	return 0;
}

//	
void SetShip::Draw()
{
	float tempX = m_pMouse->GetCursorPosX(), tempY = m_pMouse->GetCursorPosY();
	if( m_SetCount < ShipObject::TYPE_MAX )
	{
		ShipObject* tempShip = m_pPlayer[m_playerID-1]->GetShip( (ShipObject::_SHIP_TYPE_NUM_)m_SetCount );
		m_pDrawManager->VertexTransform( m_SetCount + _TEX_AIRCARRIER_, tempShip->m_vertex, tempX, tempY, 1.f, 1.f, tempShip->GetDirection()*90.f );
	}
}

//	
void SetShip::ComStandby()
{

}