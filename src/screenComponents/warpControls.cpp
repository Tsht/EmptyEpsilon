#include <i18n.h>
#include "warpControls.h"
#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "powerDamageIndicator.h"

#include "gui/gui2_slider.h"
#include "gui/gui2_keyvaluedisplay.h"

GuiWarpControls::GuiWarpControls(GuiContainer* owner, string id, P<PlayerSpaceship> targetSpaceship)
: GuiElement(owner, id), target_spaceship(targetSpaceship)
{
    // Build warp request slider.
    slider = new GuiSlider(this, id + "_SLIDER", 4.0, 0.0, 0.0, [this](float value) {
        // Round the slider value to an int.
        int warp_level = value;

        // Send a warp request command to our ship.
        if (target_spaceship)
            target_spaceship->commandWarp(warp_level);

        // Set the slider value to the warp level.
        slider->setValue(warp_level);
    });
    slider->setPosition(0, 0, sp::Alignment::TopLeft)->setSize(50, GuiElement::GuiSizeMax);

    // Snap the slider to integers up to 4.
    slider->addSnapValue(0.0, 0.5);
    slider->addSnapValue(1.0, 0.5);
    slider->addSnapValue(2.0, 0.5);
    slider->addSnapValue(3.0, 0.5);
    slider->addSnapValue(4.0, 0.5);

    if (target_spaceship)
    {
        // Set the slider's value to the current warp request.
        slider->setValue(target_spaceship->warp_request);
    }

    // Label the warp slider.
    label = new GuiKeyValueDisplay(this, id + "_LABEL", 0.5, tr("slider", "Warp"), "0.0");
    label->setTextSize(30)->setPosition(50, 0, sp::Alignment::TopLeft)->setSize(40, GuiElement::GuiSizeMax);

    // Prep the alert overlay.
    pdi = new GuiPowerDamageIndicator(this, id + "_DPI", SYS_Warp, sp::Alignment::TopCenter, target_spaceship);
    pdi->setSize(50, GuiElement::GuiSizeMax);
}

void GuiWarpControls::setTargetSpaceship(P<PlayerSpaceship> targetSpaceship){
    target_spaceship = targetSpaceship;
    pdi->setTargetSpaceship(target_spaceship);
}

void GuiWarpControls::onDraw(sp::RenderTarget& target)
{
    // Update the label with the current warp factor.
    if (target_spaceship)
    {
        label->setValue(string(target_spaceship->current_warp, 1));
        slider->setValue(my_spaceship->warp_request);
//        if (target_spaceship->current_warp > 0)
//        {
//            if (!launch_sound_played)
//            {
//                soundManager->playSound("warp.wav", target_spaceship->getPosition(), 200.0, 1.0);
//                launch_sound_played = true;
//            }
//        }else{
//            launch_sound_played = false;
//        }
            //soundManager->setMusicVolume(int(my_spaceship->current_warp * 25));
    }
}

void GuiWarpControls::onUpdate()
{
    // Handle hotkey input. Warp is a HELMS-category shortcut.
    if (target_spaceship && isVisible())
    {
        if (keys.helms_warp0.getDown())
        {
            target_spaceship->commandWarp(0);
            slider->setValue(0);
        }
        if (keys.helms_warp1.getDown())
        {
            target_spaceship->commandWarp(1);
            slider->setValue(1);
        }
        if (keys.helms_warp2.getDown())
        {
            target_spaceship->commandWarp(2);
            slider->setValue(2);
        }
        if (keys.helms_warp3.getDown())
        {
            target_spaceship->commandWarp(3);
            slider->setValue(3);
        }
        if (keys.helms_warp4.getDown())
        {
            target_spaceship->commandWarp(4);
            slider->setValue(4);
        }
        if (keys.helms_increase_warp.getDown())
        {
            if (my_spaceship->warp_request < 4) {
                my_spaceship->commandWarp(my_spaceship->warp_request+1);
                slider->setValue(my_spaceship->warp_request+1);
            }
        }
        else if (keys.helms_decrease_warp.getDown())
        {
            if (my_spaceship->warp_request > 0) {
                my_spaceship->commandWarp(my_spaceship->warp_request-1);
                slider->setValue(my_spaceship->warp_request-1);
            }
        }
    }
}
