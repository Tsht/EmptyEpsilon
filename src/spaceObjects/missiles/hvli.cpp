#include "hvli.h"
#include "particleEffect.h"
#include "spaceObjects/explosionEffect.h"

/// HVLI missile
REGISTER_SCRIPT_SUBCLASS(HVLI, MissileWeapon)
{
  //registered for typeName and creation
}

REGISTER_MULTIPLAYER_CLASS(HVLI, "HVLI");
HVLI::HVLI()
: MissileWeapon("HVLI", MissileWeaponData::getDataFor(MW_HVLI), DT_Kinetic)
{
    setRadarSignatureInfo(0.1, 0.0, 0.0);
    setCollisionBox({10, 30}); // Make it a bit harder to the HVLI to phase trough smaller enemies
}

void HVLI::hitObject(P<SpaceObject> object)
{
    DamageInfo info(owner, damage_type, getPosition());
    float alive_for = MissileWeaponData::getDataFor(MW_HVLI).lifetime - lifetime;
    if (alive_for > 2.0f)
        object->takeDamage(category_modifier * 10 * damage_multiplier, info);
    else
        object->takeDamage(category_modifier * 10 * damage_multiplier * (alive_for / 2.0f), info);
    P<ExplosionEffect> e = new ExplosionEffect();
    e->setSize(category_modifier * 20);
    e->setPosition(getPosition());
    e->setOnRadar(true);
    setRadarSignatureInfo(0.0, 0.0, 0.1);
}

    