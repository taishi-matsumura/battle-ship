/**
*@file	ShipObject.h
*@author	Tc
*/

#ifndef _SHIPOBJECT_H_
#define _SHIPOBJECT_H_

#include "gameObject.h"

#define _SHIP_ARRAY_INDEX_	5	///<	îÌzñw

/**
*@brief	îÌîêNX
*/
class ShipObject:public CGameObject
{
public:
	/**
	*@brief	îÌíÞñ
	*/
	enum _SHIP_TYPE_NUM_
	{
		TYPE_NONE=-1,
		TYPE_AIRCARRIER,
		TYPE_BATTLESHIP,
		TYPE_CRUISER,
		TYPE_DESTROYER,
		TYPE_SUBMARINE,
		TYPE_MAX,
	};

private:
	int m_userID;

public:
	int m_shipArray[_SHIP_ARRAY_INDEX_][_SHIP_ARRAY_INDEX_];	///<	îÌÀÌzñ
	int m_actionArray[_SHIP_ARRAY_INDEX_][_SHIP_ARRAY_INDEX_];	///<	îÌs®ÍÍzñBUÈÇÚ®AõGÍÍÈÇÉgp

public:
	/**
	*@brief
	*/
	ShipObject( _SHIP_TYPE_NUM_ _shipType, int _userID );

public:
	/**
	*@brief	îÌ¿åIDæ¾
	*/
	int GetUserID(){ return m_userID; }	
};


#endif