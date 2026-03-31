#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool g_toggle = false;
bool g_practicemode = false;
bool g_soggy = false;
void playSoggyAnimation(CCNode* effectSprite) {
	if (auto parent = effectSprite->getParent()) {
        auto overlay = OverlayManager::get();
        auto overlayPos = overlay->convertToNodeSpace(parent->convertToWorldSpace(effectSprite->getPosition()));
        auto sog = CCSprite::create("soggy.png"_spr);
        overlay->addChild(sog);
		sog->setScale(0);
        sog->setPosition(overlayPos);
        sog->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.1f, 0.1f, 0.3f), CCScaleTo::create(0.1f, 0.3f, 0.3f), nullptr)));
        sog->runAction(CCSequence::create(
            CCSpawn::create(CCMoveBy::create(0.6f, ccp(0, 60)), CCFadeOut::create(0.6f), nullptr),
            CCCallFunc::create(sog, callfunc_selector(CCNode::removeFromParent)),
            nullptr
        ));
};
};
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
			if (g_soggy) {
				playSoggyAnimation(effectSprite);
			};

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
void soggy(bool sogg){
	g_soggy = sogg;
};

$on_mod(Loaded) {
	toggleMod(geode::Mod::get()->getSettingValue<bool>("toggle"));
	geode::listenForSettingChanges<bool>("toggle", toggleMod);
	togglePractice(geode::Mod::get()->getSettingValue<bool>("practice-mode"));
	geode::listenForSettingChanges<bool>("practice-mode", togglePractice);
	soggy(geode::Mod::get()->getSettingValue<bool>("soggy"));
	geode::listenForSettingChanges<bool>("soggy", soggy);
}