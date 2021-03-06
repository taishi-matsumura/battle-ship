/**
*@file	StageObject.cpp
*@author	亀田
*/

#include "StageObject.h"
#include "StateManager.h"


bool StageObject::Init()
{
	float tempX, tempY;
	//	ステージマス目表示
	//	プレイヤー数
	for ( int iPlayer=0; iPlayer<_PLAYER_NUM_; iPlayer++ ){
		//	行
		for ( int iColumn=0; iColumn<_STAGE_COLUMN_MAX_; iColumn++ ){	
			//	列
			for ( int iLine=0; iLine<_STAGE_LINE_MAX_; iLine++ ){
				m_stageArray[iPlayer][iColumn][iLine] = 0;	///<	プレイヤー1のステージデータ初期化

				//	基準点の計算。ただのループ回数に合わえてブロック幅分座標を調節
				tempX = ( iLine*( _BLOCK_WIDTH_SIZE_ ))+( _BLOCK_WIDTH_SIZE_ );
				tempY = ( iColumn*( _BLOCK_HEIGHT_SIZE_ ))+( _BLOCK_HEIGHT_SIZE_ );
				if ( iPlayer == 1 )	//	プレイヤー2の表示のみX座標をずらす。
					tempX += _BLOCK_WIDTH_SIZE_*11;

				m_stageBlock[iPlayer][iColumn][iLine].Init( tempX, tempY, _BLOCK_WIDTH_SIZE_, _BLOCK_HEIGHT_SIZE_ );
			}
		}
	}
	return true;
}


int StageObject::CheckStageBlock( int _player, const int _column, const int _line, ShipObject* _ship, 
								ShipObject::_SHIP_ARRAY_TYPE_ _arrayType, const int _shipNum )
{
	if ( _player > _PLAYER_NUM_ || _player <= 0 )	
		return -1;	///<	プレイヤーIDが1か2以外だった場合

	int(*_array)[_SHIP_ARRAY_INDEX_];	///<どの配列データとのチェックを行うかの格納用
	_player--;	///<	配列指数用に直す
	
	//---どの配列データをチェックするかの分岐---
	switch ( _arrayType )
	{
	case ShipObject::ARRAY_TYPE_SHIP:
		_array = _ship->m_shipArray;
		break;
	case ShipObject::ARRAY_TYPE_ACTION:
		_array = _ship->m_actionArray;
		break;
	case ShipObject::ARRAY_TYPE_SEARCH:
		_array = _ship->m_searchArray;
		break;
	case ShipObject::ARRAY_TYPE_MOVE:	///<	この関数では移動範囲のチェックだけは出来ない
		return -1;
	
	}//---

	for ( int iColumn = 0, iStageCol = _column-2; iColumn < _SHIP_ARRAY_INDEX_; iColumn++, iStageCol++ ){
		for ( int iLine = 0, iStageLine = _line-2 ; iLine < _SHIP_ARRAY_INDEX_; iLine++, iStageLine++ ){

			bool bStageOutside = false;	///<	駒の配列情報がステージからはみ出てしまう場合のフラグ

			//	指定したブロック中心に5×5マス範囲調べる際に、ステージ外を調べてしまわない様に
			if ( iStageCol >= _STAGE_COLUMN_MAX_ || iStageCol < 0 ||
				iStageLine >= _STAGE_LINE_MAX_ || iStageLine < 0 ) {
				bStageOutside = true;
			}

			//	指定したブロック範囲がステージからはみ出た場合
			if ( bStageOutside ){
				if ( _array[iColumn][iLine] != _CONDITION_NONE_ )
					return 1;	///<	ステージ外にブロックの実体があった場合
			}
			//ステージの中だった場合
			else if ( ConditionOfData(m_stageArray[_player][iStageCol][iStageLine]) != _CONDITION_NONE_ ){
				//	駒が指定先に存在した
				if ( ShipTypeOfData(m_stageArray[_player][iStageCol][iStageLine]) == _shipNum ){
					continue;	///<	自分と同じ艦種（現時点では自分自身）の場合
				}
				else if ( _array[iColumn][iLine] != _CONDITION_NONE_ ){
					return 2;	///<	指定したブロックの範囲にすでにステージ上で(自駒以外の)何かが存在していた場合
				}
			}
		}
	}
	return 0;	///<指定した範囲は何にも接触をしなかった
}


int StageObject::CheckRangeOnStage(  int& _column, int& _line, 
							int _player, const float _x, const float _y, ShipObject* _ship, ShipObject::_SHIP_ARRAY_TYPE_ _arrayType)
{
	if ( _player > _PLAYER_NUM_ || _player <= 0 )	
		return -1;	///<	プレイヤーIDが1か2以外だった場合

	_player--;	///<	配列指数用に直す

	for ( int iColumn = 0, iStageCol = _ship->GetArrayColumn()-2; iColumn < _SHIP_ARRAY_INDEX_; iColumn++, iStageCol++ ){
		for ( int iLine = 0, iStageLine = _ship->GetArrayLine()-2 ; iLine < _SHIP_ARRAY_INDEX_; iLine++, iStageLine++ ){

			bool bStageOutside = false;	///<	駒の配列情報がステージからはみ出てしまう場合のフラグ

			//	指定したブロック中心に5×5マス範囲調べる際に、ステージ外を調べてしまわない様に
			if ( iStageCol >= _STAGE_COLUMN_MAX_ || iStageCol < 0 ||
				iStageLine >= _STAGE_LINE_MAX_ || iStageLine < 0 ) {
				bStageOutside = true;
			}

			if ( !bStageOutside )
			{
				if (m_stageBlock[_player][iStageCol][iStageLine].HitBlockCheck( _x, _y ))
				{
					int iHitResult = SelectOfData(m_stageArray[_player][iStageCol][iStageLine]);
					switch (iHitResult)
					{
					case 0:
					case 1:
					case 2:
						_column = iStageCol;
						_line = iStageLine;
						return iHitResult;
					default:
						return -1;
					}
				}
			}
		}

	}
	return -1;
}


bool StageObject::SetStageToRange( int _player, ShipObject* _ship, const int(*_array)[_SHIP_ARRAY_INDEX_], const int _shipCount )
{
	if ( _player > _PLAYER_NUM_ || _player <= 0 )	
		return false;	///<	プレイヤーIDが1か2以外だった場合

	_player--;	///<	配列指数用に直す
	
	for ( int iColumn = 0, iStageCol = _ship->GetArrayColumn()-2; iColumn < _SHIP_ARRAY_INDEX_; iColumn++, iStageCol++ ){
		for ( int iLine = 0, iStageLine = _ship->GetArrayLine()-2 ; iLine < _SHIP_ARRAY_INDEX_; iLine++, iStageLine++ ){

			bool bStageOutside = false;	///<	駒の配列情報がステージからはみ出てしまう場合のフラグ

			//	指定したブロック中心に5×5マス範囲調べる際に、ステージ外を調べてしまわない様に
			if ( iStageCol >= _STAGE_COLUMN_MAX_ || iStageCol < 0 ||
				iStageLine >= _STAGE_LINE_MAX_ || iStageLine < 0 ) {
				bStageOutside = true;
			}

			//	指定したブロック範囲がステージからはみ出た場合
			if ( bStageOutside ){
				continue;	///<	ステージ外に指定範囲の実体があると、範囲のセットのしようが無いので飛ばす
	
			}
			//	指定されてない
			if (_array[iColumn][iLine] == _CONDITION_NONE_)
			{
				continue;
			}
			//	指定範囲と今チェックしているブロックに自分の駒以外の情報があった場合（中心座標は今は調べる必要はない）
			else if ( ShipTypeOfData(m_stageArray[_player][iStageCol][iStageLine]) != _shipCount && 
				(m_stageArray[_player][iStageCol][iStageLine]/10) != _CONDITION_NONE_ ) 
			{
				SetRange( _player+1, iStageCol, iStageLine, _SELECT_FALSE_ );	
			}
			else
			{
				//----ここで移動後の予定位置（での駒とステージ）のチェックも行う----
				//	移動先は移動出来ない場所だった場合	(_playerに1足しているのは（配列の指数から）元々のプレイヤーIDに直す為)
				if ( CheckStageBlock( _player+1, iStageCol, iStageLine, _ship, ShipObject::ARRAY_TYPE_SHIP, _shipCount ) !=0 ){
					SetRange( _player+1, iStageCol, iStageLine, _SELECT_FALSE_ );	
				}
				//	移動先は駒の配置が可能だった場合
				else{
					
					SetRange( _player+1, iStageCol, iStageLine, _SELECT_TRUE_ );	
				}
				//--------
			}
		}
	}


	return true;
}

bool StageObject::SetShip( int _player, const int _column, const int _line, ShipObject* _ship )
{
	if ( _player > _PLAYER_NUM_ || _player <= 0 )	
		return false;	///<	プレイヤーIDが1か2以外だった場合

	_player--;	///<	配列指数用に直す

	for ( int iColumn = 0, iStageCol = _column-2; iColumn < _SHIP_ARRAY_INDEX_; iColumn++, iStageCol++ ){
		for ( int iLine = 0, iStageLine = _line-2 ; iLine < _SHIP_ARRAY_INDEX_; iLine++, iStageLine++ ){
		
			bool bStageOutside = false;	///<	駒の配列情報がステージからはみ出てしまう場合のフラグ
		
			//	指定したブロック中心に5×5マス範囲調べる際に、ステージ外を調べてしまわない様に
			if ( iStageCol >= _STAGE_COLUMN_MAX_ || iStageCol < 0 ||
				iStageLine >= _STAGE_LINE_MAX_ || iStageLine < 0 ) {
				bStageOutside = true;
			}
			//	ステージ内で、駒がなければ
			if ( !bStageOutside ){
				if ( _ship->m_shipArray[iColumn][iLine] != _CONDITION_NONE_ )
					m_stageArray[_player][iStageCol][iStageLine] = _ship->m_shipArray[iColumn][iLine];
			}
		}

	}

	_ship->SetArrayPos( _column, _line );	///<　配置する行と列の座標をセット
	_ship->SetDeadFlag( false );///<駒を設置したのでオブジェクトの死亡フラグを下げる
						
	//	駒の基準点（中心点）を予め算出させておく
	float tempW = _BLOCK_WIDTH_SIZE_;		///<	ステージ上の1コマのサイズの入力を簡略化
	float tempH = _BLOCK_HEIGHT_SIZE_;		///<	ステージ上の1コマのサイズの入力を簡略化
	float tempX;	
	float tempY;
	if (_player == 0){	//プレイヤーIDが1（=配列の指数だと0）だったら
		tempX = _line*tempW + tempW*1.5f ;
		tempY = _column*tempH + tempH*1.5f;
	}else{
		tempX = (_line+_STAGE_HEIGHT_MAX_)*tempW + tempW*1.5f ;
		tempY = _column*tempH + tempH*1.5f;
	}

	_ship->SetPosition( tempX, tempY, 0.5f );

	return true;
}

bool StageObject::SetRange( int _player, const int _column, const int _line, const _ARRAY_DATA_TYPE_SELECT_ _setType )
{
	if ( _player > _PLAYER_NUM_ || _player <= 0 )	
		return false;	///<	プレイヤーIDが1か2以外だった場合

	_player--;	///<	配列指数用に直す

	//	ステージ内で、今調べてる配列情報の中身が0じゃ無かったら（駒や攻撃などの範囲情報があれば）
	if ( SelectOfData(m_stageArray[_player][_column][_line]) != _CONDITION_NONE_ ){
		//        3桁目は選択情報なので、選択情報だけ消せる様にしている
		m_stageArray[_player][_column][_line] -= 100*SelectOfData(m_stageArray[_player][_column][_line]);
		m_stageArray[_player][_column][_line] += 100*_setType;
	}
	else{
		m_stageArray[_player][_column][_line] += 100*_setType;
	}
	return true;
}


bool StageObject::SetRange( int _player, const int _column, const int _line, 
							const int(*_array)[_SHIP_ARRAY_INDEX_], const int _selectNum )
{
	if ( _player > _PLAYER_NUM_ || _player <= 0 )	
		return false;	///<	プレイヤーIDが1か2以外だった場合

	_player--;	///<	配列指数用に直す

	for ( int iColumn = 0, iStageCol = _column-2; iColumn < _SHIP_ARRAY_INDEX_; iColumn++, iStageCol++ ){
		for ( int iLine = 0, iStageLine = _line-2 ; iLine < _SHIP_ARRAY_INDEX_; iLine++, iStageLine++ ){

			bool bStageOutside = false;	///<	駒の配列情報がステージからはみ出てしまう場合のフラグ

			//	指定したブロック中心に5×5マス範囲調べる際に、ステージ外を調べてしまわない様に
			if ( iStageCol >= _STAGE_COLUMN_MAX_ || iStageCol < 0 ||
				iStageLine >= _STAGE_LINE_MAX_ || iStageLine < 0 ) {
				bStageOutside = true;
			}

			//	ステージ内で、今調べてる配列情報の中身が0じゃ無かったら（駒や攻撃などの範囲情報があれば）
			if ( !bStageOutside && _array[iColumn][iLine] != _CONDITION_NONE_  ){
				switch ( _selectNum )
				{
				case GameState::_SELECT_ACTION_:
					//	ステージ内で、今調べてる配列情報の中身が0じゃ無かったら（駒や攻撃などの範囲情報があれば）
					if ( SelectOfData(m_stageArray[_player][_column][_line]) != _CONDITION_NONE_ ){
						//        3桁目は選択情報なので、選択情報だけ消せる様にしている
						m_stageArray[_player][iStageCol][iStageLine] -= 100*SelectOfData(m_stageArray[_player][iStageCol][iStageLine]);
						m_stageArray[_player][iStageCol][iStageLine] += 100*(_array[iColumn][iLine]+(_ACTION_NOMAL_-1));
					}
					else{
						m_stageArray[_player][_column][_line] += 100*(_array[iColumn][iLine]+(_ACTION_NOMAL_-1));
					}

					break;
				case GameState::_SELECT_SEARCH_:
					//	ステージ内で、今調べてる配列情報の中身が0じゃ無かったら（駒や攻撃などの範囲情報があれば）
					if ( SelectOfData(m_stageArray[_player][iStageCol][iStageLine]) != _CONDITION_NONE_ ){
						//        3桁目は選択情報なので、選択情報だけ消せる様にしている
						m_stageArray[_player][iStageCol][iStageLine] -= 100*SelectOfData(m_stageArray[_player][iStageCol][iStageLine]);
						m_stageArray[_player][iStageCol][iStageLine] += 100*(_array[iColumn][iLine]+(_SEARCH_NOMAL_-1));
					}
					else{
						m_stageArray[_player][iStageCol][iStageLine] += 100*(_array[iColumn][iLine]+(_SEARCH_NOMAL_-1));
					}

					break;
				}
			}
			
		}

	}
	return true;
}


bool StageObject::SetRange( int _player, const int _column, const int _line, const int(*_array)[_SHIP_ARRAY_INDEX_],
					const _ARRAY_DATA_TYPE_SELECT_ _setType )
{
	if ( _player > _PLAYER_NUM_ || _player <= 0 )	
		return false;	///<	プレイヤーIDが1か2以外だった場合

	_player--;	///<	配列指数用に直す

	for ( int iColumn = 0, iStageCol = _column-2; iColumn < _SHIP_ARRAY_INDEX_; iColumn++, iStageCol++ ){
		for ( int iLine = 0, iStageLine = _line-2 ; iLine < _SHIP_ARRAY_INDEX_; iLine++, iStageLine++ ){

			bool bStageOutside = false;	///<	駒の配列情報がステージからはみ出てしまう場合のフラグ

			//	指定したブロック中心に5×5マス範囲調べる際に、ステージ外を調べてしまわない様に
			if ( iStageCol >= _STAGE_COLUMN_MAX_ || iStageCol < 0 ||
				iStageLine >= _STAGE_LINE_MAX_ || iStageLine < 0 ) {
				bStageOutside = true;
			}

			//	ステージ内で、今調べてる配列情報の中身が0じゃ無かったら（駒や攻撃などの範囲情報があれば）
			if ( !bStageOutside && _array[iColumn][iLine] != _CONDITION_NONE_  ){
				m_stageArray[_player][iStageCol][iStageLine] += 100*_setType;
			}
		}

	}
	return true;
}


bool StageObject::RelocationShip( int _player, const int _column, const int _line, ShipObject* _ship, const int _shipNum )
{
	if ( _player > _PLAYER_NUM_ || _player <= 0 )	
		return false;	///<	プレイヤーIDが1か2以外だった場合

	_player--;	///<	配列指数用に直す

	int tempColunm;
	int tempLine;
	_ship->GetArrayPos( tempColunm, tempLine );

	for ( int iColumn = 0, iStageCol = tempColunm-2; iColumn < _SHIP_ARRAY_INDEX_; iColumn++, iStageCol++ ){
		for ( int iLine = 0, iStageLine = tempLine-2 ; iLine < _SHIP_ARRAY_INDEX_; iLine++, iStageLine++ ){
		
			bool bStageOutside = false;	///<	駒の配列情報がステージからはみ出てしまう場合のフラグ
		
			//	指定したブロック中心に5×5マス範囲調べる際に、ステージ外を調べてしまわない様に
			if ( iStageCol >= _STAGE_COLUMN_MAX_ || iStageCol < 0 ||
				iStageLine >= _STAGE_LINE_MAX_ || iStageLine < 0 ) {
				bStageOutside = true;
			}
			/*	ステージ内であり、
				指定した駒のデータがある場合（駒自体は実在していること）
			*/
			if ( !bStageOutside && ShipTypeOfData(m_stageArray[_player][iStageCol][iStageLine]) == _shipNum )
			{
				m_stageArray[_player][iStageCol][iStageLine] -= ShipOfData(m_stageArray[_player][iStageCol][iStageLine]);
			}
		}

	}

	for ( int iColumn = 0, iStageCol = _column-2; iColumn < _SHIP_ARRAY_INDEX_; iColumn++, iStageCol++ ){
		for ( int iLine = 0, iStageLine = _line-2 ; iLine < _SHIP_ARRAY_INDEX_; iLine++, iStageLine++ ){
		
			bool bStageOutside = false;	///<	駒の配列情報がステージからはみ出てしまう場合のフラグ
		
			//	指定したブロック中心に5×5マス範囲調べる際に、ステージ外を調べてしまわない様に
			if ( iStageCol >= _STAGE_COLUMN_MAX_ || iStageCol < 0 ||
				iStageLine >= _STAGE_LINE_MAX_ || iStageLine < 0 ) {
				bStageOutside = true;
			}
			//	ステージ内で、駒がなければ
			if ( !bStageOutside ){
				m_stageArray[_player][iStageCol][iStageLine] += _ship->m_shipArray[iColumn][iLine];
			}
		}
	}

	_ship->SetArrayPos( _column, _line );	///<　配置する行と列の座標をセット
	_ship->SetDeadFlag( false );///<駒を設置したのでオブジェクトの死亡フラグを下げる
						
	//	駒の基準点（中心点）を予め算出させておく
	float tempW = _BLOCK_WIDTH_SIZE_;		///<	ステージ上の1コマのサイズの入力を簡略化
	float tempH = _BLOCK_HEIGHT_SIZE_;		///<	ステージ上の1コマのサイズの入力を簡略化
	float tempX;	
	float tempY;
	if (_player == 0)	//プレイヤーIDが1（=配列の指数だと0）だったら
	{
		tempX = _line*tempW + tempW*1.5f ;		
		tempY = _column*tempH + tempH*1.5f;
	}
	else if (_player == 1)
	{
		tempX = (_line+_STAGE_HEIGHT_MAX_)*tempW + tempW*1.5f ;
		tempY = _column*tempH + tempH*1.5f;
	}
	_ship->SetPosition( tempX, tempY, 0.5f );
	
	return true;
}

void StageObject::ResetSelect()
{
	//	プレイヤー数
	for ( int iPlayer=0; iPlayer<_PLAYER_NUM_; iPlayer++ )
	{	
		//	行
		for ( int iColumn=0; iColumn<_STAGE_COLUMN_MAX_; iColumn++ )
		{	 
			//	列
			for ( int iLine=0; iLine<_STAGE_LINE_MAX_; iLine++ )
			{	
				//	3桁目は選択情報なので、選択情報だけ消せる様にしている
				m_stageArray[iPlayer][iColumn][iLine] -= 100*(m_stageArray[iPlayer][iColumn][iLine]/100);
			}		
		}	
	}
}


//	ステージデータの統合
bool StageObject::MargeStage( ConnectStage* _pStage, const int _playerID, const int _enemyID, const int _customMargeType )
{

	if ( _customMargeType != StateManager::STATE_SET_SHIP )
	{
		//	相手側（_pStage）から貰ってきた、お互いのステージに対して行った、選択情報（3桁目）と、
		//	お互い自身の駒に対しての行動（移動）情報を更新してやる。
		for ( int iColumn = 0; iColumn < _STAGE_COLUMN_MAX_; iColumn++ ){
			for ( int iLine = 0; iLine < _STAGE_LINE_MAX_; iLine++ ){
			//	相手側のステージ
			m_stageArray[_enemyID-1][iColumn][iLine] = 				
				( ShipOfData(_pStage->m_stageArray[_enemyID-1][iColumn][iLine])) +
				( SelectOfData(m_stageArray[_enemyID-1][iColumn][iLine])*100 );
			//	自分側のステージ
			m_stageArray[_playerID-1][iColumn][iLine] = 				
				( ShipOfData(m_stageArray[_playerID-1][iColumn][iLine])) +
				( SelectOfData(_pStage->m_stageArray[_playerID-1][iColumn][iLine])*100 );
			}
		}
	}
	else
	{
		memmove_s( m_stageArray[_enemyID-1], sizeof(int[_STAGE_COLUMN_MAX_][_STAGE_LINE_MAX_]),_pStage->m_stageArray[_enemyID-1] ,sizeof(int[_STAGE_COLUMN_MAX_][_STAGE_LINE_MAX_]));
	}

	return true;
}