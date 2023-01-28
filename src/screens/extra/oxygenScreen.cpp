#include "oxygenScreen.h"

#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "screenComponents/customShipFunctions.h"

#include "gui/gui2_element.h"
#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_progressbar.h"
#include "gui/gui2_label.h"
#include "gameGlobalInfo.h"

OxygenScreen::OxygenScreen(GuiContainer *owner)
    : GuiOverlay(owner, "OXYGEN_SCREEN", colorConfig.background)
{
    GuiOverlay *background_crosses = new GuiOverlay(this, "BACKGROUND_CROSSES", glm::u8vec4(255,255,255,255));
    background_crosses->setTextureTiled("gui/background/crosses.png");

    oxygen_view = new GuiElement(this, "OXYGEN_LAYOUT");
    oxygen_view->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setPosition(0, 0, sp::Alignment::Center)->setAttribute("layout", "horizontal");;

    (new GuiLabel(oxygen_view, "OXYGEN_LABEL", "Gestion de l'oxygene", 100))->setPosition(0, 200, sp::Alignment::Center);

    oxygen_label_intern = new GuiLabel(oxygen_view, "Ship_LABEL_INTERN", "Interne", 50);
    oxygen_label_intern->setPosition(0, 200, sp::Alignment::Center);
    oxygen_label_intern->setVisible(false);

    for(int n=0; n<max_oxygen_zones; n++)
    {
        oxygen_intern[n] = new GuiElement(oxygen_view, "OXYGEN_INTERN");

        (new GuiLabel(oxygen_intern[n], "OXYGEN_ZONE_LABEL_INTERN", "Zone " + string(n+1), 30))->setAlignment(sp::Alignment::Center)->setMargins(10, 10, 10, 10)->setAttribute("layout", "hotizontal");;

        oxygen_bar_intern[n] = new GuiProgressbar(oxygen_intern[n], "ENERGY_BAR_INTERN", 0.0, 1.0, 0.0);
        oxygen_bar_intern[n]->setColor(glm::u8vec4(192, 192, 32, 128))->setText("Oxygene")->setDrawBackground(true)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setMargins(10, 0, 10, 0);

        oxygen_value_intern[n] = new GuiKeyValueDisplay(oxygen_intern[n], "OXY_VALUE_INTERN" + string(n), 0.8, "Zone " + string(n+1), "0%");
        oxygen_value_intern[n]->setSize(GuiElement::GuiSizeMax, 40)->setPosition(0, 400, sp::Alignment::Center);

        oxygen_intern[n]->setVisible(false);
    }

    oxygen_label_extern = new GuiLabel(oxygen_view, "Ship_LABEL", "Externe", 50);
    oxygen_label_extern->setPosition(0, 200, sp::Alignment::Center);
    oxygen_label_extern->setVisible(false);

    // Choix dock
    for(int n=0; n<max_oxygen_zones; n++)
    {
        oxygen_extern[n] = new GuiElement(oxygen_view, "OXYGEN_EXTERN");

        (new GuiLabel(oxygen_extern[n], "OXYGEN_ZONE_LABEL_EXTERN", "Zone " + string(n+1), 30))->setAlignment(sp::Alignment::Center)->setMargins(10, 10, 10, 10)->setAttribute("layout", "horizontal");;

        oxygen_bar_extern[n] = new GuiProgressbar(oxygen_extern[n], "ENERGY_BAR_EXTERN", 0.0, 1.0, 0.0);
        oxygen_bar_extern[n]->setColor(glm::u8vec4(192, 192, 32, 128))->setText("Oxygene")->setDrawBackground(true)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setMargins(10, 0, 10, 0);

        oxygen_value_extern[n] = new GuiKeyValueDisplay(oxygen_extern[n], "OXY_VALUE_EXTERN" + string(n), 0.8, "Zone " + string(n+1), "0%");
        oxygen_value_extern[n]->setSize(GuiElement::GuiSizeMax, 40)->setPosition(0, 400, sp::Alignment::Center);

        oxygen_extern[n]->setVisible(false);
    }
}

void OxygenScreen::onDraw(sp::RenderTarget& renderer)
{
    GuiOverlay::onDraw(renderer);

    if (my_spaceship)
    {
        oxygen_label_intern->setVisible(my_spaceship->getOxygenMaxTotal()>0);
        for(int n=0; n<max_oxygen_zones; n++)
        {
            float rate  = my_spaceship->getOxygenRechargeRate(n);
            if (rate > 0)
                time_left = (my_spaceship->getOxygenMax(n) - my_spaceship->getOxygenPoints(n)) / rate;
            else
                time_left = - my_spaceship->getOxygenPoints(n) / rate;

            oxygen_bar_intern[n]->setValue(my_spaceship->getOxygenRatio(n));
            oxygen_bar_intern[n]->setText(string(int(100 * my_spaceship->getOxygenRatio(n))) + "%");

            if (my_spaceship->getOxygenPoints(n) == 0)
            {
                oxygen_value_intern[n]->setKey("Zone non oxygenee");
                oxygen_value_intern[n]->setValue("");
                oxygen_value_intern[n]->setColor(glm::u8vec4(255,0,0,255));
            }
            else if (my_spaceship->getOxygenPoints(n) == my_spaceship->getOxygenMax(n))
            {
                oxygen_value_intern[n]->setKey("Zone oxygenee");
                oxygen_value_intern[n]->setValue("");
                oxygen_value_intern[n]->setColor(glm::u8vec4(0,255,0,255));
            }
            else if (rate > 0)
            {
                oxygen_value_intern[n]->setKey("Oxygene en hausse. Temps avant 100 % : ");
                oxygen_value_intern[n]->setValue(timeCount(time_left));
                oxygen_value_intern[n]->setColor(glm::u8vec4(0,255,0,255));
            }
            else if (rate < 0)
            {
                oxygen_value_intern[n]->setKey("Oxygene en baisse. Temps avant 0 % : ");
                oxygen_value_intern[n]->setValue(timeCount(time_left));
                oxygen_value_intern[n]->setColor(glm::u8vec4(255,0,0,255));
            }
            else
            {
                oxygen_value_intern[n]->setKey("Oxygene stable");
                oxygen_value_intern[n]->setValue("");
                oxygen_value_intern[n]->setColor(glm::u8vec4(0,255,0,255));
            }

            oxygen_intern[n]->setVisible(my_spaceship->getOxygenMax(n)>0);
        }
    }

    // Dock
    if (my_spaceship)
    {
        if (my_spaceship->docking_state == DS_Docked)
        {
            P<SpaceShip> ship = my_spaceship->getDockTarget();
            if (ship && my_spaceship->isDockedWith(ship))
            {
                oxygen_label_extern->setVisible(ship->getOxygenMaxTotal()>0);
                for(int n=0; n<max_oxygen_zones; n++)
                {
                    float rate  = ship->getOxygenRechargeRate(n);
                    if (rate > 0)
                        time_left = (ship->getOxygenMax(n) - ship->getOxygenPoints(n)) / rate;
                    else
                        time_left = - ship->getOxygenPoints(n) / rate;

                    oxygen_bar_extern[n]->setValue(ship->getOxygenRatio(n));
                    oxygen_bar_extern[n]->setText(string(int(100 * ship->getOxygenRatio(n))) + "%");

                    if (ship->getOxygenPoints(n) == 0)
                    {
                        oxygen_value_extern[n]->setKey("Zone non oxygenee");
                        oxygen_value_extern[n]->setValue("");
                        oxygen_value_extern[n]->setColor(glm::u8vec4(255,0,0,255));
                    }
                    else if (ship->getOxygenPoints(n) == ship->getOxygenMax(n))
                    {
                        oxygen_value_extern[n]->setKey("Zone oxygenee");
                        oxygen_value_extern[n]->setValue("");
                        oxygen_value_extern[n]->setColor(glm::u8vec4(0,255,0,255));
                    }
                    else if (rate > 0)
                    {
                        oxygen_value_extern[n]->setKey("Oxygene en hausse. Temps avant 100 % : ");
                        oxygen_value_extern[n]->setValue(timeCount(time_left));
                        oxygen_value_extern[n]->setColor(glm::u8vec4(0,255,0,255));
                    }
                    else if (rate < 0)
                    {
                        oxygen_value_extern[n]->setKey("Oxygene en baisse. Temps avant 0 % : ");
                        oxygen_value_extern[n]->setValue(timeCount(time_left));
                        oxygen_value_extern[n]->setColor(glm::u8vec4(255,0,0,255));
                    }
                    else
                    {
                        oxygen_value_extern[n]->setKey("Oxygene stable");
                        oxygen_value_extern[n]->setValue("");
                        oxygen_value_extern[n]->setColor(glm::u8vec4(0,255,0,255));
                    }

                    oxygen_extern[n]->setVisible(ship->getOxygenMax(n)>0);
                }
            }
        }
    }
}

string OxygenScreen::timeCount(float numSec)
{
    string coolTime;
    if (numSec == 0)
        coolTime = "0 seconde";
    else
    {
        int nDays  = std::floor(numSec/(3600*24));
		int nHours = std::floor(numSec/3600 - (nDays*24));
		int nMins  = std::floor(numSec/60 - (nDays*24*60) - (nHours*60));
		int nSecs  = std::floor(numSec - (nDays*24*60*60) - nHours*3600 - nMins *60);
		coolTime = "";
		if (nDays > 0)
			coolTime = coolTime + string(nDays) + " J ";
		if (nHours > 0)
			coolTime = coolTime + string(nHours) + " H ";
		if (nMins > 0)
			coolTime = coolTime + string(nMins) + " M ";
		if (nHours == 0 && nSecs > 0)
			coolTime = coolTime + string(nSecs) + " secondes ";
    }
    return coolTime;
}
