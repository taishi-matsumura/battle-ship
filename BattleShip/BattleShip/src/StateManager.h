#ifndef _STATEMANAGHER_H_
#define _STATEMANAGHER_H_

#include "GameState.h"
#include "Player.h"
#include "StageObject.h"
#include "DrawStructList.h"
#include "BoardOfFrame.h"
#include "GameLog.h"
#include "Audio.h"
#include "ReportData.h"

#define _DEAD_COLOR_OF_SHIP_ 0xa0303030	///<自駒が撃破された際のカラー値

/**
*@brief	ステート管理クラス
*@details 全てのステートを管理するクラスです。
		  各ステートやGameSceneクラスだけで判別出来ない処理などもこのクラスで実行します。
*/
class StateManager {
 public:
	/**
	*@brief	戦闘ステートパターンの列挙
	*/
	enum _STATE_NUM_ {
		STATE_NONE = -1,
		STATE_SET_SHIP,
		STATE_SELECTION,
		STATE_RESULT,
		STATE_STAGE_EFFECT,
		STATE_ACTION_REPORT,
	};
	
	/**
	*@brief	コンストラクタ
	*/
	StateManager( Player* const _pPlayer1,Player* const _pPlayer2,
				  StageObject* const _pStageObject,const int _playerID );

	/**
	*@brief	デストラクタ
	*/
	~StateManager() {
		CLASS_DELETE(m_pGameState);
	}

	/**
	*@brief	ステートの初期化
	*/
	void StateInit();
	
	/**
	*@brief	ステートの基本ルーチン処理
	*/
	int StateCotrol();

	/**
	*@brief	ステートの基本描画
	*/
	void StateDraw();

	/**
	*@brief	ステート状態の取得
	*/
	_STATE_NUM_ GetState(){ return m_currentState; }

	/**
	*@brief	各ステート内の経過時間の取得
	*@details	CSceneクラス内のm_sceneTimeを元に取得
	*/
	int GetStateInTime(){ return m_stateTime; }

	/**
	*@brief	各ステート内の経過時間の更新
	*@details	CSceneクラス内のm_sceneTimeを元に更新
	*/
	void UpdateStatInTime( int _elapsed ) {
		if (m_stateTime != _elapsed) {
			//	経過時間が現在の時間と違っていれば、ステート側をインクリメント
			m_pGameState->SetElapsedTimeFromStateInstance( 
				m_pGameState->GetElapsedTimeFromStateInstance()+1);

#ifdef _DEBUG
			std::ostringstream s; 
			m_tempStr1 = "経過時間：";
			s << (m_pGameState->GetElapsedTimeFromStateInstance()/10) <<"."<< (m_pGameState->GetElapsedTimeFromStateInstance()%10) ;
			m_tempStr2 = s.str();
			m_tempStr1 = m_tempStr1+m_tempStr2;
			//	経過時間ログが空じゃ無かったら
			if (!m_gameElapsed.m_logStream.empty()) {
				//	中身（秒数表示ログ）を消す
				LogStream* temp = m_gameElapsed.m_logStream.back();
				delete temp;
				m_gameElapsed.m_logStream.pop_back();
			}
			//	経過後の現在時点での新しい経過時間をログに追加する。
			m_gameElapsed.AddStream(m_tempStr1.c_str());
#endif
		}
		m_stateTime=_elapsed;
	}

	/**
	*@brief	描画クラスセット
	*/
	void SetDraw( CDrawManager* const _pDrawManager ) {
		m_pDrawManager = _pDrawManager;
	}

	/**
	*@brief	マウスクラスセット
	*/
	void SetMouse( CMouse* const _pMouse ) {
		m_pMouse = _pMouse;
	}
	
	/**
	*@brief	音声クラスセット
	*/
	void SetAudio( Audio* const _pAudio ) {
		m_pAudio = _pAudio;
	}

	/**
	*@brief	通信を行うフラグを取得
	*/
	const bool GetConnectFlag(){ return m_connectFlag; }

	/**
	*@breif	通信を行うフラグを更新
	*/
	void SetConnectFlag( const bool _flag ){ m_connectFlag = _flag; }

 private:
	/**
	*@brief	各ステートのチェック
	*@details	各ステートのControlを呼んで、その結果などから、
				パターン変更の必要があるか管理。
	*@return	ステート	
	*@retval 1	ステートパターンの変更の必要あり。*n
	*@retval 0	変更の必要無し
	*@retval -1	ゲーム終了の場合
	*/
	int CheckState();

	/**
	*@brief	ステートパターンの切り替え
	*param[in]	_stateType	
	*@return true：ステートパターンの変更完了。*n
			false：変更失敗
	*/
	bool ChangeState( _STATE_NUM_ _stateType );

	/**
	*@brief	プレイヤーの所持駒情報
	*/
	void DrawPlayersShipInfo();

	/**
	*@brief	盤面の枠の表示
	*/
	void DrawStageFrame();

	/**
	*@brief	盤面上の駒（船）の表示
	*/
	void DrawShipObject( const int _playerIndex );

	/**
	*@brief	盤面のマス目（ブロック）の表示
	*/
	void DrawStageBlock( const int _playerIndex );

	/**
	*@brief	ゲームログの描画
	*/
	void DrawLog();

	/**
	*@brief	ターン表示ログの更新
	*/
	void TurnLogUpdate() {
		char tempNumStr[5]={0};
		std::string tempStr;
		_itoa_s( m_turnCount, tempNumStr, sizeof(tempNumStr), 10 );
		tempStr.append(tempNumStr);
		tempStr += "ターン目";
		m_gameTurn.DeleteStream();
		m_gameTurn.AddStream(tempStr.c_str());
	}

//	通常のメンバ変数
	GameState*		m_pGameState;	///< 戦闘ステートパターンオブジェクトのポインタ
	_STATE_NUM_		m_beforeState;	///< 以前のステートパターン
	_STATE_NUM_		m_currentState;	///< 現在のステートパターン
	StageObject* const	m_pStageObject;	///<	ステージ情報格納ポインタ
	ShipObject::_SHIP_TYPE_NUM_	m_currentShip;	///<	現在選択対象の駒。GameStateと共有するため取り扱いに注意！
	std::string		m_tempStr1,	m_tempStr2; ///< ログに表示する文字列の格納用（ログ表示の度に文字列型を生成するよりメンバに持たせておくほうがコストが少ないと判断しました。）
	int				m_beforeShip;	///<前フレームで選択（対象）されている駒を保持しておく。最初に入ってきた場合は初期値として-1を入れている。
	unsigned int	m_turnCount;	///<現在のターン数

//	他のオブジェクトと値のやり取りする変数
	Player* const 	m_pPlayer1;		///< プレイヤー1情報格納ポインタ
	Player* const 	m_pPlayer2;		///< プレイヤー2情報格納ポインタ
	const int		m_playerID;		///< 起動側のプレイヤーID。GameScene側からもらってくる。

	bool			m_connectFlag;	///<	通信を行うフラグ

	int m_plyaerSelectType;	///< 選択した行動の種類

	int m_resultPlayer;		///< プレイヤー側に起きている戦闘結果
	int m_resultEnemy;		///< 敵側に起きている戦闘結果
	int m_resultBattle;		///< 両者の戦況結果
	unsigned int m_stateTime;	///<	各ステート内で秒数をカウントさせたい時に、シーン側で計測した秒数を渡す為のメンバ

	ReportData		m_reportData;	///< 戦績データ記録オブジェクト

//	ステート共通の描画オブジェクトの情報
	BoardOfFrame	m_StageFrame;		///< ステージ部分のフレームオブジェクト
	BoardOfFrame	m_PlayerFrame[_PLAYER_NUM_];	///< プレイヤー情報のフレームオブジェクト	
	GameLog			m_gameLog;			///< ゲームログオブジェクト
	GameLog			m_gameElapsed;		///< 経過時間出力ログ
	GameLog			m_gameConnectState;	///< 通信状態のログ
	GameLog			m_gameTurn;			///< ターン表示

//	デバイス
	CDrawManager*	m_pDrawManager;	///< 2D描画管理クラスポインタ(constポインタ)
	CMouse*			m_pMouse;		///< マウス管理クラスポインタ(constポインタ)
	Audio*			m_pAudio;		

};


#endif