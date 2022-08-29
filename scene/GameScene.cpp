#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <math.h>
#include <random>
#include <DirectXMath.h>
#include <fstream>

float M_PI = 3.141592f;
GameScene::GameScene() {}
GameScene::~GameScene() {
	delete debugCamera_;
	delete model_;
	delete player_;
	delete skydome_;
	delete modelSkydome_;
	delete railCamera_;
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
	
	//レールカメラの生成
	railCamera_ = new RailCamera();
	//レールカメラの初期化
	railCamera_->Initalize();

	//自キャラの生成
	player_ = new Player();

	//親子関係の設定
	player_->SetRailCamera(railCamera_->GetworldTransform());

	//自キャラの初期化
	player_->Initialize(model_, textureHandle_);


	//天球の生成
	skydome_ = new Skydome();
	//天球の初期化
	skydome_->Initalize(modelSkydome_);

	LoadEnemyPopData();


}

void GameScene::Update() {
	//デスフラグの立った弾を削除
	enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});
	////デスフラグの立った敵を削除
	//enemy_.remove_if([](std::unique_ptr<Enemy>& Enemy) {
	//	return Enemy->IsDead();
	//	});


	
	railCamera_->Update();
	debugCamera_->Update();
	player_->Update();
	UpdateEnemyPopCommands();
	for (const std::unique_ptr<Enemy>& enemy : enemy_) {
		//敵キャラにゲームシーンを渡す
		enemy->SetGameScene(this);
		enemy->Update();
	}
	//弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Update();
	}
	CheckAllCollisions();
	debugText_->SetPos(10, 30);
	debugText_->Printf("%d", enemy_.size());
	/*debugText_->SetPos(10, 100);
	debugText_->Printf("%f:%f:%f", player_->GetWorldPosition().x, player_->GetWorldPosition().y, player_->GetWorldPosition().z);*/
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
	player_->Draw(railCamera_->GetViewProjection());
	for (const std::unique_ptr<Enemy>& enemy : enemy_) {
		//敵キャラの描画
		enemy->Draw(railCamera_->GetViewProjection());
	}
	//天球の描画
	skydome_->Draw(railCamera_->GetViewProjection());
	
	//敵の弾の描画
	//弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

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
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = GetBullets();

#pragma region 自キャラと敵弾の当たり判定
#pragma endregion

	//自キャラも座標
	posA = player_->GetWorldPosition();

	//自キャラと敵弾すべての当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		//敵弾の座標
		posB = bullet->GetWorldPosition();

		float lol = { (posB.x - posA.x) * (posB.x - posA.x)+ (posB.y - posA.y) * (posB.y - posA.y)+ (posB.z - posA.z) * (posB.z - posA.z) };
	
		float radius = { 1 };

		if (lol<= radius) {
			//自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			//自弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}

#pragma region 自弾と敵キャラの当たり判定
#pragma endregion
	for (const std::unique_ptr<Enemy>& enemy : enemy_) {
		//敵キャラも座標
		posA = enemy->GetWorldPosition();

		//自キャラと敵弾すべての当たり判定
		for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
			//敵弾の座標
			posB = bullet->GetWorldPosition();

			float lol = { (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z) };

			float radius = { (1 + 1) * (1 + 1) };

			if (lol <= radius) {
				//敵キャラの衝突時コールバックを呼び出す
				enemy->OnCollision();
				//自弾の衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
		}
	}

#pragma region 自弾と敵弾の当たり判定
#pragma endregion
	//自弾と敵弾すべての当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet2 : enemyBullets_) {
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

void GameScene::AddEnemyBullet(std::unique_ptr<EnemyBullet>& enemyBullet)
{
	//リストに追加する
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GameScene::ExistenceEnemy(Vector3 EnemyPos)
{
	//敵キャラの生成
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
	newEnemy->Initialize(model_, textureHandle_,EnemyPos);

	//敵キャラにアドレスを渡す
	newEnemy->SetPlayer(player_);

	//リストに登録する
	enemy_.push_back(std::move(newEnemy));
}

void GameScene::LoadEnemyPopData()
{
	//ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPos.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();

}

void GameScene::UpdateEnemyPopCommands()
{
	
	//待機処理
	if (waitFlag == true) {
		waitTimes--;
		if (waitTimes <= 0) {
			//待機完了
			waitFlag = false;
		}
		return;
	}
	//１行分の文字列を入れる変数
	std::string line;
	//コマンド実行ループ
	while (getline(enemyPopCommands, line)) {

		


		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		//"//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}
		//PoPコマンド
		if (word.find("POP") == 0) {
			//x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//敵を発生させる
			ExistenceEnemy(Vector3(x, y, z));
		}
		//WAIT
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機開始
			waitFlag = true;
			waitTimes = waitTime;

			//コマンドループ
			break;
		}
	}
}
