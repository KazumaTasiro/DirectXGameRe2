#include "Enemy.h"

void Enemy::Initialize(Model* model, uint32_t textureHandle)
{
	assert(model);
	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	this->model_ = model;
	this->textureHandle_ = textureHandle;

	//���[���h�ϊ��̏�����
	worldTransform_.Initialize();

	worldTransform_.translation_ = { 0,0,10 };

	Afin(worldTransform_);

	worldTransform_.TransferMatrix();
}

void Enemy::Update()
{
	//�L�����N�^�[�ړ�����
	Move();
}

void Enemy::Move()
{

	switch (phase_)
	{
	case Enemy::Phase::Approch:
	default:
		Approch();
		break;
	case Enemy::Phase::Leave:
		Leave();
		break;
	}

	Afin(worldTransform_);

	worldTransform_.TransferMatrix();
}

void Enemy::Afin(WorldTransform& worldTransform_)
{
	Matrix4 matScale;
	Matrix4 matRot;
	Matrix4 matRotX;
	Matrix4 matRotY;
	Matrix4 matRotZ;
	Matrix4 matTrans;
	matScale = {
	worldTransform_.scale_.x,0,0,0,
	0,worldTransform_.scale_.y,0,0,
	0,0,worldTransform_.scale_.z,0,
	0,0,0,1
	};
	matTrans = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	worldTransform_.translation_.x,worldTransform_.translation_.y,worldTransform_.translation_.z,1
	};
	//Z�p��]���̐ݒ�
	matRotZ = {
	cos(worldTransform_.rotation_.z),sin(worldTransform_.rotation_.z),0,0,
	-sin(worldTransform_.rotation_.z),cos(worldTransform_.rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1
	};

	//X�p��]���̐ݒ�
	matRotX = {
	1,0,0,0,
	0,cos(worldTransform_.rotation_.x),sin(worldTransform_.rotation_.x),0,
	0,-sin(worldTransform_.rotation_.x),cos(worldTransform_.rotation_.x),0,
	0,0,0,1
	};

	//Y�p��]���̐ݒ�
	matRotY = {
	cos(worldTransform_.rotation_.y),0,-sin(worldTransform_.rotation_.y),0,
	0,1,0,0,
	sin(worldTransform_.rotation_.y),0,cos(worldTransform_.rotation_.y),0,
	0,0,0,1
	};

	matRot = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
	};
	//�e���̉�]�s�������
	matRot *= matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;

	worldTransform_.matWorld_ = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	worldTransform_.matWorld_ *= matScale;
	worldTransform_.matWorld_ *= matRot;
	worldTransform_.matWorld_ *= matTrans;
}

void Enemy::Draw(ViewProjection& viewProjection_)
{
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
}

void Enemy::Approch()
{
	//�ړ��i�x�N�g�������Z�j
	worldTransform_.translation_ += ApprochMove;
	//�K��̈ʒu�ɓ��B�����痣�E
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::Leave()
{
	//�ړ��i�׃N�g�������Z�j
	worldTransform_.translation_ += LeaveMove;
}