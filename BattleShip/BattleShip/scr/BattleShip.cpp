/**
*@file	BattleShip.h
*@author	亀田
*/

#include "BattleShip.h"

void BattleShip::Init( int _userID )
{
	//	ファイル読み込みテスト
	ReadTableData( "table/testTable.csv", ShipObject::TABLE_MAX, _SHIP_ARRAY_INDEX_);
}