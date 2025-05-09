#include "radarScreen.h"
#include "playerInfo.h"
#include "gameGlobalInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "screenComponents/indicatorOverlays.h"
#include "screenComponents/selfDestructIndicator.h"
#include "screenComponents/globalMessage.h"
#include "screenComponents/jumpIndicator.h"
#include "screenComponents/commsOverlay.h"
#include "screenComponents/shipDestroyedPopup.h"
#include "screenComponents/radarView.h"
#include "screenComponents/alertOverlay.h"
#include "gui/gui2_overlay.h"
RadarScreen::RadarScreen(GuiContainer* owner,string type)
: GuiOverlay(owner, "RADAR_SCREEN", colorConfig.background)
{
     if (type == "tactical" && my_spaceship){
      tactical_radar = new GuiRadarView(this, "TACTICAL", my_spaceship->getShortRangeRadarRange(), nullptr, my_spaceship);
      tactical_radar->setPosition(0, 0, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
      tactical_radar->setRangeIndicatorStepSize(1000.0f)->shortRange()->enableCallsigns()->show();
    }

    if (type == "science" && my_spaceship){
      science_radar = new GuiRadarView(this, "SCIENCE", my_spaceship->getLongRangeRadarRange(), nullptr, my_spaceship);
      science_radar->setPosition(0, 0, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
      science_radar->setRangeIndicatorStepSize(5000.0f)->longRange()->enableCallsigns()->show();
      science_radar->setFogOfWarStyle(GuiRadarView::NebulaFogOfWar);
    }

    if (type == "relay" && my_spaceship){
      relay_radar = new GuiRadarView(this, "RELAY", 50000.0f, nullptr, my_spaceship);
      relay_radar->setPosition(0, 0, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
      relay_radar->setAutoCentering(true);
      relay_radar->longRange()->enableWaypoints()->enableCallsigns()->setStyle(GuiRadarView::Rectangular)->setFogOfWarStyle(GuiRadarView::FriendlysShortRangeFogOfWar);
      relay_radar->show();
    }

    new GuiJumpIndicator(this);
    new GuiSelfDestructIndicator(this);
    new GuiGlobalMessage(this);
    new GuiIndicatorOverlays(this);
}
