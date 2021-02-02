#include <i18n.h>
#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "dockingButton.h"

GuiDockingButton::GuiDockingButton(GuiContainer* owner, string id, P<PlayerSpaceship> targetSpaceship)
: GuiButton(owner, id, "", [this]() { click(); }), target_spaceship(targetSpaceship)
{
    setIcon("gui/icons/docking");
}

void GuiDockingButton::click()
{
    if (!target_spaceship)
        return;
    switch(target_spaceship->docking_state)
    {
    case DS_NotDocking:
        target_spaceship->commandDock(findDockingTarget());
        target_spaceship->commandSetDockTarget(findDockingTarget());
        break;
    case DS_Docking:
        target_spaceship->commandAbortDock();
        target_spaceship->commandSetDockTarget(NULL);
        break;
    case DS_Docked:
        target_spaceship->commandUndock();
        target_spaceship->commandSetDockTarget(NULL);
        break;
    }
}

void GuiDockingButton::onUpdate()
{
    setVisible(my_spaceship && my_spaceship->getCanDock());
}

void GuiDockingButton::onDraw(sf::RenderTarget& window)
{
    if (target_spaceship)
    {
        switch(target_spaceship->docking_state)
        {
        case DS_NotDocking:
            setText(tr("Request Dock"));
            if (target_spaceship->canStartDocking() && findDockingTarget())
            {
                enable();
                setText("Se docker : " + findDockingTarget()->callsign);
            }else{
                disable();
                setText("Se docker");
            }
            break;
        case DS_Docking:
            setText(tr("Cancel Docking"));
            enable();
            break;
        case DS_Docked:
            setText(tr("Undock"));
            enable();
            break;
        }

        setVisible(target_spaceship->hasSystem(SYS_Impulse));
    }
    GuiButton::onDraw(window);
}

void GuiDockingButton::onHotkey(const HotkeyResult& key)
{
    if (key.category == "HELMS" && target_spaceship)
    {
        if (key.hotkey == "DOCK_ACTION")
        {
            switch(target_spaceship->docking_state)
            {
            case DS_NotDocking:
                target_spaceship->commandDock(findDockingTarget());
                break;
            case DS_Docking:
                target_spaceship->commandAbortDock();
                break;
            case DS_Docked:
                target_spaceship->commandUndock();
                break;
            }
        }
        else if (key.hotkey == "DOCK_REQUEST")
            target_spaceship->commandDock(findDockingTarget());
        else if (key.hotkey == "DOCK_ABORT")
            target_spaceship->commandAbortDock();
        else if (key.hotkey == "UNDOCK")
            target_spaceship->commandUndock();
    }
}

P<SpaceObject> GuiDockingButton::findDockingTarget()
{
    PVector<Collisionable> obj_list = CollisionManager::queryArea(target_spaceship->getPosition() - sf::Vector2f(1000, 1000), target_spaceship->getPosition() + sf::Vector2f(1000, 1000));
    P<SpaceObject> dock_object;
    P<SpaceShip> dock_ship;
    foreach(Collisionable, obj, obj_list)
    {
        dock_object = obj;
        if (target_spaceship->id_galaxy != dock_object->id_galaxy)
        {
            dock_object = NULL;
            continue;
        }

        dock_ship = obj;
        if(dock_ship && dock_ship->docking_state != DS_NotDocking &&  dock_ship->landing_state != LS_NotLanding)
        {
            dock_object = NULL;
            continue;
        }
        if (dock_object
            && dock_object != target_spaceship
            && dock_object->canBeDockedBy(target_spaceship)
            && (dock_object->getPosition() - target_spaceship->getPosition()) < 1000.0f + dock_object->getRadius())
            break;
        dock_object = NULL;
    }
    return dock_object;
}
