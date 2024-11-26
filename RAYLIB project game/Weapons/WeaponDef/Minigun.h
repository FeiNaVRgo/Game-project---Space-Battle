#pragma once
#include "../Weapons-experimental.h"
#include "../WeaponPolicies/Creation/BasicCreation.h"
#include "../WeaponPolicies/VarModifier/BasicVarModifier.h"
#include "../WeaponPolicies/Detecting/ClosestTarget.h"
#include "../WeaponPolicies/Shooting/BasicShooting.h"

template<class T>
using Minigun_Creation = BasicCreation < T, "MINIGUN", "A minigun to shred all enemies", ID_WEAPON::ID_MINIGUN, ID_WEAPON_RARITY::EPIC, G::weapon_mini_minigun, G::weapon_normal_minigun, G::playerBulletTexture2, Damage{ 1.f, 1.f, Damage::DAMAGE_TYPE::PHYSICAL } > ;

template<class T>
using Minigun_VarMod = BasicVarModifier<T>;

template<class T>
using Minigun_Detect = ClosestTarget<T, ID_ENTITY::ENEMY_ID, 800.f>;

template<class T>
using Minigun_Shooting = BasicShooting<T, 200>;

struct Weapon_MINIGUN : public Weapon<Minigun_Creation, Minigun_VarMod, Minigun_Detect, Minigun_Shooting> {};