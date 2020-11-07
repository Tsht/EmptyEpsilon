#ifndef WEAPONS_SCREEN_H
#define WEAPONS_SCREEN_H

#include "gui/gui2_overlay.h"
#include "screenComponents/radarView.h"
#include "screenComponents/targetsContainer.h"
#include "gui/joystickConfig.h"

class GuiMissileTubeControls;
class GuiKeyValueDisplay;
class GuiToggleButton;
class GuiRotationDial;

class WeaponsScreen : public GuiOverlay, JoystickMappable
{
private:
    GuiOverlay* background_gradient;
    GuiOverlay* background_crosses;

    TargetsContainer targets;
    GuiKeyValueDisplay* energy_display;
    GuiKeyValueDisplay* shields_display;
    GuiKeyValueDisplay* target_display;
    GuiRadarView* radar;
    GuiMissileTubeControls* tube_controls;
    GuiRotationDial* missile_aim;
    GuiToggleButton* lock_aim;
    GuiToggleButton* lock_fire;
public:
    WeaponsScreen(GuiContainer* owner);

    virtual void onDraw(sf::RenderTarget& window) override;
    virtual void onHotkey(const HotkeyResult& key) override;
    virtual void onJoystickAxis(AxisAction& axisAction) override;
};

#endif//WEAPONS_SCREEN_H
