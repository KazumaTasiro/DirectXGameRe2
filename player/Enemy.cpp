#include "Enemy.h"
#include "Player.h"
#include"GameScene.h"

void Enemy::Initialize(Model* model, uint32_t textureHandle, Vector3 EnemyPos)
{
	assert(model);
	//引数として受け取ったデータをメンバ変数に記録する
	this->model_ = model;
	this->textureHandle_ = textureHandle;
	//ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = EnemyPos;
	Afin(worldTransform_);

	enemyBulletModel_ = Model::CreateFromOBJ("EnemyBullet", true);

	worldTransform_.TransferMatrix();

	/*Approch();*/
}

void Enemy::Update()
{
	//キャラクター移動処理
	Move();

	//Fire();
	if (EnemyHp <= 0) {
		isDead_ = true;
	}
	
	/*debugText_->SetPos(50, 70);
	debugText_->Printf("x:%f,y:%f,z:%f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);*/
	////弾更新
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
	//Z用回転軸の設定
	matRotZ = {
	cos(worldTransform_.rotation_.z),sin(worldTransform_.rotation_.z),0,0,
	-sin(worldTransform_.rotation_.z),cos(worldTransform_.rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1
	};
	//X用回転軸の設定
	matRotX = {
	1,0,0,0,
	0,cos(worldTransform_.rotation_.x),sin(worldTransform_.rotation_.x),0,
	0,-sin(worldTransform_.rotation_.x),cos(worldTransform_.rotation_.x),0,
	0,0,0,1
	};
	//Y用回転軸の設定
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
	//各軸の回転行列を合成
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
	//弾の速度
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

	//弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(enemyBulletModel_, worldTransform_.translation_, velocity);

	//弾を登録する
	gameScene_->AddEnemyBullet(newBullet);
}

Vector3 Enemy::GetWorldPosition()
{
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::Draw(ViewProjection viewProjection_)
{
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	////弾描画
	//for (std::unique_ptr<EnemyBullet>& bullet : bullets2_) {
	//	bullet->Draw(viewProjection_);
	//}
}

void Enemy::Approch()
{
	//移動（ベクトルを加算）
	worldTransform_.translation_ += ApprochMove;
	////規定の位置に到達したら離脱
	//if (worldTransform_.translation_.z < 0.0f) {
	//	phase_ = Phase::Leave;
	//}

	if (worldTransform_.translation_.z < 0.0f) {
		worldTransform_.translation_.z = 30.0f;
	}
	time -= 1;

	if (time <= 0) {
		Fire();
		//発射タイマーを初期化
		time = kFireInterval;
	}

}

void Enemy::Leave()
{
	//移動（べクトルを加算）
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
