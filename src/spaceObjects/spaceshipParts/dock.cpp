#include "dock.h"
#include "spaceObjects/spaceship.h"
#include "spaceObjects/playerSpaceship.h"
#include <algorithm>
#include "random.h"

bool isDockOpenForDocking (Dock &d){
    return d.isOpenForDocking();
}

Dock *Dock::findOpenForDocking(Dock docks[], int size)
{
    int randIdx = irandom(0, size - 1);
    Dock *dock = std::find_if(docks + randIdx, docks + size, isDockOpenForDocking);
    if (dock == docks + size)
        dock = std::find_if(docks, docks + size, isDockOpenForDocking);
    if (dock == docks + size)
        return nullptr;
    else
        return dock;
}

Dock::Dock() : parent(nullptr), dock_type(Dock_Disabled), move_target_index(-1)
{
    empty();
}

P<Cargo> Dock::getCargo()
{
    if (state == Empty)
        return nullptr;
    if (game_server)
        return game_server->getObjectById(cargo_id);
    else
        return game_client->getObjectById(cargo_id);
}

void Dock::startMoveCargo()
{
    if (state == Docked && move_target_index >= 0 && move_target_index < max_docks_count)
    {
        Dock &dest = parent->docks[move_target_index];
        if (dest.isOpenForDocking())
        {
            this->setState(EDockState::MovingOut);
            current_distance = 0;
            dest.cargo_id = cargo_id;
            dest.energy_request = getCargo()->getEnergy();
            dest.setState(EDockState::MovingIn);
            dest.current_distance = 1;
            dest.move_target_index = index_at_parent;
        }
    }
}

void Dock::cancelMoveCargo()
{
    if (state == MovingOut)
    {
        state = MovingIn;
        Dock &dest = parent->docks[move_target_index];
        dest.state = MovingOut;
    }
}

void Dock::dock(P<Cargo> cargo)
{
    cargo_id = cargo->getMultiplayerId();
    setState(EDockState::Docked);
    setEnergyRequest(cargo->getEnergy());
    current_distance = 0;
}

void Dock::empty()
{
    cargo_id = -1;
    state = Empty;
    energy_request = 0;
    current_distance = 1;
}

bool Dock::isUnoccupied()
{
    return dock_type != Dock_Disabled && state == Empty;
}

bool Dock::isOpenForDocking()
{
    return isUnoccupied();
}

void Dock::setParent(SpaceShip *parent)
{
    assert(!this->parent);
    this->parent = parent;

    parent->registerMemberReplication(&dock_type);
    parent->registerMemberReplication(&cargo_id);
    parent->registerMemberReplication(&state);
    parent->registerMemberReplication(&energy_request);
    parent->registerMemberReplication(&move_target_index);
    parent->registerMemberReplication(&current_distance);
}

glm::vec2 Dock::getLaunchPosition(float cargoRadius) {
    return parent->getPosition() - vec2FromAngle(parent->getRotation()) * (cargoRadius + parent->getRadius());
}

 float Dock::getLaunchRotation(){
    return parent->getRotation();
}

 unsigned int Dock::getFactionId(){
    return parent->getFactionId();
}

bool Dock::operator==(const Dock &other)
{
    return state == other.state && cargo_id == other.cargo_id;
}

void Dock::update(float delta)
{
    if (state == MovingOut)
    {
        float distanceDelta = delta * parent->getSystemEffectiveness(SYS_Docks) / SpaceShip::dock_move_time;
        current_distance += distanceDelta;
        if (game_server)
        {
            Dock &dest = parent->docks[move_target_index];
            if (current_distance >= 1)
            {
                empty();
                dest.setState(Docked);
            }
            else
            {
                dest.current_distance = 1 - current_distance;
            }
        }
    }
    else if (game_server && state == Docked)
    {
        auto cargo = getCargo();
        if (cargo)
        {
            if ((dock_type == Dock_Energy) || (dock_type == Dock_Maintenance)) 
            {
                energy_request = std::min(energy_request, cargo->getMaxEnergy());
                energy_request = std::max(energy_request, 0.0f);

                float energyDelta = std::min(delta * this->parent->getSystemEffectiveness(SYS_Docks) *
                                                PlayerSpaceship::energy_transfer_per_second,
                                            std::abs(energy_request - cargo->getEnergy()));
                if (energy_request > cargo->getEnergy())
                {
                    energyDelta = std::min(energyDelta, parent->energy_level);
                    parent->energy_level -= energyDelta;
                    cargo->setEnergy(cargo->getEnergy() + energyDelta);
                }
                else if (energy_request < cargo->getEnergy())
                {
                    energyDelta = std::min(energyDelta, std::max(0.0f, parent->max_energy_level - parent->energy_level));
                    parent->energy_level += energyDelta;
                    cargo->setEnergy(cargo->getEnergy() - energyDelta);
                }
            }
            if ((dock_type == Dock_Repair || dock_type == Dock_Maintenance))
            {
                if (cargo->getHealth() < cargo->getMaxHealth())
                {
                    float repairAmount = std::min(delta * this->parent->getSystemEffectiveness(SYS_Docks) *
                                                    PlayerSpaceship::cargo_repair_per_second * (cargo->getMaxHealth()/100), //1%/10 sec
                                                    cargo->getMaxHealth() - cargo->getHealth());
                    cargo->addHealth(repairAmount);
                }
            }
            
        }
    }
    if (game_server && (state == MovingOut || state == Docked)){
        auto cargo = getCargo();

        if (cargo && cargo->getHeat() > 0)
        {
            if ((dock_type == Dock_Thermic) || (dock_type == Dock_Maintenance)){
                float coolDown = std::min(
                    delta * this->parent->getSystemEffectiveness(SYS_Docks) * PlayerSpaceship::heat_transfer_per_second * 0.2f,
                    cargo->getHeat());
                parent->addHeat(SYS_Docks, coolDown * 0.2f);
                cargo->setHeat(cargo->getHeat() - coolDown);
            } else {
                float heatToAbsorve = std::min(cargo->getHeat(), delta * PlayerSpaceship::heat_transfer_per_second);
                parent->addHeat(SYS_Docks, heatToAbsorve);
                cargo->setHeat(cargo->getHeat() - heatToAbsorve);
            }
        }
    }
}

string getDockStateName(EDockState state)
{
    switch (state)
    {
    case Empty:
        return "Vide";
    case Docked:
        return "Occupe";
    case MovingIn:
        return "Transfert en cours";
    case MovingOut:
        return "Transfert en cours";
    default:
        return "Inconnu";
    }
}

string getDockTypeName(EDockType dockType)
{
    switch (dockType)
    {
    case Dock_Launcher:
        return tr("dock","Flight deck");
    case Dock_Energy:
        return tr("dock","Energy");
    case Dock_Weapons:
        return tr("dock","Missiles");
    case Dock_Thermic:
        return tr("dock","Thermal");
    case Dock_Repair:
        return tr("dock","Repair");
    case Dock_Stock:
        return tr("dock","Supply");
    case Dock_Maintenance:
        return tr("dock","Maintenance");
    default:
        return "Unknown";
    }
}

EDockType getDockTypeEnum(std::string dockType)
{
    if(tr("dock","Flight deck")==dockType)
        return Dock_Launcher;
    if(tr("dock","Energy")==dockType)
        return Dock_Energy;
    if(tr("dock","Missiles")==dockType)
        return Dock_Weapons;
    if(tr("dock","Thermal")==dockType)
        return Dock_Thermic;
    if(tr("dock","Repair")==dockType)
        return Dock_Repair;
    if(tr("dock","Supply")==dockType)
        return Dock_Stock;
    if(tr("dock","Maintenance") == dockType)
        return Dock_Maintenance;
    return Dock_Disabled;

}
