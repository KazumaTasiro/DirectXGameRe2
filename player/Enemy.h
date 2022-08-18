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
#include"EnemyBullet.h"


///<summary>
///�G�L����
///</summary>

class Player;
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

	void Fire();

	Vector3 GetWorldPosition();
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

	void SetPlayer(Player* player) { player_ = player; }

private:
	//���ˊԊu
	static const int kFireInterval = 10;

	Player* player_ = nullptr;

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

	std::list<std::unique_ptr<EnemyBullet>> bullets2_;

	int32_t time = 0;
};