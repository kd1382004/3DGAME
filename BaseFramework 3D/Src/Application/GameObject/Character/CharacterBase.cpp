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
	if (ImGui::Begin(U8("%s : Status", m_statusEditorName), nullptr, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(U8("メニュー")))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}



		if (ImGui::TreeNode(U8("座標")))
		{
			ImGui::InputFloat(U8("現在HP"), &m_status.HP.nowHP, 0.1f, 1.0f, "%.2f");
			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void CharacterBase::CollisionUpdate()
{
	//レイ判定設定
	/////////////////////////////////////////////
	// 当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();
	// 少し高いところから飛ばす(段差の許容範囲)
	static float enableStepHigh = 0.2f;
	rayInfo.m_pos.y += enableStepHigh;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さを設定
	rayInfo.m_range = m_Gravity + enableStepHigh;
	// 当たり判定タイプを設定
	rayInfo.m_type = KdCollider::TypeGround;
	/////////////////////////////////////////////

	// その他球による衝突判定
	/////////////////////////////////////////////
	// 当たり判定(球判定)用の情報作成
	DirectX::BoundingSphere sphere;
	sphere.Center = GetPos() + Math::Vector3(0, 0.5f, 0);
	sphere.Radius = 0.5f;
	KdCollider::SphereInfo spherInfo(KdCollider::TypeBump, sphere);
	/////////////////////////////////////////////

	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{

			//レイ判定
			//////////////////////////////////////////////////
			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);

			float maxOverLap = 0;
			Math::Vector3 hitPos = {};
			bool hit = false;
			for (auto& ret : retRayList)
			{
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}

			if (hit)
			{
				// 地面に当たっている
				SetPos(hitPos);
				m_Gravity = 0;
			}
			//////////////////////////////////////////////////

			//その他球による衝突判定
			//////////////////////////////////////////////////
			std::list<KdCollider::CollisionResult> retBumpList;
			spGameObj->Intersects(spherInfo, &retBumpList);

			for (auto& ret : retBumpList)
			{
				Math::Vector3 newPos = GetPos() + (ret.m_hitDir * ret.m_overlapDistance);
				SetPos(newPos);
			}
			//////////////////////////////////////////////////

		}
	}
}

void CharacterBase::Release()
{
	m_spCharaModel = nullptr;
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
	status.HP.maxHP = getFloat(data["HP"], "maxHP");

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
	m_status.HP.nowHP = m_status.HP.maxHP;

	// 攻撃力
	m_status.attck.baseAttckPowe = m_status.attck.baseAttckPowe;

	// 防御力
	m_status.defense.baseDefensePowe = m_status.defense.baseDefensePowe;

	// 移動速度
	m_status.moveSpeed.nowSpeed = m_status.moveSpeed.baseSpeed;
}

void CharacterBase::StatusEditor()
{

}
