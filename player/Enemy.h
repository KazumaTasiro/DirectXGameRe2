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

class GameScene;
class Player;
class Enemy {
public:
	///<summary>
	///������
	///</summary>
	void Initialize(Model* model, uint32_t textureHandle,Vector3 EnemyPos);
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
	void Draw(ViewProjection viewProjection_);
	//�s���t�F�[�Y
	enum class Phase {
		Approch,//�ڋ߂���
		Leave,//���E����
	};
	void Approch();

	void Leave();

	void SetPlayer(Player* player) { player_ = player; }

	//�x�N�g���𐳋K������
	int Vec3Normalize(Vector3* pOut, Vector3* pV);

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead()const { return isDead_; }
	////�e���X�g���擾
	//const std::list<std::unique_ptr<EnemyBullet>>& GetBullets() { return bullets2_; }

	Vector3 ConvertToVector3(WorldTransform& mat, Vector3 vec);

private:
	//���ˊԊu
	static const int kFireInterval = 100;

	Player* player_ = nullptr;

	GameScene* gameScene_ = nullptr;

	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;

	//���f��
	Model* model_ = nullptr;

	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;

	//�t�F�[�Y
	Phase phase_ = Phase::Approch;
	//�L�����N�^�[�̈ړ��x�N�g��
	Vector3 ApprochMove = { 0,0,-0.05f };
	Vector3 LeaveMove = { -0.1f,0.1f,-0.1f };

	std::list<std::unique_ptr<EnemyBullet>> bullets2_;

	int32_t time = 0;
	//�f�X�t���O
	bool isDead_ = false;

	//�̗�
	int EnemyHp = 10;
};