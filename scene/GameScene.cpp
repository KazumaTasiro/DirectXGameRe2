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
	for (int i = 0; i < 100; i++) {
		//ワールドトランスフォーム
		worldTransforms_[i].Initialize();
		//スケーリング
		//XYZ方向のスケーリングを設定
		worldTransforms_[i].scale_ = { 1,1,1 };
		//スケーリング倍率を行列を宣言
		//Matrix4 matScale;
		//matScale =
		//{
		//worldTransforms_[i].scale_.x,0,0,0,
		//0,worldTransforms_[i].scale_.y,0,0,
		//0,0,worldTransforms_[i].scale_.z,0,
		//0,0,0,1
		//};

		//回転
		//XYZ軸周りの回転角を設定
		worldTransforms_[i].rotation_ = { rotationDist(engine),rotationDist(engine) ,rotationDist(engine) };
		////合成用回転行列
		//Matrix4 matRot;
		////各軸用回転行列を宣言
		//Matrix4 matRotX, matRotY, matRotZ;
		////避難用の関数
		//Matrix4 matWorld2;

		////Z用回転軸の設定
		//matRotZ = {
		//cos(worldTransforms_[i].rotation_.z),sin(worldTransforms_[i].rotation_.z),0,0,
		//-sin(worldTransforms_[i].rotation_.z),cos(worldTransforms_[i].rotation_.z),0,0,
		//0,0,1,0,
		//0,0,0,1
		//};

		////X用回転軸の設定
		//matRotX = {
		//1,0,0,0,
		//0,cos(worldTransforms_[i].rotation_.x),sin(worldTransforms_[i].rotation_.x),0,
		//0,-sin(worldTransforms_[i].rotation_.x),cos(worldTransforms_[i].rotation_.x),0,
		//0,0,0,1
		//};

		////Y用回転軸の設定
		//matRotY = {
		//cos(worldTransforms_[i].rotation_.y),0,-sin(worldTransforms_[i].rotation_.y),0,
		//0,1,0,0,
		//sin(worldTransforms_[i].rotation_.y),0,cos(worldTransforms_[i].rotation_.y),0,
		//0,0,0,1
		//};

		//matRot = {
		//1,0,0,0,
		//0,1,0,0,
		//0,0,1,0,
		//0,0,0,1
		//};
		////各軸の回転行列を合成
		//matRot *= matRotZ;
		//matRot *= matRotX;
		//matRot *= matRotY;

		//worldTransforms_[i].matWorld_ *= matRot;
		////行列の転送

		//XYZ軸周りの平行移動を設定
		worldTransforms_[i].translation_ = { translationDist(engine),translationDist(engine),translationDist(engine) };
		////平行移動行列を宣言
		//Matrix4 matTrans = MathUtility::Matrix4Identity();

		//matTrans = {
		//1,0,0,0,
		//0,1,0,0,
		//0,0,1,0,
		//worldTransforms_[i].translation_.x,worldTransforms_[i].translation_.y,worldTransforms_[i].translation_.z,1
		//};

		//worldTransforms_[i].matWorld_ = {
		//1,0,0,0,
		//0,1,0,0,
		//0,0,1,0,
		//1,1,1,1
		//};

		//worldTransforms_[i].matWorld_ *= matScale;
		//worldTransforms_[i].matWorld_ *= matRot;
		//worldTransforms_[i].matWorld_ *= matTrans;

		Afin(worldTransforms_[i]);
		//行列の転送
		worldTransforms_[i].TransferMatrix();
	}


}

void GameScene::Update() {
	debugCamera_->Update();
	//視点移動処置
	{
		//視点の移動ベクトル
		//Vector3 move = { 0,0,0 };

		////視点の移動の速さ
		//const float kEyeSpeed = 0.2f;

		////押した方向で移動ベクトルを変更
		//if (input_->PushKey(DIK_W)) {
		//	move.z += kEyeSpeed;
		//}
		//else if (input_->PushKey(DIK_S)) {
		//	move.z -= kEyeSpeed;
		//}
		////視点移動（ベクトルの加算）
		//viewProjection_.eye += move;

		////行列の再計算
		//viewProjection_.UpdateMatrix();

		//デバッグ用表示
		debugText_->SetPos(50, 50);
		debugText_->Printf("eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	}
	//注視点移動処理
	{
		//注視点の移動ベクトル
		//Vector3 move = { 0,0,0 };

		////注視点の移動速さ
		//const float kTargetSpeed = 0.2f;

		////押した方向で移動ベクトルを変更
		//if (input_->PushKey(DIK_LEFT)) {
		//	move.x -= kTargetSpeed;
		//}
		//else if (input_->PushKey(DIK_RIGHT)) {
		//	move.x += kTargetSpeed;
		//}
		////注視点移動（ベクトルの加算）
		//viewProjection_.target += move;
		////行列の再計算
		//viewProjection_.UpdateMatrix();

		//デバッグ用表示
		debugText_->SetPos(50, 70);
		debugText_->Printf("target:(%f.%f.%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);
	}

	//上方向回転処理
	{
		////上方向の回転の速さ[ラジアン/frame]
		//const float kUpRotSpeed = 0.05f;

		////押した方向でベクトルを変更
		//if (input_->PushKey(DIK_SPACE)) {
		//	viewAngle += kUpRotSpeed;
		//	//2πを超えたら０に戻す
		//	viewAngle = fmodf(viewAngle, M_PI * 2.0f);
		//}
		////上方向ベクトルを計算（半径１の円周上の座標）
		//viewProjection_.up = { cosf(viewAngle),sinf(viewAngle),0.0f };

		////行列の再計算
		//viewProjection_.UpdateMatrix();

		//デバッグ用表示
		debugText_->SetPos(50, 90);
		debugText_->Printf("up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
	}

	//FoV変換処理
	{
		////上キーで視野角が広がる
		//if (input_->PushKey(DIK_UP)) {
		//	viewProjection_.fovAngleY += 0.001f;
		//	viewProjection_.fovAngleY = min(max(viewProjection_.fovAngleY, 0.01f), M_PI);
		//}
		//else if (input_->PushKey(DIK_DOWN)) {
		//	viewProjection_.fovAngleY -= 0.001f;
		//	viewProjection_.fovAngleY = max(min(viewProjection_.fovAngleY, M_PI),0.01f);
		//}

		////行列の再計算
		//viewProjection_.UpdateMatrix();

		//デバッグ用表示
		debugText_->SetPos(50, 110);
		debugText_->Printf("fovAngleY(Degree):%f", DirectX::XMConvertToRadians(viewProjection_.fovAngleY));

	}
	//クリップ距離変更処理
	{
		//上下キーでニアクリップ距離を増減
		if (input_->PushKey(DIK_UP)) {
			viewProjection_.nearZ += 0.1f;
		}
		else if (input_->PushKey(DIK_DOWN)) {
			viewProjection_.nearZ -= 0.1f;
		}
		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		debugText_->SetPos(50, 130);
		debugText_->Printf("nearZ:%f", viewProjection_.nearZ);
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
	for (int i = 0; i < 100; i++) {
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