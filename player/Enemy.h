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

class Player;
class Enemy {
public:
	///<summary>
	///初期化
	///</summary>
	void Initialize(Model* model, uint32_t textureHandle);
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
	void Draw(ViewProjection& viewProjection_);
	//行動フェーズ
	enum class Phase {
		Approch,//接近する
		Leave,//離脱する
	};
	void Approch();

	void Leave();

	void SetPlayer(Player* player) { player_ = player; }

private:
	//発射間隔
	static const int kFireInterval = 10;

	Player* player_ = nullptr;

	//ワールド変換データ
	WorldTransform worldTransform_;

	//モデル
	Model* model_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	//フェーズ
	Phase phase_ = Phase::Approch;
	//キャラクターの移動ベクトル
	Vector3 ApprochMove = { 0,0,-0.1f };
	Vector3 LeaveMove = { -0.1f,0.1f,-0.1f };

	std::list<std::unique_ptr<EnemyBullet>> bullets2_;

	int32_t time = 0;
};