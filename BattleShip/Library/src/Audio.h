#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <XAudio2.h>

/**
*@brief	音楽ファイルクラス（現状wavファイルのみ）
*@details	※使用法\n
	アプリ開始時にWin_Mainで呼出
	InitXAudio2
	
	主にシーン初期化時で呼出
		LoadSoundFile
			->	OpenWaveFile	(内部で呼出・・・必要なファイル名をここでロード)

		//サウンドの使用したい場面で呼出
		SoundPlay, SoundStop, SoundAllStop

	//主にシーン移行時で呼出
		ReleaseSoundFile, ReleaseAllSoundFile

	アプリ終了時にリリース処理で呼出
			ReleaseXAudio2
*/
class Audio {
 public:
	enum SOUND_ID {

		//　BGMの曲数を用意
		_TITLE_BGM_,
		_BATTLE_BGM_,
		_WIN_BGM_,
		_LOSE_BGM_,
		
		//　SEの曲数を用意
		_FIRE_L_SE_,
		_FIRE_M_SE_,
		_FIRE_H_SE_,
		_AC_ATTACK_SE_,
		_TORPEDO_SE_,

		_AIRCRAFT_SE_,
		_SONAR_SE_,
		_WATER_SE_,
		_EXPLOSION_SE_,

		_CLICK_SE_,
		_FAILED_SE_,

		//配列０補正
		SOUND_MAX
	};

	/**
	*@brief	コンストラクタ
	*/
	Audio() {
		//	メンバ変数をNULLで初期化しよう
		pXAudio2 = NULL;
		pMasteringVo = NULL;
		for ( int i = 0; i < SOUND_MAX; i++ ) {
			pSourceVo[i] = NULL;
			buffer[i].pAudioData = NULL;
		}
		//---
		InitXAudio2();
		LoadAllSoundFile();
	}
	
	/**
	*@brief	デストラクタ
	*/
	~Audio() {
		SoundAllStop();
		
		ReleaseAllSoundFile();
		
		ReleaseXAudio2();
	}
	
	//サウンド再生
	void SoundPlay(int ID, bool LOOP = false);

	//サウンド停止
	void SoundStop(int ID);

	//サウンド全停止
	void SoundAllStop(void);


 private:

	//XAudioの初期化
	bool InitXAudio2(void);

	//サウンド全読み込み
	void LoadAllSoundFile();

	//音声ファイルの読み込み	->	解析できたら使用準備完了
	void LoadSoundFile( char* FileName, int IN_ID);
	
	//読込んだ.wavファイルを開いて解析
	bool OpenWaveFile( char* FileName, WAVEFORMATEX &waveFormatEx, unsigned char **ppData, unsigned long &dataSize);

	//ファイル->解放
	void ReleaseSoundFile(int ID);
	
	//ファイル->全解放
	void ReleaseAllSoundFile(void);

	//XAudioの解放
	bool ReleaseXAudio2(void);

	//デバイスの準備
	IXAudio2*				pXAudio2;
	IXAudio2MasteringVoice*	pMasteringVo;
	IXAudio2SourceVoice*	pSourceVo[SOUND_MAX];
	XAUDIO2_BUFFER			buffer[SOUND_MAX];

};

#endif