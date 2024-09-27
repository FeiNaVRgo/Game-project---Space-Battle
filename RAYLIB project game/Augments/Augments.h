#pragma once
#include <memory>
#include <functional>
#include <queue>
#include <unordered_map>
#include "../ECS.h"

//Augments - will give certain components special effects
//lets say that laser pistol shoots, then it splits into three smaller lasers that going further

//forward declarations for UpgradesTable

struct Damage;
struct Health;

//very similiar to ID_WEAPON_RARITY
enum class ID_AUGMENT_RARITY {
	COMMON     = 0,
	RARE       = 1,
	EPIC       = 2,
	LEGENDARY  = 3,
};


enum class ID_AUGMENT {
	ID_CLASSIC
};
//use concepts to know what element can be used
//element will have apply func to them - maybe inherited from diffrent struct
template<typename T>
struct TableElement {
	std::unique_ptr<T> element;
	bool isUsed;
};

//Table for Augments
struct UpgradesTable {
	TableElement<Damage> telem_damage;//self explanatory
	TableElement<Health> telem_health;//for max health
};

//consider tackling that event system I wrote to dispatch activeBeahviour when needed and init once

class AugmentDispatcher;

//question: how to add augment to player?
template<typename T>
class Augment {
public:
	using Target   = ECS::Entity;
	
	Augment(AugmentDispatcher& augDispatcher, UpgradesTable const& upgTable, Target const target, ID_AUGMENT id);//TODO -- make implementation
	virtual ~Augment() = default;

	virtual void initWhenGivenToPlayer() = 0;
	virtual void activeBehaviour()       = 0;

	UpgradesTable upgTable;
	Target        augTarget;
	ID_AUGMENT    id;
};  

class AugmentDispatcher {//maybe move dispatcher to diffrent file
public:
	using Behaviour = std::function<void()>;
	using Init      = std::function<void()>;

	std::queue<Init>                          queue_init;
	std::unordered_map<ID_AUGMENT, Behaviour> hash_behaviour;
	
	template<typename T>
	void addAugment(T& aug);
	
	void dispatchInit();
	void doBehaviour();
	void removeAugment(ID_AUGMENT id);//for removing active behaviour funcs
};

class Augment_CLASSIC : public Augment<Augment_CLASSIC> {
	Augment_CLASSIC(
		AugmentDispatcher& augDispatcher, 
		UpgradesTable const& upgTable, 
		Target const target, 
		ID_AUGMENT id
	) : Augment(
			augDispatcher, 
			upgTable,
			target, 
			id) {};

	void initWhenGivenToPlayer() override;
	void activeBehaviour()       override;
};