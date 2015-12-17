#ifndef _REPORTRESULT_H_
#define _REPORTRESULT_H_

#include "ReportData.h"

/**
*@brief	ñÊÌÏñ
*/
enum _REPORT_RESULT_VAR_
{
	_ATT_COUNT_,	///< Uñ
	_SRC_COUNT_,	///< õGñ
	_SL_AVE_TIME_,	///< s®IðÜÅÌ½ÏÔ
	_TURN_COUNT_,	///< ^[
	_DMG_COUNT_,	///< íeñ
	_KO_COUNT_,		///< ¾Í
	_HIT_PROB_,		///< ½m¦
	_SIGHT_PROB_,	///< ­©m¦

	_REWARD_NAME_,
	_MAX_REPORT_VAR_
};

/**
*@brief	íÑÌ»èpÌf[^NX
*@details	ReportDataNXÅûWµ½f[^ðAMÍl¾
*/
class ReportResult
{
private:
	int		attackCount;
	int		searchCount;
	int		selectAveTime;
	int		turnCount;
	int		damageCount;
	unsigned char	KOCount;
	float	hitProbability;
	float	sightProbability;

public:
	ReportResult()
	{
		searchCount	= 0;
		attackCount	= 0;
		selectAveTime = 0;
		turnCount	= 0;
		damageCount	= 0;
		KOCount		= 0;
		hitProbability = 0.f;
		sightProbability = 0.f;
	}
	
	void ArrangementReport( ReportData& _reportData )
	{
		searchCount	= _reportData.GetSearchCount();
		attackCount	= _reportData.GetAttackCount();
		selectAveTime = _reportData.GetAveTime();
		turnCount	= _reportData.GetTurnCount();
		damageCount	= _reportData.GetDamageCount();
		KOCount		= _reportData.GetKOCount();
		hitProbability = _reportData.GetHitProbability();
		sightProbability = _reportData.GetSightProbability();
	}

public:
	inline int GetSearchCount()	{ return searchCount; }
	inline int GetAttackCount()	{ return attackCount; }
	inline int GetAveTime()	{ return selectAveTime; }
	inline int GetTurnCount()	{ return turnCount; }
	inline int GetDamageCount()	{ return damageCount; }
	inline unsigned char GetKOCount() { return KOCount; }
	inline float GetHitProbability() { return hitProbability; }
	inline float GetSightProbability() { return sightProbability; }

	inline void SetSearchCount( int _searchCount)	{ searchCount = _searchCount; }
	inline void SetAttackCount( int _attackCount)	{ attackCount = _attackCount; }
	inline void SetAveTime( int _aveTime )			{ selectAveTime = _aveTime; }
	inline void SetTurnCount( int _turnCount )		{ turnCount = _turnCount; }
	inline void SetDamageCount( int _damageCount )	{ damageCount = _damageCount; }
	inline void SetKOCount( unsigned char _koFlag )		{ KOCount = _koFlag; }
	inline void SetHitProbability( float _hitProbability ){ hitProbability = _hitProbability; }
	inline void SetSightProbability( float _sightProbability){ sightProbability = _sightProbability; }
};

#endif