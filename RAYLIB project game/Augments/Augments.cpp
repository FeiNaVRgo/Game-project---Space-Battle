#include "Augments.h"

#include "../Components/Damage.h"
#include "../Components/Health.h"

Augment::Augment(AugmentDispatcher& augDispatcher, UpgradesTable const& upgTable, Target const target, ID_AUGMENT id) {

}

void AugmentDispatcher::addAugment(Augment& aug) {
	queue_init.emplace(aug.initWhenGivenToPlayer);
	hash_behaviour.try_emplace(aug.id, aug.activeBehaviour);
}

void AugmentDispatcher::dispatchInit() {
	while (!queue_init.empty()) {
		queue_init.front()();
		queue_init.pop();
	}
}

void AugmentDispatcher::doBehaviour() {
	for (auto const& [key, beh] : hash_behaviour) {
		beh();
	}
}

void AugmentDispatcher::removeAugment(ID_AUGMENT id) {
	hash_behaviour.erase(id);
}

void Augment_CLASSIC::initWhenGivenToPlayer() {

}

void Augment_CLASSIC::activeBehaviour() {

}
