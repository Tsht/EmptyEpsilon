#include "missileWeaponData.h"
#include "multiplayer.h"
namespace {

    MissileWeaponData missile_data[MW_Count] =
    {
        //                speed, turnrate, lifetime, color, homing_range
        MissileWeaponData(200.0f, 10.f, 54.0f, sf::Color(255, 200, 0), 1200.0, "sfx/rlaunch.wav", MW_Homing, DT_Kinetic),/*MW_Homing*/
        MissileWeaponData(500.0f,  0.f, 27.0f, sf::Color(200, 200, 200), 0.0, "sfx/hvli_fire.wav", MW_HVLI, DT_Kinetic),/*MW_HVLI*/
        MissileWeaponData(200.0f, 10.f, 54.0f, sf::Color(100, 32, 255), 500.0, "sfx/rlaunch.wav", MW_EMP, DT_EMP),/*MW_EMP*/
        MissileWeaponData(200.0f, 10.f, 54.0f, sf::Color(255, 100, 32), 500.0, "sfx/rlaunch.wav", MW_Nuke, DT_Kinetic),/*MW_Nuke*/
        MissileWeaponData(100.0f,  0.f, 30.0f, sf::Color(255, 255, 255), 0.0, "sfx/missile_launch.wav", MW_Mine, DT_Kinetic),/*MW_Mine, lifetime is used at time which the mine is ejecting from the ship*/
    };
}

MissileWeaponData::MissileWeaponData(float speed, float turnrate, float lifetime, sf::Color color, float homing_range, string fire_sound, EMissileWeapons base_type, EDamageType dt)
: basetype(base_type), speed(speed), turnrate(turnrate), lifetime(lifetime), color(color), homing_range(homing_range),
fire_sound(fire_sound), damage_multiplier(1), fire_count(1) , line_count(1),damage_type(dt)
{
}

const MissileWeaponData& MissileWeaponData::getDataFor(const EMissileWeapons& type)
{
    if (type == MW_None)
        return missile_data[0];
    return missile_data[type];
}

bool isNumber(const std::string& s)
{
   return !s.empty() && s.find_first_not_of("-.0123456789") == std::string::npos;
}


const MissileWeaponData& MissileWeaponData::getDataFor(const string& type)
{
    if(isNumber(type))
    {
        EMissileWeapons typeNum = (EMissileWeapons)std::stoi(type);
        return MissileWeaponData::getDataFor(typeNum);
    }

    return CustomMissileWeaponRegistry::getMissileWeapon(type);

}


void CustomMissileWeaponRegistry::createMissileWeapon(const EMissileWeapons &iBaseType, const std::string &iNewName, const float &iDamageMultiplier, const float &iSpeed, const EDamageType &iDT, const float &iLifeTime)
{
    if((getCustomMissileWeaponTypes().size()) >= (sizeof(uint32_t) - MW_Count))
        assert(0 && "Taille maximum de custom weapon atteinte");
    MissileWeaponData base = MissileWeaponData::getDataFor(iBaseType);
    MissileWeaponData copyMWD = base;
    copyMWD.damage_multiplier = iDamageMultiplier;
    copyMWD.speed = iSpeed;
    copyMWD.basetype = iBaseType;
    copyMWD.damage_type = iDT;
    if(iLifeTime != -1)
        copyMWD.lifetime = iLifeTime;
    //copyMWD.fire_count = iFireCount;
    //copyMWD.line_count = iLineCount;

    getCustomMissileWeapons().insert(MissileWeaponMap::value_type(iNewName,copyMWD));
    getCustomMissileWeaponTypes().insert(MissileWeaponTypeMap::value_type(iNewName,getCustomMissileWeaponTypes().size()+ MW_Count +1));

}
auto CustomMissileWeaponRegistry::getMissileWeapon(const std::string &iName) -> MissileWeaponData&
{
    auto found = getCustomMissileWeapons().find(iName);

    if (found == getCustomMissileWeapons().end())
        return missile_data[0]; //This is BADDDD !!! FIXME FIXME FIXME (why does it assert on some case when AI fighting ? must be a stupid mistake)
        //assert(0 && "not found ");
    return found->second;
}

auto CustomMissileWeaponRegistry::getMissileWeaponType(const std::string &iName) -> uint32_t&
{
    auto found = getCustomMissileWeaponTypes().find(iName);
    static uint32_t type_default = 0;
    if (found == getCustomMissileWeaponTypes().end())
        return type_default; //This is BADDDD !!! FIXME FIXME FIXME (why does it assert on some case when AI fighting ? must be a stupid mistake)
        //assert(0 && "not found ");
    return found->second;
}

string getMissileSizeString(EMissileSizes size)
{
    switch (size)
    {
        case MS_Small:
            return "small";
        case MS_Medium:
            return "medium";
        case MS_Large:
            return "large";
        default:
            return string("unknown size:") + string(size);
    }
}

const float MissileWeaponData::convertSizeToCategoryModifier(EMissileSizes size)
{
    switch(size)
    {
        case MS_Small:
            return 0.5;
        case MS_Medium:
            return 1.0;
        case MS_Large:
            return 2.0;
        default:
            return 1.0;
    }
}

const EMissileSizes MissileWeaponData::convertCategoryModifierToSize(float size)
{
    if (std::abs(size - 0.5) < 0.1)
        return MS_Small;
    if (std::abs(size - 1.0) < 0.1)
        return MS_Medium;
    if (std::abs(size - 2.0) < 0.1)
        return MS_Large;
    return MS_Medium;
}

#include "missileWeaponData.hpp"
