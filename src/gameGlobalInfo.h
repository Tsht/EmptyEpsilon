#ifndef GAME_GLOBAL_INFO_H
#define GAME_GLOBAL_INFO_H

#include "spaceObjects/playerSpaceship.h"
#include "script.h"
#include "GMScriptCallback.h"
#include "GMMessage.h"
#include "gameStateLogger.h"

class GameStateLogger;
class GameGlobalInfo;
extern P<GameGlobalInfo> gameGlobalInfo;

enum EPlayerWarpJumpDrive
{
    PWJ_ShipDefault = 0,
    PWJ_WarpDrive,
    PWJ_JumpDrive,
    PWJ_WarpAndJumpDrive,
    PWJ_None,
    PWJ_MAX,
};
enum EScanningComplexity
{
    SC_None = 0,
    SC_Simple,
    SC_Normal,
    SC_Advanced,
    SC_NbItem
};
enum ECommsGmInterception
{
    CGI_None = 0,
    CGI_Temp,
    CGI_Always
};
enum EHackingGames
{
    HG_Mine,
    HG_Lights,
    HG_All
};

class GameGlobalInfo : public MultiplayerObject, public Updatable
{
    P<GameStateLogger> state_logger;
public:
    /*!
     * \brief Maximum number of player ships.
     */
    static const int max_player_ships = 32;
     /*!
     * \size of a sector.
     */
    static const int sector_size = 20000;
    int victory_faction;
    int32_t playerShipId[max_player_ships];
    int callsign_counter;
    /*!
     * \brief List of known scripts
     */
    PVector<Script> script_list;
public:
    string global_message;
    float global_message_timeout;

    string banner_string;

    std::vector<float> reputation_points;
    EScanningComplexity scanning_complexity;
    //Hacking difficulty ranges from 0 to 3
    int hacking_difficulty;
    EHackingGames hacking_games;
    bool use_beam_shield_frequencies;
    bool use_system_damage;
    bool use_repair_crew;
    bool allow_main_screen_tactical_radar;
    bool allow_main_screen_long_range_radar;
    bool allow_main_screen_global_range_radar;
    bool allow_main_screen_target_analysis;
    bool allow_main_screen_ship_state;
    bool use_nano_repair_crew;
    bool use_long_range_for_relay;
    bool use_complex_radar_signatures;
    float hack_efficiency_ratio;
    int ai_missile_attack_range;
    string gm_control_code;
    float elapsed_time;
    string scenario;
    std::unordered_map<string, string> scenario_settings;

    //List of script functions that can be called from the GM interface (Server only!)
    std::vector<ScriptSimpleCallback> gm_callback_functions;
    //List of names of gm_callback_functions scripts (replicated to clients)
    std::vector<string> gm_callback_names;
    std::list<GMMessage> gm_messages;
    //When active, all comms request goto the GM as chat, and normal scripted converstations are disabled. This does not disallow player<->player ship comms.
    ECommsGmInterception intercept_all_comms_to_gm;

    //Callback called when a new player ship is created on the ship selection screen.
    ScriptSimpleCallback on_new_player_ship;
    bool allow_new_player_ships = true;

    //Callback called when a new ship is created.
    ScriptSimpleCallback on_new_ship;

    std::function<void(glm::vec2)> on_gm_click;

    GameGlobalInfo();
    virtual ~GameGlobalInfo();

    P<PlayerSpaceship> getPlayerShip(int index);
    void setPlayerShip(int index, P<PlayerSpaceship> ship);

    int findPlayerShip(P<PlayerSpaceship> ship);
    int insertPlayerShip(P<PlayerSpaceship> ship);
    /*!
     * \brief Set a faction to victorious.
     * \param string Name of the faction that won.
     */
    void setVictory(string faction_name) { victory_faction = FactionInfo::findFactionId(faction_name); }
    /*!
     * \brief Get ID of faction that won.
     * \param int
     */
    int getVictoryFactionId() { return victory_faction; }

    void addScript(P<Script> script);
    //Reset the global game state (called when we want to load a new scenario, and clear out this one)
    void reset();
    void setScenarioSettings(const string filename, std::unordered_map<string, string> new_settings);
    void startScenario(string filename, std::unordered_map<string, string> new_settings = {});

    virtual void update(float delta) override;
    virtual void destroy() override;
    string getMissionTime();

    string getNextShipCallsign();
};

string getSectorName(glm::vec2 position, float correction_x = 0.0, float correction_y = 0.0);
glm::vec2 getSectorPosition(string sectorName, float correction_x = 0.0, float correction_y = 0.0);
bool isValidSectorName(string sectorName);
glm::vec2 getPositionFromSring(string sectorName, float correction_x = 0.0, float correction_y = 0.0);
string getStringFromPosition(glm::vec2 position,float correction_x = 0.0, float correction_y = 0.0);
bool isValidPositionString(string sectorName);
glm::vec2 sectorToXY(string sectorName);

REGISTER_MULTIPLAYER_ENUM(EScanningComplexity);
REGISTER_MULTIPLAYER_ENUM(EHackingGames);

template<> int convert<EScanningComplexity>::returnType(lua_State* L, EScanningComplexity complexity);
template<> int convert<EHackingGames>::returnType(lua_State* L, EHackingGames games);

#endif//GAME_GLOBAL_INFO_H
