/**
*@file	StageObject.h
*@author	亀田
*/

#ifndef _STAGEOBJECT_H_
#define _STAGEOBJECT_H_

#include "gameObject.h"
#include "ShipObject.h"
#include "BoardOfFrame.h"

#define _STAGE_COLUMN_MAX_ 10	///<	ステージの行数
#define _STAGE_LINE_MAX_	10	///<	ステージの列数

#define _PLAYER_NUM_ 2

class StageObject : public CGameObject
{
public:
	unsigned char	m_stageArray[_PLAYER_NUM_][_STAGE_COLUMN_MAX_][_STAGE_LINE_MAX_];	///<ステージ上の情報
	BoardOfFrame	m_stageBlock[_PLAYER_NUM_][_STAGE_COLUMN_MAX_][_STAGE_LINE_MAX_];	///<ステージブロックの座標

public:
	/**
	*@brief	コンストラクタ
	*/
	StageObject() : CGameObject(){
			
	}

	bool Init();

	void Control(){};

	void Draw(){};

	/**
	*@brief	ステージブロックのチェック
	*@param[in]	_player	プレイヤーのID
	*@param[in]	_column	行
	*@param[in]	_line	列
	*@param[in]	_vol	判定するブロックと比較する値
	*/
	bool CheckStageBlock( int _player, int _column, int _line, unsigned char _vol );
	
	/**
	*@brief	ステージブロックのチェック
	*@param[in]	_player	プレイヤーのID
	*@param[in]	_column	行
	*@param[in]	_line	列
	*@param[in]	_vol	判定するブロックと比較する値
	*@return	0：ステージ上の指定範囲とブロックの実体が接触していない場合。@n
				1：ステージ外にブロックの実体があった場合。@n
				2：指定したブロックの範囲にすでにステージ上で何かが存在していた場合。@n
				-1：上記以外の事が発生した場合。
	*/
	int CheckStageBlock( int _player, int _column, int _line, ShipObject* _ship );
	

	void SetShip( ShipObject* _pShip );

	/**
	*@brief	解放処理
	*/
	void  Free();
};

#endif