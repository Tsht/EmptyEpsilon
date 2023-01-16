#include "openCommsButton.h"

#include "targetsContainer.h"
#include "gameGlobalInfo.h"
#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "spaceObjects/wormHole.h"
#include "spaceObjects/planet.h"

GuiOpenCommsButton::GuiOpenCommsButton(GuiContainer* owner, string id, string name, TargetsContainer* targets)
: GuiButton(owner, id, name, [this]() {
    if (my_spaceship && this->targets->get())
        my_spaceship->commandOpenTextComm(this->targets->get());
}), targets(targets)
{
    icon_name = "gui/icons/station-relay";
    icon_alignment = ACenterLeft;
}

void GuiOpenCommsButton::onDraw(sp::RenderTarget& renderer)
{
    disable();
    if (gameGlobalInfo->intercept_all_comms_to_gm == CGI_None)
    {
        if (targets->get() && my_spaceship && my_spaceship->isCommsInactive())
        {
            if (P<SpaceShip>(targets->get()) || P<SpaceStation>(targets->get()) || P<WormHole>(targets->get()))
                enable();
        }
    }
    else
    {
        if (my_spaceship && my_spaceship->isCommsInactive())
        {
            enable();
            if (targets->get())
                setText("Comms : " + targets->get()->getCallSign());
            else
                setText("Diffusion large");
        }else
        {
            disable();
        }
    }

    GuiButton::onDraw(renderer);
}

void GuiOpenCommsButton::onHotkey(const HotkeyResult& key)
{
    if (key.category == "RELAY" && my_spaceship)
    {
        if (key.hotkey == "OPEN_COMM")
        {
			if (this->targets->get())
				my_spaceship->commandOpenTextComm(this->targets->get());
		}
	}
}
