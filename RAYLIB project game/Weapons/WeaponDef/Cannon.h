#pragma once
#include "../Weapons-experimental.h"


template<class T>
using Cannon_Creation = BasicCreation < T, "oh my god", "less goo", ID_WEAPON::ID_CANON, ID_WEAPON_RARITY::COMMON, G::weapon_mini_canon, G::weapon_normal_canon, G::playerBulletTexture1, Damage{ 25.f, 25.f, Damage::DAMAGE_TYPE::PHYSICAL } > ;

template<class T>
using Cannon_VarMod = BasicVarModifier<T>;

template<class T>
using Cannon_Detect = ClosestEnemy<T, ID_ENTITY::ENEMY_ID, 800.f>;

template<class T>
using Cannon_Shooting = BasicShooting<T, 800>;

class Cannon_weapon : public Weapon<Cannon_Creation, Cannon_VarMod, Cannon_Detect, Cannon_Shooting> {};