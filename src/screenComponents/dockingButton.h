#ifndef DOCKING_BUTTON_H
#define DOCKING_BUTTON_H

#include "gui/gui2_button.h"
#include "spaceObjects/playerSpaceship.h"

class SpaceObject;
class GuiDockingButton : public GuiButton
{
private:
    P<PlayerSpaceship> target_spaceship;

public:
    GuiDockingButton(GuiContainer* owner, string id, P<PlayerSpaceship> targetSpaceship);
    
    virtual void onUpdate() override;
    virtual void onDraw(sp::RenderTarget& target) override;
    void setTargetSpaceship(P<PlayerSpaceship> targetSpaceship){target_spaceship = targetSpaceship;}

private:
    void click();

    P<SpaceObject> findDockingTarget();
};

#endif//DOCKING_BUTTON_H
