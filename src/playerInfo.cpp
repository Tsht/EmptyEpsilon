#include <i18n.h>
#include "playerInfo.h"
#include "screens/mainScreen.h"
#include "screens/crewStationScreen.h"

#include "screens/crew6/helmsScreen.h"
#include "screens/crew6/weaponsScreen.h"
#include "screens/crew6/engineeringScreen.h"
#include "screens/crew6/scienceScreen.h"
#include "screens/crew6/relayScreen.h"
#include "screens/crew6/cicScreen.h"

#include "screens/crew4/tacticalScreen.h"
#include "screens/crew4/engineeringAdvancedScreen.h"
#include "screens/crew4/operationsScreen.h"

#include "screens/crew1/singlePilotScreen.h"

#include "screens/extra/damcon.h"
#include "screens/extra/powerManagement.h"
#include "screens/extra/databaseScreen.h"
#include "screens/extra/droneOperatorScreen.h"
#include "screens/extra/dockMasterScreen.h"
#include "screens/extra/droneMasterScreen.h"
#include "screens/extra/oxygenScreen.h"

#include "screens/extra/commsScreen.h"
#include "screens/extra/shipLogScreen.h"
#include "screens/extra/radarScreen.h"
#include "screens/extra/targetAnalysisScreen.h"

#include "screenComponents/mainScreenControls.h"
#include "screenComponents/selfDestructEntry.h"

static const int16_t CMD_UPDATE_CREW_POSITION = 0x0001;
static const int16_t CMD_UPDATE_SHIP_ID = 0x0002;
static const int16_t CMD_UPDATE_MAIN_SCREEN = 0x0003;
static const int16_t CMD_UPDATE_MAIN_SCREEN_CONTROL = 0x0004;
static const int16_t CMD_UPDATE_NAME = 0x0005;

P<PlayerInfo> my_player_info;
P<PlayerSpaceship> my_spaceship;
PVector<PlayerInfo> player_info_list;

REGISTER_MULTIPLAYER_CLASS(PlayerInfo, "PlayerInfo");
PlayerInfo::PlayerInfo()
: MultiplayerObject("PlayerInfo")
{
    registerMemberReplication(&client_id);

    for(int n=0; n<max_crew_positions; n++)
    {
        registerMemberReplication(&crew_position[n]);
    }
    registerMemberReplication(&ship_id);
    registerMemberReplication(&name);
    registerMemberReplication(&main_screen);
    registerMemberReplication(&main_screen_control);

    player_info_list.push_back(this);
}

void PlayerInfo::reset()
{
    ship_id = -1;
    main_screen_control = 0;
    last_ship_password = "";

    for(int n=0; n<max_crew_positions; n++)
        crew_position[n] = 0;
}

bool PlayerInfo::isOnlyMainScreen(int monitor_index)
{
    if (!(main_screen & (1 << monitor_index)))
        return false;
    for(int n=0; n<max_crew_positions; n++)
        if (crew_position[n] & (1 << monitor_index))
            return false;
    return true;
}

void PlayerInfo::commandSetCrewPosition(int monitor_index, ECrewPosition position, bool active)
{
    sp::io::DataBuffer packet;
    packet << CMD_UPDATE_CREW_POSITION << uint32_t(monitor_index) << int32_t(position) << active;
    sendClientCommand(packet);

    if (active)
        crew_position[position] |= (1 << monitor_index);
    else
        crew_position[position] &=~(1 << monitor_index);
}

void PlayerInfo::commandSetShipId(int32_t id)
{
    sp::io::DataBuffer packet;
    packet << CMD_UPDATE_SHIP_ID << id;
    sendClientCommand(packet);
}

void PlayerInfo::commandSetMainScreen(int monitor_index, bool enabled)
{
    sp::io::DataBuffer packet;
    packet << CMD_UPDATE_MAIN_SCREEN << uint32_t(monitor_index) << enabled;
    sendClientCommand(packet);

    if (enabled)
        main_screen |= (1 << monitor_index);
    else
        main_screen &=~(1 << monitor_index);
}

void PlayerInfo::commandSetMainScreenControl(int monitor_index, bool control)
{
    sp::io::DataBuffer packet;
    packet << CMD_UPDATE_MAIN_SCREEN_CONTROL << uint32_t(monitor_index) << control;
    sendClientCommand(packet);

    if (control)
        main_screen_control |= (1 << monitor_index);
    else
        main_screen_control &=~(1 << monitor_index);
}

void PlayerInfo::commandSetName(const string& name)
{
    sp::io::DataBuffer packet;
    packet << CMD_UPDATE_NAME << name;
    sendClientCommand(packet);

    this->name = name;
}

void PlayerInfo::onReceiveClientCommand(int32_t client_id, sp::io::DataBuffer& packet)
{
    if (client_id != this->client_id) return;
    int16_t command;
    uint32_t monitor_index;
    bool active;
    packet >> command;
    switch(command)
    {
    case CMD_UPDATE_CREW_POSITION:
        {
            int32_t position;
            packet >> monitor_index >> position >> active;
            if (active)
                crew_position[position] |= (1 << monitor_index);
            else
                crew_position[position] &=~(1 << monitor_index);
        }
        break;
    case CMD_UPDATE_SHIP_ID:
        packet >> ship_id;
        break;
    case CMD_UPDATE_MAIN_SCREEN:
        packet >> monitor_index >> active;
        if (active)
            main_screen |= (1 << monitor_index);
        else
            main_screen &=~(1 << monitor_index);
        break;
    case CMD_UPDATE_MAIN_SCREEN_CONTROL:
        packet >> monitor_index >> active;
        if (active)
            main_screen_control |= (1 << monitor_index);
        else
            main_screen_control &=~(1 << monitor_index);
        break;
    case CMD_UPDATE_NAME:
        packet >> name;
        break;
    }
}

void PlayerInfo::spawnUI(int monitor_index, RenderLayer* render_layer)
{
    if (my_player_info->isOnlyMainScreen(monitor_index))
    {
        new ScreenMainScreen(render_layer);
    }
    else
    {
        CrewStationScreen* screen = new CrewStationScreen(render_layer, bool(main_screen & (1 << monitor_index)));
        auto container = screen->getTabContainer();

        //Crew 6/5
        if (crew_position[helmsOfficer] & (1 << monitor_index))
            screen->addStationTab(new HelmsScreen(container), helmsOfficer, getCrewPositionName(helmsOfficer), getCrewPositionIcon(helmsOfficer));
        if (crew_position[weaponsOfficer] & (1 << monitor_index))
            screen->addStationTab(new WeaponsScreen(container), weaponsOfficer, getCrewPositionName(weaponsOfficer), getCrewPositionIcon(weaponsOfficer));
        if (crew_position[engineering] & (1 << monitor_index))
            screen->addStationTab(new EngineeringScreen(container), engineering, getCrewPositionName(engineering), getCrewPositionIcon(engineering));
        if (crew_position[scienceOfficer] & (1 << monitor_index))
            screen->addStationTab(new ScienceScreen(container), scienceOfficer, getCrewPositionName(scienceOfficer), getCrewPositionIcon(scienceOfficer));
        if (crew_position[relayOfficer] & (1 << monitor_index))
            screen->addStationTab(new RelayScreen(container, true), relayOfficer, getCrewPositionName(relayOfficer), getCrewPositionIcon(relayOfficer));
        if (crew_position[cagOfficer] & (1 << monitor_index))
            screen->addStationTab(new CicScreen(container, true), cagOfficer, getCrewPositionName(cagOfficer), getCrewPositionIcon(cagOfficer));

        //Crew 4/3
        if (crew_position[tacticalOfficer] & (1 << monitor_index))
            screen->addStationTab(new TacticalScreen(container), tacticalOfficer, getCrewPositionName(tacticalOfficer), getCrewPositionIcon(tacticalOfficer));
        if (crew_position[engineeringAdvanced] & (1 << monitor_index))
            screen->addStationTab(new EngineeringAdvancedScreen(container), engineeringAdvanced, getCrewPositionName(engineeringAdvanced), getCrewPositionIcon(engineeringAdvanced));
        if (crew_position[operationsOfficer] & (1 << monitor_index))
            screen->addStationTab(new OperationScreen(container), operationsOfficer, getCrewPositionName(operationsOfficer), getCrewPositionIcon(operationsOfficer));

        //Crew 1
        if (crew_position[singlePilot] & (1 << monitor_index))
            screen->addStationTab(new SinglePilotScreen(container), singlePilot, getCrewPositionName(singlePilot), getCrewPositionIcon(singlePilot));

        //Extra
        //Postes
        if (crew_position[dronePilot] & (1 << monitor_index))
            screen->addStationTab(new DroneOperatorScreen(container), dronePilot, getCrewPositionName(dronePilot), getCrewPositionIcon(dronePilot));
        if (crew_position[powerManagement] & (1 << monitor_index))
            screen->addStationTab(new PowerManagementScreen(container), powerManagement, getCrewPositionName(powerManagement), getCrewPositionIcon(powerManagement));
        if (crew_position[droneMaster] & (1 << monitor_index))
            screen->addStationTab(new DroneMasterScreen(container), droneMaster, getCrewPositionName(droneMaster), getCrewPositionIcon(droneMaster));
        if (crew_position[dockMaster] & (1 << monitor_index))
            screen->addStationTab(new DockMasterScreen(container), dockMaster, getCrewPositionName(dockMaster), getCrewPositionIcon(dockMaster));
        if (crew_position[damageControl] & (1 << monitor_index))
            screen->addStationTab(new DamageControlScreen(container), damageControl, getCrewPositionName(damageControl), getCrewPositionIcon(damageControl));
        if (crew_position[databaseView] & (1 << monitor_index))
            screen->addStationTab(new DatabaseScreen(container), databaseView, getCrewPositionName(databaseView), getCrewPositionIcon(databaseView));
        //Fenetres
        if (crew_position[tacticalRadar] & (1 << monitor_index))
            screen->addStationTab(new RadarScreen(container,"tactical"), tacticalRadar, getCrewPositionName(tacticalRadar), getCrewPositionIcon(tacticalRadar));
        if (crew_position[scienceRadar] & (1 << monitor_index))
            screen->addStationTab(new RadarScreen(container,"science"), scienceRadar, getCrewPositionName(scienceRadar), getCrewPositionIcon(scienceRadar));
        if (crew_position[relayRadar] & (1 << monitor_index))
            screen->addStationTab(new RadarScreen(container,"relay"), relayRadar, getCrewPositionName(relayRadar), getCrewPositionIcon(relayRadar));
        if (crew_position[shipLog] & (1 << monitor_index))
            screen->addStationTab(new ShipLogScreen(container,"generic"), shipLog, getCrewPositionName(shipLog), getCrewPositionIcon(shipLog));
        if (crew_position[internLogView] & (1 << monitor_index))
            screen->addStationTab(new ShipLogScreen(container,"intern"), internLogView, getCrewPositionName(internLogView), getCrewPositionIcon(internLogView));
        if (crew_position[targetAnalysisScreen] & (1 << monitor_index))
            screen->addStationTab(new TargetAnalysisScreen(container), targetAnalysisScreen, getCrewPositionName(targetAnalysisScreen), getCrewPositionIcon(targetAnalysisScreen));
        
        //Non utilise
        if (crew_position[altRelay])
            screen->addStationTab(new RelayScreen(container, false), altRelay, getCrewPositionName(altRelay), getCrewPositionIcon(altRelay));
        if (crew_position[commsOnly] & (1 << monitor_index))
            screen->addStationTab(new CommsScreen(container), commsOnly, getCrewPositionName(commsOnly), getCrewPositionIcon(commsOnly));
        //if (crew_position[oxygenView] & (1 << monitor_index))
        //    screen->addStationTab(new OxygenScreen(container), oxygenView, getCrewPositionName(oxygenView), getCrewPositionIcon(oxygenView));
        
        
        
        //TODO tsht : verifier que c'est bien present avec le truc en bas
        //Ship log screen, if you have comms, you have ships log. (note this is mostly replaced by the [at the bottom of the screen openable log]
        //if (crew_position[singlePilot] & (1 << monitor_index))
		//{
        //    screen->addStationTab(new ShipLogScreen(screen,"generic"), max_crew_positions, "log externe", "");
        //    screen->addStationTab(new ShipLogScreen(screen,"intern"), max_crew_positions, "log interne", "");
        //}

        GuiSelfDestructEntry* sde = new GuiSelfDestructEntry(container, "SELF_DESTRUCT_ENTRY");
        for(int n=0; n<max_crew_positions; n++)
            if (crew_position[n] & (1 << monitor_index))
                sde->enablePosition(ECrewPosition(n));
        if (crew_position[tacticalOfficer] & (1 << monitor_index))
        {
            sde->enablePosition(weaponsOfficer);
            sde->enablePosition(helmsOfficer);
        }
        if (crew_position[engineeringAdvanced] & (1 << monitor_index))
        {
            sde->enablePosition(engineering);
        }
        if (crew_position[operationsOfficer] & (1 << monitor_index))
        {
            sde->enablePosition(scienceOfficer);
            sde->enablePosition(relayOfficer);
        }

        if (main_screen_control & (1 << monitor_index))
            new GuiMainScreenControls(container);

        screen->finishCreation();
    }
}

string getCrewPositionName(ECrewPosition position)
{
    switch(position)
    {
    case helmsOfficer: return tr("station","Helms");
    case weaponsOfficer: return tr("station","Weapons");
    case engineering: return tr("station","Engineering");
    case scienceOfficer: return tr("station","Science");
    case relayOfficer: return tr("station","Relay");
    case tacticalOfficer: return tr("station","Tactical");
    case engineeringAdvanced: return tr("station","Engineering+");
    case operationsOfficer: return tr("station","Operations");
    case singlePilot: return tr("station","Single Pilot");
    case damageControl: return tr("station","Damage Control");
    case powerManagement: return tr("station","Power Management");
    case databaseView: return tr("station","Database");
    case altRelay: return tr("station","Strategic Map");
    case commsOnly: return tr("station","Comms");
    case shipLog: return tr("station","Ship's Log");
    //TODO reverifier tout ca
    case tacticalRadar: return tr("station","Tactical Radar");
    case scienceRadar: return tr("station","Science Radar");
    case relayRadar: return tr("station","Relay Radar");
    case internLogView: return tr("station","Intern Log View");
    case dronePilot: return tr("station","Single Pilot");
    case droneMaster: return tr("station","Dock Master");
    case dockMaster: return tr("station","Flight Deck Master");
    //case oxygenView: return "Log Oxygen";
    case targetAnalysisScreen: return tr("station","Target Analysis Screen");
    case cagOfficer: return tr("station","CIC");
    default: return "ErrUnk: " + string(position);
    }
}

string getCrewPositionIcon(ECrewPosition position)
{
    switch(position)
    {
    case helmsOfficer: return "gui/icons/station-helm";
    case weaponsOfficer: return "gui/icons/station-weapons";
    case engineering: return "gui/icons/station-engineering";
    case scienceOfficer: return "gui/icons/station-science";
    case relayOfficer: return "gui/icons/station-relay";
    case tacticalOfficer: return "";
    case engineeringAdvanced: return "";
    case operationsOfficer: return "";
    case singlePilot: return "";
    case damageControl: return "";
    case powerManagement: return "";
    case databaseView: return "";
    case altRelay: return "";
    case commsOnly: return "";
    case shipLog: return "";
    case internLogView: return "";
    case dronePilot: return "";
    case droneMaster: return "";
    case dockMaster: return "";
    //case oxygenView: return "";
    //ajouts Tdelc
    case tacticalRadar: return "";
    case scienceRadar: return "";
    case relayRadar: return "";
    //ajouts Tdelc Larp
    case targetAnalysisScreen: return "";
    case cagOfficer: return "gui/icons/station-cic";
    default: return "ErrUnk: " + string(position);
    }
}

/* Define script conversion function for the ECrewPosition enum. */
template<> void convert<ECrewPosition>::param(lua_State* L, int& idx, ECrewPosition& cp)
{
    string str = string(luaL_checkstring(L, idx++)).lower();

    //6/5 player crew
    if (str == "helms" || str == "helmsofficer")
        cp = helmsOfficer;
    else if (str == "weapons" || str == "weaponsofficer")
        cp = weaponsOfficer;
    else if (str == "engineering" || str == "engineeringsofficer")
        cp = engineering;
    else if (str == "science" || str == "scienceofficer")
        cp = scienceOfficer;
    else if (str == "relay" || str == "relayofficer")
        cp = relayOfficer;
    else if (str == "cag" || str == "cic")
        cp = cagOfficer;

    //4/3 player crew
    else if (str == "tactical" || str == "tacticalofficer")
        cp = tacticalOfficer;    //helms+weapons-shields
    else if (str == "engineering+" || str == "engineering+officer" || str == "engineeringadvanced" || str == "engineeringadvancedofficer")
        cp = engineeringAdvanced;//engineering+shields
    else if (str == "operations" || str == "operationsofficer")
        cp = operationsOfficer; //science+comms

    //1 player crew
    else if (str == "single" || str == "singlepilot")
        cp = singlePilot;

    //extras
    else if (str == "damagecontrol")
        cp = damageControl;
    else if (str == "powermanagement")
        cp = powerManagement;
    else if (str == "database" || str == "databaseview")
        cp = databaseView;
    else if (str == "altrelay")
        cp = altRelay;
    else if (str == "commsonly")
        cp = commsOnly;
    else if (str == "shiplog")
        cp = shipLog;
    //Ajouts Tdelc
    else if (str == "internlog" || str == "internlogview")
        cp = internLogView;
    else if (str == "dronepilot" || str == "dronepilotview")
        cp = dronePilot;
    else if (str == "dronemaster" || str == "dronemasterview")
        cp = droneMaster;
    else if (str == "dockmaster" || str == "dockmasterview")
        cp = dockMaster;
    //else if (str == "oxygen" || str == "oxygenview")
    //    cp = oxygenView;
    else if (str == "tacticalradar" || str == "tacticalradarview")
        cp = tacticalRadar;
    else if (str == "scienceradar" || str == "scienceradarview")
        cp = scienceRadar;
    else if (str == "relayradar" || str == "relayradarview")
        cp = relayRadar;
    else if (str == "log" || str == "shiplog")
        cp = shipLog;
    else if (str == "targetanalysis" || str == "analysis" || str == "targetanalysisscreen")
        cp = targetAnalysisScreen;
    else
        luaL_error(L, "Unknown value for crew position: %s", str.c_str());
}
