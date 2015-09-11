/**
*@file	Result.h
*@author	亀田
*/

#ifndef _RESULT_H_
#define _RESULT_H_

#include "GameState.h"

class Result : public GameState
{
	

public:
	/**
	*@brief	戦闘結果での初期化
	*/
	bool Init( ShipObject::_SHIP_TYPE_NUM_ _type );

	/**
	*@brief	戦闘結果でのサブルーチン処理
	*/
	int Control();
	
	/**
	*@brief	戦闘結果での描画
	*/
	void Draw();
	
	/**
	*@brief	戦闘結果での通信待機処理
	*/
	void ComStandby();

};

#endif