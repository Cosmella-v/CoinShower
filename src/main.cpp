#include <Geode/modify/GJBaseGameLayer.hpp>

bool g_toggle = false;
bool g_practicemode = false;
class $modify(CoinShowerHook, GJBaseGameLayer) {
	void collisionCheckObjects(PlayerObject *player, gd::vector<GameObject *> *sectionObjects, int objectCount, float dt) {
		if (m_isPracticeMode) {
			if (!g_practicemode)
				return GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, objectCount, dt);
		} else {
			if (!g_toggle)
				return GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, objectCount, dt);
		}
		auto playerRect = player->getObjectRect();
		for (int i = 0; i < objectCount; i++) {
			auto *obj = sectionObjects->at(i);
			if (obj->m_objectType != GameObjectType::SecretCoin && obj->m_objectType != GameObjectType::UserCoin)
				continue;

			auto *effectSprite = geode::cast::typeinfo_cast<EffectGameObject *>(obj);
			if (!effectSprite) // weird, but just in case
				continue;

			auto objectRect = effectSprite->getObjectRect();
			if (!playerRect.intersectsRect(objectRect))
				continue;
			
			bool l_isDisabled = effectSprite->m_isDisabled;
			bool l_isDisabled2 = effectSprite->m_isDisabled2;

			effectSprite->EffectGameObject::triggerObject(this, player->m_uniqueID, nullptr);
			GJBaseGameLayer::destroyObject(effectSprite); 
			
			// undo new state
			effectSprite->m_isDisabled = l_isDisabled;
			effectSprite->m_isDisabled2 = l_isDisabled2;
		};

		GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, objectCount, dt);
	}
};

void toggleMod(bool toggle) {
	g_toggle = toggle;
};

void togglePractice(bool toggle){
    g_practicemode = toggle;
};

$on_mod(Loaded) {
	toggleMod(geode::Mod::get()->getSettingValue<bool>("toggle"));
	geode::listenForSettingChanges<bool>("toggle", toggleMod);
	togglePractice(geode::Mod::get()->getSettingValue<bool>("practice-mode"));
	geode::listenForSettingChanges<bool>("practice-mode", togglePractice);
}