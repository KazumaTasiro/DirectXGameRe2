#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <cassert>
#include "DebugText.h"
#include "Input.h"
#include "ViewProjection.h"
#include "PlayerBullet.h"
#include <memory>
#include <list>
#include"Afin.h"
#include"EnemyBullet.h"



///<summary>
///敵キャラ
///</summary>

class GameScene;
class Player;
class Enemy {
public:
	///<summary>
	///初期化
	///</summary>
	void Initialize(Model* model, uint32_t textureHandle, Vector3 EnemyPos);
	///<summary>
	///更新
	///</summary>

	void Update();

	void Move();

	void Afin(WorldTransform& worldTransform_);

	void Fire();

	Vector3 GetWorldPosition();
	///<summary>
	///描画
	///</summary>
	void Draw(ViewProjection viewProjection_);
	//行動フェーズ
	enum class Phase {
		Approch,//接近する
		Leave,//離脱する
	};
	void Approch();

	void Leave();

	void SetPlayer(Player* player) { player_ = player; }

	//ベクトルを正規化する
	int Vec3Normalize(Vector3* pOut, Vector3* pV);

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	////弾リストを取得
	//const std::list<std::unique_ptr<EnemyBullet>>& GetBullets() { return bullets2_; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	//発射間隔
	static const int kFireInterval = 10000;

	Player* player_ = nullptr;

	GameScene* gameScene_ = nullptr;

	//ワールド変換データ
	WorldTransform worldTransform_;

	DebugText* debugText_;
	//モデル
	Model* model_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	//フェーズ
	Phase phase_ = Phase::Approch;
	//キャラクターの移動ベクトル
	Vector3 ApprochMove = { 0,0,-0.01f };
	Vector3 LeaveMove = { -0.1f,0.1f,-0.1f };


	int32_t time = 0;
};