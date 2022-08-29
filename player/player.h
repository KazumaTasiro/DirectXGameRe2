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

class RailCamera;
///<summary>
///���L����
///</summary>
class Player {
public:
	Player();
	~Player();
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

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision();

	Vector3 ConvertToVector3(WorldTransform& mat, Vector3 vec);

	Vector3 GetWorldPosition();

	void SetRailCamera(WorldTransform& worldTransform_);

	///<summary>
	///�`��
	///</summary>
	void Draw(ViewProjection viewProjection_);
	///<summary>
	///�U��
	///</summary>
	void Attack();


	//�e���X�g���擾
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

	int Vec3Normalize(Vector3* pOut, Vector3* pV);
private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e���f��
	Model* bulletModel_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;
	Input* input_ = nullptr;
	DebugText* debugText_ = nullptr;
	////���[���h�g�����X�t�H�[��
	//WorldTransform worldTransform_;
	//�r���[�v���W�F�N�V����
	ViewProjection viewProjection_;
	//�e
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	//
	RailCamera* railCamera_ = nullptr;

};