#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
#include "DebugText.h"

///<summary>
/// 初期化
///</summary>
class RailCamera {
public:
	///<summary>
	/// 初期化
	///</summary>
	void Initalize();

	///<summary>
	/// 更新
	///</summary>
	void Update();

	Vector3 MatVector(Matrix4 matrix4, Vector3 vector3);

	void Afin(WorldTransform& worldTransform_);

	 ViewProjection GetViewProjection() { return viewProjection_; }

	 WorldTransform& GetworldTransform() { return worldTransform_; }

	 void Reset();

private:
	//ワールド変換データ
	WorldTransform worldTransform_;

	//ビュープロジェクション
	ViewProjection viewProjection_;

	//キー入力
	Input* input_ = nullptr;

	DebugText* debugText_ = nullptr;
};