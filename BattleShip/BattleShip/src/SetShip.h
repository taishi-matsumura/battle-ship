/**
*@file	SetShip.h
*@author	亀田
*/

#ifndef _SETSHIP_H_
#define _SETSHIP_H_

#include "GameState.h"
#include "Player.h"
#include "StageObject.h"

/**
*@brief	駒の配置ステートクラス
*/
class SetShip : public GameState {
 public:
	/**
	*@brief	コンストラクタ
	*@param	_type	現在選択している艦の種類
	*/
	SetShip( ShipObject::_SHIP_TYPE_NUM_& _type, GameLog* _pGameLog )
			 : GameState( _type, _pGameLog ) {}

	~SetShip(){};

	/**
	*@brief	駒配置での初期化
	*/
	bool Init();

	/**
	*@brief	駒配置でのサブルーチン処理
	*@return	m_ShipCount	現在の選択駒が分かれば、結果を受け取ったStateManagerは、@n
				それに応じて違うステートに切り替えたりさせればいい。
	*/
	int Control();

	/**
	*@brief	駒配置でのサブルーチン描画
	*/
	void Draw();
	
 private:	
	/**
	*@brief	盤面総チェックのルーチン処理
	*@return	判定結果	0：クリックの判定を通らなかった。@n
				1：マスがすでに埋まっていたorそのマスだと置けない。@n
				2：クリックしたマスに駒を配置出来た。
	*/
	int CheckBoard();

	/**
	*@brief	配置回数の取得
	*/
	inline int GetSetCount(){ return m_ShipCount; }
};

#endif