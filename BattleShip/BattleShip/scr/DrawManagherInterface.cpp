#include "DrawManager.h"

//	�g�p����S�Ẳ摜��ǂݍ���
bool CDrawManager::AllLoadTexture ()
{
	LoadTexture ( "../BattleShip/texture/sea.png", _TEX_BACKGROUND_, 0, 0, 255 );
	LoadTexture ( "../BattleShip/texture/AirCarrier.png", _TEX_AIRCARRIER_, 0, 0, 255 );
	LoadTexture ( "../BattleShip/texture/BattleShip.png", _TEX_BATTLESHIP_, 0, 0, 255 );
	LoadTexture ( "../BattleShip/texture/Cuiser.png", _TEX_CRUISER_, 0, 0, 255 );
	
	return true;
}