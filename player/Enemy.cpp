#include "Enemy.h"
#include "Player.h"
#include"GameScene.h"

void Enemy::Initialize(Model* model, uint32_t textureHandle, Vector3 EnemyPos)
{
	assert(model);
	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	this->model_ = model;
	this->textureHandle_ = textureHandle;
	//���[���h�ϊ��̏�����
	worldTransform_.Initialize();
	worldTransform_.translation_ = EnemyPos;
	Afin(worldTransform_);

	enemyBulletModel_ = Model::CreateFromOBJ("EnemyBullet", true);

	worldTransform_.TransferMatrix();

	/*Approch();*/
}

void Enemy::Update()
{
	//�L�����N�^�[�ړ�����
	Move();

	//Fire();
	if (EnemyHp <= 0) {
		isDead_ = true;
	}
	
	/*debugText_->SetPos(50, 70);
	debugText_->Printf("x:%f,y:%f,z:%f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);*/
	////�e�X�V
	//for (std::unique_ptr<EnemyBullet>& bullet : bullets2_) {
	//	bullet->Update();
	//}
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

	//Approch();

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

void Enemy::Fire()
{
	//�e�̑��x
	const float kBulletSpeed = 1.0f;
	assert(player_);
	Vector3 velocity(0, 0, 0);

	

	Vector3 PlayerPos = player_->GetWorldPosition();
	Vector3 EnemyPos = GetWorldPosition();
	velocity.x = PlayerPos.x - EnemyPos.x;
	velocity.y = PlayerPos.y - EnemyPos.y;
	velocity.z = PlayerPos.z - EnemyPos.z;
	Vec3Normalize(&velocity, &velocity);
	velocity *= kBulletSpeed;

	//�e�𐶐����A������
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(enemyBulletModel_, worldTransform_.translation_, velocity);

	//�e��o�^����
	gameScene_->AddEnemyBullet(newBullet);
}

Vector3 Enemy::GetWorldPosition()
{
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��̕��s�ړ��������擾�i���[���h���W�j
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::Draw(ViewProjection viewProjection_)
{
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	////�e�`��
	//for (std::unique_ptr<EnemyBullet>& bullet : bullets2_) {
	//	bullet->Draw(viewProjection_);
	//}
}

void Enemy::Approch()
{
	//�ړ��i�x�N�g�������Z�j
	worldTransform_.translation_ += ApprochMove;
	////�K��̈ʒu�ɓ��B�����痣�E
	//if (worldTransform_.translation_.z < 0.0f) {
	//	phase_ = Phase::Leave;
	//}

	if (worldTransform_.translation_.z < 0.0f) {
		worldTransform_.translation_.z = 30.0f;
	}
	time -= 1;

	if (time <= 0) {
		Fire();
		//���˃^�C�}�[��������
		time = kFireInterval;
	}

}

void Enemy::Leave()
{
	//�ړ��i�׃N�g�������Z�j
	worldTransform_.translation_ += LeaveMove;

}

int Enemy::Vec3Normalize(Vector3* pOut, Vector3* pV)
{
	double len;
	double x, y, z;

	x = (double)(pV->x);
	y = (double)(pV->y);
	z = (double)(pV->z);
	len = sqrt(x * x + y * y + z * z);

	if (len < (1e-6)) return 0;

	len = 1.0 / len;
	x *= len;
	y *= len;
	z *= len;

	pOut->x = (float)x;
	pOut->y = (float)y;
	pOut->z = (float)z;

	return 1;
}

void Enemy::OnCollision()
{
	EnemyHp--;
}
