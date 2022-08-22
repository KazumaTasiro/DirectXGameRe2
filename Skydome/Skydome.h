#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <cassert>
#include "ViewProjection.h"
#include <Afin.h>


///<summary>
///天球
///</summary>
class Skydome {
public:
	///<summary>
	///初期化
	///</summary>
	void Initalize(Model* model);

	///<summary>
	///更新
	///</summary>
	void Update();

	///<summary>
	///描画
	///</summary>
	void Draw(ViewProjection viewProjection_);

	void Afin(WorldTransform& worldTransform_);

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;

};