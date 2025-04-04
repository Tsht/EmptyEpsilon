#include "engineeringAdvancedScreen.h"

#include "gameGlobalInfo.h"
#include "screenComponents/shieldFreqencySelect.h"
#include "screenComponents/shieldsEnableButton.h"
#include "playerInfo.h"

EngineeringAdvancedScreen::EngineeringAdvancedScreen(GuiContainer* owner)
: EngineeringScreen(owner, engineeringAdvanced)
{
    if (gameGlobalInfo->use_beam_shield_frequencies)
    {
        //The shield frequency selection includes a shield enable button.
        (new GuiShieldFrequencySelect(this, "SHIELD_FREQ", my_spaceship))->setPosition(-20, 100, sp::Alignment::TopRight)->setSize(240, 100);
    }else{
        (new GuiShieldsEnableButton(this, "SHIELDS_ENABLE", my_spaceship))->setPosition(-20, 200, sp::Alignment::TopRight)->setSize(240, 50);
    }
}
