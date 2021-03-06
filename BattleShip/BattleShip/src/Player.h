#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>
#include "ShipObject.h"
#include "AircraftCarrier.h"
#include "BattleShip.h"
#include "Cruiser.h"
#include "Destroyer.h"
#include "Submarine.h"

class Player {
 public:
	/**
	*@brief	コンストラクタ
	*/
	Player( int _userID ): m_userID( _userID ) {
		Init( _userID );
	}

	/**
	*@brief	デストラクタ
	*/
	~Player() {
		for ( unsigned int iCount = 0; iCount < m_pShip.size(); iCount++ ) {
			delete m_pShip[iCount];
		}
		m_pShip.clear();
	}

	/**
	*@brief	初期化
	*/
	void Init( int _userID );

	/**
	*@brief	駒情報取得
	*/
	ShipObject* GetShip( const ShipObject::_SHIP_TYPE_NUM_ _shipType );
	
	/**
	*@brief	ユーザーID取得
	*/
	int GetUserID(){ return m_userID; }

	/**
	*@brief	プレイヤーの持ち駒の損害処理
	*@warning	この関数は現時点でResultクラス内でステージ情報を基に行列座標を渡しているので、
				駒の実体すらない行列座標or関係のない駒の種類情報を渡さない様に。
	*@param[in]	_column	行座標
	*@param[in]	_line	列座標
	*@param[in]	_shipType	どの駒の損害処理をするかに必要
	*@return	損害処理の結果
	*@retval	true	そのマスに置いている駒が損傷した
	*@retval	false	すでにそのマスが損傷していた場合(またはエラーの場合)
	*/
	bool DamageControl( const int _column, const int _line, const ShipObject::_SHIP_TYPE_NUM_ _shipType );

	/**
	*@brief	轟沈判定
	*@param[in]	_shipType	どの駒の損害処理をするかに必要
	*@return	轟沈判定の結果
	*@retval	true	すでに轟沈した
	*@retval	false	まだ轟沈していない
	*/
	bool CheckDestroy( const ShipObject::_SHIP_TYPE_NUM_ _shipType );

	int	CheckHitCount( const ShipObject::_SHIP_TYPE_NUM_ _shipType );

 private:	
	int m_userID;
	std::vector<ShipObject*> m_pShip;

};

#endif 
