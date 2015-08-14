/**
*@file	Server.h
*@author	�T�c
*@brief	�T�[�o�[���ʐM�N���X
*/

#ifndef _SERVER_H_
#define _SERVER_H_

#include "Connect.h"


class Server : public Connect
{
private:
	SOCKET m_sock0;	///<	�ڑ���\�P�b�g
	struct sockaddr_in m_addr;	///<	�T�[�o���A�h���X
	struct sockaddr_in m_client;///<	�N���C�A���g���A�h���X
	int m_len;	///<	���M���b�Z�[�W�̒���
	bool m_reuseFlag;	///<	SO_REUSEADDR�L���p

	//----HTTP�T�[�o�T���v���e�X�g�p
	char m_buf[2048];
	char m_inbuf[2048];

	//----
public:
	/**
	*@brief	�\�P�b�g�ݒ胁�\�b�h
	*/
	bool SettingSocket();

	/**
	*@brief	�ڑ��v�����\�b�h
	*/
	bool KeepListen();

	/**
	*@brief	�ʐM�I������
	*/
	void EndConnect();
};

#endif