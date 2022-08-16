#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <math.h>
#include <random>

float M_PI = 3.141592f;

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
	//ワールドトランスフォーム
	worldTransform_.Initialize();
	//ビュープロジェクト
	viewProjection_.Initialize();
	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	//ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
	//スケーリング
	//XYZ方向のスケーリングを設定
	worldTransform_.scale_ = { 3,2,1 };

	//XYZ軸周りの回転角を設定
	worldTransform_.rotation_ = { (M_PI / 4),(M_PI / 4) ,(M_PI / 4) };


	//XYZ軸周りの平行移動を設定
	worldTransform_.translation_ = { 20,6,0 };


	Afin(worldTransform_);

	//行列の転送
	worldTransform_.TransferMatrix();
}

void GameScene::Update() { debugCamera_->Update(); }

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
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);
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