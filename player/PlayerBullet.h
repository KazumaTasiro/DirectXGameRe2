#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <cassert>
#include "DebugText.h"
#include "Input.h"
#include "ViewProjection.h"
#include "Afin.h"

///<summary>
///自キャラ
///</summary>
class PlayerBullet {
public:
	///<summary>
	///初期化
	///</summary>

	void Initialize(Model* model, const Vector3& position);;



	///<summary>
	///更新
	///</summary>

	void Update();

	///<summary>
	///描画
	///</summary>

	void Draw(const ViewProjection& viewProjection_);

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
};