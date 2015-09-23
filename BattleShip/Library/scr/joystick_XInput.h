#ifndef _JS_XINPUT_H_
#define _JS_XINPUT_H_

#include "lib_head.h" 

class CXInput
{
public:
	enum eSTICK_STATE 
	{
		PUSH,		
		ON,
		RELEASE,				
		OFF,		
		PRESS_TWICE,	//	2�x����

		STICK_STATE_MAX
	};

	enum eSTICK_TYPE
	{
		CROSS_UP,				//	�\���L�[��
		CROSS_DOWN,				//	�\���L�[��
		CROSS_LEFT,				//	�\���L�[��
		CROSS_RIGHT,			//	�\���L�[��
		BUTTON_START,			//	Start
		BUTTON_BACK,			//	Back
		BUTTON_LEFT_THUMB,		//	���X�e�B�b�N��������
		BUTTON_RIGHT_THUMB,		//	�E�X�e�B�b�N��������
		BUTTON_LEFT_SHOULDER,	//	�{�^��LB
		BUTTON_RIGHT_SHOULDER,	//	�{�^��RB
		BUTTON_A,				//	�{�^��A
		BUTTON_B,				//	�{�^��B
		BUTTON_X,				//	�{�^��X
		BUTTON_Y,				//	�{�^��Y
		L_TRIGGER,				//	���g���K�[
		R_TRIGGER,				//	�E�g���K�[

		STICK_TYPE_MAX
	};

	//	xCurrentkState�̏��̈ꕔ��ۑ��p
	struct STICK_STATE
	{
		//	LR�g���K�[���܂߂����E�X�e�B�b�N�ȊO�̃{�^�����̔z��
		eSTICK_STATE Button[STICK_TYPE_MAX];
	};


private:

	XINPUT_STATE xCurrentkState;	//	XInputGetState����̏�������p
	STICK_STATE StickState[XUSER_MAX_COUNT];	//	�擾�\�X�e�B�b�N���̔z��
	
private:	
	//	�{�^���̃`�F�b�N
	//	�g���K�[�ȊO�̏�ԍX�V�i�g���K�[��0�`255�ŉ��������o��̂Łj
	//	@pragma	_user	���Ԗڂ̃X�e�B�b�N��
	void CheckButton( int _user );

	//	�g���K�[�̃`�F�b�N
	//	�E���g���K�[�̌X���x��������eSTICK_STATE�Ŕ��f
	//	@pragma	_user	���Ԗڂ̃X�e�B�b�N��
	void CheckTrigger( int _user );

	//	���E�X�e�B�b�N�̃`�F�b�N
	//	�i�K�I�ɏ�Ԃ�ݒ肷�邩�͌�����
	//	@pragma	_user	���Ԗڂ̃X�e�B�b�N��
	void CheckLRStick( int _user );

public:
	//	�R���X�g���N�^
	CXInput();

	//	�X�e�B�b�N���z��̏�����
	void Initialize();

	//	�S�X�e�B�b�N���̍X�V
	void UpdateStick();

	//	StickState�z�������X�e�B�b�N�̏�Ԃ��`�F�b�N
	//	@pragma _user	�v���C���[�ԍ��i0�`3�j
	//	@pragma	_type	�{�^���̎��
	//	@pragma _state	�{�^���̏��
	//	@return	true�F�X�e�B�b�N���w�肵���ʂ�̏�ԁAfalse�F�w�肵����Ԃƈ����
	bool CheckStickState( int _user, eSTICK_TYPE _type, eSTICK_STATE _state );
};

#endif