#include "weaponTube.h"
#include "spaceObjects/missiles/EMPMissile.h"
#include "spaceObjects/missiles/homingMissile.h"
#include "spaceObjects/mine.h"
#include "spaceObjects/missiles/nuke.h"
#include "spaceObjects/missiles/hvli.h"
#include "spaceObjects/spaceship.h"

WeaponTube::WeaponTube()
{
    parent = nullptr;

    load_time = 8.0;
    direction = 0;
    //type_allowed_mask = (1 << MW_Count) - 1;
    type_allowed_mask = ~(type_allowed_mask & 0);
    type_loaded = "";
    state = WTS_Empty;
    delay = 0.0;
    tube_index = 0;
    size = MS_Medium;
}

void WeaponTube::setParent(SpaceShip* parent)
{
    assert(!this->parent);
    this->parent = parent;

    parent->registerMemberReplication(&load_time);
    parent->registerMemberReplication(&type_allowed_mask);
    parent->registerMemberReplication(&direction);
    parent->registerMemberReplication(&size);

    parent->registerMemberReplication(&type_loaded);
    parent->registerMemberReplication(&state);
    parent->registerMemberReplication(&delay, 0.5);
}

float WeaponTube::getLoadTimeConfig()
{
    return load_time;
}

void WeaponTube::setLoadTimeConfig(float load_time)
{
    this->load_time = load_time;
}

void WeaponTube::setIndex(int index)
{
    tube_index = index;
}

void WeaponTube::setDirection(float direction)
{
    this->direction = direction;
}

float WeaponTube::getDirection()
{
    return direction;
}

namespace
{
    bool isNumber(const std::string& s)
    {
       return !s.empty() && s.find_first_not_of("-.0123456789") == std::string::npos;
    }
}

void WeaponTube::startLoad(string type)
{
    if (state != WTS_Empty)
        return;
    if (!canLoad(type))
        return;

    if(isNumber(type))
    {
        int weaponType = std::stoi(type);
        if (parent->weapon_storage[weaponType] <= 0)
            return;
        parent->weapon_storage[weaponType]--;
    }
    else
    {
        if (parent->custom_weapon_storage[type] <= 0)
            return;
        parent->custom_weapon_storage[type]--;
    }
    state = WTS_Loading;
    delay = load_time;
    parent->forceMemberReplicationUpdate(&delay);
    type_loaded = type;
}

void WeaponTube::startUnload()
{
    if (state == WTS_Loaded)
    {
        state = WTS_Unloading;
        delay = load_time;
        parent->forceMemberReplicationUpdate(&delay);
    }
}

void WeaponTube::fire(float target_angle)
{
    parent->didAnOffensiveAction();

    if (parent->docking_state != DS_NotDocking) return;
    if (parent->current_warp > 0.0) return;
    if (state != WTS_Loaded) return;

    line_count = 1;

    if(isNumber(type_loaded))
    {
        int typeNumber = std::stoi(type_loaded);
        if (typeNumber == MW_HVLI)
        {
            fire_count = 5;
            state = WTS_Firing;
            delay = 0.0;
            return;
        }
    }
    else
    {
        const MissileWeaponData& data = MissileWeaponData::getDataFor(type_loaded);
        int target_fire_count = data.fire_count;
        line_count = data.line_count;

        if(target_fire_count > 1)
        {
            fire_count = target_fire_count;
            state = WTS_Firing;
            delay = 0.0;
            return;
        }
    }
    spawnProjectile(target_angle);
    state = WTS_Empty;
    type_loaded = "";
}

float WeaponTube::getSizeCategoryModifier()
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


void WeaponTube::spawnProjectile(float target_angle)
{
    //get higher bound for lined shots then substract distance each time
    constexpr int distance_line = 150.0f;
    sf::Vector2f offset(((line_count-1)/2.0f) * distance_line, 0); 

    int local_line_count{line_count};
    while(local_line_count)
    {
        sf::Vector2f fireLocation = parent->getPosition() + sf::rotateVector(offset, parent->getRotation()+direction-90) + sf::rotateVector(parent->ship_template->model_data->getTubePosition2D(tube_index), parent->getRotation());
        //sf::Vector2f fireLocation = parent->getPosition() + offset + sf::rotateVector(sf::Vector2f(parent->getRadius()/2,parent->getRadius()/2),parent->getRotation()+direction-45);
    
        const MissileWeaponData& data = MissileWeaponData::getDataFor(type_loaded);
        switch(data.basetype)
        {
        case MW_Homing:
            {
                P<HomingMissile> missile = new HomingMissile();
                missile->owner = parent;
                missile->setFactionId(parent->getFactionId());
                missile->target_id = parent->target_id;
                missile->setPosition(fireLocation);
                missile->translate_z = parent->translate_z;
                missile->setRotation(parent->getRotation() + direction);
                missile->target_angle = target_angle;
                missile->speed = data.speed * parent->getSystemEffectiveness(SYS_MissileSystem);
                missile->damage_multiplier = data.damage_multiplier;
                missile->damage_type = data.damage_type;
                missile->color = data.color;
                missile->category_modifier = getSizeCategoryModifier();
                missile->lifetime = data.lifetime;
                missile->on_detonation = data.on_detonation;
            }
            break;
        case MW_Nuke:
            {
                P<Nuke> missile = new Nuke();
                missile->owner = parent;
                missile->setFactionId(parent->getFactionId());
                missile->target_id = parent->target_id;
                missile->setPosition(fireLocation);
                missile->translate_z = parent->translate_z;
                missile->setRotation(parent->getRotation() + direction);
                missile->target_angle = target_angle;
                missile->speed = data.speed * parent->getSystemEffectiveness(SYS_MissileSystem);
                missile->damage_multiplier = data.damage_multiplier;
                missile->damage_type = data.damage_type;
                missile->color = data.color;
                missile->category_modifier = getSizeCategoryModifier();
                missile->lifetime = data.lifetime;
                missile->on_detonation = data.on_detonation;
            }
            break;
        case MW_Mine:
            {
                P<Mine> missile = new Mine();
                missile->owner = parent;
                missile->setFactionId(parent->getFactionId());
                missile->setPosition(fireLocation);
                missile->translate_z = parent->translate_z;
                missile->setRotation(parent->getRotation() + direction);
                missile->speed = data.speed * parent->getSystemEffectiveness(SYS_MissileSystem);
                missile->damage_multiplier = data.damage_multiplier;
                missile->eject();
            }
            break;
        case MW_HVLI:
            {
                P<HVLI> missile = new HVLI();
                missile->owner = parent;
                missile->setFactionId(parent->getFactionId());
                missile->setPosition(fireLocation);
                missile->translate_z = parent->translate_z;
                missile->setRotation(parent->getRotation() + direction);
                missile->target_angle = parent->getRotation() + direction;
                missile->damage_multiplier = data.damage_multiplier;
                missile->speed = data.speed * parent->getSystemEffectiveness(SYS_MissileSystem);
                missile->damage_type = data.damage_type;
                missile->color = data.color;
                missile->category_modifier = getSizeCategoryModifier();
                missile->lifetime = data.lifetime;
                missile->on_detonation = data.on_detonation;
            }
            break;
        case MW_EMP:
            {
                P<EMPMissile> missile = new EMPMissile();
                missile->owner = parent;
                missile->setFactionId(parent->getFactionId());
                missile->target_id = parent->target_id;
                missile->setPosition(fireLocation);
                missile->translate_z = parent->translate_z;
                missile->setRotation(parent->getRotation() + direction);
                missile->target_angle = target_angle;
                missile->damage_multiplier = data.damage_multiplier;
                missile->speed = data.speed * parent->getSystemEffectiveness(SYS_MissileSystem);
                missile->damage_type = data.damage_type;
                missile->color = data.color;
                missile->category_modifier = getSizeCategoryModifier();
                missile->lifetime = data.lifetime;
                missile->on_detonation = data.on_detonation;
            }
            break;
        default:
            break;
        }
        
        local_line_count--;
        offset.x-=distance_line;
    }
}

bool WeaponTube::canLoad(string type)
{
    int32_t numType;
    if(isNumber(type))
    {
        numType = std::stoi(type);
    }
    else
    {
        numType = CustomMissileWeaponRegistry::getMissileWeaponType(type);
    }
    if (type_allowed_mask & (1 << numType))
        return true;
    return false;
}

bool WeaponTube::canOnlyLoad(string type)
{
    int32_t numType;
    if(isNumber(type))
    {
        numType = std::stoi(type);
    }
    else
    {
        numType = CustomMissileWeaponRegistry::getMissileWeaponType(type);
    }

    if (type_allowed_mask == (1U << numType))
        return true;
    return false;
}

void WeaponTube::allowLoadOf(string type)
{
    int32_t numType;
    if(isNumber(type))
    {
        numType = std::stoi(type);
    }
    else
    {
        numType = CustomMissileWeaponRegistry::getMissileWeaponType(type);
    }
    type_allowed_mask |= (1 << numType);
}

void WeaponTube::disallowLoadOf(string type)
{
    int32_t numType;
    if(isNumber(type))
    {
        numType = std::stoi(type);
    }
    else
    {
        numType = CustomMissileWeaponRegistry::getMissileWeaponType(type);
    }
    type_allowed_mask &=~(1 << numType);
}

void WeaponTube::forceUnload()
{
    if (state != WTS_Empty && type_loaded != "")
    {
        state = WTS_Empty;
        if(isNumber(type_loaded))
        {
            int numType = std::stoi(type_loaded);
            if (parent->weapon_storage[numType] < parent->weapon_storage_max[numType])
                parent->weapon_storage[numType] ++;
        }
        else
        {
            if (parent->custom_weapon_storage[type_loaded] < parent->custom_weapon_storage_max[type_loaded])
                parent->custom_weapon_storage[type_loaded] ++;
        }
        type_loaded = "";
    }
}

void WeaponTube::update(float delta)
{
    if (delay > 0.0)
    {
        delay -= delta * parent->getSystemEffectiveness(SYS_MissileSystem);
    }else{
        switch(state)
        {
        case WTS_Loading:
            state = WTS_Loaded;
            break;
        case WTS_Unloading:
            state = WTS_Empty;
            if(isNumber(type_loaded))
            {
                int numLoaded = std::stoi(type_loaded);
                if (parent->weapon_storage[numLoaded] < parent->weapon_storage_max[numLoaded])
                    parent->weapon_storage[numLoaded] ++;
            }
            else
            {
                if (parent->custom_weapon_storage[type_loaded] < parent->custom_weapon_storage_max[type_loaded])
                    parent->custom_weapon_storage[type_loaded] ++;
            }

            type_loaded = "";
            break;
        case WTS_Firing:
            if (game_server)
            {
                spawnProjectile(0);

                fire_count -= 1;
                if (fire_count > 0)
                {
                    delay = 1.5;
                }
                else
                {
                    state = WTS_Empty;
                    type_loaded = "";
                }
            }
            break;
        default:
            break;
        }
    }
}

bool WeaponTube::isEmpty()
{
    return state == WTS_Empty;
}

bool WeaponTube::isLoaded()
{
    return state == WTS_Loaded;
}

bool WeaponTube::isLoading()
{
    return state == WTS_Loading;
}

bool WeaponTube::isUnloading()
{
    return state == WTS_Unloading;
}

bool WeaponTube::isFiring()
{
    return state == WTS_Firing;
}

float WeaponTube::getLoadProgress()
{
    return 1.0 - delay / load_time;
}

float WeaponTube::getUnloadProgress()
{
    return delay / load_time;
}

string WeaponTube::getLoadType()
{
    return type_loaded;
}

string WeaponTube::getTubeName()
{
    if (std::abs(sf::angleDifference(0.0f, direction)) <= 45)
        return tr("tube","Front");
    if (std::abs(sf::angleDifference(90.0f, direction)) < 45)
        return tr("tube","Right");
    if (std::abs(sf::angleDifference(-90.0f, direction)) < 45)
        return tr("tube","Left");
    if (std::abs(sf::angleDifference(180.0f, direction)) <= 45)
        return tr("tube","Rear");
    return "?" + string(direction);
}

float WeaponTube::calculateFiringSolution(P<SpaceObject> target)
{
    if (!target)
        return std::numeric_limits<float>::infinity();
    const MissileWeaponData& data = MissileWeaponData::getDataFor(type_loaded);
    if (data.turnrate == 0.0f)  //If the missile cannot turn, we cannot find a firing solution.
        return std::numeric_limits<float>::infinity();

    sf::Vector2f target_position = target->getPosition();
    sf::Vector2f target_velocity = target->getVelocity();
    float target_velocity_length = sf::length(target_velocity);
    float missile_angle = sf::vector2ToAngle(target_position - parent->getPosition());
    float turn_radius = ((360.0f / data.turnrate) * data.speed) / (2.0f * M_PI);
    float missile_exit_angle = parent->getRotation() + direction;

    for(int iterations=0; iterations<10; iterations++)
    {
        float angle_diff = sf::angleDifference(missile_angle, missile_exit_angle);

        float left_or_right = 90;
        if (angle_diff > 0)
            left_or_right = -90;

        sf::Vector2f turn_center = parent->getPosition() + sf::vector2FromAngle(missile_exit_angle + left_or_right) * turn_radius;
        sf::Vector2f turn_exit = turn_center + sf::vector2FromAngle(missile_angle - left_or_right) * turn_radius;
        if (target_velocity_length < 1.0f)
        {
            //If the target is almost standing still, just target the position directly instead of using the velocity of the target in the calculations.
            float time_missile = sf::length(turn_exit - target_position) / data.speed;
            sf::Vector2f interception = turn_exit + sf::vector2FromAngle(missile_angle) * data.speed * time_missile;
            if ((interception - target_position) < target->getRadius() / 2)
                return missile_angle;
            missile_angle = sf::vector2ToAngle(target_position - turn_exit);
        }
        else
        {
            sf::Vector2f missile_velocity = sf::vector2FromAngle(missile_angle) * data.speed;
            //Calculate the position where missile and the target will cross each others path.
            sf::Vector2f intersection = sf::lineLineIntersection(target_position, target_position + target_velocity, turn_exit, turn_exit + missile_velocity);
            //Calculate the time it will take for the target and missile to reach the intersection
            float turn_time = fabs(angle_diff) / data.turnrate;
            float time_target = sf::length((target_position - intersection)) / target_velocity_length;
            float time_missile = sf::length(turn_exit - intersection) / data.speed + turn_time;
            //Calculate the time in which the radius will be on the intersection, to know in which time range we need to hit.
            float time_radius = (target->getRadius() / 2.0) / target_velocity_length;//TODO: This value could be improved, as it is allowed to be bigger when the angle between the missile and the ship is low
            // When both the missile and the target are at the same position at the same time, we can take a shot!
            if (fabsf(time_target - time_missile) < time_radius)
                return missile_angle;

            //When we cannot hit the target with this setup yet. Calculate a new intersection target, and aim for that.
            float guessed_impact_time = (time_target * target_velocity_length / (target_velocity_length + data.speed)) + (time_missile * data.speed / (target_velocity_length + data.speed));
            sf::Vector2f new_target_position = target_position + target_velocity * guessed_impact_time;
            missile_angle = sf::vector2ToAngle(new_target_position - turn_exit);
        }
    }
    return std::numeric_limits<float>::infinity();
}

void WeaponTube::setSize(EMissileSizes size)
{
    this->size = size;
}

EMissileSizes WeaponTube::getSize()
{
    return size;
}
