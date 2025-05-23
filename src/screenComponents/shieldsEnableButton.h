#ifndef SHIELDS_ENABLE_BUTTON_H
#define SHIELDS_ENABLE_BUTTON_H

#include "gui/gui2_element.h"
#include "spaceObjects/playerSpaceship.h"

class GuiToggleButton;
class GuiProgressbar;
class GuiPowerDamageIndicator;

class GuiShieldsEnableButton : public GuiElement
{
private:
    P<PlayerSpaceship> target_spaceship;
    GuiToggleButton* button;
    GuiProgressbar* bar;
    GuiPowerDamageIndicator* pdi;
public:
    GuiShieldsEnableButton(GuiContainer* owner, string id, P<PlayerSpaceship> targetSpaceship);

    virtual void onDraw(sp::RenderTarget& target) override;
    void setTargetSpaceship(P<PlayerSpaceship> targetSpaceship);

    virtual void onUpdate() override;
};

#endif//SHIELDS_ENABLE_BUTTON_H
