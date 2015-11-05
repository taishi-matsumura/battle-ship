/**
*@file	TitleScene.h
*@author	�T�c
*/

#ifndef _TITLESCENE_H_
#define _TITLESCENE_H_

#include "SceneInterface.h"
#include "Background.h"
#include "ScreenMask.h"
#include "Button.h"

#define _BUTTON_POS_X_  400.0f
#define _BUTTON_POS_Y_	420.0f
#define _BUTTON_WIDTH_	226.0f
#define _BUTTON_HEIGHT_	50.0f
#define _CHANGE_TIME_	3.0f*60.f


class TitleScene : public CScene
{
private:
	Background	m_background;	///<�w�i�I�u�W�F�N�g
	ScreenMask	m_screenMask;	///<��ʑJ�ڎ��̃t�F�[�h�p
	Button		m_button;		///<�{�^���I�u�W�F�N�g�i�����_�ł̓Q�[�����X�^�[�g���鎞�Ɏg�p�j
	bool		m_changeSceneFlag;
	float		m_changeSceneTime;

public:
	/**
	*@brief	�R���X�g���N�^
	*@details	�g��Ȃ��Ǘ��|�C���^������ꍇNULL�|�C���^������
	*@param[in]	_id	�������g�̃V�[��ID
	*@param[in]	_pRenderManager	3D�`��Ǘ��̃|�C���^
	*@param[in]	_pDrawManager	2D�`��Ǘ��̃|�C���^
	*@param[in]	_pKey			�L�[�Ǘ��̃|�C���^
	*@param[in]	m_pMouse		�}�E�X�Ǘ��̃|�C���^
	*/
	TitleScene(int _id, CRenderManager* const _pRenderManager,
		CDrawManager*	const _pDrawManager, CKey* const _pKey, CMouse* const m_pMouse)
		: CScene( _id, _pRenderManager,	_pDrawManager, _pKey, m_pMouse)
	{
		m_changeSceneFlag = false;
		m_changeSceneTime = _CHANGE_TIME_;

		Init();
	}

	/**	
	*@brief �������
	*/
	bool Free();

	/**
	*@brief	�Q�[�����C���̏������֐�
	*@return	�������̌���
	*@retval true	����
	*@retval false	�������ɉ��炩�̎��s���������ꍇ
	*/
	bool Init();

	/**
	*@brief	�Q�[�����C���̊�{�����̊֐�
	*@return	�V�[���ύX�����锻�f�̒l
	*@retval 0	�V�[���ύX����
	*@retval 1	�V�[���ύX�L��
	*/
	int	 Control();

	/**
	*@brief	2D�`��
	*/
	void Draw();
	void Render(){};

};

#endif