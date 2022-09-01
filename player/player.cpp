#include "Player.h"
using namespace MathUtility;

Player::Player()
{
}
Player::~Player()
{
}
void Player::Initialize(Model* model, uint32_t textureHandle)
{
	assert(model);
	//�����Ƃ��Ď󂯎�����f�[�^�������o�ϐ��ɋL�^����
	this->model_ = model;
	this->textureHandle_ = textureHandle;
	//�V���O���g���C���X�^���X���擾����
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();
	//���[���h�ϊ��̏�����
	worldTransform_.Initialize();
	worldTransform3DReticle_.Initialize();

	worldTransform_.translation_ = { 0, 0, 5 };

	bulletModel_ = Model::CreateFromOBJ("bullet", true);
	//���e�B�N���p�e�N�X�`���擾
	uint32_t textureReticle = TextureManager::Load("Reticle.png");
	//�X�v���C�g����
	sprite2DReticle_.reset(
		Sprite::Create(textureReticle, Vector2(500, 350), Vector4(1, 1, 1, 1), Vector2(0.5, 0.5)));
}
void Player::Update(ViewProjection viewProjection_)
{
	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});
	//�L�����N�^�[�ړ�����
	Move();
	//�L�����N�^�[�U���X�V
	Attack();
	//�e�X�V
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}
	//���@����3D���e�B�N���ւ̋���
	const float kDistancePlayerTo3DReticle = 60.0f;
	//���@����3D���e�B�N���ւ̃I�t�Z�b�g(Z+����)
	Vector3 offset = { 0, 0, 1.0f };
	//���@�̃��[���h���W�̉�]�𔽉f
	offset = ConvertToVector3( worldTransform_, offset);
	//�x�N�g���̒����𐮂���
	Vec3Normalize(&offset, &offset);
	offset *= kDistancePlayerTo3DReticle;
	//3D���e�B�N�����W�ݒ�
	worldTransform3DReticle_.translation_ = {
		offset.x + worldTransform_.matWorld_.m[3][0],
		offset.y + worldTransform_.matWorld_.m[3][1],
		offset.z + worldTransform_.matWorld_.m[3][2]
	};
	//�s��X�V
	Afin(worldTransform3DReticle_);
	worldTransform3DReticle_.TransferMatrix();

	//3D���e�B�N���̃��[���h���W����2D���e�B�N���̃X�N���[�����W���v�Z
	Vector3 positionReticle = {worldTransform3DReticle_.matWorld_.m[3][0],worldTransform3DReticle_.matWorld_.m[3][1],worldTransform3DReticle_.matWorld_.m[3][2] };

	Vector2 windowWH =
		Vector2(WinApp::GetInstance()->kWindowWidth, WinApp::GetInstance()->kWindowHeight);

	//�r���[�|�[�g�s��
	Matrix4 Viewport = {
	   windowWH.x / 2,			  0,  0,  0,
					0,	-windowWH.y / 2,  0,  0,
					0,				  0,  1,  0,
	   windowWH.x / 2,	 windowWH.y / 2,  0,  1
	};

	//�r���[�s��ƃv���W�F�N�V�����s��A�r���[�|�[�g�s�����������
	Matrix4 matViewProjectionViewport = viewProjection_.matView * viewProjection_.matProjection * Viewport;

	//���[���h���X�N���[�����W�ϊ�(��������3D����2D�ɂȂ�)
	positionReticle = clossV3V4(positionReticle, matViewProjectionViewport);

	//�X�v���C�g�̃��e�B�N���ɍ��W�ݒ�
	sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

	//�}�E�X�J�[�\���̃X�N���[�����W���烏�[���h���W���擾����3D���e�B�N���z�u
	{
		POINT mousePosition;
		//�}�E�X���W�i�X�N���[�����W�j���擾����
		GetCursorPos(&mousePosition);

		//�N���C�A���g�G���A���W�ɕϊ�����
		HWND hwnd = WinApp::GetInstance()->GetHwnd();
		ScreenToClient(hwnd, &mousePosition);

		//�}�E�X���W��2D���e�B�N���̃X�v���C�g�ɑ������
		sprite2DReticle_->SetPosition(Vector2(mousePosition.x, mousePosition.y));

		//�r���[�s��A�ˉe�ϊ��A�r���[�|�[�g�s��̍����s����v�Z����
		Matrix4 matVPV = viewProjection_.matView * viewProjection_.matProjection * Viewport;

		//�����s��̋t�s����v�Z����
		//�����s��̋t�s����v�Z����
		Matrix4 matInverseVPV;
		MatrixInverse(matInverseVPV, matVPV);

		//�j�A�N���b�v�ʏ�̃��[���h���W����i�X�N���[�������[���h�ϊ��j
		Vector3 posNear = Vector3(mousePosition.x, mousePosition.y, 0);
		//�t�@�[�N���b�v�ʏ�̃��[���h���W�𓾂�i�X�N���[�������[���h�ϊ��j
		Vector3 posFar = Vector3(mousePosition.x, mousePosition.y, 1);

		//�X�N���[�����W�n���烏�[���h���W�n��
		posNear = clossV3V4(posNear, matInverseVPV);
		posFar = clossV3V4(posFar, matInverseVPV);

		//�}�E�X�̑O���x�N�g�����v�Z����
		//�}�E�X���C�̕���
		Vector3 mouseDirection = posFar - posNear;
		mouseDirection = Vector3Normalize(mouseDirection);
		//�j�A�N���b�v�ʏ�̃��[���h���W�����苗���O�i�����Ƃ����3D���e�B�N����z�u
		//�J��������Ə��I�u�W�F�N�g�̋���
		const float kDistanceTestObject = 222.0f;
		worldTransform3DReticle_.translation_ = AddVector(posNear, mouseDirection * kDistanceTestObject);

		//�s��X�V
		Afin(worldTransform3DReticle_);
		worldTransform3DReticle_.TransferMatrix();

		/*debugText_->SetPos(50, 150);
		debugText_->Printf(
			"translation : %f,%f,%f", worldTransform_.translation_.x,
			worldTransform_.translation_.y,
			worldTransform_.translation_.z);*/
	}
}
void Player::Move()
{
	//�L�����N�^�[�̈ړ��x�N�g��
	Vector3 move = { 0,0,0 };
	const float speed = 0.1f;
	const float RotSpeed = 0.05f;
	/*if (input_->PushKey(DIK_A)) {
		move.x -= speed;
	}
	else if (input_->PushKey(DIK_D)) {
		move.x += speed;
	}
	if (input_->PushKey(DIK_W)) {
		move.y += speed;
	}
	else if (input_->PushKey(DIK_S)) {
		move.y -= speed;
	}
	if (input_->PushKey(DIK_U)) {
		worldTransform_.rotation_.y -= RotSpeed;
	}
	else if (input_->PushKey(DIK_I)) {
		worldTransform_.rotation_.y += RotSpeed;
	}*/
	worldTransform_.translation_ += move;
	//�L�[�{�[�h���͂ɂ��ړ�����
	//�ړ����E���W
	const float kMoveLimitX = 35.0f;
	const float kMoveLimitY = 19.0f;
	//�͈͂𒴂��Ȃ�����
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);


	Afin(worldTransform_);

	worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;

	

	worldTransform_.TransferMatrix();
	/*debugText_->SetPos(50, 50);
	debugText_->Printf("x:%f,y:%f,z:%f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);*/
}
void Player::Draw(ViewProjection viewProjection_)
{
	/*model_->Draw(worldTransform_, viewProjection_, textureHandle_);*/
	//�e�`��
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
	////3D���e�B�N����`��
	//model_->Draw(worldTransform3DReticle_, viewProjection_, textureHandle_);
}
void Player::Attack()
{
	if (input_->IsTriggerMouse(0))
	{
		//�e�̑��x
		const float kBulletSpeed = 0.01f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
		velocity = ConvertToVector3(worldTransform_, velocity);

		//���@����W���I�u�W�F�N�g�ւ̃x�N�g��
		velocity = { 
		worldTransform3DReticle_.matWorld_.m[3][0] - worldTransform_.matWorld_.m[3][0],
		worldTransform3DReticle_.matWorld_.m[3][1] - worldTransform_.matWorld_.m[3][1],
		worldTransform3DReticle_.matWorld_.m[3][2] - worldTransform_.matWorld_.m[3][2] };
		Vec3Normalize(&velocity, &velocity);
		//�e�𐶐����A������
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(bulletModel_, GetWorldPosition(), velocity);
		//�e�𔭎˂���
		bullets_.push_back(std::move(newBullet));
	}
}
int Player::Vec3Normalize(Vector3* pOut, Vector3* pV)
{
	double len;
	double x, y, z;

	x = (double)(pV->x);
	y = (double)(pV->y);
	z = (double)(pV->z);
	len = sqrt(x * x + y * y + z * z);

	if (len < (1e-6)) return 0;

	len = 1.0 / len;
	x *= len;
	y *= len;
	z *= len;

	pOut->x = (float)x;
	pOut->y = (float)y;
	pOut->z = (float)z;

	return 1;
}
void Player::DrawUI()
{
	sprite2DReticle_->Draw();
}
Vector3 Player::clossV3V4(const Vector3& vec, const Matrix4& mat)
{
	Vector4 divVec = {};

	divVec.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + 1 * mat.m[3][0];

	divVec.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + 1 * mat.m[3][1];

	divVec.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + 1 * mat.m[3][2];

	divVec.w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + 1 * mat.m[3][3];

	divVec.x = divVec.x / divVec.w;

	divVec.y = divVec.y / divVec.w;

	divVec.z = divVec.z / divVec.w;

	return { divVec.x, divVec.y, divVec.z };
}
int Player::MatrixInverse(Matrix4& pOut, Matrix4& pM)
{
	Matrix4 mat;
	int i, j, loop;
	double fDat, fDat2;
	double mat_8x4[4][8];
	int flag;
	float* pF;
	double* pD;

	//8 x 4�s��ɒl������
	for (i = 0; i < 4; i++) {
		pF = pM.m[i];
		for (j = 0; j < 4; j++, pF++) mat_8x4[i][j] = (double)(*pF);
		pD = mat_8x4[i] + 4;
		for (j = 0; j < 4; j++) {
			if (i == j)   *pD = 1.0;
			else         *pD = 0.0;
			pD++;
		}
	}

	flag = 1;
	for (loop = 0; loop < 4; loop++) {
		fDat = mat_8x4[loop][loop];
		if (fDat != 1.0) {
			if (fDat == 0.0) {
				for (i = loop + 1; i < 4; i++) {
					fDat = mat_8x4[i][loop];
					if (fDat != 0.0) break;
				}
				if (i >= 4) {
					flag = 0;
					break;
				}
				//�s�����ւ���
				for (j = 0; j < 8; j++) {
					fDat = mat_8x4[i][j];
					mat_8x4[i][j] = mat_8x4[loop][j];
					mat_8x4[loop][j] = fDat;
				}
				fDat = mat_8x4[loop][loop];
			}

			for (i = 0; i < 8; i++) mat_8x4[loop][i] /= fDat;
		}
		for (i = 0; i < 4; i++) {
			if (i != loop) {
				fDat = mat_8x4[i][loop];
				if (fDat != 0.0f) {
					//mat[i][loop]��mat[loop]�̍s�ɂ�����
					//(mat[j] - mat[loop] * fDat)���v�Z
					for (j = 0; j < 8; j++) {
						fDat2 = mat_8x4[loop][j] * fDat;
						mat_8x4[i][j] -= fDat2;
					}
				}
			}
		}
	}

	if (flag) {
		for (i = 0; i < 4; i++) {
			pF = mat.m[i];
			pD = mat_8x4[i] + 4;
			for (j = 0; j < 4; j++) {
				*pF = (float)(*pD);
				pF++;
				pD++;
			}
		}
	}
	else {
		//�P�ʍs������߂�
		mat = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		};
	}

	pOut = mat;

	if (flag) return 1;
	return 0;
}
Vector3 Player::AddVector(const Vector3 v1, const Vector3 v2)
{
	Vector3 addVec = {};

	addVec.x = v1.x + v2.x;

	addVec.y = v1.y + v2.y;

	addVec.z = v1.z + v2.z;

	return addVec;
}
void Player::Reset()
{
	worldTransform_.translation_ = { 0, 0, 5 };
	bullets_.clear();
}
void Player::Afin(WorldTransform& worldTransform_)
{
	Matrix4 matScale;
	Matrix4 matRot;
	Matrix4 matRotX;
	Matrix4 matRotY;
	Matrix4 matRotZ;
	Matrix4 matTrans;
	matScale = {
	worldTransform_.scale_.x,0,0,0,
	0,worldTransform_.scale_.y,0,0,
	0,0,worldTransform_.scale_.z,0,
	0,0,0,1
	};
	matTrans = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	worldTransform_.translation_.x,worldTransform_.translation_.y,worldTransform_.translation_.z,1
	};
	//Z�p��]���̐ݒ�
	matRotZ = {
	cos(worldTransform_.rotation_.z),sin(worldTransform_.rotation_.z),0,0,
	-sin(worldTransform_.rotation_.z),cos(worldTransform_.rotation_.z),0,0,
	0,0,1,0,
	0,0,0,1
	};
	//X�p��]���̐ݒ�
	matRotX = {
	1,0,0,0,
	0,cos(worldTransform_.rotation_.x),sin(worldTransform_.rotation_.x),0,
	0,-sin(worldTransform_.rotation_.x),cos(worldTransform_.rotation_.x),0,
	0,0,0,1
	};
	//Y�p��]���̐ݒ�
	matRotY = {
	cos(worldTransform_.rotation_.y),0,-sin(worldTransform_.rotation_.y),0,
	0,1,0,0,
	sin(worldTransform_.rotation_.y),0,cos(worldTransform_.rotation_.y),0,
	0,0,0,1
	};
	matRot = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
	};
	//�e���̉�]�s�������
	matRot *= matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;
	worldTransform_.matWorld_ = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	worldTransform_.matWorld_ *= matScale;
	worldTransform_.matWorld_ *= matRot;
	worldTransform_.matWorld_ *= matTrans;
}

void Player::OnCollision()
{
	//�f�X�t���O
	bool isDead_ = false;
}

Vector3 Player::ConvertToVector3(WorldTransform& mat, Vector3 vec){

Vector3 retVec = {};

retVec.x = vec.x * mat.matWorld_.m[0][0] + vec.y * mat.matWorld_.m[1][0] + vec.z * mat.matWorld_.m[2][0];

retVec.y = vec.x * mat.matWorld_.m[0][1] + vec.y * mat.matWorld_.m[1][1] + vec.z * mat.matWorld_.m[2][1];

retVec.z = vec.x * mat.matWorld_.m[0][2] + vec.y * mat.matWorld_.m[1][2] + vec.z * mat.matWorld_.m[2][2];

return retVec;
}

Vector3 Player::GetWorldPosition()
{
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��̕��s�ړ��������擾�i���[���h���W�j
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetRailCamera(WorldTransform& worldTransform_)
{
	this->worldTransform_.parent_ = &worldTransform_;
}
