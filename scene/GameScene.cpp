#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <math.h>
#include <random>
#include <DirectXMath.h>

float M_PI = 3.141592;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	//デバックカメラの生成
	debugCamera_ = new DebugCamera(600, 400);
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	//３Dモデルの生成
	model_ = Model::Create();

	//カメラ視点座標を設定
	viewProjection_.eye = { 0,0,-50 };

	////カメラ注視点座標を設定
	//viewProjection_.target = { 0,0,0 };

	////カメラ上方向ベクトルを設定（右上４５度指定）
	//viewProjection_.up = { cosf(M_PI / 4.0f),sinf(M_PI / 4.0f),0.0f };

	//カメラ垂直方向視野角を設定
	/*viewProjection_.fovAngleY= DirectX::XMConvertToRadians(10.0f);*/


	//アスペクト比を設定
	/*viewProjection_.aspectRatio = 1.0f;*/

	////ニアクリップ距離を設定
	//viewProjection_.nearZ = 52.0f;
	////ファークリップ距離を設定
	//viewProjection_.farZ = 53.0f;

	//ワールドトランスフォーム
	for (size_t i = 0; i < _countof(worldTransforms_); i++) {

		//// X,Y、Z方向のスケーリングを設定
		//worldTransforms_[i].scale_ = { 1.0f, 1.0f, 1.0f };

		//ワールドトランスフォームの初期化
		worldTransforms_[i].Initialize();
	}
	//ビュープロジェクト
	viewProjection_.Initialize();
	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	//ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//拡大乱数範囲の指定
	std::uniform_real_distribution<float> scaleDist(1, 1);
	//回転乱数範囲の指定
	std::uniform_real_distribution<float> rotationDist(0.0f, M_PI);
	//平行移動乱数範囲の指定
	std::uniform_real_distribution<float> translationDist(-10.0f, 10.0f);
	
	//キャラクター大元
	worldTransforms_[PartId::kRoot].Initialize();
	//脊椎
	worldTransforms_[PartId::kSpine].translation_ = { 0, 4.5f, 0 };
	worldTransforms_[PartId::kSpine].parent_ = &worldTransforms_[PartId::kRoot];
	worldTransforms_[PartId::kSpine].Initialize();
	//上半身
	//胸
	worldTransforms_[PartId::kChest].translation_ = { 0, 4.5f, 0 };
	worldTransforms_[PartId::kChest].parent_ = &worldTransforms_[PartId::kSpine];
	worldTransforms_[PartId::kChest].Initialize();

	//頭
	worldTransforms_[PartId::kHead].translation_ = { 0, 9.0f, 0 };
	worldTransforms_[PartId::kHead].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kHead].Initialize();

	//左腕
	worldTransforms_[PartId::kArmL].translation_ = { -9.0f, 4.5f, 0 };
	worldTransforms_[PartId::kArmL].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmL].Initialize();

	//右腕
	worldTransforms_[PartId::kArmR].translation_ = { 9.0f, 4.5f, 0 };
	worldTransforms_[PartId::kArmR].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmR].Initialize();

	//下半身
	//尻
	worldTransforms_[PartId::kHip].translation_ = { 0, -1.0f, 0 };
	worldTransforms_[PartId::kHip].parent_ = &worldTransforms_[PartId::kSpine];
	worldTransforms_[PartId::kHip].Initialize();

	//左足
	worldTransforms_[PartId::kLegL].translation_ = { -9.0f, -9.0f, 0 };
	worldTransforms_[PartId::kLegL].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegL].Initialize();

	//右足
	worldTransforms_[PartId::kLegR].translation_ = { 9.0f, -9.0f, 0 };
	worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegR].Initialize();

}

void GameScene::Update() {
	debugCamera_->Update();
	//視点移動処置
	{


		//デバッグ用表示
		debugText_->SetPos(50, 50);
		debugText_->Printf("eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	}
	//注視点移動処理
	{


		//デバッグ用表示
		debugText_->SetPos(50, 70);
		debugText_->Printf("target:(%f.%f.%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);
	}

	//上方向回転処理
	{

		//デバッグ用表示
		debugText_->SetPos(50, 90);
		debugText_->Printf("up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
	}

	//FoV変換処理
	{

		//デバッグ用表示
		debugText_->SetPos(50, 110);
		debugText_->Printf("fovAngleY(Degree):%f", DirectX::XMConvertToRadians(viewProjection_.fovAngleY));

	}
	//クリップ距離変更処理
	{

		//デバック用表示
		debugText_->SetPos(50, 130);
		debugText_->Printf("nearZ:%f", viewProjection_.nearZ);
	}
	////キャラクターの移動ベクトル

	//キャラクター移動処理
	{
		//キャラクターの移動ベクトル
		Vector3 move = { 0, 0, 0 };

		//キャラクターの移動速さ
		const float kCharacterSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move = { -kCharacterSpeed, 0, 0 };
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move = { kCharacterSpeed, 0, 0 };
		}
		//上半身回転処理
		{
			//上半身の回転の速さ[ラジアン/frame]
			const float kChestRotSpeed = 0.05f;

			//押した方向で移動ベクトルを変更
			if (input_->PushKey(DIK_U)) {
				worldTransforms_[PartId::kChest].rotation_.y -= kChestRotSpeed;
			}
			else if (input_->PushKey(DIK_I)) {
				worldTransforms_[PartId::kChest].rotation_.y += kChestRotSpeed;
			}


		}
		//下半身回転処理
		{
			//下半身の回転速さ[ラジアン/frame]
			const float kHipRotSpeed = 0.05f;
			//押した方向で移動ベクトルを変更
			if (input_->PushKey(DIK_J)) {
				worldTransforms_[PartId::kHip].rotation_.y -= kHipRotSpeed;
			}
			else if (input_->PushKey(DIK_K)) {
				worldTransforms_[PartId::kHip].rotation_.y += kHipRotSpeed;
			}
		}
		//注視点移動（ベクトルの加算）
		worldTransforms_[PartId::kRoot].translation_.x += move.x;
		worldTransforms_[PartId::kRoot].translation_.y += move.y;
		worldTransforms_[PartId::kRoot].translation_.z += move.z;

		Afin(worldTransforms_[PartId::kRoot]);

		worldTransforms_[PartId::kRoot].TransferMatrix();
		//デバック用表示
		debugText_->SetPos(50, 150);
		debugText_->Printf(
			"Root:(%f,%f,%f)", worldTransforms_[PartId::kRoot].translation_.x,
			worldTransforms_[PartId::kRoot].translation_.y,
			worldTransforms_[PartId::kRoot].translation_.z);
	}
	//大元から順に更新していく
	for (int i = 0; i < kNumPartId; i++) {
		Afin(worldTransforms_[i]);
		if (i != 0) {
			worldTransforms_[i].matWorld_ *= worldTransforms_[i].parent_->matWorld_;
		}
		worldTransforms_[i].TransferMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描画
	for (int i = 0; i < kNumPartId; i++) {
		if (i < 2) {
			continue;
		}
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
	}
	//ライン描画が参照するビュープロジェクションを指定する
	//PrimitiveDrawer::GetInstance()->DrawLine3d(Vector3(0, 0, 0), Vector3(10, 20, 0), Vector4(0xff, 0xff, 0, 0xff));

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Afin(WorldTransform& worldTransform_)
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

void GameScene::worldTransChildUpdate(WorldTransform& worldTransform_)
{
	worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;
}