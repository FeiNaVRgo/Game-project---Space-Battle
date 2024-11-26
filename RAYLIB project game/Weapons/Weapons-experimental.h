#pragma once
#include "../Components/Inventory.h"
#include "WeaponEnumeraions.h"
#include "Weapons.h"


template
<
	template <class T> class P_VarModifier,
	template <class T> class P_Detecting,
	template <class T> class P_Shooting
>
class Behaviour 
	: public P_VarModifier<Behaviour<P_VarModifier, P_Detecting, P_Shooting>>
	, public P_Detecting<Behaviour<P_VarModifier, P_Detecting, P_Shooting>>
	, public P_Shooting<Behaviour<P_VarModifier, P_Detecting, P_Shooting>>
{
	using Beh = Behaviour<P_VarModifier, P_Detecting, P_Shooting>;
	using VarModifier = P_VarModifier<Beh>;
	using Detecting = P_Detecting<Beh>;
	using Shooting = P_Shooting<Beh>;

public:
	void behaviour(ECS::Entity weaponNormalEntity) {
		VarModifier::modify();
		Shooting::shoot(weaponNormalEntity, Detecting::detect(weaponNormalEntity));
	}
};

template
< 
	template <class T> class P_Creation,
	template <class T> class P_VarModify,
	template <class T> class P_Detection,
	template <class T> class P_Shooting
>
class Weapon  
	: public P_Creation<Weapon<P_Creation, P_VarModify, P_Detection, P_Shooting>>
	, public Behaviour<P_VarModify, P_Detection, P_Shooting>
{
private:
	using Wpn = Weapon<P_Creation, P_VarModify, P_Detection, P_Shooting>;
	using Creation = P_Creation<Wpn>;

public:
	void p_CreateMini() {
		Creation::createMini();
	}

	void p_CreateNormal(Inventory& inv, WeaponMini& weaponMini) {
		Creation::createNormal(inv, weaponMini);
	}

	void p_Behaviour(ECS::Entity weaponNormalEntity) {
		this->behaviour(weaponNormalEntity);
	}
};