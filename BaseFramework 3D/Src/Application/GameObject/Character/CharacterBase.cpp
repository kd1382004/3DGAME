#include "CharacterBase.h"

void CharacterBase::Init()
{

}

void CharacterBase::Update()
{

}

void CharacterBase::PostUpdate()
{
	CollisionUpdate();
}

void CharacterBase::GenerateDepthMapFromLight()
{
	if (m_spCharaModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld);
	}
}

void CharacterBase::DrawLit()
{
	if (m_spCharaModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld);
	}
}

void CharacterBase::ImGUI()
{
	std::string windowName = m_statusEditorName + " : Status";
	if (ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(U8("メニュー")))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}



		if (ImGui::TreeNode(U8("HP")))
		{
			ImGui::InputFloat(U8("基礎HP"), &m_status.HP.baseHP, 0.1f, 1.0f, "%.2f");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode(U8("Attck")))
		{
			ImGui::InputFloat(U8("基礎Attck"), &m_status.attck.baseAttckPowe, 0.1f, 1.0f, "%.2f");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode(U8("Defense")))
		{
			ImGui::InputFloat(U8("基礎Defense"), &m_status.defense.baseDefensePowe, 0.1f, 1.0f, "%.2f");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode(U8("MoveSpeed")))
		{
			ImGui::InputFloat(U8("基礎MoveSpeed"), &m_status.moveSpeed.baseSpeed, 0.01f, 1.0f, "%.2f");
			ImGui::InputFloat(U8("歩くときに足す値"), &m_status.moveSpeed.walkMovePowe, 0.01f, 1.0f, "%.2f");
			ImGui::Text(U8("歩く速度 : %f\n"), m_status.moveSpeed.baseSpeed + m_status.moveSpeed.walkMovePowe);



			ImGui::InputFloat(U8("走るときに足す値"), &m_status.moveSpeed.runMovePowe, 0.01f, 1.0f, "%.2f");
			ImGui::Text(U8("走る速度 : %f\n"), m_status.moveSpeed.baseSpeed + m_status.moveSpeed.runMovePowe);
			ImGui::TreePop();
		}


		if (ImGui::Button(U8("保存")))
		{
			SaveCharaStatus(m_charaStatusFilePath);
		}
	}

	ImGui::End();
}

void CharacterBase::OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate)
{	
	// ダメージ処理
	m_status.HP.nowHP -= DamagecClculationFormula(_damage, _ignoreRate);

	// のけぞり
	if (_hitStunTime > 0)
	{
		m_hitStunTimer = _hitStunTime;
		m_hitStunFlg = true;
	}

	// ふっとばし
	if (_knockbackDistance > 0)
	{
		m_pos += _knockbackDir * _knockbackDistance;
	}


	SetPos(m_pos);
}

void CharacterBase::CollisionUpdate()
{

	// あたり判定オブジェクトから期限切れの要素を削除
	auto it = m_wpHitObjectList.begin();
	while (it != m_wpHitObjectList.end())
	{
		if (it->expired())
		{
			it = m_wpHitObjectList.erase(it);
		}
		else
		{
			it++;
		}
	}


	// 地面判定するよ
	// ----- ----- ----- ----- -----

	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();
	// 少し高いところから飛ばす(段差の許容範囲)
	static float enableStepHigh = 0.2f;
	rayInfo.m_pos.y += enableStepHigh;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さを設定
	rayInfo.m_range = m_Gravity + enableStepHigh + 0.5f;
	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	float maxOverLap = 0;
	Math::Vector3 hitPos = {};
	bool hit = false;

	// ②HIT判定対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{
			float dist = (GetPos() - spGameObj->GetPos()).Length();

			//処理軽減用
			if (dist >= m_detectRange) { continue; }


			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);

			// ③ 結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出

			for (auto& ret : retRayList)
			{
				// レイを遮断しオーバーした長さが
				// 一番長いものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
		}
	}

	if (hit)
	{
		// 地面に当たっている
		SetPos(hitPos);
		m_Gravity = 0;
		m_groundHit = true;
	}
	else
	{
		m_groundHit = false;
	}

	// その他球による衝突判定 (壁判定)
	// ----- ----- ----- ----- -----
	// 反復押し戻し処理
	for (int pass = 0; pass < 3; pass++)
	{
		DirectX::BoundingSphere sphere;
		sphere.Center = GetPos() + Math::Vector3(0, 0.5f, 0);
		sphere.Radius = 1.0f;
		KdCollider::SphereInfo spherInfo(KdCollider::TypeBump, sphere);

		if (m_pDebugWire && pass == 0)
		{
			m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius);
			m_pDebugWire->AddDebugSphere(sphere.Center, m_detectRange, kRedColor);
			m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);
		}

		Math::Vector3 maxPush = Math::Vector3::Zero;
		float maxOverlap = 0.0f;
		bool bumpHit = false;

		for (auto& wpGameObj : m_wpHitObjectList)
		{
			auto spGameObj = wpGameObj.lock();
			if (!spGameObj) continue;


			//処理軽減
			float dist = (GetPos() - spGameObj->GetPos()).Length();
			if (dist >= m_detectRange) { continue; }

			std::list<KdCollider::CollisionResult> retBumpList;
			if (spGameObj->Intersects(spherInfo, &retBumpList))
			{
				//Hit後処理
				spGameObj->IsHit();
			}

			for (auto& ret : retBumpList)
			{
				if (ret.m_overlapDistance > maxOverlap)
				{
					maxOverlap = ret.m_overlapDistance;
					maxPush = ret.m_hitDir * ret.m_overlapDistance;
					bumpHit = true;
				}
			}
		}

		if (bumpHit)
		{
			SetPos(GetPos() + maxPush);
			m_wallHit = true;
		}
		else
		{
			m_wallHit = false;
			break; // 衝突が無くなればループ終了
		}
	}
}

void CharacterBase::Release()
{
	m_spCharaModel = nullptr;
}

float CharacterBase::DamagecClculationFormula(float _damage,float _ignoreRate)
{
	//100超えないようにクランプ
	_ignoreRate = std::clamp(_ignoreRate, 0.0f, 1.0f);

	float effectiveDefense = m_status.defense.nowDefense * (1.0f - _ignoreRate);
	float defense = 100.0f / (100.0f + effectiveDefense);
	float returnDamage = _damage * defense;

	return returnDamage;
}

void CharacterBase::LoadCharaStatus(std::string _filePath)
{
	Status status;

	std::ifstream ifs(_filePath);
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] ステータスファイルが見つかりません : %s\n"), _filePath.c_str());
		return;
	}

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] ステータスファイルの読み込みに失敗しました : %s\n"), _filePath.c_str());
		return;
	}

	auto getFloat = [&](const nlohmann::json& j, const char* key, float def = 0.0f)
		{
			if (j.contains(key) && j[key].is_number())
				return j[key].get<float>();
			return def;
		};

	// HP
	status.HP.baseHP = getFloat(data["HP"], "baseHP");

	// 攻撃力
	status.attck.baseAttckPowe = getFloat(data["attck"], "baseAttckPowe");

	// 防御力
	status.defense.baseDefensePowe = getFloat(data["defense"], "baseDefensePowe");

	// 移動速度
	status.moveSpeed.baseSpeed = getFloat(data["moveSpeed"], "baseSpeed");
	status.moveSpeed.walkMovePowe = getFloat(data["moveSpeed"], "walkMovePowe");
	status.moveSpeed.runMovePowe = getFloat(data["moveSpeed"], "runMovePowe");

	m_status = status;

	// HP
	m_status.HP.maxHP = m_status.HP.baseHP;
	m_status.HP.nowHP = m_status.HP.maxHP;

	// 攻撃力
	m_status.attck.baseAttckPowe = m_status.attck.baseAttckPowe;

	// 防御力
	m_status.defense.baseDefensePowe = m_status.defense.baseDefensePowe;

	// 移動速度
	m_status.moveSpeed.nowSpeed = m_status.moveSpeed.baseSpeed;
}

void CharacterBase::AngeleUpdate()
{
	if (m_moveVec.Length() == 0) { return; }


	//今キャラが向いている方向
	Math::Vector3 nowDir = m_mWorld.Backward();

	//向きたい方向
	Math::Vector3 toDir = m_moveVec;

	//内積を求める ベクトルA ・ ベクトルB
	float dot = nowDir.Dot(toDir);
	dot = std::clamp(dot, -1.0f, 1.0f);

	//角度に変換
	float angle = DirectX::XMConvertToDegrees(acos(dot));

	//少しでも回転する必要があったら
	if (angle >= 0.1f)
	{
		//回転角度の上限
		if (angle > 10)
		{
			angle = 10;
		}

		//回転軸
		//外積
		Math::Vector3 cross = nowDir.Cross(toDir);


		if (cross.y >= 0)
		{
			//右回転
			m_angle += angle;

			if (m_angle > 360)
			{
				m_angle -= 360;
			}
		}
		else
		{
			//左回転
			m_angle -= angle;

			if (m_angle < 0)
			{
				m_angle += 360;
			}
		}
	}
}


void CharacterBase::StatusEditor()
{

}

void CharacterBase::SaveCharaStatus(std::string _filePath)
{
	nlohmann::json data;

	data["HP"]["baseHP"] = m_status.HP.baseHP;
	data["attck"]["baseAttckPowe"] = m_status.attck.baseAttckPowe;
	data["defense"]["baseDefensePowe"] = m_status.defense.baseDefensePowe;

	data["moveSpeed"]["baseSpeed"] = m_status.moveSpeed.baseSpeed;
	data["moveSpeed"]["walkMovePowe"] = m_status.moveSpeed.walkMovePowe;
	data["moveSpeed"]["runMovePowe"] = m_status.moveSpeed.runMovePowe;

	std::ofstream ofs(_filePath);
	ofs << data.dump(4);

}