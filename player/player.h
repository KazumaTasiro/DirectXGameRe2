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
#include <WinApp.h>

class RailCamera;
///<summary>
///自キャラ
///</summary>
class Player {
public:
	Player();
	~Player();
	///<summary>
	///初期化
	///</summary>
	void Initialize(Model* model, uint32_t textureHandle);
	///<summary>
	///更新
	///</summary>
	void Update(ViewProjection viewProjection_);
	void Move();
	void Afin(WorldTransform& worldTransform_);

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	Vector3 ConvertToVector3(WorldTransform& mat, Vector3 vec);

	Vector3 GetWorldPosition();

	void SetRailCamera(WorldTransform& worldTransform_);

	///<summary>
	///描画
	///</summary>
	void Draw(ViewProjection viewProjection_);
	///<summary>
	///攻撃
	///</summary>
	void Attack();

	//弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

	//ベクトルを正規化する
	int Vec3Normalize(Vector3* pOut, Vector3* pV);

	///<summary>
	///UI描画
	///</summary>
	void DrawUI();

	Vector3 clossV3V4(const Vector3& vec, const Matrix4& mat);

private:
	//ワールド変換データ
	WorldTransform worldTransform_;

	//3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	//モデル
	Model* model_ = nullptr;
	//弾モデル
	Model* bulletModel_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	Input* input_ = nullptr;
	DebugText* debugText_ = nullptr;
	////ワールドトランスフォーム
	//WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//弾
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	//
	RailCamera* railCamera_ = nullptr;
	//2Dレティクル用スプライト
	std::unique_ptr<Sprite> sprite2DReticle_;

};