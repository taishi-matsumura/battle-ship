/**
*@file	StageEffect.h
*@author	亀田
*/

#ifndef _STAGEEFFECT_H_
#define _STAGEEFFECT_H_

#include "GameState.h"

#define _MOVE_SPEED_RECON_	33.f
#define _DIVISON_OF_EFFECT_TIME_	1.5

/**
*@brief	ステージ演出ステートクラス
*@details	攻撃演出などを行うステートです
*/
class StageEffect : public GameState {
 public:
	/**
	*@brief	ステージ上の演出の時間
	*/
	enum _STAGE_EFFECT_TIME_ {

		TIME_END_ACTION_EFFECT = 30,	///< 攻撃終了タイム

		TIME_CHANGE_EFFECT = 30+TIME_END_ACTION_EFFECT, ///< 中間演出終了タイム（航空機の爆弾投下音などを入れる必要があったので）

		TIME_END_RUSULT_EFFECT = 30+TIME_CHANGE_EFFECT, ///< 最終エフェクト終了タイム（爆発エフェクトなど）
	};

	/**
	*@brief	コンストラクタ
	*@param	_type	現在選択している艦の種類
	*/
	StageEffect( ShipObject::_SHIP_TYPE_NUM_& _type, GameLog* _pGameLog )
				 : GameState( _type, _pGameLog ) {
		m_plyaerSelectType = _SELECT_NONE_;
		m_enemySelectType = _SELECT_NONE_;
		m_plTargetPointX = 0.f;
		m_plTargetPointY = 0.f;
		m_enTargetPointX = 0.f;
		m_enTargetPointY = 0.f;
		m_actionShipPosColumn	= -1;
		m_actionShipPosLine		= -1;
	}

	/**
	*@brief	デストラクタ
	*/
	~StageEffect() {
		m_playerSelect.clear();
		m_enemySelect.clear();
	};

//	基本処理
	/**
	*@brief	戦闘エフェクトでの初期化
	*/
	bool Init();

	/**
	*@brief	戦闘エフェクトでのサブルーチン処理
	*/
	int Control();
	
	/**
	*@brief	戦闘エフェクトでの描画
	*/
	void Draw();

//	外部取得系
	/**
	*@brief	選択した行動の種類を更新
	*@details	SelectionステートからStateManagerが行動の選択情報をもらっているので、
				StageEffectステートがどの行動のエフェクトをすればいいか判断するために、必要だと考えました。
	*/
	void SetSelectionType( int _selectType ) { 
		m_plyaerSelectType = static_cast<_SELECT_NUM_>(_selectType); 
	}

 private:
//	初期化系
	/**
	*@brief	ステージの選択状況のチェック
	*/
	void CheckSelectOfStage();

	/**
	*@brief	自駒の座標チェック
	*/
	void CheckOfMyShipPos();

//	エフェクトのサウンドの処理
	void EffectSoundControl();

//	エフェクトの描画分岐処理
	/**
	*@brief	攻撃開始エフェクト処理
	*/
	void AttackStartControl();

	/**
	*@brief	索敵開始エフェクト処理
	*/
	void SearchStartControl();

	/**
	*@brief	着弾エフェクト処理
	*@param[in]	_playerIndex	どちらのプレイヤーが索敵をするのかを判断するため
	*/
	void HitEffectControl();

	/**
	*@brief	索敵結果エフェクト処理
	*@param[in]	_playerID	どちらのプレイヤーが索敵をするのかを判断するため
	*/
	void SearchResultControl();


//	エフェクト別描画
	/**
	@brief	発射エフェクト処理
	*/
	void FireEffect( BoardOfFrame& _block );

	/**
	*@brief	ソナーエフェクト
	*/
	void SonarEffect( BoardOfFrame& _block );

	/**
	*@brief	航空機エフェクト
	*@details _appearanceInvisibility	現状は対戦相手が戦闘機などを飛ばす時に徐々にアルファ値をいじる。
										描画座標などもこのFlagがtrueの時は敵のデータ（m_enTargetVectorなど）基準となる。
	*@param[in] _block	出現位置を決めるブロック
	*@param[in] _aircraftType	0：攻撃機、1：索敵機
	*@param[in]	_appearanceInvisibility	出現位置を見えなくさせるフラグ
	*/
	void AircraftEffect( BoardOfFrame& _block, int _aircraftType, bool _appearanceInvisibility = false );

	/**
	*@brief	魚雷攻撃エフェクト
	*@details	航空機エフェクトを一部流用して、作りました
	*/
	void TorpedoEffect( BoardOfFrame& _block, bool _appearanceInvisibility = false );

	/**
	*@brief	爆発エフェクト
	*/
	void ExplosionEffect( BoardOfFrame& _block );

	_SELECT_NUM_ m_plyaerSelectType;	///< 選択した行動の種類
	_SELECT_NUM_ m_enemySelectType;	///< 相手の選択した行動
	
	std::vector<BoardOfFrame> m_playerSelect;///< 自分が敵に対して指定したマスのコピー用
	std::vector<BoardOfFrame> m_enemySelect; ///< 敵が自分に対して指定したマスのコピー用
	
	int m_actionShipPosColumn;	///< 行動する自駒の行座標
	int m_actionShipPosLine;	///< 行動する自駒の列座標
	
	float m_plTargetPointX;	///< 自身選択した座標X
	float m_plTargetPointY;	///< 自身選択した座標Y
	D3DXVECTOR2	m_plTargetVector;	///< 自駒の座標からターゲットへのベクトル（航空機の移動などに使う）
	
	float m_enTargetPointX;	///< 敵が選択した座標X
	float m_enTargetPointY;	///< 敵が選択した座標Y
	D3DXVECTOR2	m_enTargetVector;	///< 敵が航空機を飛ばす際のベクトル
	BoardOfFrame m_myShipBlock;	///< 自駒の中心ブロック


};

#endif