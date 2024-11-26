#pragma once
#include "../Weapons-experimental.h"
#include "../WeaponPolicies/Creation/BasicCreation.h"
#include "../WeaponPolicies/VarModifier/BasicVarModifier.h"
#include "../WeaponPolicies/Detecting/ClosestTarget.h"
#include "../WeaponPolicies/Shooting/BasicShooting.h"


template<class T>
using Cannon_Creation = BasicCreation < T, "oh my god", "less goo", ID_WEAPON::ID_CANON, ID_WEAPON_RARITY::COMMON, G::weapon_mini_canon, G::weapon_normal_canon, G::playerBulletTexture1, Damage{ 5.f, 5.f, Damage::DAMAGE_TYPE::EXPLOSIVE } > ;

template<class T>
using Cannon_VarMod = BasicVarModifier<T>;

template<class T>
using Cannon_Detect = ClosestTarget<T, ID_ENTITY::ENEMY_ID, 800.f>;

template<class T>
using Cannon_Shooting = BasicShooting<T, 800>;

struct Weapon_CANON : public Weapon<Cannon_Creation, Cannon_VarMod, Cannon_Detect, Cannon_Shooting> {};