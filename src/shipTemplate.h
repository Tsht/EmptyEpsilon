#ifndef SHIP_TEMPLATE_H
#define SHIP_TEMPLATE_H

#include <unordered_map>
#include <unordered_set>
#include <optional>
#include "engine.h"
#include "modelData.h"
#include "scriptInterfaceMagic.h"
#include "multiplayer.h"

#include "beamTemplate.h"
#include "missileWeaponData.h"
#include "EDamageType.h"

constexpr static int max_beam_weapons = 16;
constexpr static int max_weapon_tubes = 16;
constexpr static int max_shield_count = 8;
constexpr static int max_docks_count = 16;

enum ESystem
{
    SYS_None = -1,
    SYS_Reactor = 0,
    SYS_BeamWeapons,
    SYS_MissileSystem,
    SYS_Maneuver,
    SYS_Impulse,
    SYS_Warp,
    SYS_JumpDrive,
    SYS_FrontShield,
    SYS_RearShield,
    SYS_Docks,
    SYS_Drones,
    SYS_Hangar,
    SYS_Cloaking,
    SYS_COUNT
};

/* Define script conversion function for the ESystem enum. */
template<> void convert<ESystem>::param(lua_State* L, int& idx, ESystem& es);
template<> int convert<ESystem>::returnType(lua_State* L, ESystem es);

class DroneTemplate {
    public:
    string template_name;
    int count;
    DroneTemplate(string template_name, int count): template_name(template_name), count(count) {}
};
class ShipRoomTemplate
{
public:
    glm::ivec2 position;
    glm::ivec2 size;
    ESystem system;

    ShipRoomTemplate(glm::ivec2 position, glm::ivec2 size, ESystem system) : position(position), size(size), system(system) {}
};
class ShipDoorTemplate
{
public:
    glm::ivec2 position;
    bool horizontal;

    ShipDoorTemplate(glm::ivec2 position, bool horizontal) : position(position), horizontal(horizontal) {}
};

class SpaceObject;
class ShipTemplate : public PObject
{
public:
    enum TemplateType
    {
        Drone,
        Ship,
        PlayerShip,
        Station
    };
    class TubeTemplate
    {
    public:
        float load_time;
        uint32_t type_allowed_mask;
        float direction;
        EMissileSizes size;
    };
private:
    static std::unordered_map<string, P<ShipTemplate> > templateMap;
    string name;
    string locale_name = "";
    string description;
    string class_name;
    string sub_class_name;
    string os_name;
    int hack_diff;
    TemplateType type;
    bool secret;
public:
    string getName();
    string getLocaleName();
    string getDescription();
    string getClass();
    string getSubClass();
    void setType(TemplateType type);
    TemplateType getType();

    void setSecret(bool enabled) { secret = enabled; }
    bool getSecret(){ return secret; }

    void setOSName(string os_name) { this->os_name = os_name; }
    void setHackDiff(int hack_diff) { this->hack_diff = hack_diff; }
    string getOSName() { return os_name; }
    int getHackDiff() { return hack_diff; }

    P<ModelData> model_data;
    bool visible{true}; //Should be visible in science/gm/other player facing locations. Invisible templates exists for backwards compatibility.

    /*!
     * List of ship classes that can dock with this ship. (only used for ship2ship docking)
     */
    std::unordered_set<string> external_dock_classes;
    std::unordered_set<string> internal_dock_classes;
    bool shares_energy_with_docked;
    bool repair_docked;
    bool has_reactor;

    bool restocks_scan_probes;
    bool restocks_missiles_docked;
    bool can_scan = true;
    bool can_hack = true;
    bool can_dock = true;
    bool can_combat_maneuver = true;
    bool can_self_destruct = true;
    bool can_launch_probe = true;

    float energy_storage_amount;
    int repair_crew_count;
    string default_ai_name;
    BeamTemplate beams[max_beam_weapons];
    int weapon_tube_count;
    TubeTemplate weapon_tube[max_weapon_tubes];
    float hull;
    int shield_count;
    float shield_level[max_shield_count];
    float shield_recharge_rate;
    float impulse_speed, impulse_reverse_speed, turn_speed, warp_speed;
    float impulse_acceleration, impulse_reverse_acceleration;
    float combat_maneuver_boost_speed;
    float combat_maneuver_strafe_speed;
    bool has_jump_drive, has_cloaking;
    float jump_drive_min_distance;
    float jump_drive_max_distance;
    float jump_drive_charge;
    float jump_drive_charge_time;
    float jump_drive_energy_per_km_charge;
    int weapon_storage[MW_Count];
    std::map<string, int> custom_weapon_storage;
    float system_damage_ratio;
    float system_damage_hull_threshold;
    int launcher_dock_count;
    int energy_dock_count;
    int weapons_dock_count;
    int thermic_dock_count;
    int repair_dock_count;
    int stock_dock_count;
    string radar_trace;
    float energy_consumption_ratio;
    float long_range_radar_range = 30000.0f;
    float short_range_radar_range = 5000.0f;
    string impulse_sound_file;

    std::vector<ShipRoomTemplate> rooms;
    std::vector<ShipDoorTemplate> doors;
    std::vector<DroneTemplate> drones;

    ShipTemplate();

    void setName(string name);
    void setLocaleName(string name);
    void setClass(string class_name, string sub_class_name);
    void setDescription(string description);
    void hidden() { visible = false; }
    void setModel(string model_name);
    void setDefaultAI(string default_ai_name);
    void setDockClasses(const std::vector<string>& classes);
    void setExternalDockClasses(const std::vector<string>& classes);
    void setInternalDockClasses(const std::vector<string>& classes);
    void setSharesEnergyWithDocked(bool enabled);
    void setRepairDocked(bool enabled);
    void setReactor(bool enabled);
    void setRestocksScanProbes(bool enabled);
    void setRestocksMissilesDocked(bool enabled);
    void setCanScan(bool enabled) { can_scan = enabled; }
    void setCanHack(bool enabled) { can_hack = enabled; }
    void setCanDock(bool enabled) { can_dock = enabled; }
    void setCanCombatManeuver(bool enabled) { can_combat_maneuver = enabled; }
    void setCanSelfDestruct(bool enabled) { can_self_destruct = enabled; }
    void setCanLaunchProbe(bool enabled) { can_launch_probe = enabled; }
    void setMesh(string model, string color_texture, string specular_texture, string illumination_texture);
    void setEnergyStorage(float energy_amount);
    void setRepairCrewCount(int amount);
    void setEnergyConsumptionRatio(float ratio) { energy_consumption_ratio = ratio;}

    void setBeam(int index, float arc, float direction, float range, float cycle_time, float damage);
    void setBeamWeapon(int index, float arc, float direction, float range, float cycle_time, float damage);
    void setBeamWeaponTurret(int index, float arc, float direction, float rotation_rate);

    /**
     * Convenience function to set the texture of a beam by index.
     */
    void setBeamTexture(int index, string texture);
    void setBeamWeaponEnergyPerFire(int index, float energy) { if (index < 0 || index >= max_beam_weapons) return; return beams[index].setEnergyPerFire(energy); }
    void setBeamWeaponHeatPerFire(int index, float heat) { if (index < 0 || index >= max_beam_weapons) return; return beams[index].setHeatPerFire(heat); }

    void setTubes(int amount, float load_time);
    void setTubeLoadTime(int index, float load_time);
    void weaponTubeAllowMissle(int index, EMissileWeapons type);
    void weaponTubeDisallowMissle(int index, EMissileWeapons type);
    void setWeaponTubeExclusiveFor(int index, EMissileWeapons type);
    void weaponTubeAllowCustomMissile(int index, string type);
    void weaponTubeDisallowCustomMissile(int index, string type);
    void setWeaponTubeExclusiveForCustom(int index, string type);
    
    void setTubeSize(int index, EMissileSizes size);

    void setTubeDirection(int index, float direction);
    void setHull(float amount) { hull = amount; }
    void setShields(const std::vector<float>& values);
    void setShieldRechargeRate(float amount) { shield_recharge_rate = amount;}
    void setSpeed(float impulse, float turn, float acceleration, std::optional<float> reverse_speed, std::optional<float> reverse_acceleration);
    void setCombatManeuver(float boost, float strafe);
    void setWarpSpeed(float warp);
    void setJumpDrive(bool enabled);
    void setJumpDriveRange(float min, float max) { jump_drive_min_distance = min; jump_drive_max_distance = max; }
    void setJumpDriveChargeTime(float time) { jump_drive_charge_time = time; }
	void setJumpDriveEnergy(float charge) { jump_drive_energy_per_km_charge = charge; }
    void setCloaking(bool enabled);
    void setCustomWeapon(EMissileWeapons weapon, string  new_name, float damage_multiplier, float speed, EDamageType dt, float lifetime);
    void setCustomWeaponMultiple(string weapon_name, int fire_count, int line_count);
    void setCustomWeaponColor(string weapon_name, char color_r, char color_g, char color_b);
    void setWeaponStorage(EMissileWeapons weapon, int amount);
    void setCustomWeaponStorage(string weapon, int amount);
    void onCustomWeaponDetonation(string weapon_name, ScriptSimpleCallback callback);
    void addRoom(glm::ivec2 position, glm::ivec2 size);
    void addRoomSystem(glm::ivec2 position, glm::ivec2 size, ESystem system);
    void addDoor(glm::ivec2 position, bool horizontal);    void addDrones(string template_name, int count);
    void setDocks(int launchers, int energy, int weapons, int thermic, int repair, int stock);
    int getDocksCount() {return launcher_dock_count + energy_dock_count + weapons_dock_count + thermic_dock_count + repair_dock_count + stock_dock_count;}
    void setRadarTrace(string trace);
    void setSystemDamageRatio(float ratio) { system_damage_ratio = ratio ;}
    void setSystemDamageHullThreshold(float ratio) {system_damage_hull_threshold = ratio;}

    void setLongRangeRadarRange(float range);
    void setShortRangeRadarRange(float range);
    void setImpulseSoundFile(string sound);

    P<ShipTemplate> copy(string new_name);

    glm::ivec2 interiorSize();
    ESystem getSystemAtRoom(glm::ivec2 position);

    void setCollisionData(P<SpaceObject> object);
public:
    static P<ShipTemplate> getTemplate(string name);
    static std::vector<string> getAllTemplateNames();
    static std::vector<string> getTemplateNameList(TemplateType type);
    static std::vector<string> getTemplateClassList(TemplateType type);
};
string getSystemName(ESystem system);
string getLocaleSystemName(ESystem system);
REGISTER_MULTIPLAYER_ENUM(ESystem);

/* Define script conversion function for the ShipTemplate::TemplateType enum. */
template<> void convert<ShipTemplate::TemplateType>::param(lua_State* L, int& idx, ShipTemplate::TemplateType& tt);
#endif//SHIP_TEMPLATE_H
