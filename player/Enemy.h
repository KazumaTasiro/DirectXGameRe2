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

///<summary>
///敵キャラ
///</summary>

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

	///<summary>
	///描画
	///</summary>

	void Draw(ViewProjection& viewProjection_);

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
};