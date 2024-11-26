#pragma once
#include "../Weapons-experimental.h"
#include "../WeaponPolicies/Creation/BasicCreation.h"
#include "../WeaponPolicies/VarModifier/BasicVarModifier.h"
#include "../WeaponPolicies/Detecting/ClosestTarget.h"
#include "../WeaponPolicies/Shooting/BasicShooting.h"

template<class T>
using LaserPistol_Creation = BasicCreation < T, "Laser gun", "Shoots powerful laser that pokes holes in enemies", ID_WEAPON::ID_LASERPISTOL, ID_WEAPON_RARITY::RARE, G::weapon_mini_lasergun, G::weapon_normal_lasergun, G::playerBulletTexture3, Damage{ 8.f, 8.f, Damage::DAMAGE_TYPE::ENERGY } > ;

template<class T>
using LaserPistol_VarMod = BasicVarModifier<T>;

template<class T>
using LaserPistol_Detect = ClosestTarget<T, ID_ENTITY::ENEMY_ID, 800.f>;

template<class T>
using LaserPistol_Shooting = BasicShooting<T, 400>;

struct Weapon_LASERPISTOL : public Weapon<LaserPistol_Creation, LaserPistol_VarMod, LaserPistol_Detect, LaserPistol_Shooting> {};
