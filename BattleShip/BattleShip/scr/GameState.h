/**
*@file	GameState.h
*@author	亀田
*/

#ifndef	_GAMESTATE_H_
#define _GAMESTATE_H_

class GameState
{
public:
	/**
	*@brief	ゲーム内でのステートパターンの列挙。
	*@details	主にステート切り替え時に使用
	*/
	enum	_STATE_NUM_
	{
		STATE_NONE,
		STATE_SET_SHIP,
		STATE_SELECTION,
		STATE_RESULT,
		STATE_STAGE_EFFECT,
	};

private:
	_STATE_NUM_ m_stateID;	///<	自身のステートのID	

public:
	virtual void StateControl() = 0;	///<	ステートパターン内でのサブルーチン処理
	virtual void StateDraw() = 0;		///<	ステートパターン内での描画
	virtual void ComStandby() = 0;		///<	ステートパターン内での通信時のサブルーチン処理
	_STATE_NUM_ GetState(){ return m_stateID; }	///<	ステートのID取得
};

#endif