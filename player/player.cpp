#include "Player.h"
using namespace MathUtility;

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
	worldTransform3DReticle_.Initialize();

	worldTransform_.translation_ = { 0, 0, 5 };

	bulletModel_ = Model::CreateFromOBJ("bullet", true);
	//レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("Reticle.png");
	//スプライト生成
	sprite2DReticle_.reset(
		Sprite::Create(textureReticle, Vector2(500, 350), Vector4(1, 1, 1, 1), Vector2(0.5, 0.5)));
}
void Player::Update(ViewProjection viewProjection_)
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
	//自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 60.0f;
	//自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = { 0, 0, 1.0f };
	//自機のワールド座標の回転を反映
	offset = ConvertToVector3( worldTransform_, offset);
	//ベクトルの長さを整える
	Vec3Normalize(&offset, &offset);
	offset *= kDistancePlayerTo3DReticle;
	//3Dレティクル座標設定
	worldTransform3DReticle_.translation_ = {
		offset.x + worldTransform_.matWorld_.m[3][0],
		offset.y + worldTransform_.matWorld_.m[3][1],
		offset.z + worldTransform_.matWorld_.m[3][2]
	};
	//行列更新
	Afin(worldTransform3DReticle_);
	worldTransform3DReticle_.TransferMatrix();

	//3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	Vector3 positionReticle = {worldTransform3DReticle_.matWorld_.m[3][0],worldTransform3DReticle_.matWorld_.m[3][1],worldTransform3DReticle_.matWorld_.m[3][2] };

	Vector2 windowWH =
		Vector2(WinApp::GetInstance()->kWindowWidth, WinApp::GetInstance()->kWindowHeight);

	//ビューポート行列
	Matrix4 Viewport = {
	   windowWH.x / 2,			  0,  0,  0,
					0,	-windowWH.y / 2,  0,  0,
					0,				  0,  1,  0,
	   windowWH.x / 2,	 windowWH.y / 2,  0,  1
	};

	//ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4 matViewProjectionViewport = viewProjection_.matView * viewProjection_.matProjection * Viewport;

	//ワールド→スクリーン座標変換(ここから3Dから2Dになる)
	positionReticle = clossV3V4(positionReticle, matViewProjectionViewport);

	//スプライトのレティクルに座標設定
	sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

	//マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	{
		POINT mousePosition;
		//マウス座標（スクリーン座標）を取得する
		GetCursorPos(&mousePosition);

		//クライアントエリア座標に変換する
		HWND hwnd = WinApp::GetInstance()->GetHwnd();
		ScreenToClient(hwnd, &mousePosition);

		//マウス座標を2Dレティクルのスプライトに代入する
		sprite2DReticle_->SetPosition(Vector2(mousePosition.x, mousePosition.y));

		//ビュー行列、射影変換、ビューポート行列の合成行列を計算する
		Matrix4 matVPV = viewProjection_.matView * viewProjection_.matProjection * Viewport;

		//合成行列の逆行列を計算する
		//合成行列の逆行列を計算する
		Matrix4 matInverseVPV;
		MatrixInverse(matInverseVPV, matVPV);

		//ニアクリップ面上のワールド座標得る（スクリーン→ワールド変換）
		Vector3 posNear = Vector3(mousePosition.x, mousePosition.y, 0);
		//ファークリップ面上のワールド座標を得る（スクリーン→ワールド変換）
		Vector3 posFar = Vector3(mousePosition.x, mousePosition.y, 1);

		//スクリーン座標系からワールド座標系へ
		posNear = clossV3V4(posNear, matInverseVPV);
		posFar = clossV3V4(posFar, matInverseVPV);

		//マウスの前方ベクトルを計算する
		//マウスレイの方向
		Vector3 mouseDirection = posFar - posNear;
		mouseDirection = Vector3Normalize(mouseDirection);
		//ニアクリップ面上のワールド座標から一定距離前進したところに3Dレティクルを配置
		//カメラから照準オブジェクトの距離
		const float kDistanceTestObject = 222.0f;
		worldTransform3DReticle_.translation_ = AddVector(posNear, mouseDirection * kDistanceTestObject);

		//行列更新
		Afin(worldTransform3DReticle_);
		worldTransform3DReticle_.TransferMatrix();

		/*debugText_->SetPos(50, 150);
		debugText_->Printf(
			"translation : %f,%f,%f", worldTransform_.translation_.x,
			worldTransform_.translation_.y,
			worldTransform_.translation_.z);*/
	}
}
void Player::Move()
{
	//キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };
	const float speed = 0.1f;
	const float RotSpeed = 0.05f;
	/*if (input_->PushKey(DIK_A)) {
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
	}*/
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

	worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;

	

	worldTransform_.TransferMatrix();
	/*debugText_->SetPos(50, 50);
	debugText_->Printf("x:%f,y:%f,z:%f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);*/
}
void Player::Draw(ViewProjection viewProjection_)
{
	/*model_->Draw(worldTransform_, viewProjection_, textureHandle_);*/
	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
	////3Dレティクルを描画
	//model_->Draw(worldTransform3DReticle_, viewProjection_, textureHandle_);
}
void Player::Attack()
{
	if (input_->IsTriggerMouse(0))
	{
		//弾の速度
		const float kBulletSpeed = 0.01f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//速度ベクトルを自機の向きに合わせて回転させる
		velocity = ConvertToVector3(worldTransform_, velocity);

		//自機から標準オブジェクトへのベクトル
		velocity = { 
		worldTransform3DReticle_.matWorld_.m[3][0] - worldTransform_.matWorld_.m[3][0],
		worldTransform3DReticle_.matWorld_.m[3][1] - worldTransform_.matWorld_.m[3][1],
		worldTransform3DReticle_.matWorld_.m[3][2] - worldTransform_.matWorld_.m[3][2] };
		Vec3Normalize(&velocity, &velocity);
		//弾を生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);
		//弾を発射する
		bullets_.push_back(std::move(newBullet));
	}
}
int Player::Vec3Normalize(Vector3* pOut, Vector3* pV)
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
void Player::DrawUI()
{
	sprite2DReticle_->Draw();
}
Vector3 Player::clossV3V4(const Vector3& vec, const Matrix4& mat)
{
	Vector4 divVec = {};

	divVec.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + 1 * mat.m[3][0];

	divVec.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + 1 * mat.m[3][1];

	divVec.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + 1 * mat.m[3][2];

	divVec.w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + 1 * mat.m[3][3];

	divVec.x = divVec.x / divVec.w;

	divVec.y = divVec.y / divVec.w;

	divVec.z = divVec.z / divVec.w;

	return { divVec.x, divVec.y, divVec.z };
}
int Player::MatrixInverse(Matrix4& pOut, Matrix4& pM)
{
	Matrix4 mat;
	int i, j, loop;
	double fDat, fDat2;
	double mat_8x4[4][8];
	int flag;
	float* pF;
	double* pD;

	//8 x 4行列に値を入れる
	for (i = 0; i < 4; i++) {
		pF = pM.m[i];
		for (j = 0; j < 4; j++, pF++) mat_8x4[i][j] = (double)(*pF);
		pD = mat_8x4[i] + 4;
		for (j = 0; j < 4; j++) {
			if (i == j)   *pD = 1.0;
			else         *pD = 0.0;
			pD++;
		}
	}

	flag = 1;
	for (loop = 0; loop < 4; loop++) {
		fDat = mat_8x4[loop][loop];
		if (fDat != 1.0) {
			if (fDat == 0.0) {
				for (i = loop + 1; i < 4; i++) {
					fDat = mat_8x4[i][loop];
					if (fDat != 0.0) break;
				}
				if (i >= 4) {
					flag = 0;
					break;
				}
				//行を入れ替える
				for (j = 0; j < 8; j++) {
					fDat = mat_8x4[i][j];
					mat_8x4[i][j] = mat_8x4[loop][j];
					mat_8x4[loop][j] = fDat;
				}
				fDat = mat_8x4[loop][loop];
			}

			for (i = 0; i < 8; i++) mat_8x4[loop][i] /= fDat;
		}
		for (i = 0; i < 4; i++) {
			if (i != loop) {
				fDat = mat_8x4[i][loop];
				if (fDat != 0.0f) {
					//mat[i][loop]をmat[loop]の行にかけて
					//(mat[j] - mat[loop] * fDat)を計算
					for (j = 0; j < 8; j++) {
						fDat2 = mat_8x4[loop][j] * fDat;
						mat_8x4[i][j] -= fDat2;
					}
				}
			}
		}
	}

	if (flag) {
		for (i = 0; i < 4; i++) {
			pF = mat.m[i];
			pD = mat_8x4[i] + 4;
			for (j = 0; j < 4; j++) {
				*pF = (float)(*pD);
				pF++;
				pD++;
			}
		}
	}
	else {
		//単位行列を求める
		mat = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		};
	}

	pOut = mat;

	if (flag) return 1;
	return 0;
}
Vector3 Player::AddVector(const Vector3 v1, const Vector3 v2)
{
	Vector3 addVec = {};

	addVec.x = v1.x + v2.x;

	addVec.y = v1.y + v2.y;

	addVec.z = v1.z + v2.z;

	return addVec;
}
void Player::Reset()
{
	worldTransform_.translation_ = { 0, 0, 5 };
	bullets_.clear();
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

void Player::OnCollision()
{
	//デスフラグ
	bool isDead_ = false;
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
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetRailCamera(WorldTransform& worldTransform_)
{
	this->worldTransform_.parent_ = &worldTransform_;
}
