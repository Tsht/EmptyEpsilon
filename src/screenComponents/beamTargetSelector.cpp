#include <i18n.h>
#include "playerInfo.h"
#include "gameGlobalInfo.h"
#include "beamTargetSelector.h"
#include "powerDamageIndicator.h"
#include "spaceObjects/playerSpaceship.h"

#include "targetsContainer.h"

GuiBeamTargetSelector::GuiBeamTargetSelector(GuiContainer* owner, string id, P<PlayerSpaceship> targetSpaceship)
: GuiElement(owner, id), target_spaceship(targetSpaceship)
{
    selector = new GuiSelector(this, id + "_SELECTOR", [this](int index, string value) {
        if (target_spaceship)
            target_spaceship->commandSetBeamSystemTarget(ESystem(std::stoi(value)));
    });
    selector->setPosition(0, 0, sp::Alignment::BottomLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    selector->addEntry(tr("target","Hull"), "-1");

    if (!gameGlobalInfo->use_system_damage)
        hide();
}

void GuiBeamTargetSelector::setTargetSpaceship(P<PlayerSpaceship> targetSpaceship){
    target_spaceship = targetSpaceship;
    pdi->setTargetSpaceship(target_spaceship);
}

void GuiBeamTargetSelector::onDraw(sp::RenderTarget& renderer)
{
//    if (target_spaceship)
//        selector->setSelectionIndex(selector->indexByValue(target_spaceship->beam_system_target));
    if(!target_spaceship)
        return;
    target = target_spaceship->getTarget();
    P<SpaceShip> ship = target;
    if(ship)
    {
        selector->enable();

        for(unsigned int n=0; n<SYS_COUNT; n++)
        {
            if (ship->hasSystem(ESystem(n)))
            {
                if (selector->indexByValue(string(n)) == -1)
                    selector->addEntry(getLocaleSystemName(ESystem(n)), string(n));
            }
            else
                selector->removeEntry(selector->indexByValue(string(n)));
        }
        for(int n=0; n<=selector->entryCount(); n++)
        {
            if (selector->getEntryName(n) == "")
                selector->removeEntry(n);
        }
    }
    else
    {
        selector->disable();
    }
}

void GuiBeamTargetSelector::onUpdate()
{
    if (target_spaceship && gameGlobalInfo->use_system_damage && isVisible())
    {
        if (keys.weapons_beam_subsystem_target_next.getDown())
        {
            if (selector->getSelectionIndex() >= selector->entryCount() - 1)
                selector->setSelectionIndex(0);
            else
                selector->setSelectionIndex(selector->getSelectionIndex() + 1);
        }
        if (keys.weapons_beam_subsystem_target_previous.getDown())
        {
            if (selector->getSelectionIndex() <= 0)
                selector->setSelectionIndex(selector->entryCount() - 1);
            else
                selector->setSelectionIndex(selector->getSelectionIndex() - 1);
        }
    }
}
