#include "damcon.h"

#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "screenComponents/shieldFreqencySelect.h"
#include "screenComponents/shipInternalView.h"
#include "screenComponents/customShipFunctions.h"

#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_label.h"
#include "gui/gui2_selector.h"
#include "gameGlobalInfo.h"

DamageControlScreen::DamageControlScreen(GuiContainer *owner)
    : GuiOverlay(owner, "DAMCON_SCREEN", colorConfig.background)
{
    if (gameGlobalInfo->use_repair_crew)
    {
        (new GuiShipInternalView(this, "SHIP_INTERNAL_VIEW", 48.0f * 1.5f))->setShip(my_spaceship)->setPosition(300, 0, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    } else {
        auto row = new GuiElement(this, "AUTO_REPAIR_LAYOUT");
        row->setPosition(300, 0, sp::Alignment::CenterLeft)->setSize(GuiElement::GuiSizeMax, 40)->setAttribute("layout", "vertical");
        (new GuiLabel(row, "AUTO_REPAIR_LABEL", "Auto Repair", 30));
        autoRepairSelector = new GuiSelector(row, "AUTO_REPAIR_LABEL", [this](int _idx, string value) {
            if (my_spaceship)
                my_spaceship->commandSetAutoRepairSystemTarget(ESystem(value.toInt()));
        });
        autoRepairSelector->setSize(800, 30);
    }
    auto system_health_layout = new GuiElement(this, "DAMCON_LAYOUT");
    system_health_layout->setPosition(0, 0, sp::Alignment::CenterLeft)->setSize(300, 600)->setAttribute("layout", "vertical");

    hull_display = new GuiKeyValueDisplay(system_health_layout, "HULL", 0.8, tr("damagecontrol", "Hull"), "0%");
    hull_display->setSize(GuiElement::GuiSizeMax, 40);

    for (unsigned int n = 0; n < SYS_COUNT; n++)
    {
        system_health[n] = new GuiKeyValueDisplay(system_health_layout, "DAMCON_HEALTH_" + string(n), 0.8, getLocaleSystemName(ESystem(n)), "0%");
        system_health[n]->setSize(GuiElement::GuiSizeMax, 40);
    }

    (new GuiCustomShipFunctions(this, damageControl, "", my_spaceship))->setPosition(-20, 120, sp::Alignment::TopRight)->setSize(250, GuiElement::GuiSizeMax);
}

void DamageControlScreen::onDraw(sp::RenderTarget &renderer)
{
    GuiOverlay::onDraw(renderer);

    if (my_spaceship)
    {
        hull_display->setValue(string(int(100 * my_spaceship->hull_strength / my_spaceship->hull_max)) + "%");
        if (my_spaceship->hull_strength < my_spaceship->hull_max / 4.0f)
            hull_display->setColor(glm::u8vec4(255, 0, 0, 255));
        else
            hull_display->setColor(glm::u8vec4{255,255,255,255});

        std::vector<string> sysNames = {"None"};
        std::vector<string> sysValues = {SYS_None};

        for(unsigned int n=0; n<SYS_COUNT; n++)
        {
            system_health[n]->setVisible(my_spaceship->hasSystem(ESystem(n)));
            system_health[n]->setValue(string(int(my_spaceship->systems[n].health * 100)) + "%");
            if (my_spaceship->systems[n].health < 0)
                system_health[n]->setColor(glm::u8vec4(255, 0, 0, 255));
            else if (my_spaceship->systems[n].health_max < 1.0f)
                system_health[n]->setColor(glm::u8vec4(255, 255, 0, 255));
            else
                system_health[n]->setColor(glm::u8vec4{255,255,255,255});

            if (my_spaceship->hasSystem(ESystem(n)))
            {
                sysNames.push_back(getSystemName(ESystem(n)));
                sysValues.push_back(n);
            }
        }
        if (!gameGlobalInfo->use_repair_crew)
        {
            autoRepairSelector->setOptions(sysNames, sysValues);
            autoRepairSelector->setSelectionIndex(autoRepairSelector->indexByValue(my_spaceship->auto_repairing_system));
        }
    }
}
