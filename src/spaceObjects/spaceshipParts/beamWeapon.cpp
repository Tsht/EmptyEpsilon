#include "beamWeapon.h"
#include "spaceObjects/spaceship.h"
#include "spaceObjects/beamEffect.h"
#include "spaceObjects/spaceObject.h"
#include "multiplayer_server.h"

#include <SDL_assert.h>


BeamWeapon::BeamWeapon()
{
    arc = 0;
    direction = 0;
    range = 0;
    turret_arc = 0.0f;
    turret_direction = 0.0f;
    turret_rotation_rate = 0.0f;
    cycle_time = 6.0f;
    cooldown = 0.0f;
    damage = 1.0f;
    energy_per_beam_fire = 3.0f;
    heat_per_beam_fire = 0.02f;
    is_valid = true;
    parent = nullptr;
    arc_color = {255, 0, 0, 128};
    arc_color_fire = {255, 255, 0, 128};
    damage_type = DT_Energy;
}

void BeamWeapon::setParent(SpaceShip* parent)
{
    SDL_assert(!this->parent);
    this->parent = parent;

    parent->registerMemberReplication(&arc);
    parent->registerMemberReplication(&direction);
    parent->registerMemberReplication(&range);
    parent->registerMemberReplication(&turret_arc);
    parent->registerMemberReplication(&turret_direction);
    parent->registerMemberReplication(&turret_rotation_rate);
    parent->registerMemberReplication(&cycle_time);
    parent->registerMemberReplication(&cooldown, 0.5);
    parent->registerMemberReplication(&arc_color);
    parent->registerMemberReplication(&arc_color_fire);
}

void BeamWeapon::setArc(float arc)
{
    this->arc = arc;
}

float BeamWeapon::getArc()
{
    return arc;
}

void BeamWeapon::setArcColor(glm::u8vec4 color)
{
    arc_color = color;
}

glm::u8vec4 BeamWeapon::getArcColor()
{
    return arc_color;
}

void BeamWeapon::setArcFireColor(glm::u8vec4 color)
{
    arc_color_fire = color;
}

glm::u8vec4 BeamWeapon::getArcFireColor()
{
    return arc_color_fire;
}

void BeamWeapon::setDamageType(EDamageType type)
{
    damage_type = type;
}

void BeamWeapon::setDirection(float direction)
{
    this->direction = direction;
}

float BeamWeapon::getDirection()
{
    return direction;
}

void BeamWeapon::setRange(float range)
{
    this->range = range;
}

float BeamWeapon::getRange()
{
    return range;
}

void BeamWeapon::setTurretArc(float arc)
{
    this->turret_arc = arc;
}

float BeamWeapon::getTurretArc()
{
    return turret_arc;
}

void BeamWeapon::setTurretDirection(float direction)
{
    this->turret_direction = direction;
}

float BeamWeapon::getTurretDirection()
{
    return turret_direction;
}

void BeamWeapon::setTurretRotationRate(float rotation_rate)
{
    this->turret_rotation_rate = rotation_rate;
}

float BeamWeapon::getTurretRotationRate()
{
    return turret_rotation_rate;
}

void BeamWeapon::setCycleTime(float cycle_time)
{
    this->cycle_time = cycle_time;
}

float BeamWeapon::getCycleTime()
{
    return cycle_time;
}

void BeamWeapon::setDamage(float damage)
{
    this->damage = damage;
}

float BeamWeapon::getDamage()
{
    return damage;
}

float BeamWeapon::getEnergyPerFire()
{
    return energy_per_beam_fire;
}

void BeamWeapon::setEnergyPerFire(float energy)
{
    energy_per_beam_fire = energy;
}

float BeamWeapon::getHeatPerFire()
{
    return heat_per_beam_fire;
}

void BeamWeapon::setHeatPerFire(float heat)
{
    heat_per_beam_fire = heat;
}

bool BeamWeapon::getIsValid()
{
    return is_valid;
}

void BeamWeapon::setIsValid(bool valid)
{
    is_valid = valid;
}

void BeamWeapon::setPosition(glm::vec3 position)
{
    this->position = position;
}

glm::vec3 BeamWeapon::getPosition()
{
    return position;
}

void BeamWeapon::setBeamTexture(string beam_texture)
{
    this->beam_texture = beam_texture;
}

string BeamWeapon::getBeamTexture()
{
    return beam_texture;
}

float BeamWeapon::getCooldown()
{
    return cooldown;
}

void BeamWeapon::update(float delta)
{
    if (cooldown > 0.0f)
        cooldown -= delta * parent->getSystemEffectiveness(SYS_BeamWeapons);

    P<SpaceObject> target = parent->getTarget();

    // Check on beam weapons only if we are on the server, have a target, and
    // not paused, and if the beams are cooled down or have a turret arc.
//    if (game_server && range > 0.0 && target && delta > 0 && parent->current_warp == 0.0 && parent->docking_state == DS_NotDocking)
    if (game_server && is_valid && range > 0.0f && target && delta > 0 && parent->docking_state == DS_NotDocking)
    {
        // Get the angle to the target.
        auto diff = target->getPosition() - (parent->getPosition() + rotateVec2(glm::vec2(position.x, position.y), parent->getRotation()));
        float distance = glm::length(diff) - target->getRadius() / 2.0f;

        // We also only care if the target is within no more than its
        // range * 1.3, which is when we want to start rotating the turret.
        // TODO: Add a manual aim override similar to weapon tubes.
        if (distance < (range + target->getRadius()) * 1.3f)
        {
            float angle = vec2ToAngle(diff);
            float angle_diff = angleDifference(direction + parent->getRotation(), angle);

            if (turret_arc > 0)
            {
                // Get the target's angle relative to the turret's direction.
                float turret_angle_diff = angleDifference(turret_direction + parent->getRotation(), angle);

                // If the turret can rotate ...
                if (turret_rotation_rate > 0)
                {
                    // ... and if the target is within the turret's arc ...
                    if (fabsf(turret_angle_diff) < turret_arc / 2.0f)
                    {
                        // ... rotate the turret's beam toward the target.
                        if (fabsf(angle_diff) > 0)
                        {
                            direction += (angle_diff / fabsf(angle_diff)) * std::min(turret_rotation_rate * parent->getSystemEffectiveness(SYS_BeamWeapons), fabsf(angle_diff));
                        }
                    // If the target is outside of the turret's arc ...
                    } else {
                        // ... rotate the turret's beam toward the turret's
                        // direction to reset it.
                        float reset_angle_diff = angleDifference(direction, turret_direction);

                        if (fabsf(reset_angle_diff) > 0)
                        {
                            direction += (reset_angle_diff / fabsf(reset_angle_diff)) * std::min(turret_rotation_rate * parent->getSystemEffectiveness(SYS_BeamWeapons), fabsf(reset_angle_diff));
                        }
                    }
                }
            }

            // If the target is in the beam's arc and range, the beam has cooled
            // down, and the beam can consume enough energy to fire ...
            P<SpaceShip> ship = target;

            if (parent->lock_fire && distance < range + target->getRadius()/4.0f && cooldown <= 0.0f && fabsf(angle_diff) < arc / 2.0f && parent->useEnergy(energy_per_beam_fire) && (!ship || ship->docking_target != parent))
            {
                // ... add heat to the beam and zap the target.
                parent->addHeat(SYS_BeamWeapons, heat_per_beam_fire);
                fire(target, parent->beam_system_target);
            }
        }
    // If the beam is turreted and can move, but doesn't have a target, reset it
    // if necessary.
    } else if (game_server && range > 0.0f && delta > 0 && turret_arc > 0.0f && direction != turret_direction && turret_rotation_rate > 0) {
        float reset_angle_diff = angleDifference(direction, turret_direction);

        if (fabsf(reset_angle_diff) > 0)
        {
            direction += (reset_angle_diff / fabsf(reset_angle_diff)) * std::min(turret_rotation_rate * parent->getSystemEffectiveness(SYS_BeamWeapons), fabsf(reset_angle_diff));
        }
    }
}

void BeamWeapon::fire(P<SpaceObject> target, ESystem system_target)
{
    //When we fire a beam, and we hit an enemy, check if we are not scanned yet, if we are not, and we hit something that we know is an enemy or friendly,
    //  we now know if this ship is an enemy or friend.
    parent->didAnOffensiveAction();

    cooldown = cycle_time; // Reset time of weapon

    auto hit_location = target->getPosition() - glm::normalize(target->getPosition() - parent->getPosition()) * target->getRadius();
    P<BeamEffect> effect = new BeamEffect();
    effect->setSource(parent, position);
    effect->setTarget(target, hit_location);
    effect->beam_texture = beam_texture;
    effect->beam_fire_sound = "sfx/laser_fire.wav";
    effect->beam_fire_sound_power = damage / 6.0f;

    DamageInfo info(parent, damage_type, hit_location);
    info.frequency = parent->beam_frequency; // Beam weapons now always use frequency of the ship.
    info.system_target = system_target;
    target->takeDamage(damage, info);
}
