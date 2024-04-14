#ifndef PLAYER_SPACESHIP_H
#define PLAYER_SPACESHIP_H

#include "spaceship.h"
#include "scanProbe.h"
#include "commsScriptInterface.h"
#include "playerInfo.h"
#include "spaceshipParts/dock.h"
#include "spaceObjects/cpuShip.h"
#include <iostream>
#include <map>
#include <set>

class ScanProbe;

enum ECommsState
{
    CS_Inactive,          // No active comms
    CS_OpeningChannel,    // Opening a comms channel
    CS_BeingHailed,       // Receiving a hail from an object
    CS_BeingHailedByGM,   //                   ... the GM
    CS_ChannelOpen,       // Comms open to an object
    CS_ChannelOpenPlayer, //           ... another player
    CS_ChannelOpenGM,     //           ... the GM
    CS_ChannelFailed,     // Comms failed to connect
    CS_ChannelBroken,     // Comms broken by other side
    CS_ChannelClosed      // Comms manually closed
};

enum EAlertLevel
{
    AL_Normal,      // No alert state
    AL_YellowAlert, // Yellow
    AL_RedAlert,    // Red
    AL_MAX          // ?
};

class PlayerSpaceship : public SpaceShip
{
public:
    // Power consumption and generation base rates
    constexpr static float energy_transfer_per_second = 5;
    constexpr static float heat_transfer_per_second = 0.1;
    constexpr static float repair_per_second = 0.007;
    constexpr static float cargo_repair_per_second = 0.1;

    // Repair change rate
    constexpr static float system_repair_level_change_per_second = 1.2;
    // Repair effectiveness
    constexpr static float system_repair_effect_per_second = 0.005;
    constexpr static float default_energy_shield_use_per_second = 1.5f;
    constexpr static float default_energy_warp_per_second = 1.7f;
    // Total coolant
    float max_coolant_per_system = 10.0f;
    float max_coolant;
    // Total repair
    float max_repair_per_system = 3.0f;
    float max_repair;
    // Overheat subsystem damage rate
    constexpr static float damage_per_second_on_overheat = 0.08f;
    // Base time it takes to perform an action
    constexpr static float shield_calibration_time = 25.0f;
    constexpr static float comms_channel_open_time = 2.0;
    constexpr static float scan_probe_charge_time = 10.0f;
    constexpr static float max_scanning_delay = 6.0;
    // Maximum number of self-destruction confirmation codes
    constexpr static int max_self_destruct_codes = 3;
    // Subsystem effectiveness base rates
    constexpr static int max_engineer_presets_number = 9;

    constexpr static unsigned int max_number_of_waiting_squadron = 20;
    constexpr static unsigned int max_squadron_launch = 3; //should then be number of flight decks (see dock view)
    
    bool has_gravity_sensor;
    bool has_electrical_sensor;
    bool has_biological_sensor;

    constexpr static int16_t CMD_PLAY_CLIENT_SOUND = 0x0001;

    // Content of a line in the ship's log
    class ShipLogEntry
    {
    public:
        string prefix;
        string text;
        glm::u8vec4 color;
        string station;
        unsigned int seq;

        ShipLogEntry() {}
        ShipLogEntry(string prefix, string text, glm::u8vec4 color, string station, unsigned int seq)
        : prefix(prefix), text(text), color(color), station(station), seq(seq) {}

        bool operator!=(const ShipLogEntry& e) { return prefix != e.prefix || text != e.text || color != e.color || station != e.station; }
    };

    class CustomShipFunction
    {
    public:
        enum class Type
        {
            Info,
            Button,
            Message
        };
        Type type;
        string name;
        string caption;
        ECrewPosition crew_position;
        ScriptSimpleCallback callback;
        int order;

        bool operator!=(const CustomShipFunction& csf) { return type != csf.type || name != csf.name || caption != csf.caption || crew_position != csf.crew_position; }
    };

    // Visual indicators of hull damage and in-progress jumps
    float hull_damage_indicator;
    float jump_indicator;
    // Time in seconds it takes to recalibrate shields
    float shield_calibration_delay;
    // Ship automation features, mostly for single-person ships like fighters
    bool auto_repair_enabled;
    bool auto_coolant_enabled;
    // Whether shields are up (true) or down
    bool shields_active;
    int warp_indicator;
    // Password to join a ship. Default is empty.
    string control_code;

    float energy_consumption_ratio;

    //Server only
    struct Modifier
    {
        string category;
        string information;
        bool activated{false};
    };
    std::map<string, Modifier> modifiers_to_data;

private:
    bool on_new_player_ship_called=false;
    // Comms variables
    ECommsState comms_state;
    float comms_open_delay;
    P<SpaceObject> comms_target; // Server only
    bool comms_generic = false;
    std::vector<int> comms_reply_id;
    std::vector<string> comms_reply_message;
    CommsScriptInterface comms_script_interface; // Server only
    // Ship's log container
    std::vector<ShipLogEntry> ships_log_generic;
    std::vector<ShipLogEntry> ships_log_intern;
    std::vector<ShipLogEntry> ships_log_docks;
    std::vector<ShipLogEntry> ships_log_science;

    float energy_shield_use_per_second = default_energy_shield_use_per_second;
    float energy_warp_per_second = default_energy_warp_per_second;
    unsigned int last_log_seq = 0;
public:
    ESystem auto_repairing_system;
    std::vector<CustomShipFunction> custom_functions;

    std::vector<glm::vec2> waypoints;

    // Presets for engeneering screen
    int active_engineer_presets_number;
    float power_presets[SYS_COUNT][max_engineer_presets_number];
    float coolant_presets[SYS_COUNT][max_engineer_presets_number];

    // Ship functionality
    // Capable of scanning a target
    bool can_scan = true;
    // Target of a scan. Server-only value
    P<SpaceObject> scanning_target;
    // Time in seconds to scan an object if scanning_complexity is 0 (none)
    float scanning_delay = 0.0;
    // Number of sliders during a scan
    int scanning_complexity = 0;
    // Number of times an object must be scanned to achieve a fully scanned
    // state
    int scanning_depth = 0;

    // Capable of hacking a target
    bool can_hack = true;
    // Capable of docking with a target
    bool can_dock = true;
    // Capable of combat maneuvers
    bool can_combat_maneuver = true;

    // Capable of self-destruction
    bool can_self_destruct = true;
    bool activate_self_destruct = false;
    uint32_t self_destruct_code[max_self_destruct_codes];
    bool self_destruct_code_confirmed[max_self_destruct_codes];
    ECrewPosition self_destruct_code_entry_position[max_self_destruct_codes];
    ECrewPosition self_destruct_code_show_position[max_self_destruct_codes];
    float self_destruct_countdown = 0.0;
    float self_destruct_damage = 150.0;
    float self_destruct_size = 1500.0;

    // Capable of probe launches
    bool can_launch_probe = true;
    int max_scan_probes = 8;
    int scan_probe_stock;
    float scan_probe_recharge = 0.0;
    ScriptSimpleCallback on_probe_launch;
    float scan_probe_recharge_dock;

        
    struct Squadron
    {
        string squadron_name {""};
        string squadron_template_name {""};

        PVector<CpuShip> ships;    
        // bool operator<(const Squadron& rhs) const 
        // {
        //     return squadron_name < rhs.squadron_name;
        // }
    };
    struct SquadronClientInfos 
    {
        string squadron_template{""};
        string squadron_name{""};
        string order{""};
        string target{""};
        int32_t leader_id{-1};
    };


    //Blueprints which are activated (by player), available (set by GM), and delay to creation
    float squadron_delay_to_next_creation[max_blueprints_count]{};
    bool squadron_bp_activated[max_blueprints_count]{false};
    bool squadron_bp_available[max_blueprints_count]{false};
    unsigned int squadron_bp_max_created[max_blueprints_count]{};

    std::map<string, float> ammo_delay_to_next_creation{};
    std::map<string, bool> ammo_bp_activated{};
    std::map<string, bool> ammo_bp_available{};
    std::map<string, unsigned int> ammo_bp_max_created{};
    
    float sq_bp_delay_factor{1};
    float ammo_bp_delay_factor{1};

    //Squadrons available to launch
    std::array<unsigned int,max_number_of_waiting_squadron> number_of_waiting_squadron_for_bp{};
    //Squadrons being launched
    std::array<int,max_squadron_launch> bp_of_launching_squadron{};
    std::array<float, max_squadron_launch> launch_delay{};
    //Squadrons launched
    constexpr static unsigned int max_squadrons_in_flight_limit {16};
    unsigned int max_squadrons_in_flight{8};
    std::vector<Squadron> launched_squadrons;
    std::array<SquadronClientInfos, max_squadrons_in_flight_limit> launched_squadrons_infos;

    float launch_duration{15};

    string comms_target_name;
    string comms_incomming_message;

    std::vector<ShipLogEntry> ships_log;

    float hack_time;
    ScriptSimpleCallback on_probe_link;
    ScriptSimpleCallback on_probe_unlink;

    // Main screen content
    EMainScreenSetting main_screen_setting;
    // Content overlaid on the main screen, such as comms
    EMainScreenOverlay main_screen_overlay;

    string texture_front;
    string texture_back;
    string texture_left;
    string texture_right;
    string texture_top;
    string texture_bottom;

    float texture_r;
    float texture_g;
    float texture_b;
    float texture_a;

    float timer_log_intern;
	float timer_log_generic;
	float timer_log_docks;
	float timer_log_science;

    EAlertLevel alert_level;

    int32_t linked_science_probe_id = -1;
    int32_t linked_probe_3D_id = -1;
    int32_t linked_analysis_object_id = -1;

    PlayerSpaceship();
    virtual ~PlayerSpaceship();

    // Comms functions
    bool isCommsInactive() { return comms_state == CS_Inactive; }
    bool isCommsOpening() { return comms_state == CS_OpeningChannel; }
    bool isCommsBeingHailed() { return comms_state == CS_BeingHailed || comms_state == CS_BeingHailedByGM; }
    bool isCommsBeingHailedByGM() { return comms_state == CS_BeingHailedByGM; }
    bool isCommsFailed() { return comms_state == CS_ChannelFailed; }
    bool isCommsBroken() { return comms_state == CS_ChannelBroken; }
    bool isCommsClosed() { return comms_state == CS_ChannelClosed; }
    bool isCommsChatOpen() { return comms_state == CS_ChannelOpenPlayer || comms_state == CS_ChannelOpenGM; }
    bool isCommsChatOpenToGM() { return comms_state == CS_ChannelOpenGM; }
    bool isCommsChatOpenToPlayer() { return comms_state == CS_ChannelOpenPlayer; }
    bool isCommsScriptOpen() { return comms_state == CS_ChannelOpen; }
    ECommsState getCommsState() { return comms_state; }
    float getCommsOpeningDelay() { return comms_open_delay; }
    const std::vector<string>& getCommsReplyOptions() const { return comms_reply_message; }
    P<SpaceObject> getCommsTarget() { return comms_target; }
    const string& getCommsTargetName() { return comms_target_name; }
    const string& getCommsIncommingMessage() { return comms_incomming_message; }
    bool hailCommsByGM(string target_name);
    bool hailByObject(P<SpaceObject> object, string opening_message);
    void setCommsMessage(string message);
    void addCommsIncommingMessage(string message);
    void addCommsOutgoingMessage(string message);
    void addCommsReply(int32_t id, string message);
    void switchCommsToGM();
    void closeComms();

    void setEnergyLevel(float amount) { energy_level = std::max(0.0f, std::min(max_energy_level, amount)); }
    void setEnergyLevelMax(float amount) { max_energy_level = std::max(0.0f, amount); energy_level = std::min(energy_level, max_energy_level); }
    float getEnergyLevel() { return energy_level; }
    float getEnergyLevelMax() { return max_energy_level; }

    void setActivePresetNumber(int amount) { if (amount >= 0 and amount <= max_engineer_presets_number) active_engineer_presets_number = amount; }
    int getActivePresetNumber() { return active_engineer_presets_number; }

    void setCanScan(bool enabled) { can_scan = enabled; }
    bool getCanScan() { return can_scan; }
    void setCanHack(bool enabled) { can_hack = enabled; }
    bool getCanHack() { return can_hack; }
    void setCanDock(bool enabled) { can_dock = enabled; }
    bool getCanDock() { return can_dock; }
    void setCanCombatManeuver(bool enabled) { can_combat_maneuver = enabled; }
    bool getCanCombatManeuver() { return can_combat_maneuver; }
    void setCanSelfDestruct(bool enabled) { can_self_destruct = enabled; }
    bool getCanSelfDestruct() { return can_self_destruct && self_destruct_size > 0 && self_destruct_damage > 0; }
    void setCanLaunchProbe(bool enabled) { can_launch_probe = enabled; }
    bool getCanLaunchProbe() { return can_launch_probe; }

    void setSelfDestructDamage(float amount) { self_destruct_damage = std::max(0.0f, amount); }
    float getSelfDestructDamage() { return self_destruct_damage; }
    void setSelfDestructSize(float size) { self_destruct_size = std::max(0.0f, size); }
    float getSelfDestructSize() { return self_destruct_size; }

    void setScanProbeCount(int amount) { scan_probe_stock = std::max(0, std::min(amount, max_scan_probes)); }
    int getScanProbeCount() { return scan_probe_stock; }
    void setMaxScanProbeCount(int amount) { max_scan_probes = std::max(0, amount); scan_probe_stock = std::min(scan_probe_stock, max_scan_probes); }
    int getMaxScanProbeCount() { return max_scan_probes; }

    // Radar capabilities
    bool hasGravitySensor() { return has_gravity_sensor; }
    bool hasElectricalSensor() { return has_electrical_sensor; }
    bool hasBiologicalSensor() { return has_biological_sensor; }
    void setGravitySensor(bool has_gravity) { has_gravity_sensor = has_gravity; }
    void setElectricalSensor(bool has_electrical) { has_electrical_sensor = has_electrical; }
    void setBiologicalSensor(bool has_biological) { has_biological_sensor = has_biological; }
    void onProbeLaunch(ScriptSimpleCallback callback);
    void onProbeLink(ScriptSimpleCallback callback);
    void onProbeUnlink(ScriptSimpleCallback callback);

    void addCustomButton(ECrewPosition position, string name, string caption, ScriptSimpleCallback callback, std::optional<int> order);
    void addCustomInfo(ECrewPosition position, string name, string caption, std::optional<int> order);
    void addCustomMessage(ECrewPosition position, string name, string caption);
    void addCustomMessageWithCallback(ECrewPosition position, string name, string caption, ScriptSimpleCallback callback);
    void removeCustom(string name);

    ESystem getBeamSystemTarget(){ return beam_system_target; }
    string getBeamSystemTargetName(){ return getSystemName(beam_system_target); }
    // Client command functions
    virtual void onReceiveClientCommand(int32_t client_id, sp::io::DataBuffer& packet) override;
    void commandTargetRotation(float target);
    void commandTurnSpeed(float turnSpeed);
    void commandImpulse(float target);
    void commandWarp(int8_t target);
    void commandJump(float distance);
    void commandSetTarget(P<SpaceObject> target);
    void commandSetDockTarget(P<SpaceObject> target);
    void commandSetLandingTarget(P<SpaceObject> target);
    void commandSetAnalysisLink(int32_t id);
    void commandSetProbe3DLink(int32_t id);
    void commandLoadTube(int8_t tubeNumber, string missileType);
    void commandSetScienceLink(P<ScanProbe> probe);
    void commandClearScienceLink();
    void commandUnloadTube(int8_t tubeNumber);
    void commandFireTube(int8_t tubeNumber, float missile_target_angle);
    void commandFireTubeAtTarget(int8_t tubeNumber, P<SpaceObject> target);
    void commandSetShields(bool enabled);
    void commandMainScreenSetting(EMainScreenSetting mainScreen);
    void commandMainScreenOverlay(EMainScreenOverlay mainScreen);
    void commandScan(P<SpaceObject> object);
    void commandSetSystemPowerRequest(ESystem system, float power_level);
    void commandSetSystemPowerPreset(ESystem system, int preset, float power_level);
    void commandSetSystemCoolantRequest(ESystem system, float coolant_level);
    void commandSetSystemCoolantPreset(ESystem system, int preset, float coolant_level);
    void commandSetSystemRepairRequest(ESystem system, float repair_level);
    void commandDock(P<SpaceObject> station);
    void commandUndock();
    void commandAbortDock();
    void commandLand(P<SpaceObject> obj);
    void commandAbortLanding();
    void commandOpenTextComm(P<SpaceObject> obj);
    void commandCloseTextComm();
    void commandAnswerCommHail(bool awnser);
    void commandSendComm(uint8_t index);
    void commandSendCommPlayer(string message);
    void commandSetAutoRepair(bool enabled);
    void commandSetBeamFrequency(int32_t frequency);
    void commandSetBeamSystemTarget(ESystem system);
    void commandSetShieldFrequency(int32_t frequency);
    void commandAddWaypoint(glm::vec2 position);
    void commandRemoveWaypoint(int32_t index);
    void commandMoveWaypoint(int32_t index, glm::vec2 position);
    void commandActivateSelfDestruct();
    void commandCancelSelfDestruct();
    void commandConfirmDestructCode(int8_t index, uint32_t code);
    void commandCombatManeuverBoost(float amount);
    void commandCombatManeuverStrafe(float strafe);
    void commandLaunchProbe(glm::vec2 target_position);
    void commandLaunchCargo(int index);
    void commandMoveCargo(int index);
    void commandCancelMoveCargo(int index);
    void commandSetDockMoveTarget(int srcIdx, int destIdx);
    void commandSetDockEnergyRequest(int index, float value);
    void commandScanDone();
    void commandScanCancel();
    void commandSetAlertLevel(EAlertLevel level);
    void commandHackingFinished(P<SpaceObject> target, string target_system, string target_faction);
    void commandCustomFunction(string name);
    void commandSetAutoRepairSystemTarget(ESystem system);

    void setTexture(string tfront, string tback, string tleft, string tright, string ttop, string tbottom) {
        texture_front = tfront;
        texture_back = tback;
        texture_left = tleft;
        texture_right = tright;
        texture_top = ttop;
        texture_bottom = tbottom;
    };

    void setTextureColor(float r, float g, float b, float a) {
        texture_r = r;
        texture_g = g;
        texture_b = b;
        texture_a = a;
    };

    virtual void onReceiveServerCommand(sp::io::DataBuffer& packet) override;

    // Template function
    virtual void applyTemplateValues() override;

    // Ship status functions
    virtual void executeJump(float distance) override;
    virtual void takeHullDamage(float damage_amount, DamageInfo& info) override;
    void setSystemCoolantRequest(ESystem system, float request);
    void setMaxCoolant(float coolant);
    float getMaxCoolant() { return max_coolant; }
    void setAutoCoolant(bool active) { auto_coolant_enabled = active; }
    void setMaxCoolantPerSystem(float coolant){ max_coolant_per_system = coolant; }
    float getMaxCoolantPerSystem() { return max_coolant_per_system; }
    void setSystemRepairRequest(ESystem system, float request);
    void setMaxRepair(float repair) { max_repair = repair; max_repair_per_system = std::min(max_repair_per_system, max_repair);} 
    float getMaxRepair() { return max_repair; }
    void setMaxRepairPerSystem(float amount){ max_repair_per_system = amount; max_repair_per_system = std::min(max_repair_per_system, max_repair);}
    float getMaxRepairPerSystem() { return max_repair_per_system; }
    void setAutoRepair(bool active) { auto_repair_enabled = active; }
    int getRepairCrewCount();
    void setRepairCrewCount(int amount);
    EAlertLevel getAlertLevel() { return alert_level; }

    void setEnergyConsumptionRatio(float ratio) { energy_consumption_ratio = ratio;}
    // Flow rate controls.
    float getEnergyShieldUsePerSecond() const { return energy_shield_use_per_second; }
    void setEnergyShieldUsePerSecond(float rate) { energy_shield_use_per_second = rate; }
    float getEnergyWarpPerSecond() const { return energy_warp_per_second; }
    void setEnergyWarpPerSecond(float rate) { energy_warp_per_second = rate; }

    // Ship update functions
    virtual void update(float delta) override;
    virtual bool useEnergy(float amount) override;
    virtual void addHeat(ESystem system, float amount) override;

    // Call on the server to play a sound on the main screen.
    void playSoundOnMainScreen(string sound_name);

    float getNetSystemEnergyUsage();

    // Ship's log functions
    void addToSpecificShipLog(string message, glm::u8vec4 color, string station); // avec le register script, impossible d'avoir des signatures avec arguments differents
    void addToShipLog(string message, glm::u8vec4 color) { addToSpecificShipLog(message, color, "generic"); }
    void addToShipLogBy(string message, P<SpaceObject> target);
    const std::vector<ShipLogEntry>& getShipsLog(string station) const;

    // Ship's crew functions
    void transferPlayersToShip(P<PlayerSpaceship> other_ship);
    void transferPlayersAtPositionToShip(ECrewPosition position, P<PlayerSpaceship> other_ship);
    bool hasPlayerAtPosition(ECrewPosition position);

    // Ship shields functions
    virtual bool getShieldsActive() override { return shields_active; }
    void setShieldsActive(bool active) { shields_active = active; }

    // Waypoint functions
    int getWaypointCount() { return waypoints.size(); }
    glm::vec2 getWaypoint(int index) { if (index > 0 && index <= int(waypoints.size())) return waypoints[index - 1]; return glm::vec2(0, 0); }

    // Ship control code/password setter
    void setControlCode(string code) { control_code = code.upper(); }

    // Radar function
    virtual void drawOnGMRadar(sp::RenderTarget& renderer, glm::vec2 position, float scale, float rotation, bool long_range) override;

    //specific to player ship
    /*!
    * Check if object can accept landing from this ship
    * \param object Object that wants to land
    */
    virtual bool canBeLandedOn(P<SpaceObject> obj) override;
    // Radar range
    float getLongRangeRadarRange() override;
    float getShortRangeRadarRange() override;
    float getProbeRangeRadarRange() override;

    //Ajout Tdelc, deplace depuis
    float getDronesControlRange();
    // Script export function
    virtual string getExportLine() override;

    //Tsht
    ScriptSimpleCallback on_modifier_toggle;
    void registerModifier(string category, string name, string description) { modifiers_to_data.emplace (std::make_pair(name, Modifier{category, description, false}));}
    void activateModifier(string name);
    void deActivateModifier(string name);
    
    void onModifierToggle(ScriptSimpleCallback callback)
    {
        on_modifier_toggle = callback;
    }

    bool isModifierActivated(string name)
    {
        if(auto iter = modifiers_to_data.find(name); iter != modifiers_to_data.end())
        {
            return iter->second.activated;
        }
        return false;
    }



    void setMaxSquadrons(unsigned int max) { max_squadrons_in_flight = max;}

    void instantiateSquadron(const string& type);
    void launchWaitingSquadron(unsigned int deck, const string& identifier);
    void commandLaunchSquadron(unsigned int deck, const string& identifier);
    void commandSetSquadronBlueprintActivation(int idx, bool val);
    void commandSetAmmoBlueprintActivation(string idx_str, bool val);
    void launchSquadron(unsigned int deck);
    bool isSquadronBlueprintAvailable(int idx) { return squadron_bp_available[idx];}
    bool isSquadronBlueprintActivated(int idx) { return squadron_bp_activated[idx];}
    bool isAmmoBlueprintAvailable(const string& name) { return ammo_bp_available[name];}
    bool isAmmoBlueprintActivated(const string& name) { return ammo_bp_activated[name];}

    bool canLaunchSquadron() 
    {
        unsigned int total_nbr_launching{0};
        for(int bp_launching : bp_of_launching_squadron)
        {
            if(bp_launching != -1)
            total_nbr_launching ++;
        }
        if(total_nbr_launching >= max_squadron_launch)
        {
            return false;
        }
        if((total_nbr_launching + getLaunchedSquadronsCount())>= getMaximumNumberOfSquadronsInFlight())
        {
            return false;
        }
        //TODO effectiveness of system
        return true;
    }

    unsigned int getLaunchedSquadronsCount() 
    { 
        unsigned int nbr{0};
        for(auto sqi : launched_squadrons_infos)
        {
            if(sqi.squadron_name != "")
            {
                nbr++;
            }
        } 
        return nbr;
    }
    unsigned int getWaitingSquadronsCount() 
    { 
        unsigned int res{0};
        for(unsigned int nbr : number_of_waiting_squadron_for_bp)
        {
            res+=nbr;
        }
        return res; 
    }

    unsigned int getWaitingSquadronsCount(unsigned int idx) 
    { 
        return number_of_waiting_squadron_for_bp[idx];
    }

    unsigned int getLaunchingSquadronCount(unsigned int idx)
    {
        unsigned int nbr{0};
        for(int launching : bp_of_launching_squadron)
        {
            if(launching == (int)idx)
            {
                nbr++;
            }
        }
        return nbr;
    }
    
    unsigned int getLaunchedSquadronsCount(const string &tp_name) 
    { 
        unsigned int n =0;
        for (auto &squadron : launched_squadrons_infos)
        {
            if(squadron.squadron_template == tp_name)
            {
                n++;
            }
        }
        return n;
    }

    unsigned int getSquadronCount(unsigned int idx) 
    { 
        string name = ship_template->squadrons_compositions[idx].template_name;
       
        return getWaitingSquadronsCount(idx) + getLaunchingSquadronCount(idx) + getLaunchedSquadronsCount(name);

    }
    unsigned int getMaximumNumberOfSquadronsInFlight() { return max_squadrons_in_flight;}
    std::vector<SquadronTemplate>& getSquadronCompositions()
    {
       return ship_template->squadrons_compositions;
    }

    std::array<unsigned int,max_number_of_waiting_squadron> getNbrWaitingSquadrons()
    {
        return number_of_waiting_squadron_for_bp;
    }
    std::vector<Squadron>& getLaunchedSquadrons()
    {
        return launched_squadrons;
    }

    std::array<SquadronClientInfos, max_squadrons_in_flight_limit>& getLaunchedSquadronsInfos()
    {
        return launched_squadrons_infos;
    }

    float getSquadronCreationProgression(unsigned int idx)
    {
        return 1.0f - squadron_delay_to_next_creation[idx] / ship_template->squadrons_compositions[idx].construction_duration;
    }
    float getAmmoCreationProgression(const string &name)
    {
        return 1.0f - ammo_delay_to_next_creation[name] / ship_template->ammo_blueprints[name].construction_duration;
    }

    bool isLaunchingSquadron(int n)
    {
        return (bp_of_launching_squadron[n] != -1) ? true : false;
    }

    float getLaunchSquadronProgression(int n)
    {
        return 1.0f - launch_delay[n] / launch_duration;
    }

    bool isInSquadron(P<SpaceShip> &iSpaceship)
    {
        for(auto &sq : getLaunchedSquadrons())
        {
            for(auto &ship : sq.ships)
            {
                if(iSpaceship == ship)
                    return true;
            }
        }
        return false;
    }

    void commandOrderSquadronPosition(EAIOrder order, unsigned int idx, const glm::vec2& pos);
    void commandOrderSquadronTarget(EAIOrder order, unsigned int idx, P<SpaceObject>& obj);
    void orderSquadron(EAIOrder order, unsigned int idx, const glm::vec2& pos);
    void orderSquadron(EAIOrder order, unsigned int idx, P<SpaceObject>& obj);

    float getSquadronLaunchDuration() { return launch_duration;}
    void setSquadronLaunchDuration(float duration) { launch_duration = duration ;}
    
};
REGISTER_MULTIPLAYER_ENUM(ECommsState);
template<> int convert<EAlertLevel>::returnType(lua_State* L, EAlertLevel l);
template<> void convert<EAlertLevel>::param(lua_State* L, int& idx, EAlertLevel& al);
REGISTER_MULTIPLAYER_ENUM(EAlertLevel);

static inline sp::io::DataBuffer& operator << (sp::io::DataBuffer& packet, const PlayerSpaceship::CustomShipFunction& csf) { return packet << uint8_t(csf.type) << uint8_t(csf.crew_position) << csf.name << csf.caption; } \
static inline sp::io::DataBuffer& operator >> (sp::io::DataBuffer& packet, PlayerSpaceship::CustomShipFunction& csf) { int8_t tmp; packet >> tmp; csf.type = PlayerSpaceship::CustomShipFunction::Type(tmp); packet >> tmp; csf.crew_position = ECrewPosition(tmp); packet >> csf.name >> csf.caption; return packet; }

string alertLevelToString(EAlertLevel level);
string alertLevelToLocaleString(EAlertLevel level);

static inline bool operator < (const PlayerSpaceship::CustomShipFunction& a, const PlayerSpaceship::CustomShipFunction& b) {return (a.order < b.order);}
#endif//PLAYER_SPACESHIP_H
