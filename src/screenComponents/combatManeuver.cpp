#include <i18n.h>
#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "combatManeuver.h"
#include "powerDamageIndicator.h"
#include "snapSlider.h"

#include "gui/gui2_progressbar.h"

GuiCombatManeuver::GuiCombatManeuver(GuiContainer* owner, string id, P<PlayerSpaceship> targetSpaceship)
: GuiElement(owner, id), target_spaceship(targetSpaceship)
{
    charge_bar = new GuiProgressbar(this, id + "_CHARGE", 0.0, 1.0, 0.0);
    charge_bar->setColor(glm::u8vec4(192, 192, 192, 64));
    charge_bar->setPosition(0, 0, sp::Alignment::BottomCenter)->setSize(GuiElement::GuiSizeMax, 50);
    (new GuiLabel(charge_bar, "CHARGE_LABEL", tr("Combat maneuver"), 20))->setPosition(0, 0, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    slider = new GuiSnapSlider2D(this, id + "_STRAFE", glm::vec2(-1.0, 1.0), glm::vec2(1.0, 0.0), glm::vec2(0.0, 0.0), [this](glm::vec2 value) {
        if (target_spaceship)
        {
            target_spaceship->commandCombatManeuverBoost(value.y);
            target_spaceship->commandCombatManeuverStrafe(value.x);
        }
    });
    slider->setPosition(0, -50, sp::Alignment::BottomCenter)->setSize(GuiElement::GuiSizeMax, 165);

    strafe_pdi = new GuiPowerDamageIndicator(slider, id + "_STRAFE_INDICATOR", SYS_Maneuver, sp::Alignment::CenterLeft, target_spaceship);
    strafe_pdi->setPosition(0, 0, sp::Alignment::BottomLeft)->setSize(GuiElement::GuiSizeMax, 50);
    boost_pdi = new GuiPowerDamageIndicator(slider, id + "_BOOST_INDICATOR", SYS_Impulse, sp::Alignment::BottomLeft, target_spaceship);
    boost_pdi->setPosition(0, -50, sp::Alignment::BottomLeft)->setSize(GuiElement::GuiSizeMax, 50);
}

 void GuiCombatManeuver::setTargetSpaceship(P<PlayerSpaceship> targetSpaceship){
    target_spaceship = targetSpaceship;
    strafe_pdi->setTargetSpaceship(target_spaceship);
    boost_pdi->setTargetSpaceship(target_spaceship);
}

void GuiCombatManeuver::onUpdate()
{
    setVisible(my_spaceship && my_spaceship->getCanCombatManeuver()); //FIXME c'est pas target spaceship ?

    if (isVisible())
    {
        float strafe = keys.helms_combat_right.getValue() - keys.helms_combat_left.getValue();
        float boost = std::max(0.0f, keys.helms_combat_boost.getValue());
        if (strafe != 0.0f || hotkey_strafe_active)
        {
            setStrafeValue(strafe);
            my_spaceship->commandCombatManeuverStrafe(strafe);
        }
        hotkey_strafe_active = strafe != 0.0f;
        if (boost > 0.0f || hotkey_boost_active)
        {
            setBoostValue(boost);
            my_spaceship->commandCombatManeuverBoost(boost);
        }
        hotkey_boost_active = boost > 0.0f;
        // if (key.hotkey == "COMBAT_STOP") //FIXME
        // {
        //     setBoostValue(0.0f);
        //     setStrafeValue(0.0f);
        //     my_spaceship->commandCombatManeuverBoost(0.0f);
        //     my_spaceship->commandCombatManeuverStrafe(0.0f);
        // }
    }
}

void GuiCombatManeuver::onDraw(sp::RenderTarget& target)
{
    if (target_spaceship)
    {
        if (target_spaceship->combat_maneuver_boost_speed <= 0.0f && target_spaceship->combat_maneuver_strafe_speed <= 0.0f)
        {
            charge_bar->hide();
            slider->hide();
        }else{
            charge_bar->setValue(target_spaceship->combat_maneuver_charge)->show();
            slider->show();
        }
        if (target_spaceship->combat_maneuver_boost_speed <= 0.f)
            setBoostValue(0.0f);
        if (target_spaceship->combat_maneuver_strafe_speed <= 0.f)
            setStrafeValue(0.0f);
    }
}

void GuiCombatManeuver::setBoostValue(float value)
{
    slider->setValue(glm::vec2(slider->getValue().x, value));
    my_spaceship->commandCombatManeuverBoost(value);
}

void GuiCombatManeuver::setStrafeValue(float value)
{
    slider->setValue(glm::vec2(value, slider->getValue().y));
    my_spaceship->commandCombatManeuverStrafe(value);
}
