#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <math.h>
#include <random>
#include <DirectXMath.h>
float M_PI = 3.141592f;
GameScene::GameScene() {}
GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
	delete player_;
	delete enemy_;
	delete skydome_;
	delete modelSkydome_;
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
	//for (size_t i = 0; i < _countof(worldTransforms_); i++) {
	//	
	//	//ワールドトランスフォームの初期化
	//	worldTransforms_[i].Initialize();
	//}
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
	////拡大乱数範囲の指定
	//std::uniform_real_distribution<float> scaleDist(1, 1);
	////回転乱数範囲の指定
	//std::uniform_real_distribution<float> rotationDist(0.0f, M_PI);
	////平行移動乱数範囲の指定
	//std::uniform_real_distribution<float> translationDist(-10.0f, 10.0f);
	
	//3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("uv", true);
	
	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	player_->Initialize(model_, textureHandle_);
	//敵キャラの生成
	enemy_ = new Enemy();
	//敵キャラの初期化
	enemy_->Initialize(model_, textureHandle_);

	//敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	//天球の生成
	skydome_ = new Skydome();
	//天球の初期化
	skydome_->Initalize(modelSkydome_);

}

void GameScene::Update() {
	debugCamera_->Update();
	player_->Update();
	enemy_->Update();
	CheckAllCollisions();
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
	//自キャラの描画
	player_->Draw(viewProjection_);
	//敵キャラの描画
	enemy_->Draw(viewProjection_);
	//天球の描画
	skydome_->Draw(viewProjection_);
	
	/*for (int i = 0; i < 100; i++) {
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
	}*/
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

void GameScene::CheckAllCollisions()
{
	//判定対象AとBの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
#pragma endregion

	//自キャラも座標
	posA = player_->GetWorldPosition();

	//自キャラと敵弾すべての当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
		//敵弾の座標
		posB = bullet->GetWorldPosition();

		float lol = { (posB.x - posA.x) * (posB.x - posA.x)+ (posB.y - posA.y) * (posB.y - posA.y)+ (posB.z - posA.z) * (posB.z - posA.z) };
	
		float radius = { (1 + 1)* (1 + 1) };

		if (lol<= radius) {
			//自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			//自弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

#pragma region 自弾と敵キャラの当たり判定
#pragma endregion
	//敵キャラも座標
	posA = enemy_->GetWorldPosition();

	//自キャラと敵弾すべての当たり判定
	for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
		//敵弾の座標
		posB = bullet->GetWorldPosition();

		float lol = { (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z) };

		float radius = { (1 + 1) * (1 + 1) };

		if (lol <= radius) {
			//敵キャラの衝突時コールバックを呼び出す
			enemy_->OnCollision();
			//自弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

#pragma region 自弾と敵弾の当たり判定
#pragma endregion
	//自弾と敵弾すべての当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet2 : enemyBullets) {
		for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
			//自弾の座標
			posA = bullet2->GetWorldPosition();
			//敵弾の座標
			posB = bullet->GetWorldPosition();

			float lol = { (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z) };

			float radius = { (1 + 1) * (1 + 1) };

			if (lol <= radius) {
				//自キャラの衝突時コールバックを呼び出す
				bullet2->OnCollision();
				//自弾の衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
		}
	}
}
