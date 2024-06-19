#include "playerSpaceship.h"
#include "gui/colorConfig.h"
#include "repairCrew.h"
#include "explosionEffect.h"
#include "gameGlobalInfo.h"
#include "main.h"
#include "preferenceManager.h"
#include "soundManager.h"
#include "random.h"

#include "scriptInterface.h"

#include <SDL_assert.h>

/// A PlayerSpaceship is a SpaceShip controlled by a player crew.
/// If a function name begins with "command", the function is equivalent to the crew taking a corresponding action.
/// Such commands can be limited by the ship's capabilities, including systems damage, lack of power, or insufficient weapons stocks.
REGISTER_SCRIPT_SUBCLASS(PlayerSpaceship, SpaceShip)
{
    /// Returns the coordinates of a waypoint with the given index that's been set by this PlayerSpaceship.
    /// Waypoints are 1-indexed.
    /// Example: x,y = player:getWaypoint(1)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getWaypoint);
    /// Returns the total number of active waypoints owned by this PlayerSpaceship.
    /// Example: player:getWaypointCount()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getWaypointCount);
    /// Returns this PlayerSpaceship's EAlertLevel.
    /// Returns "Normal", "YELLOW ALERT", "RED ALERT", which differ from the valid values for PlayerSpaceship:commandSetAlertLevel().
    /// Example: player:getAlertLevel()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getAlertLevel);
    /// Defines whether this PlayerSpaceship's shields are raised (true) or lowered (false).
    /// Compare to CpuShips, whose shields are always active.
    /// Example: player:setShieldsActive(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setShieldsActive);
    /// Adds a message to this PlayerSpaceship's log.
    /// Takes a string as the message and a color applied to the logged message.
    /// Example: player:addToShipLog("Acknowledged","yellow") -- adds "Acknowledged" in yellow to the `player` ship's log
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addToShipLog);
    /// Moves all players connected to this ship to the same crew positions on another PlayerSpaceship.
    /// If the target isn't a PlayerSpaceship, this function has no effect.
    /// Use this in scenarios to change the crew's ship.
    /// Example: player:transferPlayersToShip(player2) -- transfer all player crew to `player2`
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, transferPlayersToShip);
    /// Transfers only the crew members on a specific crew position to another PlayerSpaceship.
    /// If a player is in multiple positions, this matches any of their positions and moves that player to all of the same positions on the destination ship.
    /// Example: player:transferPlayersAtPositionToShip("helms",player2) -- transfer all crew on Helms to `player2`
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, transferPlayersAtPositionToShip);
    /// Returns whether a player occupies the given crew position on this PlayerSpaceship.
    /// Example: player:hasPlayerAtPosition("helms")
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, hasPlayerAtPosition);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setTexture);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setTextureColor);

    /// Returns whether this PlayerSpaceship's comms are not in use.
    /// Use this to determine whether the player can accept an incoming hail or chat.
    /// Example: player:isCommsInactive()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsInactive);
    /// Returns whether this PlayerSpaceship is opening comms with another SpaceObject.
    /// Example: player:isCommsOpening()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsOpening);
    /// Returns whether this PlayerSpaceship is being hailed by another SpaceObject.
    /// Example: player:isCommsBeingHailed()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsBeingHailed);
    /// Returns whether this PlayerSpaceship is being hailed by the GM.
    /// Example: player:isCommsBeingHailedByGM()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsBeingHailedByGM);
    /// Returns whether comms to this PlayerSpaceship have failed to open.
    /// Example: player:isCommsFailed()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsFailed);
    /// Returns whether comms to this PlayerSpaceship were broken off by the other SpaceObject.
    /// Example: player:isCommsBroken()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsBroken);
    /// Returns whether comms between this PlayerSpaceship and a SpaceObject were intentionally closed.
    /// Example: player:isCommsClosed()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsClosed);
    /// Returns whether this PlayerSpaceship is engaged in text chat with either the GM or another PlayerSpaceship.
    /// Example: player:isCommsChatOpen()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsChatOpen);
    /// Returns whether this PlayerSpaceship is engaged in text chat with the GM.
    /// Example: player:isCommsChatOpenToGM()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsChatOpenToGM);
    /// Returns whether this PlayerSpaceship is engaged in text chat with another PlayerSpaceship.
    /// Example: player:isCommsChatOpenToPlayer()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsChatOpenToPlayer);
    /// Returns whether this PlayerSpaceship is engaged in comms with a scripted SpaceObject.
    /// Example: player:isCommsScriptOpen()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsScriptOpen);

    /// Sets this PlayerSpaceship's energy level.
    /// Values are limited from 0 to the energy level max. Negative or excess values are capped to the limits.
    /// Example: player:setEnergyLevel(1000) -- sets the ship's energy to either 1000 or the max limit, whichever is lower
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setEnergyLevel);
    /// Sets this PlayerSpaceship's energy capacity.
    /// Valid values are 0 or any positive number.
    /// If the new limit is lower than the ship's current energy level, this also reduces the energy level.
    /// Example: player:setEnergyLevelMax(1000) -- sets the ship's energy limit to 1000
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setEnergyLevelMax);
    /// Returns this PlayerSpaceship's energy level.
    /// Example: player:getEnergyLevel()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getEnergyLevel);
    /// Returns this PlayerSpaceship's energy capacity.
    /// Example: player:getEnergyLevelMax()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getEnergyLevelMax);

    /// Returns how much energy is consumed per second by this PlayerSpaceship's shields while active.
    /// Example: player:getEnergyShieldUsePerSecond()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getEnergyShieldUsePerSecond);
    /// Sets how much energy is consumed per second by this PlayerSpaceship's shields while active.
    /// Example: player:setEnergyShieldUsePerSecond(1.5)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setEnergyShieldUsePerSecond);
    /// Returns how much energy is consumed per second by this PlayerSpaceship's warp drive while in use.
    /// Example: player:getEnergyWarpPerSecond()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getEnergyWarpPerSecond);
    /// Sets how much energy is consumed per second by this PlayerSpaceship's warp drive while in use.
    /// Example: player:setEnergyWarpPerSecond(1.7)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setEnergyWarpPerSecond);

    /// Sets the maximum amount of coolant available to engineering on this PlayerSpaceship.
    /// Defaults to 10, which by default allows engineering to set 100% coolant on one system.
    /// Valid values are 0 or any positive number.
    /// If the new limit is less than the coolant already distributed, this automatically reduces distribution percentages.
    /// Example: player:setMaxCoolant(5) -- halves the amount of available coolant
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setMaxCoolant);
    /// Returns the maximum amount of coolant available to engineering on this PlayerSpaceship.
    /// Example: player:getMaxCoolant()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getMaxCoolant);
    /// Set the maximum coolant availbale for each system. Default is 10
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setMaxCoolantPerSystem);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getMaxCoolantPerSystem);

    /// Sets the number of scan probes stocked by this PlayerSpaceship.
    /// Values are limited from 0 to the scan probe count max. Negative or excess values are capped to the limits.
    /// Example: player:setScanProbeCount(20) -- sets the ship's scan probes to either 20 or the max limit, whichever is fewer
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setScanProbeCount);
    /// Returns the number of scan probes stocked by this PlayerSpaceship.
    /// Example: player:getScanProbeCount()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getScanProbeCount);
    /// Sets this PlayerSpaceship's capacity for scan probes.
    /// Valid values are 0 or any positive number.
    /// If the new limit is less than the current scan probe stock, this automatically reduces the stock.
    /// Example: player:setMaxScanProbeCount(30) -- sets the ship's scan probe capacity to 30
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setMaxScanProbeCount);
    /// Returns this PlayerSpaceship's capacity for scan probes.
    /// Example: player:getMaxScanProbeCount()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getMaxScanProbeCount);

    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, hasGravitySensor);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setGravitySensor);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, hasElectricalSensor);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setElectricalSensor);
	REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, hasBiologicalSensor);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setBiologicalSensor);
    /// Adds a custom interactive button with the given reference name to the given crew position screen.
    /// By default, custom buttons and info are stacked in order of creation. Use the order value to specify a priority, with lower values appearing higher in the list.
    /// If the reference name is unique, this creates a new button. If the reference name exists, this modifies the existing button.
    /// The caption sets the button's text label.
    /// When clicked, the button calls the given function.
    /// Example:
    /// -- Add a custom button to Engineering, lower in the order relative to other items, that prints the player ship's coolant max to the console or logging file when clicked
    /// player:addCustomButton("engineering","get_coolant_max","Get Coolant Max",function() print("Coolant: " .. player:getMaxCoolant()) end,10)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addCustomButton);
    /// Adds a custom non-interactive info label with the given reference name to the given crew position screen.
    /// By default, custom buttons and info are stacked in order of creation. Use the order value to specify a priority.
    /// If the reference name is unique, this creates a new info. If the reference name exists, this modifies the existing info.
    /// The caption sets the info's text value.
    /// Example:
    /// -- Displays the coolant max value on Engineering at or near the top of the custom button/info order
    /// player:addCustomInfo("engineering","show_coolant_max","Coolant Max: " .. player:getMaxCoolant(),0)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addCustomInfo);
    /// Displays a dismissable message with the given reference name on the given crew position screen.
    /// The caption sets the message's text.
    /// Example:
    /// -- Displays the coolant max value on Engineering as a dismissable message
    /// player:addCustomMessage("engineering","message_coolant_max","Coolant max: " .. player:getMaxCoolant())
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addCustomMessage);
    /// As PlayerSpaceship:addCustomMessage(), but calls the given function when dismissed.
    /// Example:
    /// -- Displays the coolant max value on Engineering as a dismissable message, and prints "dismissed" to the console or logging file when dismissed
    /// player:addCustomMessageWithCallback("engineering","message_coolant_max","Coolant max: " .. player:getMaxCoolant(),function() print("Dismissed!") end)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addCustomMessageWithCallback);
    /// Removes the custom function, info, or message with the given reference name.
    /// Example: player:removeCustom("show_coolant_max") -- removes the custom item named "show_coolant_max"
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, removeCustom);

    /// Returns the index of the ESystem targeted by this PlayerSpaceship's weapons.
    /// Returns -1 for the hull.
    /// Example: player:getBeamSystemTarget()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getBeamSystemTarget);
    /// Returns the name of the ESystem targeted by this PlayerSpaceship's weapons.
    /// Returns "UNKNOWN" for the hull.
    /// Example: player:getBeamSystemTargetName()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getBeamSystemTargetName);

    /// Commands this PlayerSpaceship to set a new target rotation.
    /// A value of 0 is equivalent to a heading of 90 degrees ("east").
    /// Accepts 0, positive, or negative values.
    /// To objectively rotate the PlayerSpaceship as a SpaceObject, rather than commanding it to turn using its maneuverability, use SpaceObject:setRotation().
    /// Examples:
    /// player:commandTargetRotation(0) -- command the ship toward a heading of 90 degrees
    /// heading = 180; player:commandTargetRotation(heading - 90) -- command the ship toward a heading of 180 degrees
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandTargetRotation);
    /// Commands this PlayerSpaceship to request a new impulse speed.
    /// Valid values are -1.0 (-100%; full reverse) to 1.0 (100%; full forward).
    /// The ship's impulse value remains bound by its impulse acceleration rates.
    /// Example: player:commandImpulse(0.5) -- command this ship to engage forward half impulse
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandImpulse);
    /// Commands this PlayerSpaceship to request a new warp level.
    /// Valid values are any positive integer, or 0.
    /// Warp controls on crew position screens are limited to 4.
    /// Example: player:commandWarp(2) -- activate the warp drive at level 2
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandWarp);
    /// Commands this PlayerSpaceship to request a jump of the given distance.
    /// Valid values are any positive number, or 0, including values outside of the ship's minimum and maximum jump ranges.
    /// A jump of a greater distance than the ship's maximum jump range results in a negative jump drive charge.
    /// Example: player:commandJump(25000) -- initiate a 25U jump on the current heading
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandJump);
    /// Commands this PlayerSpaceship to set its weapons target to the given SpaceObject.
    /// Example: player:commandSetTarget(enemy)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetTarget);
    /// Commands this PlayerSpaceship to load the WeaponTube with the given index with the given weapon type.
    /// This command respects tube allow/disallow limits.
    /// Example: player:commandLoadTube(0,"HVLI")
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandLoadTube);
    /// Commands this PlayerSpaceship to unload the WeaponTube with the given index.
    /// Example: player:commandUnloadTube(0)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandUnloadTube);
    /// Commands this PlayerSpaceship to fire the WeaponTube with the given index at the given missile target angle in degrees, without a weapons target.
    /// The target angle behaves as if the Weapons crew had unlocked targeting and manually aimed its trajectory.
    /// A target angle value of 0 is equivalent to a heading of 90 degrees ("east").
    /// Accepts 0, positive, or negative values.
    /// Examples:
    /// player:commandFireTube(0,0) -- command firing tube 0 at a heading 90
    /// target_heading = 180; player:commandFireTube(0,target_heading - 90) -- command firing tube 0 at a heading 180
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandFireTube);
    /// Commands this PlayerSpaceship to fire the given weapons tube with the given SpaceObject as its target.
    /// Example: player:commandFireTubeAtTarget(0,enemy) -- command firing tube 0 at target `enemy`
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandFireTubeAtTarget);
    /// Commands this PlayerSpaceship to raise (true) or lower (false) its shields.
    /// Example: player:commandSetShields(true) -- command raising shields
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetShields);
    /// Commands this PlayerSpaceship to change its Main Screen view to the given setting.
    /// Example: player:commandMainScreenSetting("tactical") -- command setting the main screen view to tactical radar
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandMainScreenSetting);
    /// Commands this PlayerSpaceship to change its Main Screen comms overlay to the given setting.
    /// Example: player:commandMainScreenOverlay("hidecomms") -- command setting the main screen view to hide the comms overlay
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandMainScreenOverlay);
    /// Commands this PlayerSpaceship to initiate a scan of the given SpaceObject.
    /// If the scanning mini-game is enabled, this opens it on the relevant crew screens.
    /// This command does NOT respect the player's ability to select the object for scanning, whether due to it being out of radar range or otherwise untargetable.
    /// Example: player:commandScan(enemy)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandScan);
    /// Commands this PlayerSpaceship to set the power level of the given system.
    /// Valid values are 0 or greater, with 1.0 equivalent to 100 percent. Values greater than 1.0 are allowed.
    /// Example: player:commandSetSystemPowerRequest("impulse",1.0) -- command setting the impulse drive power to 100%
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetSystemPowerRequest);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetSystemPowerPreset);
    /// Commands this PlayerSpaceship to set the coolant level of the given system.
    /// Valid values are from 0 to 10.0, with 10.0 equivalent to 100 percent.
    /// Values greater than 10.0 are allowed if the ship's coolant max is greater than 10.0, but controls on crew position screens are limited to 10.0 (100%).
    /// Example: player:commandSetSystemCoolantRequest("impulse",10.0) -- command setting the impulse drive coolant to 100%
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetSystemCoolantRequest);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetSystemCoolantPreset);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetSystemRepairRequest);
    /// Commands this PlayerSpaceship to initiate docking with the given SpaceObject.
    /// This initiates docking only if the target is dockable and within docking range.
    /// Example: player:commandDock(base)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandDock);
    /// Commands this PlayerSpaceship to undock from any SpaceObject it's docked with.
    /// Example: player:commandUndock()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandUndock);
    /// Commands this PlayerSpaceship to abort an in-progress docking operation.
    /// Example: player:commandAbortDock()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandAbortDock);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandLand);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandAbortLanding);
    /// Commands this PlayerSpaceship to hail the given SpaceObject.
    /// If the target object is a PlayerSpaceship or the GM is intercepting all comms, open text chat comms.
    /// Example: player:commandOpenTextComm(base)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandOpenTextComm);
    /// Commands this PlayerSpaceship to close comms.
    /// Example: player:commandCloseTextComm()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandCloseTextComm);
    /// Commands whether this PlayerSpaceship answers (true) or rejects (false) an incoming hail.
    /// Example: player:commandAnswerCommHail(false) -- commands to reject an active incoming hail
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandAnswerCommHail);
    /// Commands this PlayerSpaceship to select the reply with the given index during a comms dialogue.
    /// Example: player:commandSendComm(0) -- commands to select the first option in a comms dialogue
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSendComm);
    /// Commands this PlayerSpaceship to send the given message to the active text comms chat.
    /// This works whether the chat is with another PlayerSpaceship or the GM.
    /// Example: player:commandSendCommPlayer("I will destroy you!") -- commands to send this message in the active text chat
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSendCommPlayer);
    /// Commands whether repair crews on this PlayerSpaceship automatically move to rooms of damaged systems.
    /// Use this command to reduce the need for player interaction in Engineering, especially when combined with setAutoCoolant/auto_coolant_enabled.
    /// Crews set to move automatically don't respect crew collisions, allowing multiple crew to occupy a single space.
    /// Example: player:commandSetAutoRepair(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetAutoRepair);
    /// Commands this PlayerSpaceship to set its beam frequency to the given value.
    /// Valid values are 0 to 20, which map to 400THz to 800THz at 20THz increments. (spaceship.cpp frequencyToString())
    /// Example: player:commandSetAutoRepair(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetBeamFrequency);
    /// Commands this PlayerSpaceship to target the given ship system with its beam weapons.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetBeamSystemTarget);
    /// Sets this SpaceShip's shield frequency index.
    /// To convert the index to the value used by players, multiply it by 20, then add 400.
    /// Valid values are 0 (400THz) to 20 (800THz).
    /// Unlike SpaceShip:setShieldsFrequency(), this initiates shield calibration to change the frequency, which disables shields for a period.
    /// Example:
    /// frequency = ship:setShieldsFrequency(10) -- frequency is 600THz
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetShieldFrequency);
    /// Commands this PlayerSpaceship to add a waypoint at the given coordinates.
    /// This respects the 9-waypoint limit and won't add more waypoints if 9 already exist.
    /// Example: player:commandAddWaypoint(1000,2000)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandAddWaypoint);
    /// Commands this PlayerSpaceship to remove the waypoint with the given index.
    /// This uses a 0-index, while waypoints are numbered on player screens with a 1-index.
    /// Example: player:commandRemoveWaypoint(0) -- removes waypoint 1
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandRemoveWaypoint);
    /// Commands this PlayerSpaceship to move the waypoint with the given index to the given coordinates.
    /// This uses a 0-index, while waypoints are numbered on player screens with a 1-index.
    /// Example: player:commandMoveWaypoint(0,-1000,-2000) -- moves waypoint 1 to -1000,-2000
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandMoveWaypoint);
    /// Commands this PlayerSpaceship to activate its self-destruct sequence.
    /// Example: player:commandActivateSelfDestruct()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandActivateSelfDestruct);
    /// Commands this PlayerSpaceship to cancel its self-destruct sequence.
    /// Example: player:commandCancelSelfDestruct()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandCancelSelfDestruct);
    /// Commands this PlayerSpaceship to submit the given self-destruct authorization code for the code request with the given index.
    /// Codes are 0-indexed. Index 0 corresponds to code A, 1 to B, etc.
    /// Example: player:commandConfirmDestructCode(0,46223) -- commands submitting 46223 as self-destruct confirmation code A
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandConfirmDestructCode);
    /// Commands this PlayerSpaceship to set its forward combat maneuver to the given value.
    /// Valid values are any from -1.0 (full reverse) to 1.0 (full forward).
    /// The maneuver continues until the ship's combat maneuver reserves are depleted.
    /// Crew screens allow only forward combat maneuvers, and the combat maneuver controls do not reflect a boost set via this command.
    /// Example: player:commandCombatManeuverBoost(0.5) -- commands boosting forward at half combat maneuver capacity
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandCombatManeuverBoost);
    /// Commands this PlayerSpaceship to launch a ScanProbe to the given coordinates.
    /// Example: player:commandLaunchProbe(1000,2000) -- commands launching a scan probe to 1000,2000
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandLaunchProbe);
    /// Commands this PlayerSpaceship to link the science screen to the given ScanProbe.
    /// This is equivalent to selecting a probe on Relay and clicking "Link to Science".
    /// Unlike "Link to Science", this function can link science to any given probe, regardless of which ship launched it or what faction it belongs to.
    /// Example: player:commandSetScienceLink(probe_object) -- link ScanProbe `probe` to this ship's science
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetScienceLink);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetProbe3DLink);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetAnalysisLink);
    /// Commands this PlayerSpaceship to unlink the science screen from any ScanProbe.
    /// This is equivalent to clicking "Link to Science" on Relay when a link is already active.
    /// Example: player:commandClearScienceLink()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandClearScienceLink);
    /// Commands this PlayerSpaceship to set the given alert level.
    /// Valid values are "normal", "yellow", "red", which differ from the values returned by PlayerSpaceship:getAlertLevel().
    /// Example: player:commandSetAlertLevel("red") -- commands red alert
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetAlertLevel);

    /// Returns the number of repair crews on this PlayerSpaceship.
    /// Example: player:getRepairCrewCount()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getRepairCrewCount);
    /// Sets the total number of repair crews on this PlayerSpaceship.
    /// If the value is less than the number of repair crews, this function removes repair crews.
    /// If the value is greater, this function adds new repair crews into random rooms.
    /// Example: player:setRepairCrewCount(5)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setRepairCrewCount);
    /// Set the maximum nano repair crew available to engineering (per system). Default is 3.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setMaxRepairPerSystem);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getMaxRepairPerSystem);
    /// Set the maximum nano repair crew available to engineering (global). Default is 3.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setMaxRepair);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getMaxRepair);
    /// Set the maximum number of presets available for the engineering screen
    /// Must be between 0 and 9
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setActivePresetNumber);
    /// Get the actual maximum number of presets for the engineering screen
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getActivePresetNumber);    
    /// Defines whether automatic coolant distribution is enabled on this PlayerSpaceship.
    /// If true, coolant is automatically distributed proportionally to the amount of heat in that system.
    /// Use this command to reduce the need for player interaction in Engineering, especially when combined with commandSetAutoRepair/auto_repair_enabled.
    /// Example: player:setAutoCoolant(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setAutoCoolant);
    /// Sets a control code password required for a player to join this PlayerSpaceship.
    /// Control codes are case-insensitive.
    /// Example: player:setControlCode("abcde") -- matches "abcde", "ABCDE", "aBcDe"
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setControlCode);
    /// Apply a rate to energy decrease. Float, default is 1. Won't affect production of energy.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setEnergyConsumptionRatio);
    /// Defines a function to call when this PlayerSpaceship launches a probe.
    /// Passes the launching PlayerSpaceship and launched ScanProbe.
    /// Example:
    /// -- Prints probe launch details to the console output or logging file
    /// player:onProbeLaunch(function (player, probe)
    ///     print("Probe " .. probe:getCallSign() .. " launched from ship " .. player:getCallSign())
    /// end)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, onProbeLaunch);
    /// Defines a function to call when this PlayerSpaceship links a probe to the science screen.
    /// Passes the PlayerShip and linked ScanProbe.
    /// Example:
    /// -- Prints probe linking details to the console output or logging file
    /// player:onProbeLink(function (player, probe)
    ///     print("Probe " .. probe:getCallSign() .. " linked to Science on ship " .. player:getCallSign())
    /// end)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, onProbeLink);
    /// Defines a function to call when this PlayerSpaceship unlinks a probe from the science screen.
    /// Passes the PlayerShip and previously linked ScanProbe.
    /// This function is not called when the probe is destroyed or expires.
    /// See ScanProbe:onDestruction() and ScanProbe:onExpiration().
    /// Example:
    /// -- Prints probe unlinking details to the console output or logging file
    /// player:onProbeUnlink(function (player, probe)
    ///     print("Probe " .. probe:getCallSign() .. " unlinked from Science on ship " .. player:getCallSign())
    /// end)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, onProbeUnlink);
    /// Returns this PlayerSpaceship's long-range radar range.
    /// Example: player:getLongRangeRadarRange()
    REGISTER_SCRIPT_CLASS_FUNCTION(ShipTemplateBasedObject, getLongRangeRadarRange);
    /// Returns this PlayerSpaceship's short-range radar range.
    /// Example: player:getShortRangeRadarRange()
    REGISTER_SCRIPT_CLASS_FUNCTION(ShipTemplateBasedObject, getShortRangeRadarRange);
    /// Sets this PlayerSpaceship's long-range radar range.
    /// PlayerSpaceships use this range on the science and operations screens' radar.
    /// Example: player:setLongRangeRadarRange(30000) -- sets the ship's long-range radar range to 30U
    REGISTER_SCRIPT_CLASS_FUNCTION(ShipTemplateBasedObject, setLongRangeRadarRange);
    /// Sets this PlayerSpaceship's short-range radar range.
    /// PlayerSpaceships use this range on the helms, weapons, and single pilot screens' radar.
    /// This also defines the shared radar radius on the relay screen for friendly ships and stations, and how far into nebulae that this SpaceShip can detect objects.
    /// Example: player:setShortRangeRadarRange(5000) -- sets the ship's long-range radar range to 5U
    REGISTER_SCRIPT_CLASS_FUNCTION(ShipTemplateBasedObject, setShortRangeRadarRange);
    /// Defines whether scanning features appear on related crew screens in this PlayerSpaceship.
    /// Example: player:setCanScan(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setCanScan);
    /// Returns whether scanning features appear on related crew screens in this PlayerSpaceship.
    /// Example: player:getCanScan()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getCanScan);
    /// Defines whether hacking features appear on related crew screens in thisPlayerSpaceship.
    /// Example: player:setCanHack(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setCanHack);
    /// Returns whether hacking features appear on related crew screens in this PlayerSpaceship.
    /// Example: player:getCanHack()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getCanHack);
    /// Defines whether the "Request Docking" button appears on related crew screens in this PlayerSpaceship.
    /// This doesn't override any docking class restrictions set on a target SpaceShip.
    /// Example: player:setCanDock(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setCanDock);
    /// Returns whether the "Request Docking" button appears on related crew screens in this PlayerSpaceship.
    /// Example: player:getCanDock()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getCanDock);
    /// Defines whether combat maneuver controls appear on related crew screens in this PlayerSpaceship.
    /// Example: player:setCanCombatManeuver(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setCanCombatManeuver);
    /// Returns whether combat maneuver controls appear on related crew screens in this PlayerSpaceship.
    /// Example: player:getCanCombatManeuver()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getCanCombatManeuver);
    /// Defines whether ScanProbe-launching controls appear on related crew screens in this PlayerSpaceship.
    /// Example: player:setCanLaunchProbe(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setCanLaunchProbe);
    /// Returns whether ScanProbe-launching controls appear on related crew screens in this PlayerSpaceship.
    /// Example: player:getCanLaunchProbe()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getCanLaunchProbe);
    /// Defines whether self-destruct controls appear on related crew screens in this PlayerSpaceship.
    /// Example: player:setCanSelfDestruct(true)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setCanSelfDestruct);
    /// Returns whether self-destruct controls appear on related crew screens in this PlayerSpaceship.
    /// This returns false if this ship's self-destruct size and damage are both 0, even if you set setCanSelfDestruct(true).
    /// Example: player:getCanSelfDestruct()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getCanSelfDestruct);
    /// Sets the amount of damage done to nearby SpaceObjects when this PlayerSpaceship self-destructs.
    /// Any given value is randomized +/- 33 percent upon self-destruction.
    /// Example: player:setSelfDestructDamage(150)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setSelfDestructDamage);
    /// Returns the amount of base damage done to nearby SpaceObjects when this PlayerSpaceship self-destructs.
    /// Example: player:getSelfDestructDamage()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getSelfDestructDamage);
    /// Sets the radius of the explosion created when this PlayerSpaceship self-destructs.
    /// All SpaceObjects within this radius are dealt damage upon self-destruction.
    /// Example: player:setSelfDestructSize(1500) -- sets a 1.5U self-destruction explosion and damage radius
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setSelfDestructSize);
    /// Returns the radius of the explosion created when this PlayerSpaceship self-destructs.
    /// All SpaceObjects within this radius are dealt damage upon self-destruction.
    /// Example: ship:getSelfDestructSize()
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getSelfDestructSize);

    ///Registers the modifier with the provided category, name and infos
    ///Example : ship:registerModifier("Skill, "Name of my comp", "Upgrades hull (Engineer)")
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, registerModifier);
    ///Registers a callback when tweak button is called for a particular modifier
    ///Returns the name of the modifier, and "activate" to tell the action is to activate the modifier,
    ///deactivate if the action is to deactivate the modifier
    ///three arguments callback : self (current ship), name, desired state (activate or deactivate)
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, onModifierToggle);
    ///Checks if modifier is activated or not, on this ship.
    ///Arguments is name for the modifier
    ///Returns true or false
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isModifierActivated);

    ///Sets maximum number of simultaneous controllable squadrons
    ///If already beyond this number, current squadrons will not be destroyed
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setMaxSquadrons);

    ///Creates a squadron ships, needs a name and a type, if under max squadrons number.
    ///First argument is its name/identifier ("Rogue Squadron 42")
    ///Second mandatory argument is it composition identifier ("Interceptors")
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, instantiateSquadron);
    
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getSquadronLaunchDuration);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setSquadronLaunchDuration);

    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isInSquadron);

    ///Destroys squadron specifiing its name/identifier
    //REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, destroySquadron);

    ///Launch a squadron specifying its name
    //REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, launchSquadron);
    ///Recalls a squadron specifying its name. 
    ///If not at range, launch an order to go to based ship
    ///If at range, land the squadron.
    //REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, recallSquadron);

}

static const int16_t CMD_TARGET_ROTATION = 0x0001;
static const int16_t CMD_IMPULSE = 0x0002;
static const int16_t CMD_WARP = 0x0003;
static const int16_t CMD_JUMP = 0x0004;
static const int16_t CMD_SET_TARGET = 0x0005;
static const int16_t CMD_LOAD_TUBE = 0x0006;
static const int16_t CMD_UNLOAD_TUBE = 0x0007;
static const int16_t CMD_FIRE_TUBE = 0x0008;
static const int16_t CMD_SET_SHIELDS = 0x0009;
static const int16_t CMD_SET_MAIN_SCREEN_SETTING = 0x000A; // Overlay is 0x0027
static const int16_t CMD_SCAN_OBJECT = 0x000B;
static const int16_t CMD_SCAN_DONE = 0x000C;
static const int16_t CMD_SCAN_CANCEL = 0x000D;
static const int16_t CMD_SET_SYSTEM_POWER_REQUEST = 0x000E;
static const int16_t CMD_SET_SYSTEM_COOLANT_REQUEST = 0x000F;
static const int16_t CMD_DOCK = 0x0010;
static const int16_t CMD_UNDOCK = 0x0011;
static const int16_t CMD_OPEN_TEXT_COMM = 0x0012; //TEXT communication
static const int16_t CMD_CLOSE_TEXT_COMM = 0x0013;
static const int16_t CMD_SEND_TEXT_COMM = 0x0014;
static const int16_t CMD_SEND_TEXT_COMM_PLAYER = 0x0015;
static const int16_t CMD_ANSWER_COMM_HAIL = 0x0016;
static const int16_t CMD_SET_AUTO_REPAIR = 0x0017;
static const int16_t CMD_SET_BEAM_FREQUENCY = 0x0018;
static const int16_t CMD_SET_BEAM_SYSTEM_TARGET = 0x0019;
static const int16_t CMD_SET_SHIELD_FREQUENCY = 0x001A;
static const int16_t CMD_ADD_WAYPOINT = 0x001B;
static const int16_t CMD_REMOVE_WAYPOINT = 0x001C;
static const int16_t CMD_MOVE_WAYPOINT = 0x001D;
static const int16_t CMD_ACTIVATE_SELF_DESTRUCT = 0x001E;
static const int16_t CMD_CANCEL_SELF_DESTRUCT = 0x001F;
static const int16_t CMD_CONFIRM_SELF_DESTRUCT = 0x0020;
static const int16_t CMD_COMBAT_MANEUVER_BOOST = 0x0021;
static const int16_t CMD_COMBAT_MANEUVER_STRAFE = 0x0022;
static const int16_t CMD_LAUNCH_PROBE = 0x0023;
static const int16_t CMD_SET_ALERT_LEVEL = 0x0024;
static const int16_t CMD_SET_SCIENCE_LINK = 0x0025;
static const int16_t CMD_SET_PROBE_3D_LINK = 0x0026;
static const int16_t CMD_ABORT_DOCK = 0x0027;
static const int16_t CMD_SET_MAIN_SCREEN_OVERLAY = 0x0028;
static const int16_t CMD_HACKING_FINISHED = 0x0029;
static const int16_t CMD_CUSTOM_FUNCTION = 0x0030;
static const int16_t CMD_LAUNCH_CARGO = 0x0031;
static const int16_t CMD_MOVE_CARGO = 0x0032;
static const int16_t CMD_CANCEL_MOVE_CARGO = 0x0033;
static const int16_t CMD_SET_DOCK_MOVE_TARGET = 0x0034;
static const int16_t CMD_SET_DOCK_ENERGY_REQUEST = 0x0035;
static const int16_t CMD_SET_AUTO_REPAIR_SYSTEM_TARGET = 0x0036;
static const int16_t CMD_SET_DOCK_TARGET = 0x0037;
static const int16_t CMD_LAND = 0x0038;
static const int16_t CMD_ABORT_LANDING = 0x0039;
static const int16_t CMD_SET_LANDING_TARGET = 0x0040;
static const int16_t CMD_TURN_SPEED = 0x002A;
static const int16_t CMD_SET_SYSTEM_REPAIR_REQUEST = 0x002B;
static const int16_t CMD_SET_ANALYSIS_LINK = 0x003A;
static const int16_t CMD_SET_SYSTEM_POWER_PRESET = 0x003B;
static const int16_t CMD_SET_SYSTEM_COOLANT_PRESET = 0x003C;
static const int16_t CMD_LAUNCH_SQUADRON = 0x003D;
static const int16_t CMD_SQUADRON_BLUEPRINT_ACTIVATION = 0x003E;
static const int16_t CMD_AMMO_BLUEPRINT_ACTIVATION = 0x003F;
static const int16_t CMD_ORDER_SQUADRON_TARGET = 0x0041;
static const int16_t CMD_ORDER_SQUADRON_POSITION = 0x0042;

string alertLevelToString(EAlertLevel level)
{
    // Convert an EAlertLevel to a string.
    switch(level)
    {
    case AL_RedAlert: return "RED ALERT";
    case AL_YellowAlert: return "YELLOW ALERT";
    case AL_Normal: return "Normal";
    default:
        return "???";
    }
}

string alertLevelToLocaleString(EAlertLevel level)
{
    // Convert an EAlertLevel to a translated string.
    switch(level)
    {
    case AL_RedAlert: return tr("alert","RED ALERT");
    case AL_YellowAlert: return tr("alert","YELLOW ALERT");
    case AL_Normal: return tr("alert","Normal");
    default:
        return "???";
    }
}

// Configure ship's log packets.
static inline sp::io::DataBuffer& operator << (sp::io::DataBuffer& packet, const PlayerSpaceship::ShipLogEntry& e) { return packet << e.prefix << e.text << e.color.r << e.color.g << e.color.b << e.color.a  << e.station << e.seq; }
static inline sp::io::DataBuffer& operator >> (sp::io::DataBuffer& packet, PlayerSpaceship::ShipLogEntry& e) { packet >> e.prefix >> e.text >> e.color.r >> e.color.g >> e.color.b >> e.color.a  >> e.station; return packet >> e.seq; }

REGISTER_MULTIPLAYER_CLASS(PlayerSpaceship, "PlayerSpaceship");
PlayerSpaceship::PlayerSpaceship()
: SpaceShip("PlayerSpaceship", 5000)
{
    // Initialize ship settings
    main_screen_setting = MSS_Front;
    main_screen_overlay = MSO_HideComms;
    texture_front = "StarsFront";
    texture_back = "StarsBack";
    texture_left = "StarsLeft";
    texture_right = "StarsRight";
    texture_top = "StarsTop";
    texture_bottom = "StarsBottom";
    texture_r = 1.0;
    texture_g = 1.0;
    texture_b = 1.0;
    texture_a = 1.0;

    hull_damage_indicator = 0.0;
    jump_indicator = 0.0;
    comms_state = CS_Inactive;
    comms_open_delay = 0.0;
    shield_calibration_delay = 0.0;
    auto_repair_enabled = false;
    auto_coolant_enabled = false;
    max_coolant = max_coolant_per_system;
    max_repair = max_repair_per_system;
    active_engineer_presets_number = max_engineer_presets_number;
    scan_probe_stock = max_scan_probes;
    scan_probe_recharge_dock = 0.0;
    scan_probe_recharge = 0.0;
    alert_level = AL_Normal;
    shields_active = false;
    warp_indicator = 0;
    auto_repairing_system = SYS_None;
    control_code = "";
    has_gravity_sensor = true;
	has_electrical_sensor = true;
	has_biological_sensor = true;
	timer_log_intern = 0.0;
	timer_log_generic = 0.0;
	timer_log_docks = 0.0;
	timer_log_science = 0.0;

    setFactionId(1);

    // For now, set player ships to always be fully scanned to all other ships
    for(unsigned int faction_id = 0; faction_id < factionInfo.size(); faction_id++)
        setScannedStateForFaction(faction_id, SS_FullScan);

    updateMemberReplicationUpdateDelay(&target_rotation, 0.1);
    registerMemberReplication(&can_scan);
    registerMemberReplication(&can_hack);
    registerMemberReplication(&can_dock);
    registerMemberReplication(&can_combat_maneuver);
    registerMemberReplication(&can_self_destruct);
    registerMemberReplication(&can_launch_probe);
    registerMemberReplication(&hull_damage_indicator, 0.5);
    registerMemberReplication(&jump_indicator, 0.5);
    registerMemberReplication(&energy_level, 0.1);
    registerMemberReplication(&energy_warp_per_second, .5f);
    registerMemberReplication(&energy_shield_use_per_second, .5f);
    registerMemberReplication(&max_energy_level);
    registerMemberReplication(&main_screen_setting);
    registerMemberReplication(&main_screen_overlay);
    registerMemberReplication(&scanning_delay, 0.5);
    registerMemberReplication(&scanning_complexity);
    registerMemberReplication(&scanning_depth);
    registerMemberReplication(&shields_active);
    registerMemberReplication(&shield_calibration_delay, 0.5);
    registerMemberReplication(&auto_repair_enabled);
    registerMemberReplication(&max_coolant);
    registerMemberReplication(&max_coolant_per_system);
    registerMemberReplication(&max_repair);
    registerMemberReplication(&max_repair_per_system);
    registerMemberReplication(&active_engineer_presets_number);
    registerMemberReplication(&auto_coolant_enabled);
    registerMemberReplication(&beam_system_target);
    registerMemberReplication(&comms_state);
    registerMemberReplication(&comms_open_delay, 1.0);
    registerMemberReplication(&comms_reply_message);
    registerMemberReplication(&comms_target_name);
    registerMemberReplication(&comms_incomming_message);
    registerMemberReplication(&ships_log_generic);
    registerMemberReplication(&ships_log_intern);
    registerMemberReplication(&ships_log_docks);
    registerMemberReplication(&ships_log_science);
    registerMemberReplication(&waypoints);
    registerMemberReplication(&max_scan_probes);
    registerMemberReplication(&scan_probe_stock);
    registerMemberReplication(&activate_self_destruct);
    registerMemberReplication(&self_destruct_countdown, 0.2);
    registerMemberReplication(&alert_level);
    registerMemberReplication(&linked_science_probe_id);
    registerMemberReplication(&linked_probe_3D_id);
    registerMemberReplication(&linked_analysis_object_id);
    registerMemberReplication(&control_code);
    registerMemberReplication(&has_gravity_sensor);
    registerMemberReplication(&has_electrical_sensor);
    registerMemberReplication(&has_biological_sensor);
    registerMemberReplication(&long_range_radar_range);
    registerMemberReplication(&short_range_radar_range);
    registerMemberReplication(&custom_functions);
    registerMemberReplication(&auto_repairing_system);

    registerMemberReplication(&texture_front);
    registerMemberReplication(&texture_back);
    registerMemberReplication(&texture_left);
    registerMemberReplication(&texture_right);
    registerMemberReplication(&texture_top);
    registerMemberReplication(&texture_bottom);
    registerMemberReplication(&texture_r);
    registerMemberReplication(&texture_g);
    registerMemberReplication(&texture_b);
    registerMemberReplication(&texture_a);
    registerMemberReplication(&timer_log_intern);
    registerMemberReplication(&timer_log_generic);
    registerMemberReplication(&timer_log_docks);
    registerMemberReplication(&timer_log_science);

    // Determine which stations must provide self-destruct confirmation codes.
    for(int n = 0; n < max_self_destruct_codes; n++)
    {
        self_destruct_code[n] = 0;
        self_destruct_code_confirmed[n] = false;
        self_destruct_code_entry_position[n] = helmsOfficer;
        self_destruct_code_show_position[n] = helmsOfficer;
        registerMemberReplication(&self_destruct_code[n]);
        registerMemberReplication(&self_destruct_code_confirmed[n]);
        registerMemberReplication(&self_destruct_code_entry_position[n]);
        registerMemberReplication(&self_destruct_code_show_position[n]);
    }
    // Prepare presets initialization based on preference manager
    string power_default_presets = "";
    string power_saved_presets = "";
    string coolant_default_presets = "";
    string coolant_saved_presets = "";
    for(int n = 0; n < SYS_COUNT-1; n++)
    {
        power_default_presets += "1.00|";
        coolant_default_presets += "0.00|";
    }
    power_default_presets += "1.00";
    coolant_default_presets += "0.00";

    energy_consumption_ratio = 1.0f;
    registerMemberReplication(&energy_consumption_ratio);
    for(unsigned int n = 0; n < SYS_COUNT; n++)
    {
        for(int o = 0; o < max_engineer_presets_number; o++)
        {
            power_saved_presets = PreferencesManager::get("ENGINEERING.PRESET_POWER_"+string(o+1), power_default_presets);
            coolant_saved_presets = PreferencesManager::get("ENGINEERING.PRESET_COOLANT_"+string(o+1), coolant_default_presets);
            power_presets[n][o] = power_saved_presets.split("|")[n].toFloat();
            coolant_presets[n][o] = coolant_saved_presets.split("|")[n].toFloat();
            registerMemberReplication(&power_presets[n][o]);
            registerMemberReplication(&coolant_presets[n][o]);
        }
    }

    //delay_to_next_creation.reserve(max_blueprints_count);
    for(unsigned int n = 0; n < max_blueprints_count; n++)
    {
        registerMemberReplication(&squadron_delay_to_next_creation[n]);
        registerMemberReplication(&squadron_bp_activated[n]);
        registerMemberReplication(&squadron_bp_available[n]);
        registerMemberReplication(&squadron_bp_max_created[n]);
    }
    
    for(unsigned int n = 0; n < MW_Count; n++)
    {
        const string &name = getLocaleMissileWeaponName(EMissileWeapons(n));
        ammo_delay_to_next_creation[name] = 0;
        ammo_bp_activated[name] = false;
        ammo_bp_available[name] = false;

        registerMemberReplication(&ammo_delay_to_next_creation[name]);
        registerMemberReplication(&ammo_bp_activated[name]);
        registerMemberReplication(&ammo_bp_available[name]);
    }

    for(auto &kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
        ammo_delay_to_next_creation[kv.first] = 0;
        ammo_bp_activated[kv.first] = false;
        ammo_bp_available[kv.first] = false;
        //set in apply template values
        registerMemberReplication(&ammo_delay_to_next_creation[kv.first]);
        registerMemberReplication(&ammo_bp_activated[kv.first]);
        registerMemberReplication(&ammo_bp_available[kv.first]);
    }
    
    registerMemberReplication(&sq_bp_delay_factor);
    registerMemberReplication(&ammo_bp_delay_factor);

    //number_of_waiting_squadron_for_bp.reserve(max_number_of_waiting_squadron);
    for(unsigned int n = 0; n < max_blueprints_count; n++)
    {
        registerMemberReplication(&number_of_waiting_squadron_for_bp[n]);
    }
    for(unsigned int n=0; n< max_squadron_launch; n++)
    {
        bp_of_launching_squadron[n] = -1;
        registerMemberReplication(&bp_of_launching_squadron[n]);
        registerMemberReplication(&launch_delay[n]);
    }

    registerMemberReplication(&max_squadrons_in_flight);
    for(unsigned int n=0; n< max_squadrons_in_flight; n++)
    {
        registerMemberReplication(&(launched_squadrons_infos[n].squadron_name));
        registerMemberReplication(&(launched_squadrons_infos[n].order));
        registerMemberReplication(&(launched_squadrons_infos[n].target));
        registerMemberReplication(&(launched_squadrons_infos[n].squadron_template));
        registerMemberReplication(&(launched_squadrons_infos[n].leader_id));
        
    }

    registerMemberReplication(&launch_duration);

    if (game_server)
    {
        if (gameGlobalInfo->insertPlayerShip(this) < 0)
        {
            destroy();
        }

        // Initialize the ship's log.
        addToShipLog(tr("General log initialization"), colorConfig.log_generic);
        addToSpecificShipLog(tr("Internal log initialization"), colorConfig.log_generic,"intern");
        addToSpecificShipLog(tr("Science log initialization"), colorConfig.log_generic,"science");
        addToSpecificShipLog(tr("Dock log initialization"), colorConfig.log_generic,"docks");
    }

    // Initialize player ship callsigns with a "PL" designation.
    setCallSign("PL" + string(getMultiplayerId()));
}

//due to a suspected compiler bug this deconstructor needs to be explicitly defined
PlayerSpaceship::~PlayerSpaceship()
{
}

void PlayerSpaceship::update(float delta)
{
    // If we're flashing the screen for hull damage, tick the fade-out.
    if (hull_damage_indicator > 0)
        hull_damage_indicator -= delta;

    // If we're jumping, tick the countdown timer.
    if (jump_indicator > 0)
        jump_indicator -= delta;

    // hack procedure
    // Si hack, lancement d'un delay
    if (hack_time > 0)
        hack_time += delta;

    // si log, clignotement durant 5 secondes
    if (timer_log_intern > 0)
        timer_log_intern -= delta;
    if (timer_log_generic > 0)
        timer_log_generic -= delta;
    if (timer_log_docks > 0)
        timer_log_docks -= delta;
    if (timer_log_science > 0)
        timer_log_science -= delta;

    // If shields are calibrating, tick the calibration delay. Factor shield
    // subsystem effectiveness when determining the tick rate.
    if (shield_calibration_delay > 0)
    {
        shield_calibration_delay -= delta * (getSystemEffectiveness(SYS_FrontShield) + getSystemEffectiveness(SYS_RearShield)) / 2.0f;
    }

    // Docking actions.
    if (docking_state == DS_Docked)
    {
        P<SpaceShip> docked_with_ship = docking_target;
        if (docked_with_ship && docked_with_ship->tryDockDrone(this))
        {
            // this drone has docked with a carrier
            destroy();
        } else {
            P<ShipTemplateBasedObject> docked_with_template_based = docking_target;
            P<SpaceShip> docked_with_ship = docking_target;

            // Derive a base energy request rate from the player ship's maximum
            // energy capacity.
            float energy_request = std::min(delta * 10.0f, max_energy_level - energy_level);

            // If we're docked with a shipTemplateBasedObject, and that object is
            // set to share its energy with docked ships, transfer energy from the
            // mothership to docked ships until the mothership runs out of energy
            // or the docked ship doesn't require any.
            if (docked_with_template_based && docked_with_template_based->shares_energy_with_docked)
            {
                if (!docked_with_ship || docked_with_ship->useEnergy(energy_request))
                    energy_level += energy_request;
            }

            // If a shipTemplateBasedObject and is allowed to restock
            // scan probes with docked ships.
            if (docked_with_template_based && docked_with_template_based->restocks_scan_probes)
            {
                if (scan_probe_stock < max_scan_probes)
                {
                    scan_probe_recharge_dock += delta;

                    if (scan_probe_recharge_dock > scan_probe_charge_time)
                    {
                        scan_probe_stock += 1;
                        scan_probe_recharge_dock = 0.0;
                        scan_probe_recharge = 0.0;
                    }
                }
            }
        }
    }else{
        scan_probe_recharge_dock = 0.0;
    }

	// Ajout de probe si a l'arret
	if (current_impulse == 0 && scan_probe_stock < max_scan_probes)
    {
        scan_probe_recharge += delta;

        if (scan_probe_recharge > scan_probe_charge_time * 20.0f)
        {
            scan_probe_stock += 1;
            scan_probe_recharge_dock = 0.0;
            scan_probe_recharge = 0.0;
        }
    }else{
        scan_probe_recharge = 0.0;
    }

    // Automate cooling if auto_coolant_enabled is true. Distributes coolant to
    // subsystems proportionally to their share of the total generated heat.
    if (auto_coolant_enabled)
    {
        float total_heat = 0.0;

        for(int n = 0; n < SYS_COUNT; n++)
        {
            if (!hasSystem(ESystem(n))) continue;
            total_heat += systems[n].heat_level;
        }
        if (total_heat > 0.0f)
        {
            for(int n = 0; n < SYS_COUNT; n++)
            {
                if (!hasSystem(ESystem(n))) continue;
                systems[n].coolant_request = max_coolant * systems[n].heat_level / total_heat;
            }
        }
    }
    
    // Automate repair if auto_repair_enabled is true. Distributes repair to
    // subsystems proportionally to their share of the total generated health.
    if (auto_repair_enabled)
    {
        float total_damage = 0.f;

        for(int n = 0; n < SYS_COUNT; n++)
        {
            if (!hasSystem(ESystem(n))) continue;
            total_damage += (1.f - systems[n].health);
        }
        if (total_damage > 0.f)
        {
            for(unsigned int n = 0; n < SYS_COUNT; n++)
            {
                if (!hasSystem(ESystem(n))) continue;
                systems[n].repair_request = max_repair * (1.f - systems[n].health) / total_damage;
            }
        }
    }

    // Actions performed on the server only.
    if (game_server)
    {
        // Comms actions
        if (comms_state == CS_OpeningChannel)
        {
            if (comms_open_delay > 0)
            {
                comms_open_delay -= delta;
            }else{
                if (!comms_target && !comms_generic)
//                if (!comms_target)
                {
                    comms_state = CS_ChannelBroken;
                }else{
                    comms_reply_id.clear();
                    comms_reply_message.clear();
                    P<PlayerSpaceship> playerShip = comms_target;
                    if (playerShip)
                    {
                        comms_open_delay = PlayerSpaceship::comms_channel_open_time;

                        if (playerShip->comms_state == CS_Inactive || playerShip->comms_state == CS_ChannelFailed || playerShip->comms_state == CS_ChannelBroken || playerShip->comms_state == CS_ChannelClosed)
                        {
                            playerShip->comms_state = CS_BeingHailed;
                            playerShip->comms_target = this;
                            playerShip->comms_target_name = getCallSign();
                        }
                    }else{
                        if (gameGlobalInfo->intercept_all_comms_to_gm)
                        {
                            comms_state = CS_ChannelOpenGM;
                        }else{
                            if (comms_script_interface.openCommChannel(this, comms_target))
                                comms_state = CS_ChannelOpen;
                            else
                                comms_state = CS_ChannelFailed;
                        }
                    }
                }
            }
        }
        if (comms_state == CS_ChannelOpen || comms_state == CS_ChannelOpenPlayer)
        {
//            if (!comms_target)
            if (!comms_target && !comms_generic)
                comms_state = CS_ChannelBroken;
        }

        // Consume power if shields are enabled.
        if (shields_active)
            useEnergy(delta * getEnergyShieldUsePerSecond());

        // Consume power based on subsystem requests and state.
        {
            float request = delta * getNetSystemEnergyUsage();
            if(request < 0)
            {
                request*=energy_consumption_ratio;
            }
            energy_level += request;
        }

        // Check how much coolant we have requested in total, and if that's beyond the
        //  amount of coolant we have, see how much we need to adjust our request.
        float total_coolant_request = 0.0f;
        for(int n = 0; n < SYS_COUNT; n++)
        {
            if (!hasSystem(ESystem(n))) continue;
            total_coolant_request += systems[n].coolant_request;
        }
        float coolant_request_factor = 1.0f;
        if (total_coolant_request > max_coolant)
            coolant_request_factor = max_coolant / total_coolant_request;

        //The same for repair, if using nano repair
        //Request should never be superior to request by system 
        float repair_request_factor = 1.0f; 

        if (gameGlobalInfo->use_nano_repair_crew)
        {
            float total_repair_request = 0.0f;
            for(int n = 0; n < SYS_COUNT; n++)
            {
                if (!hasSystem(ESystem(n))) continue;
                total_repair_request += systems[n].repair_request;
            }
            if(total_repair_request > max_repair)
            {
                repair_request_factor = max_repair / total_repair_request;
            }
        }

        for(int n = 0; n < SYS_COUNT; n++)
        {
            if (!hasSystem(ESystem(n))) continue;

            if (systems[n].power_request > systems[n].power_level)
            {
                systems[n].power_level += delta * systems[n].power_rate_per_second;
                if (systems[n].power_level > systems[n].power_request)
                    systems[n].power_level = systems[n].power_request;
            }
            else if (systems[n].power_request < systems[n].power_level)
            {
                systems[n].power_level -= delta * systems[n].power_rate_per_second;
                if (systems[n].power_level < systems[n].power_request)
                    systems[n].power_level = systems[n].power_request;
            }

            float coolant_request = systems[n].coolant_request * coolant_request_factor;
            if (coolant_request > systems[n].coolant_level)
            {
                systems[n].coolant_level += delta * systems[n].coolant_rate_per_second;
                if (systems[n].coolant_level > coolant_request)
                    systems[n].coolant_level = coolant_request;
            }
            else if (coolant_request < systems[n].coolant_level)
            {
                systems[n].coolant_level -= delta * systems[n].coolant_rate_per_second;
                if (systems[n].coolant_level < coolant_request)
                    systems[n].coolant_level = coolant_request;
            }
            
            float repair_request = systems[n].repair_request * repair_request_factor;
            if (repair_request > systems[n].repair_level)
            {
                systems[n].repair_level += delta * system_repair_level_change_per_second;
                if (systems[n].repair_level > repair_request)
                    systems[n].repair_level = repair_request;
            }
            else if (repair_request < systems[n].repair_level)
            {
                systems[n].repair_level -= delta * system_repair_level_change_per_second;
                if (systems[n].repair_level < repair_request)
                    systems[n].repair_level = repair_request;
            }

            // Add heat to overpowered subsystems
            addHeat(ESystem(n), delta * systems[n].getHeatingDelta() * systems[n].heat_rate_per_second);
            
            // Repair if nano repair crew
            if (gameGlobalInfo->use_nano_repair_crew)
            {
                systems[n].health += system_repair_effect_per_second * systems[n].repair_level * delta;
                if (systems[n].health > 1.f)
                    systems[n].health = 1.f;
                systems[n].hacked_level -= system_repair_effect_per_second * systems[n].repair_level * delta;;
                if (systems[n].hacked_level < 0.f)
                    systems[n].hacked_level = 0.f;
            }
        }

        // If reactor health is worse than -90% and overheating, it explodes,
        // destroying the ship and damaging a 0.5U radius.
        if (can_be_destroyed && systems[SYS_Reactor].health < -0.9f && systems[SYS_Reactor].heat_level == 1.0f) //TODO verifier si on veut mettre ça sou soptions
        {
            ExplosionEffect* e = new ExplosionEffect();
            e->setSize(1000.0f);
            e->setPosition(getPosition());
            e->setRadarSignatureInfo(0.0, 0.4, 0.4);

            DamageInfo info(this, DT_Kinetic, getPosition());
            SpaceObject::damageArea(getPosition(), 500, 30, 60, info, 0.0);

            destroy();
            return;
        }
        
        if (energy_level < 0.0f)
            energy_level = 0.0f;

        // If the ship has less than 10 energy, drop shields automatically.
        if (energy_level < 10.0f)
        {
            shields_active = false;
        }

        // auto-repair
        if (!gameGlobalInfo->use_repair_crew){
            ESystem system = ESystem(auto_repairing_system);
            if (system > SYS_None && system < SYS_COUNT && hasSystem(system))
            {
                systems[system].health += repair_per_second * delta;
                if (systems[system].health > systems[system].health_max)
                    systems[system].health = systems[system].health_max;
            }
            if (auto_repair_enabled && (system == SYS_None || !hasSystem(system) || systems[system].health == systems[system].health_max))
            {
                int n=irandom(0, SYS_COUNT - 1);
                 if (hasSystem(ESystem(n)) && systems[n].health < systems[n].health_max)
                {
                    auto_repairing_system = ESystem(n);
                }
            }
        }

        // If a ship is jumping or warping, consume additional energy.
        if (has_warp_drive && warp_request > 0 && !(has_jump_drive && jump_delay > 0))
        {
            // If warping, consume energy at a rate of 120% the warp request.
            // If shields are up, that rate is increased by an additional 50%.
            if (!useEnergy(getEnergyWarpPerSecond() * delta * getSystemEffectiveness(SYS_Warp) * powf(current_warp, 1.3f) * (shields_active ? 1.7f : 1.0f)))
                // If there's not enough energy, fall out of warp.
                warp_request = 0;

            for(float n=0; n<=4; n++)
            {
                if ((current_warp > n-0.1f && current_warp < n+0.1f) && warp_indicator != n)
                {
                    warp_indicator = n;
                    addToSpecificShipLog(tr("Warp ") + string(abs(n)), glm::u8vec4(255,255,255,255),"intern");
                }
            }
        }

        if (scanning_target)
        {
            // If the scan setting or a target's scan complexity is none/0,
            // complete the scan after a delay.
            if (scanning_complexity < 1)
            {
                scanning_delay -= delta;
                if (scanning_delay < 0)
                {
                    scanning_target->scannedBy(this);
                    scanning_target = NULL;
                }
            }
        }else{
            // Otherwise, ignore the scanning_delay setting.
            scanning_delay = 0.0;
        }

        if (activate_self_destruct)
        {
            // If self-destruct has been activated but not started ...
            if (self_destruct_countdown <= 0.0f)
            {
                bool do_self_destruct = true;
                // ... wait until the confirmation codes are entered.
                for(int n = 0; n < max_self_destruct_codes; n++)
                    if (!self_destruct_code_confirmed[n])
                        do_self_destruct = false;

                // Then start and announce the countdown.
                if (do_self_destruct)
                {
                    self_destruct_countdown = PreferencesManager::get("self_destruct_countdown", "10").toFloat();
                    playSoundOnMainScreen("sfx/vocal_self_destruction.wav");
                }
            }else{
                // If the countdown has started, tick the clock.
                self_destruct_countdown -= delta;

                // When time runs out, blow up the ship and damage a
                // configurable radius.
                if (self_destruct_countdown <= 0.0f)
                {
                    for(int n = 0; n < 5; n++)
                    {
                        ExplosionEffect* e = new ExplosionEffect();
                        e->setSize(self_destruct_size * 0.67f);
                        e->setPosition(getPosition() + rotateVec2(glm::vec2(0, random(0, self_destruct_size * 0.33f)), random(0, 360)));
                        e->setRadarSignatureInfo(0.0, 0.6, 0.6);
                    }

                    DamageInfo info(this, DT_Kinetic, getPosition());
                    SpaceObject::damageArea(getPosition(), self_destruct_size, self_destruct_damage - (self_destruct_damage / 3.0f), self_destruct_damage + (self_destruct_damage / 3.0f), info, 0.0);

                    destroy();
                    return;
                }
            }
        }

    }else{
        // Actions performed on the client-side only.

        // If scan settings or the scan target's complexity is 0/none, tick
        // the scan delay timer.
        if (scanning_complexity < 1)
        {
            if (scanning_delay > 0.0f)
                scanning_delay -= delta;
        }

        // If opening comms, tick the comms open delay timer.
        if (comms_open_delay > 0)
            comms_open_delay -= delta;
    }

    // unsigned int number_activated{0};
    // for(const auto& [name, sqt] : squadrons_compositions) //if only I had an iterator only on values I could use count if...
    // {
    //     if((sqt.activated == true) && (sqt.available == true))
    //     {
    //         number_activated++;
    //     }
    // }

    std::set<int> to_progress_sq;
    std::set<string> to_progress_ammo;

    //Auto creation of blueprints 
    unsigned int squadron_bp_idx{0};
    if(ship_template)
    {
        //Squadrons
        for(const auto& sqt : ship_template->squadrons_compositions)
        {    
            
            if((getSquadronCount(squadron_bp_idx) > squadron_bp_max_created[squadron_bp_idx]) 
            || squadron_bp_available[squadron_bp_idx] == false)
            {
                //Strictement superieur : si la derniere escadre est en train de rentrer, il faut tout de meme 
                //ajuster le delay pour ne pas tout perdre a la destruction
                squadron_delay_to_next_creation[squadron_bp_idx] = sqt.construction_duration;
            }
            else if (squadron_delay_to_next_creation[squadron_bp_idx] <= 0.0f)
            {
                instantiateSquadron(sqt.template_name);
                squadron_delay_to_next_creation[squadron_bp_idx] += sqt.construction_duration;
            }
            else if((squadron_bp_activated[squadron_bp_idx] == true) && (getSquadronCount(squadron_bp_idx) < squadron_bp_max_created[squadron_bp_idx]))
            {
                to_progress_sq.insert(squadron_bp_idx);
            }
            squadron_bp_idx++;
        }
        //Ammo
        auto &amts = ship_template->ammo_blueprints;
        for (int n = 0; n < MW_Count; n++)
        {
            const auto &missile_name = getLocaleMissileWeaponName(EMissileWeapons(n));
            if(amts.find(missile_name) != amts.end())
            {
                
                if(weapon_storage_max[n] > 0)
                {
                    //weapon storage max negatif indique qu'on n'utilise pas cette arme
                    if((weapon_storage[n] >= weapon_storage_max[n]) 
                    || ammo_bp_available[missile_name] == false) 
                    {
                        ammo_delay_to_next_creation[missile_name] = amts[missile_name].construction_duration; 
                    }
                    else if(ammo_delay_to_next_creation[missile_name] <= 0.0f)
                    {
                        weapon_storage[n]++;
                        ammo_delay_to_next_creation[missile_name] += amts[missile_name].construction_duration;
                    }
                    else if((ammo_bp_activated[missile_name] == true) && (weapon_storage[n] < weapon_storage_max[n]))
                    {
                        to_progress_ammo.insert(missile_name);
                    }
                }
            }
        }
        for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
        {
            if(amts.find(kv.first) != amts.end())
            {
                if(custom_weapon_storage_max[kv.first] > 0)
                {
                    if((custom_weapon_storage[kv.first] >= custom_weapon_storage_max[kv.first]) 
                    || ammo_bp_available[kv.first] == false) 
                    {
                        ammo_delay_to_next_creation[kv.first] = amts[kv.first].construction_duration; 
                    }
                    else if(ammo_delay_to_next_creation[kv.first] <= 0.0f)
                    {
                        custom_weapon_storage[kv.first]++;
                        ammo_delay_to_next_creation[kv.first] += amts[kv.first].construction_duration;
                    }
                    else if((ammo_bp_activated[kv.first] == true) && (custom_weapon_storage[kv.first] < custom_weapon_storage_max[kv.first]))
                    {
                        to_progress_ammo.insert(kv.first);
                    }
                }
            }
        }
        

        int total_size = to_progress_sq.size() + to_progress_ammo.size();
        for(int idx : to_progress_sq)
        {
            squadron_delay_to_next_creation[idx] -= delta * getSystemEffectiveness(SYS_Hangar) * (1.0f / total_size * sq_bp_delay_factor);
        }
        for(string idx : to_progress_ammo)
        {
            ammo_delay_to_next_creation[idx] -= delta * getSystemEffectiveness(SYS_Hangar) * (1.0f / total_size * ammo_bp_delay_factor);
        }

    }
    //Launch of a waiting squadron
    int deck_to_launch = -1;
    unsigned int shortest_delay = std::numeric_limits<unsigned int>::max();
    for(unsigned int deck = 0; deck< max_squadron_launch; deck++)
    {
        //one at a time, find the deck the shortest to launch
        if(launch_delay[deck] > 0 && bp_of_launching_squadron[deck] >= 0)
        {
            if(launch_delay[deck] < shortest_delay)
            {
                shortest_delay = launch_delay[deck];
                deck_to_launch = deck;
            }
        }
        
        if(launch_delay[deck] <= 0 && bp_of_launching_squadron[deck] >= 0)
        {
            launchSquadron(deck);
        }
    }
    if(deck_to_launch != -1)
    {
        launch_delay[(unsigned int)deck_to_launch] -= delta * getSystemEffectiveness(SYS_Hangar);
    }

    if(game_server)
    {
        std::vector<Squadron>::iterator iter = launched_squadrons.begin();
        while(iter != launched_squadrons.end())
        {
            string template_name = iter->squadron_template_name;
            bool found = false;
            unsigned int nbr_destroyed{0};

            for(P<CpuShip> cpu : iter->ships)
            {

                if(cpu && cpu->isDockedWith(this))
                {
                    cpu->destroy();
                    nbr_destroyed++; 
                }
                if(cpu)
                {
                    found = true;
                }
            }
            if(!found)
            {
                iter = launched_squadrons.erase(iter);
            }
            else
            {
                iter++;
            }

            if(nbr_destroyed > 0)
            {
                int n=0;
                for(auto &sqt : getSquadronCompositions())
                {
                    if(template_name == sqt.template_name)
                    {
                        float recall_ratio = (float)nbr_destroyed / sqt.ship_names.size();
                        float time_gained = recall_ratio * sqt.construction_duration;
                        squadron_delay_to_next_creation[n] -= time_gained;
                    }
                    n++;
                }
            }

        }
        
        {
            unsigned int n=0;
            for(auto &sq : launched_squadrons)
            {
                P<CpuShip> cpu = sq.ships[0];
                if(cpu)
                {
                
                    launched_squadrons_infos[n].squadron_name = sq.squadron_name;
                    launched_squadrons_infos[n].squadron_template= sq.squadron_template_name;
                    
                    string target{""};
                    P<SpaceObject> spo = cpu->getOrderTarget();
                    if(spo)
                    {
                        target = spo->getCallSign();
                    }
                    else
                    {
                        target = getStringFromPosition(cpu->getOrderTargetLocation());
                    }
                    
                    launched_squadrons_infos[n].leader_id = cpu->getMultiplayerId();
                    launched_squadrons_infos[n].target = target;
                    launched_squadrons_infos[n].order = getAIOrderString(cpu->getOrder());
                }
                n++;
            }
            
            while(n < max_squadrons_in_flight_limit)
            {
                launched_squadrons_infos[n].squadron_name="";
                launched_squadrons_infos[n].order="";
                launched_squadrons_infos[n].target="";
                launched_squadrons_infos[n].squadron_template="";
                launched_squadrons_infos[n].leader_id=-1;
                n++;
            }
        }
    }

    // Perform all other ship update actions.
    SpaceShip::update(delta);

    // Cap energy at the max_energy_level.
    if (energy_level > max_energy_level)
        energy_level = max_energy_level;
}

void PlayerSpaceship::applyTemplateValues()
{
    // Apply default spaceship object values first.
    SpaceShip::applyTemplateValues();

    

    if (gameGlobalInfo->use_repair_crew){
        // Set the ship's number of repair crews in Engineering from the ship's template.
        setRepairCrewCount(ship_template->repair_crew_count);
    }
    
    energy_consumption_ratio = ship_template->energy_consumption_ratio;

    // Set the ship's capabilities.
    can_scan = ship_template->can_scan;
    can_hack = ship_template->can_hack;
    can_dock = ship_template->can_dock;
    can_combat_maneuver = ship_template->can_combat_maneuver;
    can_self_destruct = ship_template->can_self_destruct;
    can_launch_probe = ship_template->can_launch_probe;
    if (!on_new_player_ship_called)
    {
        on_new_player_ship_called = true;
        gameGlobalInfo->on_new_player_ship.call<void>(P<PlayerSpaceship>(this));
    }

    {
        int n=0;
        for(auto &sqt : ship_template->squadrons_compositions)
        {
            squadron_bp_activated[n] = sqt.activated;
            squadron_bp_available[n] = sqt.available;
            squadron_delay_to_next_creation[n] = sqt.construction_duration;
            squadron_bp_max_created[n] = sqt.max_created;
            n++;
        }
    }
    
    {
        for(auto &amt : ship_template->ammo_blueprints)
        {
            ammo_bp_activated[amt.first] = amt.second.activated;
            ammo_bp_available[amt.first] = amt.second.available;
            ammo_delay_to_next_creation[amt.first] = amt.second.construction_duration;
        }
    }

    

}

void PlayerSpaceship::executeJump(float distance)
{
    // When jumping, reset the jump effect and move the ship.
    jump_indicator = 2.0;
    SpaceShip::executeJump(distance);
}

void PlayerSpaceship::takeHullDamage(float damage_amount, DamageInfo& info)
{
    // If taking non-EMP damage, light up the hull damage overlay.
    if (info.type != DT_EMP)
    {
        hull_damage_indicator = 1.5;
    }

    // Take hull damage like any other ship.
    float systems_diff[SYS_COUNT];
    for(int n=0; n<SYS_COUNT; n++)
    {
        systems_diff[n] = systems[n].health;
    }
    SpaceShip::takeHullDamage(damage_amount, info);

    // Infos for log intern
    string system_damage = string(abs(damage_amount)) + string(tr(" damage to hull. Damaged system : "));
    for(int n=0; n<SYS_COUNT; n++)
    {
        if(systems_diff[n] != systems[n].health)
            system_damage += string(getSystemName(ESystem(n))) + string(" / ");
    }
    system_damage.erase(system_damage.end()-2, system_damage.end());
    addToSpecificShipLog(system_damage,glm::u8vec4(255,0,0,255),"intern");
}

void PlayerSpaceship::setMaxCoolant(float coolant)
{
    max_coolant = std::max(coolant, 0.0f);
}

void PlayerSpaceship::setSystemCoolantRequest(ESystem system, float request)
{
    request = std::max(0.0f, std::min(request, std::min((float) max_coolant_per_system, max_coolant)));
    systems[system].coolant_request = request;
}

void PlayerSpaceship::setSystemRepairRequest(ESystem system, float request)
{
    request = std::max(0.0f, std::min(request, std::min((float) max_repair_per_system, max_repair)));
    // Set coolant levels on a system.
    // float total_repair = 0;
    // int cnt = 0;
    // for(int n = 0; n < SYS_COUNT; n++)
    // {
    //     if (!hasSystem(ESystem(n))) continue;
    //     if (n == system) continue;

    //     total_repair += systems[n].repair_request;
    //     cnt++;
    // }
    // if (total_repair > max_repair - request)
    // {
    //     for(int n = 0; n < SYS_COUNT; n++)
    //     {
    //         if (!hasSystem(ESystem(n))) continue;
    //         if (n == system) continue;

    //         systems[n].repair_request *= (max_repair - request) / total_repair;
    //     }
    // }else{
    //     for(int n = 0; n < SYS_COUNT; n++)
    //     {
    //         if (!hasSystem(ESystem(n))) continue;
    //         if (n == system) continue;

    //         if (total_repair > 0)
    //             systems[n].repair_request = std::min(systems[n].repair_request * (max_repair - request) / total_repair, (float) max_repair_per_system);
    //         else
    //             systems[n].repair_request = std::min((max_repair - request) / float(cnt), float(max_repair_per_system));
    //     }
    // }

    systems[system].repair_request = request;
}

bool PlayerSpaceship::useEnergy(float amount)
{
    // Try to consume an amount of energy. If it works, return true.
    // If it doesn't, return false.

    amount *= energy_consumption_ratio;

    if (energy_level >= amount)
    {
        energy_level -= amount;
        return true;
    }
    return false;
}

void PlayerSpaceship::addHeat(ESystem system, float amount)
{
    // Add heat to a subsystem if it's present.
    if (!hasSystem(system)) return;

    systems[system].heat_level += amount;

    if (systems[system].heat_level > 0.95f)
    {
        float overheat = systems[system].heat_level - 0.95f;
        systems[system].heat_level = 0.95f;

        if (gameGlobalInfo->use_system_damage)
        {
            // Heat damage is specified as damage per second while overheating.
            // Calculate the amount of overheat back to a time, and use that to
            // calculate the actual damage taken.
            systems[system].health -= overheat / systems[system].heat_rate_per_second * damage_per_second_on_overheat;

            if (systems[system].health < -1.0f)
                systems[system].health = -1.0f;
        }
    }

    if (systems[system].heat_level < 0.0f)
        systems[system].heat_level = 0.0f;
}

void PlayerSpaceship::playSoundOnMainScreen(string sound_name)
{
    sp::io::DataBuffer packet;
    packet << CMD_PLAY_CLIENT_SOUND;
    packet << max_crew_positions;
    packet << sound_name;
    broadcastServerCommand(packet);
}

float PlayerSpaceship::getNetSystemEnergyUsage()
{
    // Get the net delta of energy draw for subsystems.
    float net_power = 0.0;

    // Determine each subsystem's energy draw.
    for(int n = 0; n < SYS_COUNT; n++)
    {
        
        if (!hasSystem(ESystem(n))) continue;
        if (systems[n].health < 0.f) continue;

        const auto& system = systems[n];
        // Factor the subsystem's health into energy generation.
        auto power_user_factor = system.getPowerUserFactor();
        if (power_user_factor < 0)
        {
            float f = getSystemEffectiveness(ESystem(n));
            if (f > 1.0f)
                f = (1.0f + f) / 2.0f;
            net_power -= power_user_factor * f;
        }
        else
        {
            net_power -= power_user_factor * system.power_level;
        }
    }

    // Return the net subsystem energy draw.
    return net_power;
}

int PlayerSpaceship::getRepairCrewCount()
{
    // Count and return the number of repair crews on this ship.
    return getRepairCrewFor(this).size();
}

void PlayerSpaceship::setRepairCrewCount(int amount)
{
    // This is a server-only function, and we only care about repair crews when
    // we care about subsystem damage.
    if (!game_server || !gameGlobalInfo->use_system_damage)
        return;

    // Prevent negative values.
    amount = std::max(0, amount);

    // Get the number of repair crews for this ship.
    PVector<RepairCrew> crew = getRepairCrewFor(this);

    // Remove excess crews by shifting them out of the array.
    while(int(crew.size()) > amount)
    {
        crew[0]->destroy();
        crew.update();
    }

    if (ship_template->rooms.size() == 0 && amount != 0)
    {
        LOG(WARNING) << "Not adding repair crew to ship \"" << callsign << "\", because it has no rooms. Fix this by adding rooms to the ship template \"" << template_name << "\".";
        return;
    }

    // Add crews until we reach the provided amount.
    for(int create_amount = amount - crew.size(); create_amount > 0; create_amount--)
    {
        P<RepairCrew> rc = new RepairCrew();
        rc->ship_id = getMultiplayerId();
    }
    
    // For nano repair Crew
    max_repair = (float) amount;
}

void PlayerSpaceship::addToSpecificShipLog(string message, glm::u8vec4 color, string station)
{
    if (station == "generic")
    {
        if (ships_log_generic.size() > 100)
            ships_log_generic.erase(ships_log_generic.begin());
        // Timestamp a log entry, color it, and add it to the end of the log.
        ships_log_generic.emplace_back(gameGlobalInfo->getMissionTime() + string(": "), message, color, station, last_log_seq++);
        timer_log_generic = 6;
    }
    else if (station == "intern")
    {
        if (ships_log_intern.size() > 100)
            ships_log_intern.erase(ships_log_intern.begin());
        // Timestamp a log entry, color it, and add it to the end of the log.
        ships_log_intern.emplace_back(gameGlobalInfo->getMissionTime() + string(": "), message, color, station, last_log_seq++);
        timer_log_intern = 6;
    }
    else if (station == "docks")
    {
        if (ships_log_docks.size() > 100)
            ships_log_docks.erase(ships_log_docks.begin());
        // Timestamp a log entry, color it, and add it to the end of the log.
        ships_log_docks.emplace_back(gameGlobalInfo->getMissionTime() + string(": "), message, color, station, last_log_seq++);
        timer_log_docks = 6;
    }
    else if (station == "science")
    {
        if (ships_log_science.size() > 100)
            ships_log_science.erase(ships_log_science.begin());
        // Timestamp a log entry, color it, and add it to the end of the log.
        ships_log_science.emplace_back(gameGlobalInfo->getMissionTime() + string(": "), message, color, station, last_log_seq++);
        timer_log_science = 6;
    }
}

void PlayerSpaceship::addToShipLogBy(string message, P<SpaceObject> target)
{
    // Log messages received from other ships. Friend-or-foe colors are drawn
    // from colorConfig (colors.ini).
    if (!target)
        addToShipLog(message, colorConfig.log_receive_neutral);
    else if (isFriendly(target))
        addToShipLog(message, colorConfig.log_receive_friendly);
    else if (isEnemy(target))
        addToShipLog(message, colorConfig.log_receive_enemy);
    else
        addToShipLog(message, colorConfig.log_receive_neutral);
}

const std::vector<PlayerSpaceship::ShipLogEntry>& PlayerSpaceship::getShipsLog(string station) const
{
    // Return the ship's log.
    if (station == "generic")
        return ships_log_generic;
    if (station == "intern")
        return ships_log_intern;
    if (station == "docks")
        return ships_log_docks;
    if (station == "science")
        return ships_log_science;
     else
     {
       //FIXME OKE
       static std::vector<PlayerSpaceship::ShipLogEntry> fixme;
       return fixme;
     }
}

void PlayerSpaceship::transferPlayersToShip(P<PlayerSpaceship> other_ship)
{
    // Don't do anything without a valid target. The target must be a
    // PlayerSpaceship.
    if (!other_ship)
        return;

    // For each player, move them to the same station on the target.
    foreach(PlayerInfo, i, player_info_list)
    {
        if (i->ship_id == getMultiplayerId())
        {
            i->ship_id = other_ship->getMultiplayerId();
        }
    }
}

void PlayerSpaceship::transferPlayersAtPositionToShip(ECrewPosition position, P<PlayerSpaceship> other_ship)
{
    // Don't do anything without a valid target. The target must be a
    // PlayerSpaceship.
    if (!other_ship)
        return;

    // For each player, check which position they fill. If the position matches
    // the requested position, move that player. Otherwise, ignore them.
    foreach(PlayerInfo, i, player_info_list)
    {
        if (i->ship_id == getMultiplayerId() && i->crew_position[position])
        {
            i->ship_id = other_ship->getMultiplayerId();
        }
    }
}

bool PlayerSpaceship::hasPlayerAtPosition(ECrewPosition position)
{
    // If a position is occupied by a player, return true.
    // Otherwise, return false.
    foreach(PlayerInfo, i, player_info_list)
    {
        if (i->ship_id == getMultiplayerId() && i->crew_position[position])
        {
            return true;
        }
    }
    return false;
}

void PlayerSpaceship::addCustomButton(ECrewPosition position, string name, string caption, ScriptSimpleCallback callback, std::optional<int> order)
{
    removeCustom(name);
    custom_functions.emplace_back();
    CustomShipFunction& csf = custom_functions.back();
    csf.type = CustomShipFunction::Type::Button;
    csf.name = name;
    csf.crew_position = position;
    csf.caption = caption;
    csf.callback = callback;
    csf.order = order.value_or(0);
    std::stable_sort(custom_functions.begin(), custom_functions.end());
}

void PlayerSpaceship::addCustomInfo(ECrewPosition position, string name, string caption, std::optional<int> order)
{
    removeCustom(name);
    custom_functions.emplace_back();
    CustomShipFunction& csf = custom_functions.back();
    csf.type = CustomShipFunction::Type::Info;
    csf.name = name;
    csf.crew_position = position;
    csf.caption = caption;
    csf.order = order.value_or(0);
    std::stable_sort(custom_functions.begin(), custom_functions.end());
}

void PlayerSpaceship::addCustomMessage(ECrewPosition position, string name, string caption)
{
    removeCustom(name);
    custom_functions.emplace_back();
    CustomShipFunction& csf = custom_functions.back();
    csf.type = CustomShipFunction::Type::Message;
    csf.name = name;
    csf.crew_position = position;
    csf.caption = caption;
    std::stable_sort(custom_functions.begin(), custom_functions.end());
}

void PlayerSpaceship::addCustomMessageWithCallback(ECrewPosition position, string name, string caption, ScriptSimpleCallback callback)
{
    removeCustom(name);
    custom_functions.emplace_back();
    CustomShipFunction& csf = custom_functions.back();
    csf.type = CustomShipFunction::Type::Message;
    csf.name = name;
    csf.crew_position = position;
    csf.caption = caption;
    csf.callback = callback;
    std::stable_sort(custom_functions.begin(), custom_functions.end());
}

void PlayerSpaceship::removeCustom(string name)
{
    for(auto it = custom_functions.begin(); it != custom_functions.end();)
    {
        if (it->name == name)
            it = custom_functions.erase(it);
        else
            it++;
    }
}

void PlayerSpaceship::setCommsMessage(string message)
{
    // Record a new comms message to the ship's log.
    for(string line : message.split("\n"))
        addToShipLog(line, glm::u8vec4(192, 192, 255, 255));
    // Display the message in the messaging window.
    comms_incomming_message = message;
}

void PlayerSpaceship::addCommsIncommingMessage(string message)
{
    // Record incoming comms messages to the ship's log.
    for(string line : message.split("\n"))
        addToShipLog(line, glm::u8vec4(192, 192, 255, 255));
    // Add the message to the messaging window.
    comms_incomming_message = comms_incomming_message + "\n> " + message;
}

void PlayerSpaceship::addCommsOutgoingMessage(string message)
{
    // Record outgoing comms messages to the ship's log.
    for(string line : message.split("\n"))
        addToShipLog(line, colorConfig.log_send);
    // Add the message to the messaging window.
    comms_incomming_message = comms_incomming_message + "\n< " + message;
}

void PlayerSpaceship::addCommsReply(int32_t id, string message)
{
    if (comms_reply_id.size() >= 200)
        return;
    comms_reply_id.push_back(id);
    comms_reply_message.push_back(message);
}

bool PlayerSpaceship::hailCommsByGM(string target_name)
{
    // If a ship's comms aren't engaged, receive the GM's hail.
    // Otherwise, return false.
    if (!isCommsInactive() && !isCommsFailed() && !isCommsBroken() && !isCommsClosed())
        return false;

    // Log the hail.
    addToShipLog(tr("shiplog", "Hailed by {name}").format({{"name", target_name}}), colorConfig.log_generic);
	soundManager->playSound("incoming_com.wav");

    // Set comms to the hail state and notify Relay/comms.
    comms_state = CS_BeingHailedByGM;
    comms_target_name = target_name;
    comms_target = nullptr;
    return true;
}

bool PlayerSpaceship::hailByObject(P<SpaceObject> object, string opening_message)
{
    // If trying to open comms with a non-object, return false.
    if (isCommsOpening() || isCommsBeingHailed())
    {
        if (comms_target != object)
        {
            return false;
        }
    }

    // If comms are engaged, return false.
    if (isCommsBeingHailedByGM())
    {
        return false;
    }
    if (isCommsChatOpen() || isCommsScriptOpen())
    {
        return false;
    }

    // Receive a hail from the object.
    comms_target = object;
    comms_target_name = object->getCallSign();
    comms_state = CS_BeingHailed;
    comms_incomming_message = opening_message;
    return true;
}

void PlayerSpaceship::switchCommsToGM()
{
    comms_state = CS_ChannelOpenGM;
    if (comms_incomming_message == "?")
        comms_incomming_message = "";
}

void PlayerSpaceship::closeComms()
{
    // If comms are closed, state it and log it to the ship's log.
    if (comms_state != CS_Inactive)
    {
        if (comms_state == CS_ChannelOpenPlayer && comms_target)
        {
            P<PlayerSpaceship> player_ship = comms_target;
            player_ship->comms_state = CS_ChannelClosed;
            player_ship->addToShipLog(tr("shiplog", "Communication channel closed by other side"), colorConfig.log_generic);
        }
        if (comms_state == CS_OpeningChannel && comms_target)
        {
            P<PlayerSpaceship> player_ship = comms_target;
            if (player_ship)
            {
                if (player_ship->comms_state == CS_BeingHailed && player_ship->comms_target == this)
                {
                    player_ship->comms_state = CS_Inactive;
                    player_ship->addToShipLog(tr("shiplog", "Hailing from {callsign} stopped").format({{"callsign", getCallSign()}}), colorConfig.log_generic);
                }
            }
        }
        addToShipLog(tr("shiplog", "Communication channel closed"), colorConfig.log_generic);
        if (comms_state == CS_ChannelOpenGM)
            comms_state = CS_ChannelClosed;
        else
            comms_state = CS_Inactive;
    }
}

void PlayerSpaceship::onReceiveClientCommand(int32_t client_id, sp::io::DataBuffer& packet)
{
    // Receive a command from a client. Code in this function is executed on
    // the server only.
    int16_t command;
    packet >> command;

    switch(command)
    {
    case CMD_TARGET_ROTATION:
        turnSpeed = 0;
        packet >> target_rotation;
        break;
    case CMD_TURN_SPEED:
        target_rotation = getRotation();
        packet >> turnSpeed;
        break;
    case CMD_IMPULSE:
        packet >> impulse_request;
        break;
    case CMD_WARP:
        packet >> warp_request;
        warp_request = std::clamp(int(warp_request), 0, 4);
        break;
    case CMD_JUMP:
        {
            float distance;
            packet >> distance;
            initializeJump(distance);
            addToSpecificShipLog(tr("Jump initialization"),glm::u8vec4(255,255,255,255),"intern");
        }
        break;
    case CMD_SET_TARGET:
        {
            packet >> target_id;
            //if (target_id != int32_t(-1))
            //    addToSpecificShipLog("Cible active : " + string(SpaceShip::getTarget()->SpaceObject::getCallSign()),glm::u8vec4(255,255,0,255),"intern");
        }
        break;
    case CMD_SET_DOCK_TARGET:
        {
            packet >> dock_target_id;
            //if (dock_target_id != int32_t(-1))
            //    addToSpecificShipLog("Cible du docking : " + string(SpaceShip::getDockTarget()->SpaceObject::getCallSign()),glm::u8vec4(255,255,0,255),"intern");
        }
        break;
    case CMD_SET_LANDING_TARGET:
        {
            packet >> landing_target_id;
            //if (landing_target_id != int32_t(-1))
            //    addToSpecificShipLog("Cible de l'atterrissage : " + string(SpaceShip::getLandingTarget()->SpaceObject::getCallSign()),glm::u8vec4(255,255,0,255),"intern");
        }
        break;
    case CMD_LOAD_TUBE:
        {
            int8_t tube_nr;
            string type;
            packet >> tube_nr >> type;

            if (tube_nr >= 0 && tube_nr < max_weapon_tubes)
                weapon_tube[tube_nr].startLoad(type);
        }
        break;
    case CMD_UNLOAD_TUBE:
        {
            int8_t tube_nr;
            packet >> tube_nr;

            if (tube_nr >= 0 && tube_nr < max_weapon_tubes)
            {
                weapon_tube[tube_nr].startUnload();
            }
        }
        break;
    case CMD_FIRE_TUBE:
        {
            int8_t tube_nr;
            float missile_target_angle;
            packet >> tube_nr >> missile_target_angle;

            if (tube_nr >= 0 && tube_nr < max_weapon_tubes)
            {
                weapon_tube[tube_nr].fire(missile_target_angle);
                addToSpecificShipLog(tr("Missile fired"),glm::u8vec4(255,255,0,255),"intern");
            }
        }
        break;
    case CMD_SET_SHIELDS:
        {
            bool active;
            packet >> active;

            if (shield_calibration_delay <= 0.0f && active != shields_active)
            {
                shields_active = active;
                if (active)
                {
                    //Tdelc : son joue volontairement sur le client
                    soundManager->playSound("sfx/shield_up.wav");
                    addToSpecificShipLog(tr("Shields up"),glm::u8vec4(0,255,0,255),"intern");
                }
                else
                {
                    //Tdelc : son joue volontairement sur le client
                    soundManager->playSound("sfx/shield_down.wav");
                    addToSpecificShipLog(tr("Shields down"),glm::u8vec4(0,255,0,255),"intern");
                }
            }
        }
        break;
    case CMD_SET_MAIN_SCREEN_SETTING:
        packet >> main_screen_setting;
        break;
    case CMD_SET_MAIN_SCREEN_OVERLAY:
        packet >> main_screen_overlay;
        break;
    case CMD_SCAN_OBJECT:
        {
            int32_t id;
            packet >> id;

            P<SpaceObject> obj = game_server->getObjectById(id);
            if (obj)
            {
                scanning_target = obj;
                scanning_complexity = obj->scanningComplexity(this);
                scanning_depth = obj->scanningChannelDepth(this);
                scanning_delay = max_scanning_delay;
            }
        }
        break;
    case CMD_SCAN_DONE:
        if (scanning_target && scanning_complexity > 0)
        {
            if (scanning_complexity == scanning_target->scanningComplexity(this) && scanning_depth == scanning_target->scanningChannelDepth(this))
                scanning_target->scannedBy(this);
            scanning_target = nullptr;
        }
        break;
    case CMD_SCAN_CANCEL:
        if (scanning_target && scanning_complexity > 0)
        {
            scanning_target = nullptr;
        }
        break;
    case CMD_SET_SYSTEM_POWER_REQUEST:
        {
            ESystem system;
            float request;
            packet >> system >> request;
            if (system < SYS_COUNT && request >= 0.0f && request <= 3.0f)
                systems[system].power_request = request;
        }
        break;
    case CMD_SET_SYSTEM_COOLANT_REQUEST:
        {
            ESystem system;
            float request;
            packet >> system >> request;
            if (system < SYS_COUNT && request >= 0.0f && request <= 10.0f)
                setSystemCoolantRequest(system, request);
        }
        break;
    case CMD_SET_SYSTEM_POWER_PRESET:
        {
            ESystem system;
            int preset;
            float request;
            packet >> system >> preset >> request;
            if (system < SYS_COUNT && request >= 0.f && request <= 3.f && preset > 0 && preset <= max_engineer_presets_number)
            {
                power_presets[system][preset] = request;
                if (system == 0)
                    addToSpecificShipLog(tr("preset","Preset {id_preset} updated").format({{"id_preset", string(preset+1)}}), colorConfig.log_receive_neutral, "intern");
            }
        }
        break;
    case CMD_SET_SYSTEM_REPAIR_REQUEST:
        {
            ESystem system;
            float request;
            packet >> system >> request;
            if (system < SYS_COUNT && request >= 0.f && request <= max_repair_per_system)
                setSystemRepairRequest(system, request);
        }
        break;
    case CMD_SET_SYSTEM_COOLANT_PRESET:
        {
            ESystem system;
            int preset;
            float request;
            packet >> system >> preset >> request;
            if (system < SYS_COUNT && request >= 0.f && request <= max_coolant_per_system && preset > 0 && preset <= max_engineer_presets_number)
                coolant_presets[system][preset] = request;
        }
        break;
    case CMD_DOCK:
        {
            int32_t id;
            packet >> id;
            requestDock(game_server->getObjectById(id));
            addToSpecificShipLog(tr("Docking procedure asked"),glm::u8vec4(0,255,255,255),"intern");
        }
        break;
    case CMD_LAND:
        {
            int32_t id;
            packet >> id;
            requestLanding(game_server->getObjectById(id));
            addToSpecificShipLog(tr("Landing procedure asked"),glm::u8vec4(0,255,255,255),"intern");
        }
        break;
    case CMD_UNDOCK:
        {
            requestUndock();
            addToSpecificShipLog(tr("Undocking procedure asked"),glm::u8vec4(0,255,255,255),"intern");
        }
        break;
    case CMD_ABORT_DOCK:
        {
            abortDock();
            addToSpecificShipLog(tr("Undocking procedure aborted"),glm::u8vec4(0,255,255,255),"intern");
        }
        break;
    case CMD_ABORT_LANDING:
        {
            abortLanding();
            addToSpecificShipLog(tr("Landing procedure aborted"),glm::u8vec4(0,255,255,255),"intern");
        }
        break;
    case CMD_OPEN_TEXT_COMM:
        comms_incomming_message = "";
        if (comms_state == CS_Inactive || comms_state == CS_BeingHailed || comms_state == CS_BeingHailedByGM || comms_state == CS_ChannelClosed)
        {
            int32_t id;
            packet >> id;
            comms_target = game_server->getObjectById(id);
            if (comms_target)
            {
                comms_generic = false;
                P<PlayerSpaceship> player = comms_target;
                comms_state = CS_OpeningChannel;
                comms_open_delay = comms_channel_open_time;
                comms_target_name = comms_target->getCallSign();
                comms_incomming_message = tr("chatdialog", "Opened comms with {name}").format({{"name", comms_target_name}});
                addToShipLog(tr("shiplog", "Hailing: {name}").format({{"name", comms_target_name}}), colorConfig.log_generic);
            }else{
//                comms_state = CS_Inactive;
                comms_generic = true;
                comms_state = CS_OpeningChannel;
                comms_open_delay = comms_channel_open_time;
                comms_target_name = "";
                comms_incomming_message = tr("chatdialog","Those messages will be broadcasted on all frequencies");
                addToShipLog(tr("Open communications"), colorConfig.log_generic);
            }
        }
        break;
    case CMD_CLOSE_TEXT_COMM:
        closeComms();
        break;
    case CMD_ANSWER_COMM_HAIL:
        comms_incomming_message = "";
        if (comms_state == CS_BeingHailed)
        {
            bool anwser;
            packet >> anwser;
            P<PlayerSpaceship> playerShip = comms_target;

            if (playerShip)
            {
                if (anwser)
                {
                    comms_state = CS_ChannelOpenPlayer;
                    playerShip->comms_state = CS_ChannelOpenPlayer;

                    comms_incomming_message = tr("chatdialog", "Opened comms to {callsign}").format({{"callsign", playerShip->getCallSign()}});
                    playerShip->comms_incomming_message = tr("chatdialog", "Opened comms to {callsign}").format({{"callsign", getCallSign()}});
//                    addToShipLog(tr("shiplog", "Opened communication channel to {callsign}").format({{"callsign", playerShip->getCallSign()}}), colorConfig.log_generic);
//                    playerShip->addToShipLog(tr("shiplog", "Opened communication channel to {callsign}").format({{"callsign", getCallSign()}}), colorConfig.log_generic);
                }else{
//                    addToShipLog(tr("shiplog", "Refused communications from {callsign}").format({{"callsign", playerShip->getCallSign()}}), colorConfig.log_generic);
//                    playerShip->addToShipLog(tr("shiplog", "Refused communications to {callsign}").format({{"callsign", getCallSign()}}), colorConfig.log_generic);
                    comms_state = CS_Inactive;
                    playerShip->comms_state = CS_ChannelFailed;
                }
            }else{
                if (anwser)
                {
                    if (!comms_target)
                    {
                        addToShipLog(tr("shiplog", "Hail suddenly went dead."), colorConfig.log_generic);
                        comms_state = CS_ChannelBroken;
                    }else{
//                        addToShipLog(tr("shiplog", "Accepted hail from {callsign}").format({{"callsign", comms_target->getCallSign()}}), colorConfig.log_generic);
                        comms_reply_id.clear();
                        comms_reply_message.clear();
                        if (comms_incomming_message == "")
                        {
                            if (comms_script_interface.openCommChannel(this, comms_target))
                                comms_state = CS_ChannelOpen;
                            else
                                comms_state = CS_ChannelFailed;
                        }else{
                            // Set the comms message again, so it ends up in
                            // the ship's log.
                            // comms_incomming_message was set by
                            // "hailByObject", without ending up in the log.
                            setCommsMessage(comms_incomming_message);
                            comms_state = CS_ChannelOpen;
                        }
                    }
                }else{
                   // if (comms_target)
                   //     addToShipLog(tr("shiplog", "Refused hail from {callsign}").format({{"callsign", comms_target->getCallSign()}}), colorConfig.log_generic);
                    comms_state = CS_Inactive;
                }
            }
        }
        if (comms_state == CS_BeingHailedByGM)
        {
            bool anwser;
            packet >> anwser;

            if (anwser)
            {
                comms_state = CS_ChannelOpenGM;

                addToShipLog(tr("shiplog", "Opened communication channel to {name}").format({{"name", comms_target_name}}), colorConfig.log_generic);
                comms_incomming_message = tr("chatdialog", "Opened comms with {name}").format({{"name", comms_target_name}});
            }else{
                addToShipLog(tr("shiplog", "Refused hail from {name}").format({{"name", comms_target_name}}), colorConfig.log_generic);
                comms_state = CS_Inactive;
            }
        }
        break;
    case CMD_SEND_TEXT_COMM:
        if (comms_state == CS_ChannelOpen && comms_target)
        {
            uint8_t index;
            packet >> index;
            if (index < comms_reply_id.size())
            {
                addToShipLog(comms_reply_message[index], colorConfig.log_send);

                comms_incomming_message = "?";
                int id = comms_reply_id[index];
                comms_reply_id.clear();
                comms_reply_message.clear();
                comms_script_interface.commChannelMessage(id);
            }
        }
        break;
    case CMD_SEND_TEXT_COMM_PLAYER:
        if (comms_state == CS_ChannelOpenPlayer || comms_state == CS_ChannelOpenGM)
        {
            string message;
            packet >> message;

            addCommsOutgoingMessage(message);
            if (comms_target)
            {
                P<PlayerSpaceship> playership = comms_target;
                if (comms_state == CS_ChannelOpenPlayer && playership)
                    playership->addCommsIncommingMessage(message);
            }else{
                if (!message.upper().startswith("ORGA"))
                    addBroadcast(2,message);
            }
        }
        break;
    case CMD_SET_AUTO_REPAIR:
        packet >> auto_repair_enabled;
		if (auto_repair_enabled)
			addToSpecificShipLog(tr("Automatic repair activated"),glm::u8vec4(255,255,255,255),"intern");
        break;
    case CMD_SET_BEAM_FREQUENCY:
        {
            int32_t new_frequency;
            packet >> new_frequency;
            beam_frequency = new_frequency;
            if (beam_frequency < 0)
                beam_frequency = 0;
            if (beam_frequency > SpaceShip::max_frequency)
                beam_frequency = SpaceShip::max_frequency;
            addToSpecificShipLog(tr("Beam frequencies modified: ") + frequencyToString(new_frequency),glm::u8vec4(255,255,0,255),"intern");
        }
        break;
    case CMD_SET_BEAM_SYSTEM_TARGET:
        {
            ESystem system;
            packet >> system;
            beam_system_target = system;
            if (beam_system_target < SYS_None)
                beam_system_target = SYS_None;
            if (beam_system_target > ESystem(int(SYS_COUNT) - 1))
                beam_system_target = ESystem(int(SYS_COUNT) - 1);
            addToSpecificShipLog(tr("Beam weapon target modified: ") + getLocaleSystemName(system),glm::u8vec4(255,255,0,255),"intern");
        }
        break;
    case CMD_SET_SHIELD_FREQUENCY:
        if (shield_calibration_delay <= 0.0f)
        {
            int32_t new_frequency;
            packet >> new_frequency;
            if (new_frequency != shield_frequency)
            {
                shield_frequency = new_frequency;
                shield_calibration_delay = shield_calibration_time;
                shields_active = false;
                if (shield_frequency < 0)
                    shield_frequency = 0;
                if (shield_frequency > SpaceShip::max_frequency)
                    shield_frequency = SpaceShip::max_frequency;
                addToSpecificShipLog(tr("Shield frequencies modified: ") + frequencyToString(new_frequency),glm::u8vec4(0,255,0,255),"intern");
            }
        }
        break;
    case CMD_ADD_WAYPOINT:
        {
            glm::vec2 position{};
            packet >> position;
            if (waypoints.size() < 9)
                waypoints.push_back(position);
        }
        break;
    case CMD_REMOVE_WAYPOINT:
        {
            int32_t index;
            packet >> index;
            if (index >= 0 && index < int(waypoints.size()))
                waypoints.erase(waypoints.begin() + index);
        }
        break;
    case CMD_MOVE_WAYPOINT:
        {
            int32_t index;
            glm::vec2 position{};
            packet >> index >> position;
            if (index >= 0 && index < int(waypoints.size()))
                waypoints[index] = position;
        }
        break;
    case CMD_ACTIVATE_SELF_DESTRUCT:
        activate_self_destruct = true;
        addToSpecificShipLog(tr("Self destruct activated"),glm::u8vec4(255,0,0,255),"intern");
        for(int n=0; n<max_self_destruct_codes; n++)
        {
            self_destruct_code[n] = irandom(0, 99999);
            self_destruct_code_confirmed[n] = false;
            self_destruct_code_entry_position[n] = max_crew_positions;
            while(self_destruct_code_entry_position[n] == max_crew_positions)
            {
                self_destruct_code_entry_position[n] = ECrewPosition(irandom(0, cagOfficer));
                for(int i=0; i<n; i++)
                    if (self_destruct_code_entry_position[n] == self_destruct_code_entry_position[i])
                        self_destruct_code_entry_position[n] = max_crew_positions;
            }
            self_destruct_code_show_position[n] = max_crew_positions;
            while(self_destruct_code_show_position[n] == max_crew_positions)
            {
                self_destruct_code_show_position[n] = ECrewPosition(irandom(0, cagOfficer));
                if (self_destruct_code_show_position[n] == self_destruct_code_entry_position[n])
                    self_destruct_code_show_position[n] = max_crew_positions;
                for(int i=0; i<n; i++)
                    if (self_destruct_code_show_position[n] == self_destruct_code_show_position[i])
                        self_destruct_code_show_position[n] = max_crew_positions;
            }
        }
        break;
    case CMD_CANCEL_SELF_DESTRUCT:
        if (self_destruct_countdown <= 0.0f)
        {
            activate_self_destruct = false;
            addToSpecificShipLog(tr("Self destruct aborted"),glm::u8vec4(255,0,0,255),"intern");
        }
        break;
    case CMD_CONFIRM_SELF_DESTRUCT:
        {
            int8_t index;
            uint32_t code;
            packet >> index >> code;
            if (index >= 0 && index < max_self_destruct_codes && self_destruct_code[index] == code)
                self_destruct_code_confirmed[index] = true;
        }
        break;
    case CMD_COMBAT_MANEUVER_BOOST:
        {
            float request_amount;
            packet >> request_amount;
            if (request_amount >= 0.0f && request_amount <= 1.0f)
                combat_maneuver_boost_request = request_amount;
        }
        break;
    case CMD_COMBAT_MANEUVER_STRAFE:
        {
            float request_amount;
            packet >> request_amount;
            if (request_amount >= -1.0f && request_amount <= 1.0f)
                combat_maneuver_strafe_request = request_amount;
        }
        break;
    case CMD_LAUNCH_PROBE:
        if (scan_probe_stock > 0)
        {
            glm::vec2 target{};
            packet >> target;
            P<ScanProbe> p = new ScanProbe();
            p->setPosition(getPosition());
            p->setTarget(target);
            p->setOwner(this);
            if (on_probe_launch.isSet())
            {
                on_probe_launch.call<void>(P<PlayerSpaceship>(this), P<ScanProbe>(p));
            }
            scan_probe_stock--;
        }
        break;
    case CMD_LAUNCH_CARGO:
        int dockIndex;
        packet >> dockIndex;
        if (docks[dockIndex].state == EDockState::Docked
            && docks[dockIndex].getCargo()->onLaunch(docks[dockIndex]))
        {
            docks[dockIndex].getCargo()->destroy();
            docks[dockIndex].empty();
        }
        break;
    case CMD_SET_DOCK_MOVE_TARGET:
        {
            int srcIdx, destIdx;
            packet >> srcIdx >> destIdx;
            Dock& src = docks[srcIdx];
            src.setMoveTarget(destIdx);
        }
        break;
        break;
    case CMD_MOVE_CARGO:
        {
            int index;
            packet >> index;
            Dock& src = docks[index];
            src.startMoveCargo();
        }
        break;
    case CMD_CANCEL_MOVE_CARGO:
        {
            int index;
            packet >> index;
            Dock& src = docks[index];
            src.cancelMoveCargo();
        }
        break;
    case CMD_SET_DOCK_ENERGY_REQUEST:
        packet >> dockIndex;
        if (docks[dockIndex].state == EDockState::Docked && 
        ((docks[dockIndex].dock_type == Dock_Energy) || (docks[dockIndex].dock_type == Dock_Maintenance)) )
        {
            float value;
            packet >> value;
            docks[dockIndex].energy_request = value;
        }
        break;
    case CMD_SET_ALERT_LEVEL:
        {
            packet >> alert_level;
            if(alertLevelToString(alert_level) == "RED ALERT")
                addToSpecificShipLog(tr("Red alert"),glm::u8vec4(255,0,0,255),"intern");
            if(alertLevelToString(alert_level) == "YELLOW ALERT")
                addToSpecificShipLog(tr("Yellow alert"),glm::u8vec4(255,255,0,255),"intern");
        }
        break;
    case CMD_SET_SCIENCE_LINK:
        {
            // Capture previously linked probe, if there is one.
            P<ScanProbe> old_linked_probe;

            if (linked_science_probe_id != -1)
            {
                old_linked_probe = game_server->getObjectById(linked_science_probe_id);
            }

            packet >> linked_science_probe_id;

            if (linked_science_probe_id != -1 && on_probe_link.isSet())
            {
                P<ScanProbe> new_linked_probe = game_server->getObjectById(linked_science_probe_id);

                if (new_linked_probe)
                {
                    on_probe_link.call<void>(P<PlayerSpaceship>(this), P<ScanProbe>(new_linked_probe));
                }
            }
            else if (linked_science_probe_id == -1 && on_probe_unlink.isSet())
            {
                on_probe_unlink.call<void>(P<PlayerSpaceship>(this), P<ScanProbe>(old_linked_probe));
            }
        }
        break;
    case CMD_SET_PROBE_3D_LINK:
        {
            packet >> linked_probe_3D_id;
        }
        break;    
    case CMD_SET_ANALYSIS_LINK:
        {
            packet >> linked_analysis_object_id;
        }
        break;
    case CMD_HACKING_FINISHED:
        {
            int32_t id;
            string target_system;
            string target_faction;
            packet >> id >> target_system >> target_faction;
            P<SpaceObject> obj = game_server->getObjectById(id);
            if (obj)
            {
                if (target_system != "")
                    obj->hackFinished(this, target_system);
                if (target_faction != "")
                    obj->setFactionId(FactionInfo::findFactionId(target_faction));
            }
        }
        break;
    case CMD_CUSTOM_FUNCTION:
        {
            string name;
            packet >> name;
            for(CustomShipFunction& csf : custom_functions)
            {
                if (csf.name == name)
                {
                    if (csf.type == CustomShipFunction::Type::Button)
                    {
                        auto cb = csf.callback;
                        cb.call<void>();
                    }
                    else if (csf.type == CustomShipFunction::Type::Message)
                    {
                        auto cb = csf.callback;
                        cb.call<void>();
                        removeCustom(name);
                    }
                    break;
                }
            }
        }
        break;
    case CMD_SET_AUTO_REPAIR_SYSTEM_TARGET:
        {
            ESystem system;
            packet >> system;
            if (system < SYS_COUNT)
                auto_repairing_system = system;
        }
        break;
    case CMD_LAUNCH_SQUADRON:
        //TODO : mettre ça dans un dock
        {
            unsigned int deck;
            string name;
            packet >> deck >> name;
            launchWaitingSquadron(deck, name);
        }
        break;
    case CMD_ORDER_SQUADRON_TARGET:
        {
            uint32_t order;
            unsigned int idx;
            int32_t id_obj;
            packet >> order >> idx >> id_obj;
            P<SpaceObject> obj = (game_server ? game_server->getObjectById(id_obj) : game_client->getObjectById(id_obj));
            orderSquadron((EAIOrder)order, idx, obj);
        }
        break;
    case CMD_ORDER_SQUADRON_POSITION:
        {
            uint32_t order;
            unsigned int idx;
            glm::vec2 pos;
            packet >> order >> idx >> pos;
            orderSquadron((EAIOrder)order, idx, pos);
        }
        break;
    case CMD_SQUADRON_BLUEPRINT_ACTIVATION:
        {
            int idx;
            bool val;
            packet >> idx >> val;
            squadron_bp_activated[idx] = val;
            if(val == false)
            {
                squadron_delay_to_next_creation[idx] = ship_template->squadrons_compositions[idx].construction_duration;
            }
        }
        break;
    case CMD_AMMO_BLUEPRINT_ACTIVATION:
        {
            string idx_str;
            bool val;
            packet >> idx_str >> val;
            ammo_bp_activated[idx_str] = val;
            if(val == false)
            {
                ammo_delay_to_next_creation[idx_str] = ship_template->ammo_blueprints[idx_str].construction_duration;
            }
        }
        break;
    }
}

void PlayerSpaceship::orderSquadron(EAIOrder order, unsigned int idx, P<SpaceObject> &obj)
{
    //attack, defendtarget dock

    for(auto &lq : launched_squadrons)
    {
        for(auto &ship : lq.ships)
        {
            if(obj == ship)
            {
                LOG(WARNING) << "Can't put order on another ship squadron, fly towards instead";
                orderSquadron(AI_FlyTowards, idx, obj->getPosition());
                return;
            }
        }
    }

    if(idx < launched_squadrons.size())
    {
        for(auto & ship : launched_squadrons[idx].ships)
        {
            if(!ship)
            {
                continue;
            }
            switch(EAIOrder(order))
            {
                case AI_Attack:
                {
                    ship->orderAttack(obj);
                }
                break;
                case AI_DefendTarget:
                {
                    ship->orderDefendTarget(obj);
                }
                break;
                case AI_Dock:
                {
                    ship->orderDock(this);
                }
                break;
                default:
                {
                    LOG(WARNING) << "Unknown order for squadron";
                    break;
                }

            }

        }
    }
}

void PlayerSpaceship::orderSquadron(EAIOrder order, unsigned int idx, const glm::vec2 &pos)
{
    //FlyTowards, FlyTowardsBlind
    if(idx < launched_squadrons.size())
    {
        for(auto & ship : launched_squadrons[idx].ships)
        {
            if(!ship)
            {
                continue;
            }
            switch(EAIOrder(order))
            {
                case AI_FlyTowards:
                {
                    ship->orderFlyTowards(pos);
                }
                break;
                case AI_FlyTowardsBlind:
                {
                    ship->orderFlyTowardsBlind(pos);
                }
                break;
                default:
                {
                    LOG(WARNING) << "Unknown order for squadron";
                    break;
                }
            }
        }
    }
}

void PlayerSpaceship::launchWaitingSquadron(unsigned int deck, const string &template_name)
{
    int n =0;
    for(auto& sqt : ship_template->squadrons_compositions)
    {
        if(sqt.template_name == template_name)
        {
            if((number_of_waiting_squadron_for_bp[n] > 0)
            &&(canLaunchSquadron()))
            {
                number_of_waiting_squadron_for_bp[n]--;
                bp_of_launching_squadron[deck] = n;
                launch_delay[deck] = launch_duration;
            }
            break;
        }
        n++;
    }
}


// Client-side functions to send a command to the server.
void PlayerSpaceship::commandTargetRotation(float target)
{
    sp::io::DataBuffer packet;
    packet << CMD_TARGET_ROTATION << target;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandTurnSpeed(float turnSpeed)
{
    sp::io::DataBuffer packet;
    packet << CMD_TURN_SPEED << turnSpeed;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandImpulse(float target)
{
    sp::io::DataBuffer packet;
    packet << CMD_IMPULSE << target;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandWarp(int8_t target)
{
    sp::io::DataBuffer packet;
    packet << CMD_WARP << target;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandJump(float distance)
{
    sp::io::DataBuffer packet;
    packet << CMD_JUMP << distance;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetTarget(P<SpaceObject> target)
{
    sp::io::DataBuffer packet;
    if (target)
        packet << CMD_SET_TARGET << target->getMultiplayerId();
    else
        packet << CMD_SET_TARGET << int32_t(-1);
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetDockTarget(P<SpaceObject> target)
{
    sp::io::DataBuffer packet;
    if (target)
        packet << CMD_SET_DOCK_TARGET << target->getMultiplayerId();
    else
        packet << CMD_SET_DOCK_TARGET << int32_t(-1);
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetLandingTarget(P<SpaceObject> target)
{
    sp::io::DataBuffer packet;
    if (target)
        packet << CMD_SET_LANDING_TARGET << target->getMultiplayerId();
    else
        packet << CMD_SET_LANDING_TARGET << int32_t(-1);
    sendClientCommand(packet);
}

void PlayerSpaceship::commandLoadTube(int8_t tubeNumber, string missileType)
{
    sp::io::DataBuffer packet;
    packet << CMD_LOAD_TUBE << tubeNumber << missileType;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandUnloadTube(int8_t tubeNumber)
{
    sp::io::DataBuffer packet;
    packet << CMD_UNLOAD_TUBE << tubeNumber;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandFireTube(int8_t tubeNumber, float missile_target_angle)
{
    sp::io::DataBuffer packet;
    packet << CMD_FIRE_TUBE << tubeNumber << missile_target_angle;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandFireTubeAtTarget(int8_t tubeNumber, P<SpaceObject> target)
{
  float targetAngle = 0.0;

  if (!target || tubeNumber < 0 || tubeNumber >= getWeaponTubeCount())
    return;

  targetAngle = weapon_tube[tubeNumber].calculateFiringSolution(target);

  if (targetAngle == std::numeric_limits<float>::infinity())
      targetAngle = getRotation() + weapon_tube[tubeNumber].getDirection();

  commandFireTube(tubeNumber, targetAngle);
}

void PlayerSpaceship::commandSetShields(bool enabled)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_SHIELDS << enabled;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandMainScreenSetting(EMainScreenSetting mainScreen)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_MAIN_SCREEN_SETTING << mainScreen;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandMainScreenOverlay(EMainScreenOverlay mainScreen)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_MAIN_SCREEN_OVERLAY << mainScreen;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandScan(P<SpaceObject> object)
{
    sp::io::DataBuffer packet;
    packet << CMD_SCAN_OBJECT << object->getMultiplayerId();
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetSystemPowerRequest(ESystem system, float power_request)
{
    sp::io::DataBuffer packet;
    systems[system].power_request = power_request;
    packet << CMD_SET_SYSTEM_POWER_REQUEST << system << power_request;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetSystemPowerPreset(ESystem system, int preset, float power_preset)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_SYSTEM_POWER_PRESET << system << preset << power_preset;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetSystemCoolantRequest(ESystem system, float coolant_request)
{
    sp::io::DataBuffer packet;
    systems[system].coolant_request = coolant_request;
    packet << CMD_SET_SYSTEM_COOLANT_REQUEST << system << coolant_request;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetSystemCoolantPreset(ESystem system, int preset, float coolant_preset)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_SYSTEM_COOLANT_PRESET << system << preset << coolant_preset;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetSystemRepairRequest(ESystem system, float repair_request)
{
    sp::io::DataBuffer packet;
    systems[system].repair_request = repair_request;
    packet << CMD_SET_SYSTEM_REPAIR_REQUEST << system << repair_request;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandDock(P<SpaceObject> object)
{
    if (!object) return;
    sp::io::DataBuffer packet;
    packet << CMD_DOCK << object->getMultiplayerId();
    sendClientCommand(packet);
}

void PlayerSpaceship::commandUndock()
{
    sp::io::DataBuffer packet;
    packet << CMD_UNDOCK;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAbortDock()
{
    sp::io::DataBuffer packet;
    packet << CMD_ABORT_DOCK;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandLand(P<SpaceObject> object)
{
    if (!object) return;
    sp::io::DataBuffer packet;
    packet << CMD_LAND << object->getMultiplayerId();
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAbortLanding()
{
    sp::io::DataBuffer packet;
    packet << CMD_ABORT_LANDING;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandOpenTextComm(P<SpaceObject> obj)
{
//    if (!obj) return;
    if (obj)
    {
        sp::io::DataBuffer packet;
        packet << CMD_OPEN_TEXT_COMM << obj->getMultiplayerId();
        sendClientCommand(packet);
    }
    else
    {
        sp::io::DataBuffer packet;
        packet << CMD_OPEN_TEXT_COMM;
        sendClientCommand(packet);
    }

}

void PlayerSpaceship::commandCloseTextComm()
{
    sp::io::DataBuffer packet;
    packet << CMD_CLOSE_TEXT_COMM;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAnswerCommHail(bool awnser)
{
    sp::io::DataBuffer packet;
    packet << CMD_ANSWER_COMM_HAIL << awnser;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSendComm(uint8_t index)
{
    sp::io::DataBuffer packet;
    packet << CMD_SEND_TEXT_COMM << index;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSendCommPlayer(string message)
{
    sp::io::DataBuffer packet;
    packet << CMD_SEND_TEXT_COMM_PLAYER << message;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetAutoRepair(bool enabled)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_AUTO_REPAIR << enabled;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetBeamFrequency(int32_t frequency)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_BEAM_FREQUENCY << frequency;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetBeamSystemTarget(ESystem system)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_BEAM_SYSTEM_TARGET << system;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetShieldFrequency(int32_t frequency)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_SHIELD_FREQUENCY << frequency;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAddWaypoint(glm::vec2 position)
{
    sp::io::DataBuffer packet;
    packet << CMD_ADD_WAYPOINT << position;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandRemoveWaypoint(int32_t index)
{
    sp::io::DataBuffer packet;
    packet << CMD_REMOVE_WAYPOINT << index;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandMoveWaypoint(int32_t index, glm::vec2 position)
{
    sp::io::DataBuffer packet;
    packet << CMD_MOVE_WAYPOINT << index << position;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandActivateSelfDestruct()
{
    sp::io::DataBuffer packet;
    packet << CMD_ACTIVATE_SELF_DESTRUCT;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCancelSelfDestruct()
{
    sp::io::DataBuffer packet;
    packet << CMD_CANCEL_SELF_DESTRUCT;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandConfirmDestructCode(int8_t index, uint32_t code)
{
    sp::io::DataBuffer packet;
    packet << CMD_CONFIRM_SELF_DESTRUCT << index << code;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCombatManeuverBoost(float amount)
{
    combat_maneuver_boost_request = amount;
    sp::io::DataBuffer packet;
    packet << CMD_COMBAT_MANEUVER_BOOST << amount;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCombatManeuverStrafe(float amount)
{
    combat_maneuver_strafe_request = amount;
    sp::io::DataBuffer packet;
    packet << CMD_COMBAT_MANEUVER_STRAFE << amount;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandLaunchProbe(glm::vec2 target_position)
{
    sp::io::DataBuffer packet;
    packet << CMD_LAUNCH_PROBE << target_position;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandLaunchCargo(int index)
{
    sp::io::DataBuffer packet;
    packet << CMD_LAUNCH_CARGO << index;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandLaunchSquadron(unsigned int deck, const string& identifier)
{
    sp::io::DataBuffer packet;
    packet << CMD_LAUNCH_SQUADRON << deck << identifier;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandOrderSquadronTarget(EAIOrder order, unsigned int idx, P<SpaceObject> &obj)
{
    sp::io::DataBuffer packet;
    packet << CMD_ORDER_SQUADRON_TARGET << uint32_t(order) << idx << obj->getMultiplayerId();
    sendClientCommand(packet);
}

void PlayerSpaceship::commandOrderSquadronPosition(EAIOrder order, unsigned int idx, const glm::vec2 &pos)
{
    sp::io::DataBuffer packet;
    packet << CMD_ORDER_SQUADRON_POSITION << uint32_t(order) << idx << pos;
    sendClientCommand(packet);
}


void PlayerSpaceship::commandSetSquadronBlueprintActivation(int idx, bool val)
{
    sp::io::DataBuffer packet;
    packet << CMD_SQUADRON_BLUEPRINT_ACTIVATION << idx << val;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetAmmoBlueprintActivation(string idx_str, bool val)
{
    sp::io::DataBuffer packet;
    packet << CMD_AMMO_BLUEPRINT_ACTIVATION << idx_str << val;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandMoveCargo(int index)
{
    sp::io::DataBuffer packet;
    packet << CMD_MOVE_CARGO << index;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCancelMoveCargo(int index)
{
    sp::io::DataBuffer packet;
    packet << CMD_CANCEL_MOVE_CARGO << index;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetDockMoveTarget(int srcIdx, int destIdx)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_DOCK_MOVE_TARGET << srcIdx << destIdx;
    sendClientCommand(packet);
}
void PlayerSpaceship::commandSetDockEnergyRequest(int index, float value)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_DOCK_ENERGY_REQUEST << index << value;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandScanDone()
{
    sp::io::DataBuffer packet;
    packet << CMD_SCAN_DONE;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandScanCancel()
{
    sp::io::DataBuffer packet;
    packet << CMD_SCAN_CANCEL;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetAlertLevel(EAlertLevel level)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_ALERT_LEVEL;
    packet << level;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandHackingFinished(P<SpaceObject> target, string target_system, string target_faction)
{
    sp::io::DataBuffer packet;
    packet << CMD_HACKING_FINISHED;
    packet << target->getMultiplayerId();
    packet << target_system;
    packet << target_faction;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCustomFunction(string name)
{
    sp::io::DataBuffer packet;
    packet << CMD_CUSTOM_FUNCTION;
    packet << name;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetScienceLink(P<ScanProbe> probe)
{
    sp::io::DataBuffer packet;

    // Pass the probe's multiplayer ID if the probe isn't nullptr.
    if (probe)
    {
        packet << CMD_SET_SCIENCE_LINK;
        packet << probe->getMultiplayerId();
        sendClientCommand(packet);
    }
    // Otherwise, it's invalid. Warn and do nothing.
    else
    {
        LOG(WARNING) << "commandSetScienceLink received a null or invalid ScanProbe, so no command was sent.";
    }
}

void PlayerSpaceship::commandClearScienceLink()
{
    sp::io::DataBuffer packet;

    packet << CMD_SET_SCIENCE_LINK;
    packet << int32_t(-1);
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetProbe3DLink(int32_t id){
    sp::io::DataBuffer packet;
    packet << CMD_SET_PROBE_3D_LINK << id;
    sendClientCommand(packet);
}
void PlayerSpaceship::commandSetAnalysisLink(int32_t id){
    sp::io::DataBuffer packet;
    packet << CMD_SET_ANALYSIS_LINK << id;
    sendClientCommand(packet);
}

 void PlayerSpaceship::commandSetAutoRepairSystemTarget(ESystem system)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_AUTO_REPAIR_SYSTEM_TARGET << system;
    sendClientCommand(packet);
}

void PlayerSpaceship::onReceiveServerCommand(sp::io::DataBuffer& packet)
{
    int16_t command;
    packet >> command;
    switch(command)
    {
    case CMD_PLAY_CLIENT_SOUND:
        if (my_spaceship == this && my_player_info)
        {
            ECrewPosition position;
            string sound_name;
            packet >> position >> sound_name;
            if ((position == max_crew_positions && my_player_info->main_screen) || (position < sizeof(my_player_info->crew_position) && my_player_info->crew_position[position]))
            {
                soundManager->playSound(sound_name);
            }
        }
        break;
    }
}

void PlayerSpaceship::drawOnGMRadar(sp::RenderTarget& renderer, glm::vec2 position, float scale, float rotation, bool long_range)
{
    if (docked_style == DockStyle::Internal) return;

    SpaceShip::drawOnGMRadar(renderer, position, scale, rotation, long_range);
    if (long_range)
    {
        // Draw long-range radar radius indicator
        renderer.drawCircleOutline(position, getLongRangeRadarRange() * scale, 3.0, glm::u8vec4(255, 255, 255, 64));

        // Draw short-range radar radius indicator
        renderer.drawCircleOutline(position, getShortRangeRadarRange() * scale, 3.0, glm::u8vec4(255, 255, 255, 64));

    }
}

bool PlayerSpaceship::canBeLandedOn(P<SpaceObject> obj)
{
    if (isEnemy(obj) || !ship_template)
        return false;
    P<SpaceShip> ship = obj;
    if (!ship || !ship->ship_template)
        return false;
    if(getMultiplayerId() == ship->getMultiplayerId())
        return false;
    if (ship->getFactionId() != getFactionId())
        return false;
    Dock* dock = Dock::findOpenForDocking(docks, max_docks_count);
    if (!dock)
        return false; //TODO message dans le log
    // return ship_template->can_be_docked_by_class.count(ship->ship_template->getClass()) > 0;
    return true;
}

float PlayerSpaceship::getLongRangeRadarRange()
{
    if(hasSystem(SYS_Drones) && getSystemEffectiveness(SYS_Drones) < 0.1f)
        return long_range_radar_range * std::sqrt(0.1f); //backup generator
    return hasSystem(SYS_Drones) ? long_range_radar_range * std::sqrt(getSystemEffectiveness(SYS_Drones)) : long_range_radar_range;
}

float PlayerSpaceship::getShortRangeRadarRange()
{
    if(hasSystem(SYS_Drones) && getSystemEffectiveness(SYS_Drones) < 0.1f)
        return short_range_radar_range * std::sqrt(0.1f); //backup generator
    return hasSystem(SYS_Drones) ? short_range_radar_range * std::sqrt(getSystemEffectiveness(SYS_Drones)) : short_range_radar_range;
}

float PlayerSpaceship::getProbeRangeRadarRange()
{
    return short_range_radar_range;
}

string PlayerSpaceship::getExportLine()
{
    string result = "PlayerSpaceship():setTemplate(\"" + template_name + "\"):setPosition(" + string(getPosition().x, 0) + ", " + string(getPosition().y, 0) + ")" + getScriptExportModificationsOnTemplate();
    if (short_range_radar_range != ship_template->short_range_radar_range)
        result += ":setShortRangeRadarRange(" + string(short_range_radar_range, 0) + ")";
    if (long_range_radar_range != ship_template->long_range_radar_range)
        result += ":setLongRangeRadarRange(" + string(long_range_radar_range, 0) + ")";
    if (can_scan != ship_template->can_scan)
        result += ":setCanScan(" + string(can_scan, true) + ")";
    if (can_hack != ship_template->can_hack)
        result += ":setCanHack(" + string(can_hack, true) + ")";
    if (can_dock != ship_template->can_dock)
        result += ":setCanDock(" + string(can_dock, true) + ")";
    if (can_combat_maneuver != ship_template->can_combat_maneuver)
        result += ":setCanCombatManeuver(" + string(can_combat_maneuver, true) + ")";
    if (can_self_destruct != ship_template->can_self_destruct)
        result += ":setCanSelfDestruct(" + string(can_self_destruct, true) + ")";
    if (can_launch_probe != ship_template->can_launch_probe)
        result += ":setCanLaunchProbe(" + string(can_launch_probe, true) + ")";
    if (auto_coolant_enabled)
        result += ":setAutoCoolant(true)";
    if (auto_repair_enabled)
        result += ":commandSetAutoRepair(true)";

    // Update power factors, only for the systems where it changed.
    for (unsigned int sys_index = 0; sys_index < SYS_COUNT; ++sys_index)
    {
        auto system = static_cast<ESystem>(sys_index);
        if (hasSystem(system))
        {
            SDL_assert(sys_index < default_system_power_factors.size());
            auto default_factor = default_system_power_factors[sys_index];
            auto current_factor = getSystemPowerFactor(system);
            auto difference = std::fabs(current_factor - default_factor) > std::numeric_limits<float>::epsilon();
            if (difference)
            {
                result += ":setSystemPowerFactor(\"" + getSystemName(system) + "\", " + string(current_factor, 1) + ")";
            }

            if (std::fabs(getSystemCoolantRate(system) - ShipSystem::default_coolant_rate_per_second) > std::numeric_limits<float>::epsilon())
            {
                result += ":setSystemCoolantRate(\"" + getSystemName(system) + "\", " + string(getSystemCoolantRate(system), 2) + ")";
            }

            if (std::fabs(getSystemHeatRate(system) - ShipSystem::default_heat_rate_per_second) > std::numeric_limits<float>::epsilon())
            {
                result += ":setSystemHeatRate(\"" + getSystemName(system) + "\", " + string(getSystemHeatRate(system), 2) + ")";
            }

            if (std::fabs(getSystemPowerRate(system) - ShipSystem::default_power_rate_per_second) > std::numeric_limits<float>::epsilon())
            {
                result += ":setSystemPowerRate(\"" + getSystemName(system) + "\", " + string(getSystemPowerRate(system), 2) + ")";
            }
        }
    }

    if (std::fabs(getEnergyShieldUsePerSecond() - default_energy_shield_use_per_second) > std::numeric_limits<float>::epsilon())
    {
        result += ":setEnergyShieldUsePerSecond(" + string(getEnergyShieldUsePerSecond(), 2) + ")";
    }

    if (std::fabs(getEnergyWarpPerSecond() - default_energy_warp_per_second) > std::numeric_limits<float>::epsilon())
    {
        result += ":setEnergyWarpPerSecond(" + string(getEnergyWarpPerSecond(), 2) + ")";
    }
    return result;
}

float PlayerSpaceship::getDronesControlRange() 
{
    return Tween<float>::easeInQuad(getSystemEffectiveness(SYS_Drones), 0.0, 1.5, 0.001, getLongRangeRadarRange());
}

void PlayerSpaceship::onProbeLaunch(ScriptSimpleCallback callback)
{
    this->on_probe_launch = callback;
}

void PlayerSpaceship::onProbeLink(ScriptSimpleCallback callback)
{
    this->on_probe_link = callback;
}

void PlayerSpaceship::onProbeUnlink(ScriptSimpleCallback callback)
{
    this->on_probe_unlink = callback;
}

void PlayerSpaceship::activateModifier(string name)
{ 
    if(!on_modifier_toggle.isSet())
        return;
    if (auto search = modifiers_to_data.find(name); search != modifiers_to_data.end())
    {
        search->second.activated = true;
        on_modifier_toggle.call<void>(P<PlayerSpaceship>(this), name, string("activated"));
    }
}

void PlayerSpaceship::deActivateModifier(string name)
{ 
    if(!on_modifier_toggle.isSet())
        return;
    if (auto search = modifiers_to_data.find(name); search != modifiers_to_data.end())
    {
        search->second.activated = false;
        on_modifier_toggle.call<void>(P<PlayerSpaceship>(this), name, string("deactivated"));
    }
}

void PlayerSpaceship::instantiateSquadron(const string& compo_identifier)
{
    int n = 0;
    for(auto &sqt : ship_template->squadrons_compositions)
    {
        if(sqt.template_name == compo_identifier)
        {
            number_of_waiting_squadron_for_bp[n]++;
            return;
        }
        n++;
    }
    LOG(ERROR) << "Failed to find ship template for squadron : " << compo_identifier;
    
}


void PlayerSpaceship::launchSquadron(unsigned int deck)
{
    if(game_server)
    {
        int bp_idx = bp_of_launching_squadron[deck];
        if(bp_idx < 0)
        {
            LOG(WARNING) << "No more squadron to launch";
            return;
        }

        SquadronTemplate sqt = ship_template->squadrons_compositions[bp_idx];

        Squadron squadron;
        string squadron_callsign = gameGlobalInfo->getNextShipCallsign();
        squadron.squadron_name = sqt.template_name + "-" +squadron_callsign;
        squadron.squadron_template_name = sqt.template_name;
        
        unsigned int n=1;
        for(const string &template_name : sqt.ship_names)
        {
            std::vector<string> v = ShipTemplate::getAllTemplateNames();
            if(std::find(v.begin(), v.end(), template_name) != v.end())
            {
                P<CpuShip> ship = new CpuShip();
                ship->setCallSign(squadron_callsign+"-"+string(n));
                ship->setTemplate(template_name);
                ship->setPosition(getPosition());
                ship->setFaction(getFaction());
                ship->orderDefendTarget(this);
                //ship->setScannedByFaction(getFaction(), true);
                ship->setScannedStateForFaction(FactionInfo::findFactionId(getFaction()), SS_SimpleScan);
                ship->setAllowAttackAlly();
                squadron.ships.push_back(ship);
                // if(n > 0)
                // {
                //     ship->orderFlyFormation(squadron.ships[0]);
                // }
                n++;
            }
            
            else
            {
                LOG(ERROR) << "Failed to find ship template for squadron creation : " << template_name << ", squadron template name : " << sqt.template_name;
            }
        }
        bp_of_launching_squadron[deck] = -1;
        launched_squadrons.push_back(squadron);
    }
}

#include "playerSpaceship.hpp"
