/**
*@file	Client.h
*@author	亀田
*@brief	クライアント側通信クラス
*/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Connect.h"

class Client : public Connect
{
private:
	struct sockaddr_in m_server;
	char m_buf[32];

public:
	/**
	*@brief	ソケット設定メソッド
	*/
	bool SettingSocket();

	/**
	*@brief	接続メソッド
	*/
	void ConnectToServer();

	/**
	*@brief	受信メソッド
	*/
	bool Receive();

	/**
	*@brief	通信終了処理
	*/
	void EndConnect();
};

#endif