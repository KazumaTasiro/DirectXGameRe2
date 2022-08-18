#include "Player.h"
Player::Player()
{
}
Player::~Player()
{
}
void Player::Initialize(Model* model, uint32_t textureHandle)
{
	assert(model);
	//引数として受け取ったデータをメンバ変数に記録する
	this->model_ = model;
	this->textureHandle_ = textureHandle;
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();
	//ワールド変換の初期化
	worldTransform_.Initialize();
}
void Player::Update()
{
	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});
	//キャラクター移動処理
	Move();
	//キャラクター攻撃更新
	Attack();
	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}
}
void Player::Move()
{
	//キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };
	const float speed = 1.0f;
	const float RotSpeed = 0.05f;
	if (input_->PushKey(DIK_A)) {
		move.x -= speed;
	}
	else if (input_->PushKey(DIK_D)) {
		move.x += speed;
	}
	if (input_->PushKey(DIK_W)) {
		move.y += speed;
	}
	else if (input_->PushKey(DIK_S)) {
		move.y -= speed;
	}
	if (input_->PushKey(DIK_U)) {
		worldTransform_.rotation_.y -= RotSpeed;
	}
	else if (input_->PushKey(DIK_I)) {
		worldTransform_.rotation_.y += RotSpeed;
	}
	worldTransform_.translation_ += move;
	//キーボード入力による移動処理
	//移動限界座標
	const float kMoveLimitX = 35.0f;
	const float kMoveLimitY = 19.0f;
	//範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	Afin(worldTransform_);

	worldTransform_.TransferMatrix();
	debugText_->SetPos(50, 50);
	debugText_->Printf("x:%f,y:%f,z:%f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
}
void Player::Draw(ViewProjection& viewProjection_)
{
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}
void Player::Attack()
{
	if (input_->PushKey(DIK_SPACE))
	{
		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//速度ベクトルを自機の向きに合わせて回転させる
		velocity = ConvertToVector3(worldTransform_, velocity);

		//弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);
		//弾を発射する
		bullets_.push_back(std::move(newBullet));
	}
}
void Player::Afin(WorldTransform& worldTransform_)
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

Vector3 Player::ConvertToVector3(WorldTransform& mat, Vector3 vec){

Vector3 retVec = {};

retVec.x = vec.x * mat.matWorld_.m[0][0] + vec.y * mat.matWorld_.m[1][0] + vec.z * mat.matWorld_.m[2][0];

retVec.y = vec.x * mat.matWorld_.m[0][1] + vec.y * mat.matWorld_.m[1][1] + vec.z * mat.matWorld_.m[2][1];

retVec.z = vec.x * mat.matWorld_.m[0][2] + vec.y * mat.matWorld_.m[1][2] + vec.z * mat.matWorld_.m[2][2];

return retVec;
}

Vector3 Player::GetWorldPosition()
{
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}