#include "spaceObject.h"
#include "factionInfo.h"
#include "gameGlobalInfo.h"
#include "explosionEffect.h"

#include "scriptInterface.h"
/// The SpaceObject is the base for every object which can be seen in space.
/// General properties can read and set for each object. Each object has a position, rotation and collision shape.
REGISTER_SCRIPT_CLASS_NO_CREATE(SpaceObject)
{
    /// Set the position of this object in 2D space, in meters
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, setPosition);
    /// Sets the absolute rotation of this object. In degrees.
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, setRotation);
    /// Gets the position of this object, returns x, y
    /// Example: local x, y = obj:getPosition()
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, getPosition);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getRadius);
    /// Gets the rotation of this object. In degrees. 0 degrees is pointing to the right of the world. So this does not match the heading of a ship.
    /// The value returned here can also go below 0 degrees or higher then 360 degrees, there is no limiting on the rotation.
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, getRotation);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setCorrectionX);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setCorrectionY);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getCorrectionX);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getCorrectionY);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setGalaxyId);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getGalaxyId);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setTranslateZ);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getTranslateZ);
    /// Get the heading of this object, in the range of 0 to 360. The heading is 90 degrees off from the rotation.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getHeading);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setHeading);
    /// Gets the velocity of the object, in 2D space, in meters/second
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, getVelocity);
    /// Gets the rotational velocity of the object, in degree/second
    REGISTER_SCRIPT_CLASS_FUNCTION(Collisionable, getAngularVelocity);

    /// Sets the faction to which this object belongs. Requires a string as input.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setFaction);
    /// Gets the faction name to which this object belongs.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getFaction);
    /// Gets the localized name of the faction to which this object belongs, for displaying to the players.
    /// Example: local faction = obj:getLocaleFaction()
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getLocaleFaction);
    /// Sets the faction to which this object belongs, by the faction's index
    /// in the faction list.
    /// Requires the index of a faction in the faction list.
    /// Example: local faction_id = obj:getFactionId()
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setFactionId);
    /// Gets the index in the faction list from this object.
    /// Can be used in combination with setFactionId to make sure two objects have the same faction.
    /// Example: other:setFactionId(obj:getFactionId())
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getFactionId);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getDockId);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setDockId);

	/// Sets the personality of an object. Requires a index in the personality list.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setPersonalityId);
    /// Gets the personality name to which this object belongs.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getPersonality);
    /// Gets the personality ID to which this object belongs.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getPersonalityId);
    
    /// Gets the friend-or-foe status of the parameter's faction relative to
    /// this object's faction.
    /// Requires a SpaceObject.
    /// Returns true if the parameter's faction is hostile to this object's.
    /// Example: local is_enemy = obj:isEnemy()
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, isEnemy);
    /// Requires a SpaceObject.
    /// Returns true if the parameter's faction is friendly to this object's.
    /// If an object is neither friendly nor enemy, it is neutral.
    /// Example: local is_friendly = obj:isFriendly()
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, isFriendly);
    /// Sets the communications script used when this object is hailed.
    /// Accepts the filename of a Lua script as a string, or can be set to an
    /// empty string to disable comms with this object.
    /// In the script, `comms_source` (or `player`, deprecated) (PlayerSpaceship)
    /// and `comms_target` (SpaceObject) are available.
    /// Compare `setCommsFunction`.
    /// Examples:
    ///   obj:setCommsScript("")
    ///   obj:setCommsScript("comms_custom_script.lua")
    /// Defaults:
    ///   "comms_station.lua" (in `spaceStation.cpp`)
    ///   "comms_ship.lua" (in `cpuShip.cpp`)
    /// Call `setCommsMessage` once and `addCommsReply` zero or more times in each dialogue.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setCommsScript);
    /// Defines a callback function to use when handling hails, in lieu of any
    /// current or default comms script.
    /// For a detailed example, see scenario_53_escape.lua.
    /// Requires a function to call back to when hailed.
    /// The function has no parameters, but `comms_source` (PlayerSpaceship)
    /// and `comms_target` (SpaceObject) are available.
    /// Compare `setCommsScript`.
    /// Example: obj:setCommsFunction(commsStation)
    /// where commsStation is a function
    /// calling `setCommsMessage` once and `addCommsReply` zero or more times.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setCommsFunction);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, isEnemy);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, isFriendly);
    /// Set a custom callsign for this object. Objects get assigned random callsigns at creation, but you can overrule this from scenario scripts.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setCallSign);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getCallSign);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, areEnemiesInRange);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getObjectsInRange);
    /// Sets the reputation to a value.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setReputationPoints);
    /// Return the current amount of reputation points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getReputationPoints);
    /// Take a certain amount of reputation points, returns true when there are enough points to take. Returns false when there are not enough points and does not lower the points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, takeReputationPoints);
    /// Add a certain amount of reputation points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, addReputationPoints);
     /// Sets the oxygen max to a value.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setOxygenMax);
    /// Return the max amount of oxygen points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getOxygenMax);
    /// Set the rate of oxygen points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setOxygenRate);
    /// return the rate of oxygen points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getOxygenRate);
     /// Sets the oxygen to a value.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setOxygenPoints);
    /// Return the current amount of oxygen points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getOxygenPoints);
    /// Return the current total of oxygen points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getOxygenTotal);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getOxygenMaxTotal);
    /// Take a certain amount of oxygen points, returns true when there are enough points to take. Returns false when there are not enough points and does not lower the points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, takeOxygenPoints);
    /// Add a certain amount of Oxygen points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, addOxygenPoints);
    /// Remove a certain amount of Oxygen points.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject,removeOxygenPoints);
    /// Get the name of the sector this object is in (A4 for example)
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getSectorName);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getSectorNameLevel);
    /// Hail a player ship from this object. The ship will get a notification and can accept or deny the hail.
    /// Warning/ToFix: If the player refuses the hail, no feedback is given to the script in any way.
    /// Return true when the hail is enabled with succes. Returns false when the target player cannot be hailed right now (because it's already communicating with something else)
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, openCommsTo);
    /// Hail a player ship from this object. The ship will get a notification and can accept or deny the hail.
    /// Warning/ToFix: If the player refuses the hail, no feedback is given to the script in any way.
    /// Return true when the hail is enabled with succes. Returns false when the target player cannot be hailed right now (because it's already communicating with something else)
    /// This function will display the message given as parameter when the hail is answered.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, sendCommsMessage);
    /// Let this object take damage, the DamageInfo parameter can be empty, or a string which indicates if it's energy, kinetic or emp damage
    /// optionally followed by the location of the damage's origin (to damage the correct shield), followed by the frequency band (0-20 for energy damage) and the ESystem to target.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, takeDamage);
    /// Set the description of this object. The description is visible on the Science station.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setDescription);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setDescriptionForScanState);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getDescription);
    /// Sets the description of this object in scanned and unscanned states. First parameter is the description in unscanned state, while the 2nd parameter is in scanned state.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setDescriptions);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, addInfos);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getInfosLabel);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getInfosValue);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getInfosValueByLabel);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, removeInfos);
    /// Set the radar signature of this object. Objects' signatures create noise
    /// on the Science station's raw radar signal ring.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setRadarSignatureInfo);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getRadarSignatureGravity);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getRadarSignatureElectrical);
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, getRadarSignatureBiological);
    /// Sets this object's scanning complexity (number of bars in the scanning
    /// minigame) and depth (number of scanning minigames to complete).
    /// Also clears the scanned state.
    /// Requires two integer values.
    /// Example: obj:setScanningParameters(2, 3)
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setScanningParameters);
    /// Gets the scanning complexity for the parameter object.
    /// Requires a SpaceObject.
    /// Returns an integer value.
    /// Example: local scan_complexity = obj:scanningComplexity(obj)
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, scanningComplexity);
    ///Get the scanning depth of this object (number of minigames to complete)
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, scanningChannelDepth);
    ///Set the scanning complexity and depth for this object.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setScanningParameters);
    ///[DEPRICATED] Check if this object is scanned already.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, isScanned);
    /// Check if this object is scanned by the faction of another object
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, isScannedBy);
    /// Check if this object is scanned by another faction
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, isScannedByFaction);
    /// Set if this object is scanned or not by every faction.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setScanned);
    /// Set if this object is scanned or not by a particular faction.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, setScannedByFaction);
    // Register a callback that is called when this object is destroyed, by any means.
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceObject, onDestroyed);
}

PVector<SpaceObject> space_object_list;

SpaceObject::SpaceObject(float collision_range, string multiplayer_name, float multiplayer_significant_range)
: Collisionable(collision_range), MultiplayerObject(multiplayer_name)
{
    object_radius = collision_range;
    space_object_list.push_back(this);
    faction_id = 0;
    personality_id = 0;
    hull = 0;
    translate_z = 0;
    id_dock = "";

    for(int n = 0; n < 10; n++)
    {
        infos_label[n] = "";
        infos_value[n] = "";
    }

    scanning_complexity_value = 0;
    scanning_depth_value = 0;

    registerMemberReplication(&translate_z);
    registerMemberReplication(&object_radius);
    registerMemberReplication(&callsign);
    registerMemberReplication(&id_dock);
    registerMemberReplication(&hull);
    registerMemberReplication(&faction_id);
    registerMemberReplication(&personality_id);
    registerMemberReplication(&scanned_by_faction);
    registerMemberReplication(&object_description.not_scanned);
    registerMemberReplication(&object_description.friend_of_foe_identified);
    registerMemberReplication(&object_description.simple_scan);
    registerMemberReplication(&object_description.full_scan);
    registerMemberReplication(&radar_signature.gravity);
    registerMemberReplication(&radar_signature.electrical);
    registerMemberReplication(&radar_signature.biological);
    registerMemberReplication(&scanning_complexity_value);
    registerMemberReplication(&scanning_depth_value);
    registerCollisionableReplication(multiplayer_significant_range);

    for (int n=0; n<10; n++)
    {
        registerMemberReplication(&infos_label[n]);
        registerMemberReplication(&infos_value[n]);
    }
    oxygen_zones = 0;
    registerMemberReplication(&oxygen_zones);
    for(int n=0; n<max_oxygen_zones; n++)
    {
        oxygen_points[n] = 0.0;
        oxygen_max[n] = 0.0;
        oxygen_rate[n] = 0.0;
        registerMemberReplication(&oxygen_points[n]);
        registerMemberReplication(&oxygen_max[n]);
        registerMemberReplication(&oxygen_rate[n]);
    }

    transparency = 0.0f;
    registerMemberReplication(&transparency);

    correction_x = 0.0;
    correction_y = 0.0;
    id_galaxy = 1;
    registerMemberReplication(&correction_x);
    registerMemberReplication(&correction_y);
    registerMemberReplication(&id_galaxy);
}

//due to a suspected compiler bug this deconstructor needs to be explicitly defined
SpaceObject::~SpaceObject()
{
}

void SpaceObject::draw3D()
{
#if FEATURE_3D_RENDERING
    model_info.render(getPosition(), getRotation(), 1 - transparency);
#endif//FEATURE_3D_RENDERING
}



void SpaceObject::drawOnRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, float rotation, bool longRange)
{
}

void SpaceObject::drawOnGMRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, float rotation, bool longRange)
{
}

void SpaceObject::destroy()
{
    on_destroyed.call(P<SpaceObject>(this));
    MultiplayerObject::destroy();
}

void SpaceObject::takeDamage(float damage_amount, DamageInfo info)
{
    // If no hull, then it could no be destroyed
    if(hull <= 0)
        return;
    if (info.type == DT_EMP)
        return;

    hull -= damage_amount;
    if (hull <= 0)
    {
        P<ExplosionEffect> e = new ExplosionEffect();
        e->setSize(getRadius());
        e->setPosition(getPosition());
        e->setTranslateZ(getTranslateZ());
        destroy();
    }
}

bool SpaceObject::canBeTargetedBy(P<SpaceObject> other)
{
    if(hull <= 0)
        return false;
    if (getTransparency() > 0.5)
        return false;
    return true;
}

bool SpaceObject::canBeSelectedBy(P<SpaceObject> other)
{
    if (getTransparency() > 0.5)
        return false;
    if (getDescriptionFor(other).length() > 0)
        return true;
    if (canBeScannedBy(other))
        return true;
    if (canBeTargetedBy(other))
        return true;
    return true;
}

bool SpaceObject::canBeScannedBy(P<SpaceObject> other)
{
    if (getTransparency() > 0.5)
        return false;
    if (getScannedStateFor(other) == SS_NotScanned)
        return true;
    if (getScannedStateFor(other) == SS_FullScan)
        return false;
//    if (scanning_complexity_value > 0)
//        return true;
//    if (scanning_depth_value > 0)
//        return true;
//    return false;
    return true;
}

bool SpaceObject::canBeHackedBy(P<SpaceObject> other)
{
    return false;
}

std::vector<std::pair<string, float> > SpaceObject::getHackingTargets()
{
    return std::vector<std::pair<string, float> >();
}

void SpaceObject::hackFinished(P<SpaceObject> source, string target)
{
}

EScannedState SpaceObject::getScannedStateFor(P<SpaceObject> other)
{
    if (!other)
    {
        return SS_NotScanned;
    }
    return getScannedStateForFaction(other->getFactionId());
}

void SpaceObject::setScannedStateFor(P<SpaceObject> other, EScannedState state)
{
    if (!other)
    {
        LOG(ERROR) << "setScannedStateFor called with no other";
        return;
    }
    setScannedStateForFaction(other->getFactionId(), state);
}

EScannedState SpaceObject::getScannedStateForFaction(int faction_id)
{
    if (int(scanned_by_faction.size()) <= faction_id)
        return SS_NotScanned;
    return scanned_by_faction[faction_id];
}

void SpaceObject::setScannedStateForFaction(int faction_id, EScannedState state)
{
    while (int(scanned_by_faction.size()) <= faction_id)
        scanned_by_faction.push_back(SS_NotScanned);
    scanned_by_faction[faction_id] = state;
}

bool SpaceObject::isScanned()
{
    LOG(WARNING) << "Depricated \"isScanned\" function called, use isScannedBy or isScannedByFaction.";
    for(unsigned int faction_id = 0; faction_id < scanned_by_faction.size(); faction_id++)
    {
        if (scanned_by_faction[faction_id] > SS_FriendOrFoeIdentified)
            return true;
    }
    return false;
}

void SpaceObject::setScanned(bool scanned)
{
    for(unsigned int faction_id = 0; faction_id < factionInfo.size(); faction_id++)
    {
        if (!scanned)
            setScannedStateForFaction(faction_id, SS_NotScanned);
        else
            setScannedStateForFaction(faction_id, SS_FullScan);
    }
}

void SpaceObject::setScannedByFaction(string faction_name, bool scanned)
{
    if (!scanned)
        setScannedStateForFaction(FactionInfo::findFactionId(faction_name), SS_NotScanned);
    else
        setScannedStateForFaction(FactionInfo::findFactionId(faction_name), SS_FullScan);
}

bool SpaceObject::isScannedBy(P<SpaceObject> obj)
{
    return getScannedStateFor(obj) > SS_FriendOrFoeIdentified;
}

bool SpaceObject::isScannedByFaction(string faction)
{
    int faction_id = FactionInfo::findFactionId(faction);
    return getScannedStateForFaction(faction_id) > SS_FriendOrFoeIdentified;
}

void SpaceObject::scannedBy(P<SpaceObject> other)
{
//    setScannedStateFor(other, SS_FullScan);
    switch(getScannedStateFor(other))
    {
    case SS_NotScanned:
    case SS_FriendOrFoeIdentified:
        setScannedStateFor(other, SS_SimpleScan);
        break;
    case SS_SimpleScan:
        setScannedStateFor(other, SS_FullScan);
        break;
    case SS_FullScan:
        break;
    }
}

void SpaceObject::setScanningParameters(int complexity, int depth)
{
    scanning_complexity_value = std::min(4, std::max(0, complexity));
    scanning_depth_value = std::max(0, depth);

    scanned_by_faction.clear();
}

//    P_normal = 1,
//    P_peaceful = 2,
//    P_hostile = 3,
//    P_solo = 4

string SpaceObject::getPersonality()
{
    if (personality_id == 1)
        return "Pacifique";
    if (personality_id == 2)
        return "Hostile";
    if (personality_id == 3)
        return "Solo";

    return "Normal";
}

bool SpaceObject::isEnemy(P<SpaceObject> obj)
{
    if (obj)
    {
        if (personality_id == 1)
            return false;
        if (personality_id == 2)
            return faction_id != obj->faction_id;
        if (personality_id == 3)
            return callsign != obj->callsign;
        return factionInfo[faction_id]->states[obj->faction_id] == FVF_Enemy;
    } else {
        return false;
    }
}

bool SpaceObject::isFriendly(P<SpaceObject> obj)
{
    if (obj)
    {
        if (personality_id == 1)
            return true;
        if (personality_id == 2)
            return faction_id == obj->faction_id;
        if (personality_id == 3)
            return callsign == obj->callsign;
        return factionInfo[faction_id]->states[obj->faction_id] == FVF_Friendly;
    } else {
        return false;
    }
}

void SpaceObject::damageArea(sf::Vector2f position, float blast_range, float min_damage, float max_damage, DamageInfo info, float min_range)
{
    PVector<Collisionable> hitList = CollisionManager::queryArea(position - sf::Vector2f(blast_range, blast_range), position + sf::Vector2f(blast_range, blast_range));
    foreach(Collisionable, c, hitList)
    {
        P<SpaceObject> obj = c;
        if (obj)
        {
            float dist = sf::length(position - obj->getPosition()) - obj->getRadius() - min_range;
            if (dist < 0) dist = 0;
            if (dist < blast_range - min_range)
            {
                obj->takeDamage(max_damage - (max_damage - min_damage) * dist / (blast_range - min_range), info);
            }
        }
    }
}

bool SpaceObject::areEnemiesInRange(float range)
{
    PVector<Collisionable> hitList = CollisionManager::queryArea(getPosition() - sf::Vector2f(range, range), getPosition() + sf::Vector2f(range, range));
    foreach(Collisionable, c, hitList)
    {
        P<SpaceObject> obj = c;
        if (obj && isEnemy(obj))
        {
            if (getPosition() - obj->getPosition() < range + obj->getRadius())
                return true;
        }
    }
    return false;
}

PVector<SpaceObject> SpaceObject::getObjectsInRange(float range)
{
    PVector<SpaceObject> ret;
    PVector<Collisionable> hitList = CollisionManager::queryArea(getPosition() - sf::Vector2f(range, range), getPosition() + sf::Vector2f(range, range));
    foreach(Collisionable, c, hitList)
    {
        P<SpaceObject> obj = c;
        if (obj && getPosition() - obj->getPosition() < range + obj->getRadius())
        {
            ret.push_back(obj);
        }
    }
    return ret;
}

void SpaceObject::setReputationPoints(float amount)
{
    if (gameGlobalInfo->reputation_points.size() < faction_id)
        return;
    gameGlobalInfo->reputation_points[faction_id] = amount;
}

int SpaceObject::getReputationPoints()
{
    if (gameGlobalInfo->reputation_points.size() < faction_id)
        return 0;
    return gameGlobalInfo->reputation_points[faction_id];
}

bool SpaceObject::takeReputationPoints(float amount)
{
    if (gameGlobalInfo->reputation_points.size() < faction_id)
        return false;
     if (gameGlobalInfo->reputation_points[faction_id] < amount)
        return false;
    gameGlobalInfo->reputation_points[faction_id] -= amount;
    return true;
}

void SpaceObject::removeReputationPoints(float amount)
{
    addReputationPoints(-amount);
}

void SpaceObject::addReputationPoints(float amount)
{
    if (gameGlobalInfo->reputation_points.size() < faction_id)
        return;
    gameGlobalInfo->reputation_points[faction_id] += amount;
    if (gameGlobalInfo->reputation_points[faction_id] < 0.0)
        gameGlobalInfo->reputation_points[faction_id] = 0.0;
}

void SpaceObject::setOxygenMax(std::vector<float> values)
{
    oxygen_zones = int(values.size());
    for(int n=0; n<oxygen_zones; n++)
    {
        oxygen_max[n] = values[n];
    }
}
void SpaceObject::setOxygenPoints(std::vector<float> values)
{
    for(int n=0; n<oxygen_zones; n++)
    {
        oxygen_points[n] = std::min(values[n],oxygen_max[n]);
    }
}
void SpaceObject::setOxygenRate(std::vector<float> values)
{
    for(int n=0; n<oxygen_zones; n++)
    {
        oxygen_rate[n] = values[n];
    }
}
float SpaceObject::getOxygenPoints(int index)
{
    return oxygen_points[index];
}
float SpaceObject::getOxygenMax(int index)
{
    return oxygen_max[index];
}
float SpaceObject::getOxygenRatio(int index)
{
    if (oxygen_max[index] == 0)
        return 0;
    else
        return oxygen_points[index] / oxygen_max[index];
}
float SpaceObject::getOxygenRate(int index)
{
    return oxygen_rate[index];
}
float SpaceObject::getOxygenTotal()
{
    float oxygen_total = 0.0;
    for(int n=0; n<oxygen_zones; n++)
        oxygen_total += (oxygen_points[n] / oxygen_max[n]) / oxygen_zones;
    return oxygen_total;
}
float SpaceObject::getOxygenMaxTotal()
{
    float oxygen_max_total = 0.0;
    for(int n=0; n<oxygen_zones; n++)
        oxygen_max_total += oxygen_max[n];
    return oxygen_max_total;
}
bool SpaceObject::takeOxygenPoints(float amount,int index)
{
	if (oxygen_points[index] < amount)
		return false;
	oxygen_points[index] -= amount;
    	return true;
}
void SpaceObject::removeOxygenPoints(float amount,int index)
{
    addOxygenPoints(-amount, index);
}
void SpaceObject::addOxygenPoints(float amount,int index)
{
    oxygen_points[index] += amount;
    if (oxygen_points[index] < 0.0)
    	oxygen_points[index] = 0.0;
    if (oxygen_points[index] > oxygen_max[index])
    	oxygen_points[index] = oxygen_max[index];
}

string SpaceObject::getSectorName()
{
    return ::getSectorName(getPosition() - sf::Vector2f(correction_x,correction_y) );
}

string SpaceObject::getSectorNameLevel(int level)
{
    int factor = std::pow(8,level) * GameGlobalInfo::sector_size;

    sf::Vector2f position = getPosition();
    position.x = floorf((position.x) / factor) * factor - correction_x;
    position.y = floorf((position.y) / factor) * factor - correction_y;

    return ::getSectorName(position);
}

bool SpaceObject::openCommsTo(P<PlayerSpaceship> target)
{
    return sendCommsMessage(target, "");
}

bool SpaceObject::sendCommsMessage(P<PlayerSpaceship> target, string message)
{
    if (!target)
        return false;

    bool result = target->hailByObject(this, message);
    if (!result && message != "")
    {
        target->addToShipLogBy(message, this);
    }
    return result;
}

// Define a script conversion function for the DamageInfo structure.
template<> void convert<DamageInfo>::param(lua_State* L, int& idx, DamageInfo& di)
{
    if (!lua_isstring(L, idx))
        return;
    string str = string(luaL_checkstring(L, idx++)).lower();
    if (str == "energy")
        di.type = DT_Energy;
    else if (str == "kinetic")
        di.type = DT_Kinetic;
    else if (str == "emp")
        di.type = DT_EMP;

    if (!lua_isnumber(L, idx))
        return;

    di.location.x = luaL_checknumber(L, idx++);
    di.location.y = luaL_checknumber(L, idx++);

    if (lua_isnil(L, idx))
        idx++;
    else if (!lua_isnumber(L, idx))
        return;
    else
        di.frequency = luaL_checkinteger(L, idx++);

    if (!lua_isstring(L, idx))
        return;

    convert<ESystem>::param(L, idx, di.system_target);
}

template<> void convert<EScannedState>::param(lua_State* L, int& idx, EScannedState& ss)
{
    ss = SS_NotScanned;
    if (!lua_isstring(L, idx))
        return;
    string str = string(luaL_checkstring(L, idx++)).lower();
    if (str == "notscanned" || str == "not")
        ss = SS_NotScanned;
    else if (str == "friendorfoeidentified")
        ss = SS_FriendOrFoeIdentified;
    else if (str == "simple" || str == "simplescan")
        ss = SS_SimpleScan;
    else if (str == "full" || str == "fullscan")
        ss = SS_FullScan;
}
