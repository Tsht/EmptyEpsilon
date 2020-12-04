#ifndef TWEAK_H
#define TWEAK_H

#include "gui/gui2_panel.h"
#include "missileWeaponData.h"
#include "shipTemplate.h"
#include "playerInfo.h"
#include "spaceObjects/planet.h"
#include "spaceObjects/playerSpaceship.h"
#include "spaceObjects/warpJammer.h"

class SpaceShip;
class GuiKeyValueDisplay;
class GuiLabel;
class GuiTextEntry;
class GuiSlider;
class GuiSelector;
class GuiToggleButton;
class GuiButton;
class GuiListbox;

enum ETweakType
{
    TW_Object,  // TODO: Space object
    TW_Jammer,  // WarpJammer
    TW_Ship,    // Ships
    TW_Template,// Template : Ships & station
    TW_Station, // TODO: Space stations
    TW_Player,   // Player ships
    TW_Planet   // Planet
};

class GuiTweakPage : public GuiElement
{
public:
    GuiTweakPage(GuiContainer* owner) : GuiElement(owner, "") {}

    virtual void open(P<SpaceObject> target) = 0;
};

class GuiObjectTweak : public GuiPanel
{
public:
    GuiObjectTweak(GuiContainer* owner, ETweakType tweak_type);

    void open(P<SpaceObject> target);

    virtual void onDraw(sf::RenderTarget& window) override;
private:
    P<SpaceObject> target;
    std::vector<GuiTweakPage*> pages;
};

class GuiObjectTweakBase : public GuiTweakPage
{
private:
    P<SpaceObject> target;

    GuiTextEntry* callsign;
    GuiSlider* hull_slider;
    GuiSlider* zaxis_slider;
    GuiTextEntry* description;
    GuiTextEntry* description_fof;
    GuiTextEntry* description_scan;
    GuiTextEntry* description_full_scan;
    GuiSlider* gravity_s_slider;
    GuiSlider* electrical_s_slider;
    GuiSlider* biological_s_slider;
    GuiSlider* scanning_complexity_selector;
    GuiSlider* scanning_channel_depth_selector;
    GuiSlider* radius_slider;
public:
    GuiObjectTweakBase(GuiContainer* owner);

    virtual void onDraw(sf::RenderTarget& window) override;
    virtual void open(P<SpaceObject> target) override;
};

class GuiTemplateTweak : public GuiTweakPage
{
private:
    P<ShipTemplateBasedObject> target;

    GuiTextEntry* type_name;
    GuiSlider* heading_slider;
    GuiSlider* rotation_slider;
    GuiSlider* hull_max_slider;
    GuiSlider* hull_slider;
    GuiSlider* system_damage_ratio_slider;
    GuiSlider* system_damage_hull_threshold_slider;
    GuiToggleButton* can_be_destroyed_toggle;
    GuiSlider* transparency_slider;
public:
    GuiTemplateTweak(GuiContainer* owner);
    virtual void onDraw(sf::RenderTarget& window) override;
    virtual void open(P<SpaceObject> target) override;
};

class GuiShipTweakShields : public GuiTweakPage
{
private:
    P<ShipTemplateBasedObject> target;

    GuiSlider* shield_max_slider[max_shield_count];
    GuiSlider* shield_slider[max_shield_count];
    GuiSlider* shield_recharge_slider;
public:
    GuiShipTweakShields(GuiContainer* owner);

    virtual void onDraw(sf::RenderTarget& window) override;

    virtual void open(P<SpaceObject> target) override;
};

class GuiShipTweak : public GuiTweakPage
{
private:
    P<SpaceShip> target;

    GuiSlider* heading_slider;
    GuiSlider* impulse_speed_slider;
    GuiSlider* turn_speed_slider;
    GuiToggleButton* warp_toggle;
    GuiToggleButton* jump_toggle;
    GuiToggleButton* reactor_toggle;
    GuiToggleButton* cloaking_toggle;
    GuiSlider* combat_maneuver_boost_speed_slider;
    GuiSlider* combat_maneuver_strafe_speed_slider;
    GuiSlider* jump_drive_min_distance_slider;
    GuiSlider* jump_drive_max_distance_slider;
    GuiSlider* jump_drive_charge_slider;
    GuiSlider* jump_drive_charge_time_slider;
    GuiSlider* jump_drive_energy_slider;
    GuiSlider* jump_delay_slider;
    GuiSlider* warp_speed_slider;
    GuiSlider* short_range_radar_slider;
    GuiSlider* long_range_radar_slider;
    GuiSlider* engineering_presets_slider;

public:
    GuiShipTweak(GuiContainer* owner);
    virtual void onDraw(sf::RenderTarget& window) override;
    virtual void open(P<SpaceObject> target) override;
};

class GuiJammerTweak : public GuiTweakPage
{
private:
    P<WarpJammer> target;

    GuiSlider* jammer_range_slider;
public:
    GuiJammerTweak(GuiContainer* owner);

    virtual void onDraw(sf::RenderTarget& window) override;

    virtual void open(P<SpaceObject> target) override;
};

class GuiShipTweakMissileWeapons : public GuiTweakPage
{
private:
    P<SpaceShip> target;

    std::vector<GuiSlider*> missile_storage_amount_slider;
    std::vector<GuiSlider*> missile_current_amount_slider;
    std::vector<GuiLabel*> custom_missile_storage_labels;
    std::vector<GuiLabel*> custom_missile_current_labels;
public:
    GuiShipTweakMissileWeapons(GuiContainer* owner);

    virtual void onDraw(sf::RenderTarget& window) override;

    virtual void open(P<SpaceObject> target) override;
};

class GuiShipTweakMissileTubes : public GuiTweakPage
{
private:
    P<SpaceShip> target;

    int tube_index;
    GuiSelector* index_selector;
    GuiSelector* missile_tube_amount_selector;
    GuiSlider* direction_slider;
    GuiSlider* load_time_slider;
    GuiSelector* size_selector;
    std::vector<GuiToggleButton*> allowed_use;
public:
    GuiShipTweakMissileTubes(GuiContainer* owner);

    virtual void onDraw(sf::RenderTarget& window) override;
    virtual void open(P<SpaceObject> target) override;
};

class GuiShipTweakBeamweapons : public GuiTweakPage
{
private:
    P<SpaceShip> target;

    int beam_index;
    GuiToggleButton* valid_toggle;
    GuiSlider* arc_slider;
    GuiSlider* direction_slider;
    GuiSlider* range_slider;
    GuiSlider* turret_arc_slider;
    GuiSlider* turret_direction_slider;
    GuiSlider* turret_rotation_rate_slider;
    GuiLabel* turret_rotation_rate_overlay_label;
    GuiSlider* cycle_time_slider;
    GuiSlider* damage_slider;
    GuiSlider* heat_slider;
    GuiSlider* energy_slider;
public:
    GuiShipTweakBeamweapons(GuiContainer* owner);

    virtual void open(P<SpaceObject> target) override;

    virtual void onDraw(sf::RenderTarget& window) override;
};

class GuiShipTweakSystems : public GuiTweakPage
{
private:
    P<SpaceShip> target;

    GuiLabel* system_name[SYS_COUNT];
    GuiLabel* temp_1[SYS_COUNT];
    GuiLabel* temp_2[SYS_COUNT];
    GuiLabel* temp_3[SYS_COUNT];
    GuiLabel* temp_4[SYS_COUNT];
    GuiSlider* system_damage[SYS_COUNT];
    GuiSlider* system_health_max[SYS_COUNT];
    GuiSlider* system_heat[SYS_COUNT];
    GuiSlider* system_hack[SYS_COUNT];
    GuiSlider* system_coolant[SYS_COUNT];

public:
    GuiShipTweakSystems(GuiContainer* owner);

    virtual void open(P<SpaceObject> target) override;

    virtual void onDraw(sf::RenderTarget& window) override;
};

class GuiShipTweakPlayer : public GuiTweakPage
{
private:
    P<PlayerSpaceship> target;

    GuiTextEntry* control_code;
    GuiSlider* repair_total_slider;
    GuiSlider* probe_max_slider;
//    GuiSlider* oxygen_point_slider;
//    GuiSlider* max_oxygen_point_slider;
    GuiListbox* list_ships_box;
    GuiSlider* energy_level_slider;
    GuiSlider* energy_conso_ratio_slider;
    GuiSlider* max_energy_level_slider;
    GuiSlider* max_coolant_slider;
    GuiSlider* coolant_per_system_slider;
    GuiSlider* repair_slider;
    GuiSlider* repair_per_system_slider;
    GuiToggleButton* auto_repair_toogle;
    GuiToggleButton* auto_coolant_toogle;
    GuiToggleButton* gravity_toggle;
    GuiToggleButton* electrical_toggle;
    GuiToggleButton* biological_toggle;
    GuiLabel* position_count;
    GuiKeyValueDisplay* position[max_crew_positions];
    GuiToggleButton* can_scan;
    GuiToggleButton* can_hack;
    GuiToggleButton* can_dock;
    GuiToggleButton* can_combat_maneuver;
    GuiToggleButton* can_self_destruct;
    GuiToggleButton* can_launch_probe;
public:
    GuiShipTweakPlayer(GuiContainer* owner);

    virtual void open(P<SpaceObject> target) override;

    virtual void onDraw(sf::RenderTarget& window) override;
};

class GuiShipTweakDock : public GuiTweakPage
{
private:
    P<PlayerSpaceship> target;
    //std::vector<GuiListbox*> list_state_boxes;
    std::vector<GuiSelector*> type_selector;
    std::vector<GuiButton*> content_button;
    GuiListbox* list_envol_box;
public:
    GuiShipTweakDock(GuiContainer* owner);
    virtual void open(P<SpaceObject> target);

    virtual void onDraw(sf::RenderTarget& window) override;

};

class GuiShipTweakOxygen : public GuiTweakPage
{
private:
    P<SpaceShip> target;

    GuiSlider* oxygen_point_slider[max_oxygen_zones];
    GuiSlider* oxygen_max_slider[max_oxygen_zones];
    GuiSlider* oxygen_rate_slider[max_oxygen_zones];

    GuiSlider* passagers_slider;
    GuiSlider* max_passagers_slider;

public:
    GuiShipTweakOxygen(GuiContainer* owner);

    virtual void open(P<SpaceObject> target);

    virtual void onDraw(sf::RenderTarget& window) override;
};

class GuiShipTweakMessages : public GuiTweakPage
{
private:
    P<PlayerSpaceship> target;

    sf::Color color_message;
    string type_log;
    string message;

    GuiSelector* color_selector;
    GuiSelector* log_selector;
    GuiTextEntry* message_entry;
    GuiButton* send_message_log;
    GuiToggleButton* message_all_toggle;

public:
    GuiShipTweakMessages(GuiContainer* owner);

    virtual void open(P<SpaceObject> target);

    virtual void onDraw(sf::RenderTarget& window) override;
};

class GuiShipTweakPlanet : public GuiTweakPage
{
private:
    P<Planet> target;
    GuiSelector* texture_selector;

public:
    GuiShipTweakPlanet(GuiContainer* owner);

    virtual void open(P<SpaceObject> target);

    virtual void onDraw(sf::RenderTarget& window) override;
};

class GuiShipTweakInfos : public GuiTweakPage
{
private:
    P<SpaceObject> target;

    GuiTextEntry* infos_label[10];
    GuiTextEntry* infos_value[10];

public:
    GuiShipTweakInfos(GuiContainer* owner);

    virtual void open(P<SpaceObject> target) override;

    virtual void onDraw(sf::RenderTarget& window) override;
};

#endif//TWEAK_H
