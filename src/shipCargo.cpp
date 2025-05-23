#include "shipCargo.h"
#include "cargo.h"
#include "spaceObjects/spaceship.h"
#include "spaceObjects/playerSpaceship.h"
#include "gameGlobalInfo.h"
#include "tween.h"

REGISTER_MULTIPLAYER_CLASS(ShipCargo, "ShipCargo");

ShipCargo::ShipCargo() : Cargo("ShipCargo")
{
    registerMemberReplication(&callsign);
    registerMemberReplication(&template_name);
    registerMemberReplication(&hull_strength);
    registerMemberReplication(&has_reactor);
    registerMemberReplication(&has_cloaking);
    registerMemberReplication(&base_carrier_id);
    for(int n=0; n<SYS_COUNT; n++) {
        registerMemberReplication(&systems_health[n]);
    }
}

ShipCargo::ShipCargo(P<ShipTemplate> ship_template) : ShipCargo()
{
    base_carrier_id = -1;
    template_name = ship_template->getName();
    std::string prefix = (ship_template->getType() == ShipTemplate::TemplateType::Drone) ? "DRN-" : "SHP-";
    callsign = prefix + gameGlobalInfo->getNextShipCallsign();
    setEnergy(ship_template->energy_storage_amount);
    hull_strength = ship_template->hull;
    has_reactor = ship_template->has_reactor;
    has_cloaking = ship_template->has_cloaking;
    for(int n=0; n<SYS_COUNT; n++) {
        systems_health[n] = 1;
    }
    for(int n=0; n < MW_Count; n++)
    {
//        setWeaponStorage(EMissileWeapons(n), ship_template->weapon_storage[n]);
        setWeaponStorage(EMissileWeapons(n), 0);
        setWeaponStorageMax(EMissileWeapons(n), ship_template->weapon_storage[n]);
    }
    for (auto &kv : ship_template->custom_weapon_storage )
    {
        setCustomWeaponStorage(kv.first, kv.second);
        setCustomWeaponStorageMax(kv.first, kv.second);
    }
    auto_repair_enabled=true;
    auto_coolant_enabled=true;
}

ShipCargo::ShipCargo(P<SpaceShip> ship) : ShipCargo()
{
    template_name = ship->getTypeName();
    callsign = ship->getCallSign();
    setEnergy(ship->getEnergy());
    hull_strength = ship->getHull();
    has_reactor = ship->has_reactor;
    has_cloaking = ship->has_cloaking;
    float totalHeat = 0;
    for(unsigned int n=0; n<SYS_COUNT; n++)
        totalHeat += ship->getSystemHeat(ESystem(n));
    setHeat(totalHeat);
    hull_strength = ship->getHull();
    for(int n=0; n<SYS_COUNT; n++) {
        systems_health[n] = ship->systems[n].health;
    }
    for(int n=0; n < MW_Count; n++)
    {
        setWeaponStorage(EMissileWeapons(n), ship->weapon_storage[n]);
        setWeaponStorageMax(EMissileWeapons(n), ship->weapon_storage_max[n]);
    }
    for (auto &kv : ship->custom_weapon_storage )
    {
        setCustomWeaponStorage(kv.first, kv.second);
    }
    for (auto &kv : ship->custom_weapon_storage_max )
    {
        setCustomWeaponStorageMax(kv.first, kv.second);
    }
    P<PlayerSpaceship> pship  = ship;
    if(pship)
    {
        auto_repair_enabled=pship->auto_repair_enabled;
        auto_coolant_enabled=pship->auto_coolant_enabled;
    }
    base_carrier_id = ship->base_carrier_id;
}

P<ModelData> ShipCargo::getModel()
{
    P<ShipTemplate> ship_template = ShipTemplate::getTemplate(template_name);
    if (ship_template)
        return ship_template->model_data;
    else
        return nullptr;
}

float ShipCargo::getHealth()
{
    const float maxHull = getTemplate()->hull;
    float health = hull_strength;
    for(int n=0; n<SYS_COUNT; n++) {
        health += Tween<float>::linear(systems_health[n], -1, 1, 0, maxHull);
    }
    return health;
}

void ShipCargo::addHealth(float amount)
{
    const float maxHull = getTemplate()->hull;
    const float normAmount = amount / getMaxHealth();
    for(int n=0; n<SYS_COUNT; n++) {
        systems_health[n] = std::min(1.0f, systems_health[n] + (2 * normAmount));
    }
    hull_strength = std::min(maxHull, hull_strength + (maxHull * normAmount));
}

bool ShipCargo::onLaunch(Dock &source)
{
    if (game_server)
    {
        P<PlayerSpaceship> ship = new PlayerSpaceship();
        if (ship)
        {
            ship->setFactionId(source.getFactionId());
            ship->setTemplate(template_name);
            ship->setCallSign(callsign);
            ship->setEnergyLevel(getEnergy());
            ship->setPosition(source.getLaunchPosition(ship->getRadius()));
            ship->setRotation(source.getLaunchRotation());
            ship->setHull(hull_strength);
            ship->has_reactor = has_reactor;
            ship->has_cloaking = has_cloaking;
            ship->impulse_request = -0.5;
            ship->base_carrier_id = base_carrier_id;
            int systemsCount = 0;
            for (unsigned int n = 0; n < SYS_COUNT; n++){
                if (ship->hasSystem(ESystem(n)))
                    systemsCount++;
                ship->systems[n].health = systems_health[n];
            }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
            for (unsigned int n = 0; n < SYS_COUNT; n++)
                if (ship->hasSystem(ESystem(n)))
                    ship->addHeat(ESystem(n), getHeat() / systemsCount);
            for(int n = 0; n < MW_Count; n++)
            {
                ship->weapon_storage_max[n] = getWeaponStorageMax(EMissileWeapons(n));
                //ship->weapon_storage[n] = getWeaponStorage(EMissileWeapons(n));
                ship->weapon_storage[n] = getWeaponStorageMax(EMissileWeapons(n)); //auto recharge
                
            }
            
            for (auto &kv : custom_weapon_storage_max )
            {
                ship->custom_weapon_storage_max[kv.first] = kv.second;
                // auto recharge
                ship->custom_weapon_storage[kv.first] = kv.second;
            }
            //for (auto &kv : custom_weapon_storage )
            //{
            //    ship->custom_weapon_storage[kv.first] = kv.second;
            //}

            ship->auto_coolant_enabled=auto_coolant_enabled;
            ship->auto_repair_enabled=auto_repair_enabled;
            return true;
        }
    }
    return false;
}

ShipCargo::Entries ShipCargo::getEntries()
{
//    ShipCargo::Entries result = Cargo::getEntries();
    ShipCargo::Entries result;
    P<ShipTemplate> ship_template = ShipTemplate::getTemplate(template_name);
    if (ship_template)
    {
        result.push_back(std::make_tuple("gui/icons/hull", tr("health","Hull"), string(int(100 * hull_strength / ship_template->hull)) + "%"));
    }
    result.push_back(std::make_tuple("", "ID", callsign));
    result.push_back(std::make_tuple("", "type", template_name));

    if (has_reactor)
        result.push_back(std::make_tuple("",tr("system", "Reactor") + " ? ", tr("Yes")));
    else
        result.push_back(std::make_tuple("", tr("system", "Reactor") + " ?", tr("No")));

    float velocity = ship_template->impulse_speed / 1000 * 60;
    result.push_back(std::make_tuple("", "Vitesse", string(velocity, 1) + DISTANCE_UNIT_1K + "/min"));

    if (ship_template->weapon_tube_count > 0)
        result.push_back(std::make_tuple("", tr("cargo", "Missile tubes"), ship_template->weapon_tube_count));

    int beam_weapons_count = 0;
    for(int n=0; n<max_beam_weapons; n++)
        if (ship_template->beams[n].getRange() > 0)
            beam_weapons_count += 1;
    if (beam_weapons_count > 0)
        result.push_back(std::make_tuple("", tr("system", "Beam Weapons"), beam_weapons_count));

    return result;
}
