/**
*@file	StateManager.cpp
*@author	亀田
*/

#include "StateManager.h"

//--------ステートパターンクラスのヘッダー-------
#include "SetShip.h"
#include "Selection.h"
#include "Result.h"
#include "StageEffect.h"
#include "ActionReport.h"
//-------------------------------------------
#include "DebugNew.h"


#define _FIRST_SETATE_ StateManager::STATE_SET_SHIP	///<	最初のステートパターン
#define _POS_PLAYER1FRAME_	0.f, HEIGHT*(_STAGE_HEIGHT_MAX_/_BLOCK_HEIGHT_MAX_)						///<プレイヤー1情報の枠の表示座標
#define _POS_PLAYER2FRAME_	_BLOCK_WIDTH_SIZE_*12, HEIGHT*(_STAGE_HEIGHT_MAX_/_BLOCK_HEIGHT_MAX_)	///<プレイヤー2情報の枠の表示座標
#define _SIZE_PLAYERFRAME_	_BLOCK_WIDTH_SIZE_*11, _BLOCK_HEIGHT_SIZE_*5	///<プレイヤー情報の枠サイズ


//#define _DEBUG_SCENE_TEST_

//	コンストラクタ 
StateManager::StateManager( Player* const _pPlayer1, Player* const _pPlayer2, 
							StageObject* const _pStageObject, const int _playerID) 
	: m_pPlayer1(_pPlayer1), m_pPlayer2(_pPlayer2), m_pStageObject( _pStageObject), m_playerID(_playerID)
{
	m_connectFlag = false;
	m_resultBattle	= static_cast<int>(Result::TYPE_DRAW);
	m_resultPlayer = static_cast<int>(Result::RESULT_NONE);
	m_resultEnemy = static_cast<int>(Result::RESULT_NONE);
	m_plyaerSelectType	= static_cast<int>(GameState::_SELECT_NONE_);
	m_turnCount = 1;
}

//	ステートの初期化
void StateManager::StateInit()
{
	float tempX = 0.f, tempY = 0.f;	
	float tempW = 0.f, tempH = 0.f;

	m_beforeState	= STATE_NONE;		///<	初期化なので以前のステートはそもそも無い為、STATE_NONEに。
	m_currentState	= _FIRST_SETATE_;///<	ステートの初期化なので最初に読み込むであろうパターンの定数を入れる
	m_currentShip	= ShipObject::TYPE_AIRCARRIER;	///<	初期選択駒は空母なので数値を空母にセット。
	
	m_StageFrame.Init( 0.f, -2.f, WIDTH, HEIGHT*(_STAGE_HEIGHT_MAX_/_BLOCK_HEIGHT_MAX_));
	///<画面上部から１１マス分目まで盤面フレームがあるので11/16となる。
	m_PlayerFrame[0].Init( _POS_PLAYER1FRAME_, _SIZE_PLAYERFRAME_ );
	m_PlayerFrame[1].Init( _POS_PLAYER2FRAME_, _SIZE_PLAYERFRAME_ );

	//	ゲームログの初期化
	int plIndex = m_playerID%2;	
	m_gameLog.Init( static_cast<long>( m_PlayerFrame[plIndex].GetPositionX() + static_cast<long>(_LOG_POS_TWEAK_)), 
		static_cast<long>( m_PlayerFrame[plIndex].GetPositionY() + static_cast<long>(_LOG_POS_TWEAK_)) );
	
	//	ゲーム経過時間ログの初期化
	m_gameElapsed.Init( static_cast<long>( m_PlayerFrame[plIndex].GetPositionX() + static_cast<long>(_LOG_POS_TWEAK_) ),
		static_cast<long>( m_PlayerFrame[plIndex].GetPositionY()) + static_cast<long>(_LOG_HEIGHT_MAX_) );
	
	//	通信状態ログの初期化
	tempX = ( m_playerID/_PLAYER_NUM_ ? (_BOARD_OF_SHIPDATA_LINE_P1_) : (_BOARD_OF_SHIPDATA_LINE_P2_+_SHIP_ARRAY_INDEX_) ) * _BLOCK_WIDTH_SIZE_;
	m_gameConnectState.Init( static_cast<long>( tempX ), static_cast<long>( m_PlayerFrame[plIndex].GetPositionY()) + static_cast<long>(_LOG_HEIGHT_MAX_) );
	
	//	ターン表示のログ初期化
	tempX = _STAGE_LINE_MAX_*_BLOCK_WIDTH_SIZE_;
	m_gameTurn.Init( static_cast<long>(tempX), 0, "");
	TurnLogUpdate();

	m_tempStr1 = m_gameLog.m_fixedPhrase.m_phrase[FixedPhrase::START_BATTLE];
	m_gameLog.AddStream( m_tempStr1.c_str(), _LOG_COLOR_DEFAULT_, _LOG_FONT_BIGSIZE_, _LOG_FONT_BIGSIZE_, DT_CENTER );
	
	//	ステート変更（＆初期化）
	ChangeState(m_currentState);	///<	まだステートポイントには何も入っていないので初期化も兼ねて

	m_beforeShip = -1;	///<前フレームで選択（対象）されている駒を保持しておく。最初に入ってきた場合は初期値として-1を入れている。	

	for ( int iPlayer=0; iPlayer<_PLAYER_NUM_; iPlayer++ )	///<表示位置などを予め初期化しておき、描画時や当たり判定時などにも利用する。
	{
		//	艦種分ループで全ての駒を初期化
		for ( int iShip=0; iShip<ShipObject::TYPE_MAX; iShip++ ){

		//	------自駒のステータス（損害情報）の初期化------
			tempW = _BLOCK_WIDTH_SIZE_;
			tempH = _BLOCK_HEIGHT_SIZE_;	///<駒別に可変性があるので縦横幅の仮保存をして少し効率を上げる。
			tempX = tempW;
			tempY = (_STAGE_HEIGHT_MAX_*tempH)+(iShip*tempH);	///<Y座標はまずプレイヤー情報枠の基準点から
			
			//	駒の向きの初期化
			ShipObject* tempShip;
			
			if ( iPlayer == 0 ){
				tempX += tempW*5;	///<プレイヤー1は1マス目から5マス分ずらすので
				tempShip = m_pPlayer1->GetShip( (ShipObject::_SHIP_TYPE_NUM_)iShip );
			}else{
				tempX += tempW*11;	///<プレイヤー2は1マス目から11マス分ずらすので
				tempShip = m_pPlayer2->GetShip( (ShipObject::_SHIP_TYPE_NUM_)iShip );
			}
		}
	}
}

//	ステートの基本ルーチン処理
int StateManager::StateCotrol()
{
	m_beforeState = m_currentState;	///<	ルーチン的にな処理で前フレーム時のステートを現在のステートに合わせる。
	int gameResult = this->CheckState();	///<	ステートの変更チェックの処理
	UpdateStatInTime( m_stateTime );	///<	タイムの更新
	
#ifdef _DEBUG_SCENE_TEST_
	gameResult = -1;

#endif
	if ( gameResult == 1 )	///<	ステートのルーチン処理の結果シーンが変わる必要があれば
	{
		switch ( m_currentState )	///<	変更するステートは順番がある程度決まっているので分岐
		{
		case STATE_SET_SHIP:
			ChangeState(STATE_SELECTION);

			break;
		case STATE_SELECTION:
			ChangeState(STATE_STAGE_EFFECT);

			break;
		case STATE_STAGE_EFFECT:
			ChangeState(STATE_RESULT);
			
			break;
		case STATE_RESULT:
			ChangeState(STATE_SELECTION);

			break;
		}
	}
	else if ( gameResult == 2 )
	{
		ChangeState(STATE_ACTION_REPORT);
	}
	else if ( gameResult == -1 )
	{
		m_pAudio->SoundAllStop();
		/*
			戦闘終了に伴う戦況の最終結果をログに表示
		*/
		m_tempStr1 = "左クリックでタイトルに戻りましょう";
		m_gameLog.AddStream( m_tempStr1.c_str(), _LOG_COLOR_NOMAL_, _LOG_FONT_BIGSIZE_, _LOG_FONT_BIGSIZE_, DT_CENTER );
		/**/
	}
	
	return gameResult;
}

int StateManager::CheckState()
{
	
	int checkResult = 0;
	int stateResult = 0;

	//	戦闘続行中なら各ステートの処理を通す
	//	通信を必要とするステートはSetShip、Selectionのみなので、他のステートではこのm_connectFlagには干渉しない事。
	m_pGameState->SetConnectFlag( m_connectFlag );	//	毎フレーム通信のフラグを更新
	stateResult = m_pGameState->Control();	///<　ステートごとの処理に移行
	SetConnectFlag( m_pGameState->GetConnectFlag() );	//	ステート内でのフラグ変更を反映させる
	
	switch ( m_currentState )	///<　シーン毎にステートの結果への対処が変わるので分岐
	{
	//	駒が配置され、配置情報などの送受信が完了するまでループ
	case STATE_SET_SHIP:
		//	駒配置を促すログを出す
		if ( m_beforeShip != m_currentShip && m_currentShip != ShipObject::TYPE_MAX )
		{
			m_tempStr2 = m_gameLog.m_fixedPhrase.m_phrase[FixedPhrase::SET_PHRASE_STR];
			
			m_tempStr1 = m_gameLog.GetMultiplePhrase( 2, static_cast<int>(m_currentShip), static_cast<int>(FixedPhrase::SET_PHRASE_STR) );
			m_gameLog.AddStream( m_tempStr1.c_str() );
		}

		if ( m_currentShip >= ShipObject::TYPE_MAX && !m_connectFlag )	///<　全ての駒がセットされた
			checkResult = 1;
		break;
	//	選択が完了して、その情報の送受信が完了するまでループ
	case STATE_SELECTION:
		if ( stateResult == 1 ){	///<　結果が1且つ、通信が完了していた場合
			static bool selectTimeUpdateFlag = false;	///< 行動決定までの時間計測の更新のためのフラグ
			
			if ( !selectTimeUpdateFlag )
			{
				Player* _pPlayer = m_playerID%_PLAYER_NUM_? m_pPlayer1: m_pPlayer2;
				bool deadFlag = _pPlayer->CheckDestroy( m_currentShip );
				//	もし駒が轟沈していた場合、行動選択の更新をしてしまうとダメなので調べておく
				if (!deadFlag)
					m_reportData.UpdateSelectAveTime( m_pGameState->GetElapsedTimeFromStateInstance() );	///< 行動決定までの時間の更新
				
				selectTimeUpdateFlag = true;
			}
			if ( !m_connectFlag )
			{
				Selection* pSelection = dynamic_cast<Selection*>(m_pGameState);	///<Selectionの関数にアクセスする必要があるので、ダウンキャストする。
				m_plyaerSelectType = pSelection->GetSelectionType();
				m_reportData.UpdateSelectCount( m_plyaerSelectType );	///< 選択した行動の回数カウントの更新

				checkResult = 1;	///<　エフェクトに移る
				selectTimeUpdateFlag = false;
			}
			
		}
		break;
	//	エフェクトが終わるまでループ
	case STATE_STAGE_EFFECT:
		if ( stateResult == 1 ){	///<　結果が1(ステージの演出が完了)の場合
			
			checkResult = 1;
		}
		break;

	case STATE_ACTION_REPORT:
		if ( stateResult == 1 ){	///<　結果が1(戦績結果終了)の場合
			
			checkResult = -1;
		}
		break;

	//	戦闘結果を判定しているだけなので事実上1フレームしか入らない。
	case STATE_RESULT:

		Result* pResult = dynamic_cast<Result*>(m_pGameState);	///<Resultの関数にアクセスする必要があるので、ダウンキャストする。	
		pResult->GetResultOfBattle( m_resultBattle );	///< 戦闘結果を取得
		pResult->GetResultPlayerAndEnemy( m_resultPlayer, m_resultEnemy );
		m_reportData.UpdateResultCount( m_resultEnemy );
		m_reportData.SetTurnCount(m_turnCount);

		//勝利or敗北or戦闘終了
		if ( stateResult == Result::TYPE_VICTORY ||
			stateResult == Result::TYPE_DEFEAT ||
			stateResult == Result::TYPE_STALEMATE )
		{
			if ( stateResult != Result::TYPE_DEFEAT )
				checkResult = 2;	//StateManager側に戦闘結果＝戦闘終了した事を教えてやる。
			else
				checkResult = -1;	///負けた時は戦績の報告はしない。

			m_pStageObject->ResetSelect();	//判定を取ったので選択情報は消す
			//	プレイヤー自身のポインタを取得
			Player* pPlayerPtr = m_playerID/_PLAYER_NUM_? m_pPlayer2 : m_pPlayer1;
			m_reportData.UpdateKOCount( pPlayerPtr );
			/*
			戦闘終了に伴う戦況の最終結果をログに表示
			*/
			if ( m_resultBattle == Result::TYPE_STALEMATE )
			{
				m_tempStr1 = m_gameLog.GetPhrase( FixedPhrase::RESULT_STALEMATE_STR );
				m_pAudio->SoundPlay( Audio::_LOSE_BGM_, true );
			}
			else if ( m_resultBattle == Result::TYPE_VICTORY )
			{
				m_tempStr1 = m_gameLog.GetPhrase( FixedPhrase::RESULT_VICTORY_STR );
				m_pAudio->SoundPlay( Audio::_WIN_BGM_, true );
			}
			else if ( m_resultBattle == Result::TYPE_DEFEAT )
			{
				m_tempStr1 = m_gameLog.GetPhrase( FixedPhrase::RESULT_DEFEAT_STR );
				m_pAudio->SoundPlay( Audio::_LOSE_BGM_, true );
			}
			m_gameLog.AddStream( m_tempStr1.c_str(), _LOG_COLOR_DEFAULT_, _LOG_FONT_BIGSIZE_, _LOG_FONT_BIGSIZE_, DT_CENTER );
			//	戦闘終了ログを表示
			m_tempStr1 = m_gameLog.GetPhrase( FixedPhrase::RESULT_END );
			m_gameLog.AddStream( m_tempStr1.c_str(), _LOG_COLOR_DEFAULT_, _LOG_FONT_BIGSIZE_, _LOG_FONT_BIGSIZE_, DT_CENTER );
		}
		//勝敗はまだついていない
		else	
		{
			checkResult = 1;
			m_pStageObject->ResetSelect();	//判定を取ったので選択情報は消す
			
			// Selectionステートに移る前に現在のターンが終了したので次の駒を指定
			if ( m_currentShip < ShipObject::TYPE_SUBMARINE){

				m_currentShip = static_cast< ShipObject::_SHIP_TYPE_NUM_ >( m_beforeShip+1 );
			}else{
				//	各艦の行動フェーズが一周したのでターン数を増やす
				m_turnCount++;
				TurnLogUpdate();	///< ターンログのアップデートも行う
				m_currentShip = ShipObject::TYPE_AIRCARRIER;
			}
		}
		break;
	
	}

	// ステート別Controlが終わったので前フレームでの選択駒を更新
	m_beforeShip = static_cast<int>(m_currentShip);
	return checkResult;
}

//	ステートパターンの切り替え
bool StateManager::ChangeState( _STATE_NUM_ _stateType )
{
	if ( _stateType > STATE_STAGE_EFFECT&& _stateType < STATE_SET_SHIP )
	{
		MessageBoxA(0,"ステートパターンの変更に失敗しました！\n引数を確認して下さい！(＞＜;)",NULL,MB_OK);
		return false;
	}
	
	//	すでにステートが決まっていた場合（変更する際）は前のインスタンス生成したものを消す必要がある。
	if ( m_beforeState != STATE_NONE )
	{
		CLASS_DELETE(m_pGameState); 
	}
	//	各ステート別にステート生成
	switch ( _stateType )
	{
	case STATE_SET_SHIP:
		m_tempStr1 = m_gameLog.m_fixedPhrase.m_phrase[FixedPhrase::STATE_SET_SHIP_STR];
		m_gameLog.AddStream(m_tempStr1.c_str(), _LOG_COLOR_DEFAULT_, _LOG_FONT_BIGSIZE_, _LOG_FONT_BIGSIZE_, DT_CENTER );
		// 現状は最初に配置するのは空母で、CheckState関数内では空母のログ
		m_pGameState = new SetShip( m_currentShip, &m_gameLog );

		break;
	case STATE_SELECTION:
		m_tempStr1 = m_gameLog.m_fixedPhrase.m_phrase[FixedPhrase::STATE_SELECTION_STR];
		m_gameLog.AddStream(m_tempStr1.c_str(), _LOG_COLOR_DEFAULT_, _LOG_FONT_BIGSIZE_, _LOG_FONT_BIGSIZE_, DT_CENTER );
		m_pGameState = new Selection( m_currentShip, &m_gameLog );

		break;
	case STATE_STAGE_EFFECT:
		m_pGameState = new StageEffect( m_currentShip, &m_gameLog );
		static_cast<StageEffect*>(m_pGameState)->SetSelectionType( m_plyaerSelectType );	///<エフェクトにはどの行動を選択したかの判断がいるので情報を渡してやる。
		
		break;
	case STATE_RESULT:
		m_tempStr1 = m_gameLog.m_fixedPhrase.m_phrase[FixedPhrase::STATE_RESULT_STR];
		m_gameLog.AddStream(m_tempStr1.c_str(), _LOG_COLOR_DEFAULT_, _LOG_FONT_BIGSIZE_, _LOG_FONT_BIGSIZE_, DT_CENTER );
		m_pGameState = new Result( m_currentShip, &m_gameLog );

		break;
	case STATE_ACTION_REPORT:
		m_pGameState = new ActionReport( m_currentShip, &m_gameLog, m_reportData );

		break;
	}


	m_currentState = _stateType;	///<	現在のステート変数を更新
	
	if ( m_currentShip >= ShipObject::TYPE_MAX )	///<	ステート変更のついでに選択駒が範囲を超えているかチェック
		m_currentShip = ShipObject::TYPE_AIRCARRIER;	///<	空母に変更

	//	ステートが変わったので、一連の初期化を行う
	m_pGameState->SetPlayerPtr( m_pPlayer1, 0 );
	m_pGameState->SetPlayerPtr( m_pPlayer2, 1 );
	m_pGameState->SetStagePtr( m_pStageObject );
	m_pGameState->SetDraw( m_pDrawManager );
	m_pGameState->SetMouse( m_pMouse );
	m_pGameState->SetAudio( m_pAudio );
	m_pGameState->SetPlayerID( m_playerID );
	m_pGameState->Init();	///<最後にステート側の初期化も行う

	return true;
}

//	ステートの基本描画
void StateManager::StateDraw()
{	
	DrawPlayersShipInfo();

	//	プレイヤー数ループして、駒とマス目を描画する
	for ( int ip=0; ip<_PLAYER_NUM_; ip++ )
	{
		DrawShipObject( ip );
		DrawStageBlock( ip );
	}

	/**
	*@brief	各ステート描画
	*@details	例外的にActionReportステートとSelectionだけは最後に描画させる。
	*/
	if ( m_currentState != STATE_ACTION_REPORT &&
		m_currentState != STATE_SELECTION )
		m_pGameState->Draw();

	DrawStageFrame();

	DrawLog();

	//	ActionReportとSelectionのみ最後に描画
	if ( m_currentState == STATE_ACTION_REPORT ||
		m_currentState == STATE_SELECTION )
		m_pGameState->Draw();


	//	通信待ち中
	if ( GetConnectFlag() )
	{
		m_pGameState->ComStandby( m_gameConnectState );
	}
}

void StateManager::DrawPlayersShipInfo()
{
	float tempX, tempY;

	if ( m_playerID == 1 )
	{
		//	プレイヤー1枠表示
		m_PlayerFrame[0].GetPosition( &tempX, &tempY );
		m_pDrawManager->VertexDraw( _TEX_STAGEMAP_, tempX, tempY, 
			m_PlayerFrame[0].GetWidth(),  m_PlayerFrame[0].GetHeight(),
			0.f, _STAGE_HEIGHT_MAX_/_BLOCK_HEIGHT_MAX_, 
			11/_BLOCK_WIDTH_MAX_, 1.f,
			180, 255, 100, 100);	///<	プレイヤー1の枠描画
		
	}
	else
	{
		//	プレイヤー2枠表示
		m_PlayerFrame[1].GetPosition( &tempX, &tempY );
		m_pDrawManager->VertexDraw( _TEX_STAGEMAP_, tempX, tempY, 
			m_PlayerFrame[1].GetWidth(),  m_PlayerFrame[1].GetHeight(),
			12/_BLOCK_WIDTH_MAX_, _STAGE_HEIGHT_MAX_/_BLOCK_HEIGHT_MAX_, 
			1.f, 1.f,
			180, 100, 100, 255);	///<	プレイヤー2の枠描画
	}

}

void StateManager::DrawStageFrame()
{	
	float tempX, tempY;
	
	//	盤面枠表示（左）
	m_StageFrame.GetPosition( &tempX, &tempY );
	m_pDrawManager->VertexDraw( _TEX_STAGEMAP_, tempX, tempY, 
		m_StageFrame.GetWidth()*(_STAGE_WIDTH_MAX_/_BLOCK_WIDTH_MAX_),  m_StageFrame.GetHeight(),
		0.f, 0.f, 
		_STAGE_WIDTH_MAX_/_BLOCK_WIDTH_MAX_, _STAGE_HEIGHT_MAX_/_BLOCK_HEIGHT_MAX_,
		180, 220, 220, 220);	///<	盤面の左側の描画

	//	盤面枠表示（右）
	m_pDrawManager->VertexDraw( _TEX_STAGEMAP_, tempX+_BLOCK_WIDTH_SIZE_*12, tempY, 
		m_StageFrame.GetWidth()*(_STAGE_WIDTH_MAX_/_BLOCK_WIDTH_MAX_),  m_StageFrame.GetHeight(),
		(_STAGE_WIDTH_MAX_+1)/_BLOCK_WIDTH_MAX_, 0.f, 
		1.f, _STAGE_HEIGHT_MAX_/_BLOCK_HEIGHT_MAX_,
		180, 220, 220, 220);	///<	盤面の右側の描画

	//	盤面枠表示（中）
	m_pDrawManager->VertexDraw( _TEX_STAGEMAP_, tempX+_BLOCK_WIDTH_SIZE_*_STAGE_WIDTH_MAX_, tempY, 
		_BLOCK_WIDTH_SIZE_, HEIGHT,
		_STAGE_WIDTH_MAX_/_BLOCK_WIDTH_MAX_, 0.f,
		(_STAGE_WIDTH_MAX_+1)/_BLOCK_WIDTH_MAX_, 1.f,
		80, 200, 200, 200);	///<	盤面の真ん中の描画

	
}

void StateManager::DrawShipObject( const int _playerIndex )
{
	//	プレイヤー別の駒情報の表示
	for ( int iShip=0; iShip<ShipObject::TYPE_MAX; iShip++ )
	{
		//	ステージ上の駒の表示
		float tempW = _BLOCK_WIDTH_SIZE_;	
		float tempH = _BLOCK_HEIGHT_SIZE_;
		
		if ( _playerIndex == m_playerID-1 )	///<自分のプレイヤーの駒しか表示しない
		{
			Player* tempPlayer = m_playerID%_PLAYER_NUM_ ? m_pPlayer1:m_pPlayer2;	//描画に必要なプレイヤー情報を取得
			ShipObject* tempShip = tempPlayer->GetShip( (ShipObject::_SHIP_TYPE_NUM_)iShip ); //描画に必要な駒情報を取得
			const int texNum = iShip+_TEX_AIRCARRIER_;	//描画する対象の駒テクスチャを取得（）
			const float shipRotation = tempShip->GetDirection()*90.f;

			if (!tempShip->GetDeadFlag()) {
				//	盤面上に置かれている自分の駒を描画
				m_pDrawManager->VertexTransform( texNum,tempShip->m_vertex, 
												 tempShip->GetPositionX(),
												 tempShip->GetPositionY(),
												 1.f,1.f,tempShip->GetDirection()*90.f );

				if (_playerIndex== 0) {	//	プレイヤー1の場合

					//	配置したものからプレイヤー側の所持駒情報も描画
					m_pDrawManager->VertexTransform( texNum,tempShip->m_vertex,
													 (_BOARD_OF_SHIPDATA_LINE_P1_*tempW)+(tempW/2),
													 ((_BOARD_OF_SHIPDATA_COLUMN_+iShip)*tempH)+(tempH/2),
													 1.f,1.f,CGameObject::CH_RIGHT*90.f );
				}
				else {	//	プレイヤー2の場合

					//	配置したものからプレイヤー側の所持駒情報も描画
					m_pDrawManager->VertexTransform( texNum,tempShip->m_vertex,
													 (_BOARD_OF_SHIPDATA_LINE_P2_*tempW)+(tempW/2),
													 ((_BOARD_OF_SHIPDATA_COLUMN_+iShip)*tempH)+(tempH/2),
													 1.f,1.f,CGameObject::CH_RIGHT*90.f );
				}
				
			}
			else if (m_currentState != STATE_SET_SHIP) {
				//	撃沈された艦はカラー値を暗めにして描画
				if (tempShip->m_vertex[0].color != _DEAD_COLOR_OF_SHIP_) {
					m_pDrawManager->ChangeVertexColor( tempShip->m_vertex, _DEAD_COLOR_OF_SHIP_ );
				}

				if (_playerIndex== 0) {	//	プレイヤー1の場合
					//	配置したものからプレイヤー側の所持駒情報も描画
					m_pDrawManager->VertexTransform( texNum,tempShip->m_vertex,
													 (_BOARD_OF_SHIPDATA_LINE_P1_*tempW)+(tempW/2),
													 ((_BOARD_OF_SHIPDATA_COLUMN_+iShip)*tempH)+(tempH/2),
													 1.f,1.f,CGameObject::CH_RIGHT*90.f );

				}
				else {	//	プレイヤー2の場合

					//	配置したものからプレイヤー側の所持駒情報も描画
					m_pDrawManager->VertexTransform( texNum,tempShip->m_vertex,
													 (_BOARD_OF_SHIPDATA_LINE_P2_*tempW)+(tempW/2),
													 ((_BOARD_OF_SHIPDATA_COLUMN_+iShip)*tempH)+(tempH/2),
													 1.f,1.f,CGameObject::CH_RIGHT*90.f );
				}
			}
		}
		
	}
		
}


void StateManager::DrawStageBlock( const int _playerIndex )
{
	float tempX, tempY;

	//	行
	for ( int ic=0; ic<_STAGE_COLUMN_MAX_; ic++ )
	{	
		//	列
		for ( int il=0; il<_STAGE_LINE_MAX_; il++ )
		{
			m_pStageObject->m_stageBlock[_playerIndex][ic][il].GetPosition( &tempX, &tempY );
			
			int tempA = 0, tempR = 200, tempG = 200, tempB = 200;
			int tempArrayData = m_pStageObject->m_stageArray[_playerIndex][ic][il];
		
			//	選択マスor損傷マスは見えるようにする
			if ( StageObject::SelectOfData(tempArrayData) != StageObject::_SELECT_NOMAL_ ||
				StageObject::ConditionOfData(tempArrayData) == StageObject::_CONDITION_DAMAGE_)
			{
					
				//	範囲指定桁チェック
				switch ( StageObject::SelectOfData(tempArrayData) )
				{
				case StageObject::_SELECT_TRUE_:	///<選択されているマス
					tempA = 100;
					break;
				case StageObject::_SELECT_FALSE_:	///<駒が置けないor選択範囲が何かに接触しているマス
					tempA = 100, tempR = 255, tempG = 0, tempB = 0;
					break;
				case StageObject::_SEARCH_NOMAL_:
				case StageObject::_SEARCH_ALL_:
					tempA = 100, tempR = 0, tempG = 255, tempB = 0;
					break;
				case StageObject::_ACTION_NOMAL_:
				case StageObject::_ACTION_ALL_:
					tempA = 100, tempR = 0, tempG = 0, tempB = 255;
					break;
				}
	
				//	損傷状態桁チェック
				switch ( StageObject::ConditionOfData(tempArrayData) )
				{
				case StageObject::_CONDITION_NONE_:
				case StageObject::_CONDITION_NOMAL_:
					break;
				case StageObject::_CONDITION_DAMAGE_:
					tempA = 200, tempR = 100, tempG = 50, tempB = 50;
					break;
	
				}
				
			m_pDrawManager->VertexDraw( _TEX_BLOCK_, tempX, tempY, 
				m_pStageObject->m_stageBlock[_playerIndex][ic][il].GetWidth(), 
				m_pStageObject->m_stageBlock[_playerIndex][ic][il].GetHeight(),
				0.f, 0.f, 
				1.f, 1.f,
				tempA, tempR, tempG, tempB);	///<	マスの描画
			}
			m_pDrawManager->VertexDraw( _TEX_BLOCKFRAME_, tempX, tempY, 
				m_pStageObject->m_stageBlock[_playerIndex][ic][il].GetWidth(), 
				m_pStageObject->m_stageBlock[_playerIndex][ic][il].GetHeight(),
				0.f, 0.f, 
				1.f, 1.f,
				100, 200, 200, 200);	///<	マスの描画
		}
	}
}

void StateManager::DrawLog()
{
	DirectXFont* const pDxFont =  &m_pDrawManager->m_dxFont;
	int logValue = 0;
	float fTempX, fTempY;
	unsigned int iTempW = 0, iTempH = 0;
	long lTempX = 0, lTempY = 0;

	//ログの枠表示
	if ( m_playerID == 1 )
	{
		//	ゲームログ表示
		m_PlayerFrame[1].GetPosition( &fTempX, &fTempY );
		m_pDrawManager->VertexDraw( _TEX_GAMELOG_, fTempX, fTempY, 
			m_PlayerFrame[1].GetWidth(),  m_PlayerFrame[1].GetHeight(),
			0.f, 0.f, 1.f, 1.f,
			200, 255, 255, 255 );	///<	プレイヤー2の枠描画
	}
	else
	{
		//	ゲームログ表示
		m_PlayerFrame[0].GetPosition( &fTempX, &fTempY );
		m_pDrawManager->VertexDraw( _TEX_GAMELOG_, fTempX, fTempY, 
			m_PlayerFrame[0].GetWidth(),  m_PlayerFrame[0].GetHeight(),
			0.f, 0.f, 1.f, 1.f,
			200, 255, 255, 255 );	///<	プレイヤー1の枠描画
	}
	
	//	戦闘系ログ
	if ( !m_gameLog.m_logStream.empty() )
	{
		logValue = m_gameLog.m_logStream.size();
		lTempX = 0; 
		lTempY = 0;
		iTempW = 0; 
		iTempH = 0;
		std::string* pStr;
		LPCSTR lpTempStr;
		std::list<LogStream*>::const_iterator itEnd = m_gameLog.m_logStream.end();
		for ( std::list<LogStream*>::iterator it = m_gameLog.m_logStream.begin();
			it != itEnd; ++it)
		{
			(*it)->GetPosition( lTempX, lTempY );
			(*it)->GetSize( iTempW, iTempH );
			pStr = (*it)->GetStringPtr();
			lpTempStr = const_cast<char *>(pStr->c_str());
			if (pDxFont->DrawA( lTempX, lTempY, static_cast<unsigned long>(_LOG_FONT_WIDTH_), 
							iTempH, lpTempStr, (*it)->GetColor(), (*it)->GetFormat() ))
			{

			}
		}
	}
#ifdef _DEBUG
	//	経過時間ログ
	if ( !m_gameElapsed.m_logStream.empty() )
	{
		logValue = m_gameElapsed.m_logStream.size();
		lTempX = 0; 
		lTempY = 0;
		iTempW = 0; 
		iTempH = 0;
		std::string* pStr;
		LPCSTR lpTempStr;
		std::list<LogStream*>::iterator it = m_gameElapsed.m_logStream.begin();
		(*it)->GetPosition( lTempX, lTempY );
		(*it)->GetSize( iTempW, iTempH );
		pStr = (*it)->GetStringPtr();
		lpTempStr = const_cast<char *>(pStr->c_str());
		if (pDxFont->DrawA( lTempX, lTempY, iTempW, iTempH, lpTempStr, (*it)->GetColor(), (*it)->GetFormat() ))
		{

		}
	}
#endif
	//	ターン数ログ
	if ( !m_gameTurn.m_logStream.empty() )
	{
		logValue = m_gameTurn.m_logStream.size();
		lTempX = 0; 
		lTempY = 0;
		iTempW = 0; 
		iTempH = 0;
		std::string* pStr;
		LPCSTR lpTempStr;
		std::list<LogStream*>::iterator it = m_gameTurn.m_logStream.begin();
		(*it)->GetPosition( lTempX, lTempY );
		(*it)->GetSize( iTempW, iTempH );
		fTempX = static_cast<float>(lTempX);
		fTempY = static_cast<float>(lTempY);
		m_pDrawManager->VertexDraw( _TEX_GAMELOG_, fTempX, fTempY, 
			_BLOCK_WIDTH_SIZE_*3.f, _BLOCK_HEIGHT_SIZE_*1.f,
			1.f, 1.f, 0.f, 0.f,
			150, 255, 255, 255 );	///< ターン数表示の枠描画

		pStr = (*it)->GetStringPtr();
		lpTempStr = const_cast<char *>(pStr->c_str());
		if (pDxFont->DrawA( lTempX+static_cast<long>(_LOG_POS_TWEAK_), lTempY+static_cast<long>(_LOG_POS_TWEAK_),
						static_cast<unsigned long>(_LOG_FONT_WIDTH_), 
						iTempH, lpTempStr, (*it)->GetColor(), (*it)->GetFormat() ))
		{

		}
	}


	//	通信待ち中
	if ( GetConnectFlag() )
	{
		//	通信中ログ
		if ( !m_gameConnectState.m_logStream.empty() )
		{
			logValue = m_gameConnectState.m_logStream.size();
			lTempX = 0; 
			lTempY = 0;
			iTempW = 0; 
			iTempH = 0;
			std::string* pStr;
			LPCSTR lpTempStr;
			std::list<LogStream*>::iterator it = m_gameConnectState.m_logStream.begin();
			(*it)->GetPosition( lTempX, lTempY );
			(*it)->GetSize( iTempW, iTempH );
			pStr = (*it)->GetStringPtr();
			lpTempStr = const_cast<char *>(pStr->c_str());
			if (pDxFont->DrawA( lTempX, lTempY, iTempW, iTempH, lpTempStr, (*it)->GetColor(), (*it)->GetFormat() ))
			{

			}
		}
	}
	
}
