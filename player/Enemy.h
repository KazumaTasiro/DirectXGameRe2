#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <cassert>
#include "DebugText.h"
#include "Input.h"
#include "ViewProjection.h"
#include "PlayerBullet.h"
#include <memory>
#include <list>
#include"Afin.h"

///<summary>
///�G�L����
///</summary>

class Enemy {
public:
	///<summary>
	///������
	///</summary>

	void Initialize(Model* model, uint32_t textureHandle);

	///<summary>
	///�X�V
	///</summary>

	void Update();
	void Move();

	void Afin(WorldTransform& worldTransform_);

	///<summary>
	///�`��
	///</summary>

	void Draw(ViewProjection& viewProjection_);

	//�s���t�F�[�Y
	enum class Phase {
		Approch,//�ڋ߂���
		Leave,//���E����
	};
	void Approch();

	void Leave();

private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;

	//�t�F�[�Y
	Phase phase_ = Phase::Approch;

	//�L�����N�^�[�̈ړ��x�N�g��
	Vector3 ApprochMove = { 0,0,-0.1f };
	Vector3 LeaveMove = { -0.1f,0.1f,-0.1f };
};