﻿#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "RailCamera.h"
#include <sstream>


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数


	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void AddEnemyBullet(std::unique_ptr<EnemyBullet>& enemyBullet);

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void UpdateEnemyPopCommands();

	void ExistenceEnemy(const Vector3& EnemyPos);

	//弾リストを取得
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullets() { return bullets2_; }

	//ゲームフェーズ
	enum class Phase {
		TITLE,//タイトル
		GAME,//本編
		CLEAR,//クリア画面
		GAMEOVER,//ゲームオーバー画面
	};

	void PhaseReset();

	void EnemyPopComandReset();

private: // メンバ変数

	///自キャラ
	Player* player_ = nullptr;

	///敵キャラ

	std::list<std::unique_ptr<Enemy>> enemy_;

	//天球
	Skydome* skydome_ = nullptr;

	//レールカメラ
	RailCamera* railCamera_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	Model* modelSkydome_ = nullptr;


	//カメラ上方向の角度
	float viewAngle = 0.0f;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//3Dモデル
	Model* model_ = nullptr;
	////ワールドトランスフォーム
	//WorldTransform worldTransforms_[100];
	//ビュープロジェクション
	ViewProjection viewProjection_;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	/// 
	//デバックカメラ
	DebugCamera* debugCamera_ = nullptr;

	std::list<std::unique_ptr<EnemyBullet>> bullets2_;

	//敵発生コマンド
	std::stringstream enemyPopCommands;

	bool waitflag = false;
	int waitTimer = 0;

	//フェーズ
	Phase phase_ = Phase::TITLE;
	Vector3 eee = { 0,0,0 };

	int EnemyDeadCount = 0;


	//タイトル用スプライト
	std::unique_ptr<Sprite> sprite2DTitle_;
	std::unique_ptr<Sprite> sprite2DClear_;
	std::unique_ptr<Sprite> sprite2DOver_;
};
