#ifndef SHIP_TEMPLATE_BASED_OBJECT_H
#define SHIP_TEMPLATE_BASED_OBJECT_H

#include "engine.h"
#include "spaceObject.h"
#include "shipTemplate.h"

class SpaceShip;

/**
    An object which is based on a ship template. Contains generic behaviour for:
    * Hull damage
    * Shield damage
    * Rendering
    Used as a base class for stations and ships.
*/
class ShipTemplateBasedObject : public SpaceObject, public Updatable
{
protected:
    float long_range_radar_range;
    float short_range_radar_range;
public:
    string template_name;
    string type_name;
    string locale_name;
    string class_name;
    string sub_class_name;
    int hack_diff;
    bool hack_protect;
    string radar_trace;
    string impulse_sound_file;
    string warp_sound_file = "warp.wav";
    P<ShipTemplate> ship_template;

    int shield_count;
    float shield_level[max_shield_count];
    float shield_max[max_shield_count];
    float hull_strength, hull_max;
    float shield_hit_effect[max_shield_count];
    bool can_be_destroyed;
    bool lock_fire;
    float rotation_speed;

    float system_damage_ratio;
    float system_damage_hull_threshold;
    float shield_recharge_rate;

    bool shares_energy_with_docked;       //[config]
    bool repair_docked;                   //[config]
    bool restocks_scan_probes;
    bool restocks_missiles_docked;        //only restocks cpuships; playerships should use comms

    ScriptSimpleCallback on_destruction;
    ScriptSimpleCallback on_taking_damage;
public:
    ShipTemplateBasedObject(float collision_range, string multiplayer_name, float multiplayer_significant_range=-1);

    virtual void draw3DTransparent() override;
    virtual void drawShieldsOnRadar(sp::RenderTarget& renderer, glm::vec2 position, float scale, float rotation, float sprite_scale, bool show_levels);
    virtual void update(float delta) override;

    virtual std::unordered_map<string, string> getGMInfo() override;
    virtual bool canRestockMissiles() override { return restocks_missiles_docked; }
    virtual bool canBeTargetedBy(P<SpaceObject> other) override { return true; }
    virtual bool hasShield() override;
    virtual string getCallSign() override { return callsign; }
    virtual void takeDamage(float damage_amount, DamageInfo info) override;
    virtual void takeHullDamage(float damage_amount, DamageInfo& info);
    virtual void destroyedByDamage(DamageInfo& info) = 0;
    virtual float getShieldDamageFactor(DamageInfo& info, int shield_index);

    void setCanBeDestroyed(bool enabled) { can_be_destroyed = enabled; }
    bool getCanBeDestroyed(){ return can_be_destroyed; }

    void setRotationSpeed(float speed) { rotation_speed = speed; }
    float getRotationSpeed(){ return rotation_speed; }

    virtual void applyTemplateValues() = 0;
    virtual float getShieldRechargeRate(int shield_index);

    void setTemplate(string template_name);
    void setShipTemplate(string template_name) { LOG(WARNING) << "Deprecated \"setShipTemplate\" function called."; setTemplate(template_name); }
    void setTypeName(string type_name) { this->type_name = type_name; }
    void setLocaleName(string locale_name) { this->locale_name = locale_name; }
    string getTypeName() { return type_name; }
    string getLocaleName() { return locale_name; }

    string getClass() { return class_name; }
    string getSubClass() { return sub_class_name; }

    void setHackDiff(int hack_diff) { this->hack_diff = hack_diff; }
    unsigned int getHackDiff() { return hack_diff; }

    float getHull() { return hull_strength; }
    float getHullMax() { return hull_max; }
    void setHull(float amount) { if (amount < 0) return; hull_strength = std::min(amount, hull_max); }
    void setHullMax(float amount) { if (amount < 0) return; hull_max = amount; hull_strength = std::min(hull_strength, hull_max); }

    void setSystemDamageRatio(float ratio) { system_damage_ratio = ratio ;}
    void setSystemDamageHullThreshold(float ratio) {system_damage_hull_threshold = ratio;}

    virtual bool getShieldsActive() { return true; }

    ///Shield script binding functions
    float getShieldLevel(int index) { if (index < 0 || index >= shield_count) return 0; return shield_level[index]; }
    float getShieldMax(int index) { if (index < 0 || index >= shield_count) return 0; return shield_max[index]; }
    int getShieldCount() { return shield_count; }
    void setShieldCount(int value) { if (value < 0 || value > max_shield_count) return; shield_count = value; }
    void setShields(const std::vector<float>& amounts);
    void setShieldRechargeRate(float amount) {shield_recharge_rate = amount;}
    void setShieldsMax(const std::vector<float>& amounts);

    int getShieldPercentage(int index) { if (index < 0 || index >= shield_count || shield_max[index] <= 0.0f) return 0; return int(100 * shield_level[index] / shield_max[index]); }
    ESystem getShieldSystemForShieldIndex(int index);

    ///Deprecated old script functions for shields
    float getFrontShield() { return shield_level[0]; }
    float getFrontShieldMax() { return shield_max[0]; }
    void setFrontShield(float amount) { if (amount < 0) return; shield_level[0] = amount; }
    void setFrontShieldMax(float amount) { if (amount < 0) return; shield_level[0] = amount; shield_level[0] = std::min(shield_level[0], shield_max[0]); }
    float getRearShield() { return shield_level[1]; }
    float getRearShieldMax() { return shield_max[1]; }
    void setRearShield(float amount) { if (amount < 0) return; shield_level[1] = amount; }
    void setRearShieldMax(float amount) { if (amount < 0) return; shield_max[1] = amount; shield_level[1] = std::min(shield_level[1], shield_max[1]); }

    // Radar range
    virtual float getProbeRangeRadarRange() { return short_range_radar_range; };
    virtual float getLongRangeRadarRange() { return long_range_radar_range; }
    virtual float getShortRangeRadarRange() { return short_range_radar_range; }
    void setLongRangeRadarRange(float range);
    void setShortRangeRadarRange(float range);

    void setRadarTrace(string trace) { radar_trace = "radar/" + trace; }
    void setImpulseSoundFile(string sound) { impulse_sound_file = sound; }

    bool getSharesEnergyWithDocked() { return shares_energy_with_docked; }
    void setSharesEnergyWithDocked(bool enabled) { shares_energy_with_docked = enabled; }
    bool getRepairDocked() { return repair_docked; }
    void setRepairDocked(bool enabled) { repair_docked = enabled; }
    bool getRestocksScanProbes() { return restocks_scan_probes; }
    void setRestocksScanProbes(bool enabled) { restocks_scan_probes = enabled; }
    bool getRestocksMissilesDocked() { return restocks_missiles_docked; }
    void setRestocksMissilesDocked(bool enabled) { restocks_missiles_docked = enabled; }

    void onTakingDamage(ScriptSimpleCallback callback);
    void onDestruction(ScriptSimpleCallback callback);

    string getShieldDataString();

};

int frequencyToDisplayNumber(int frequency);

#endif//SHIP_TEMPLATE_BASED_OBJECT_H
