#include "cicScreen.h"
#include "gameGlobalInfo.h"
#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "spaceObjects/wormHole.h"
#include "spaceObjects/scanProbe.h"
#include "spaceObjects/planet.h"
#include "scriptInterface.h"
#include "gameGlobalInfo.h"
#include "main.h" //bold_font

#include "screenComponents/radarView.h"
#include "screenComponents/openCommsButton.h"
#include "screenComponents/commsOverlay.h"
#include "screenComponents/shipsLogControl.h"
#include "screenComponents/customShipFunctions.h"
#include "screenComponents/alertLevelButton.h"
#include "screenComponents/blueprintControls.h"
#include "screenComponents/squadronControls.h"
#include "screenComponents/launchSquadronControls.h"

#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_selector.h"
#include "gui/gui2_slider.h"
#include "gui/gui2_label.h"
#include "gui/gui2_togglebutton.h"
#include "gui/gui2_progressbar.h"
#include "gui/gui2_textentry.h"


CicScreen::CicScreen(GuiContainer* owner, bool allow_comms)
: GuiOverlay(owner, "CIC_SCREEN", colorConfig.background), mode(TargetSelection)
{
    target_for_ai.setAllowWaypointSelection();
    radar = new GuiRadarView(this, "CIC_RADAR", 50000.0f, &targets_squadron, my_spaceship);
    //radar long range is only for two things : draw ship without beam arc etc. and set distance. This is hacky.
    radar->longRange()->enableWaypoints()->enableCallsigns()->enableSquadronBeamArcs()->setStyle(GuiRadarView::Rectangular)->setFogOfWarStyle(GuiRadarView::FriendlysShortRangeFogOfWar);
    radar->setAutoCentering(false);
    radar->setPosition(0, 0, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    radar->setCallbacks(
        [this](sp::io::Pointer::Button button, glm::vec2 position) { //down
            if (mode == TargetSelection && target_for_ai.getWaypointIndex() > -1 && my_spaceship)
            {
                if (glm::length(my_spaceship->waypoints[target_for_ai.getWaypointIndex()] - position) < 1000.0f)
                {
                    mode = MoveWaypoint;
                    drag_waypoint_index = target_for_ai.getWaypointIndex();
                }
            }
            mouse_down_position = position;
            button_pressed = button;
        },
        [this](glm::vec2 position) { //drag
            // if (mode == TargetSelection)
            //     {
            //         position_text_custom = false;
            //         glm::vec2 newPosition = radar->getViewPosition() - (position - mouse_down_position);
            //         radar->setViewPosition(newPosition);
            //         if(!position_text_custom && my_spaceship)
            //             position_text->setText(getStringFromPosition(newPosition, my_spaceship->correction_x, my_spaceship->correction_y));
            //     }
            if (mode == MoveWaypoint && my_spaceship)
                my_spaceship->commandMoveWaypoint(drag_waypoint_index, position);
        },
        [this](glm::vec2 position) { //up
            switch(mode)
            {
            case TargetSelection:
                {
                    P<CpuShip> sel_squadron_leader = targets_squadron.get();
                    if(!sel_squadron_leader)
                    {
                        return;
                    }
                    int squadron_index{-1};
                    int n=0;
                    for(auto info : my_spaceship->launched_squadrons_infos)
                    {
                        if(sel_squadron_leader->getMultiplayerId() == info.leader_id)
                        {
                            squadron_index = n;
                            break;
                        }
                        n++;
                    }
                    if(squadron_index == -1)
                    {
                        return;
                    }
                    if(sel_squadron_leader)
                    {
                        if (button_pressed == sp::io::Pointer::Button::Right)
                        {           
                            bool shift_down = SDL_GetModState() & KMOD_SHIFT;
                            target_for_ai.setToClosestTo(position, 1000, TargetsContainer::Targetable, sel_squadron_leader);
                            
                            P<SpaceObject> target  = target_for_ai.get();
                            if(target)
                            {
                                if (target != sel_squadron_leader && target->canBeTargetedBy(sel_squadron_leader))
                                {
                                    EAIOrder order = AI_Idle;
                                    if (sel_squadron_leader->isEnemy(target) || ((target->getScannedStateFor(my_spaceship) == SS_NotScanned)))
                                    {
                                        order = AI_Attack;
                                        if(shift_down)
                                        {
                                            order = AI_DefendTarget;
                                        }
                                    }
                                    else
                                    {
                                        order = AI_DefendTarget;
                                        if(shift_down)
                                        {
                                            if(target == my_spaceship)
                                            {
                                                if(target->canBeDockedBy(sel_squadron_leader) != DockStyle::None)
                                                {
                                                    order = AI_Dock;
                                                }
                                                else
                                                {
                                                    LOG(WARNING) << "Squadron class can't dock with target ship !";
                                                    LOG(WARNING) << "Class: " << sel_squadron_leader->ship_template->getClass();
                                                    for(auto & auth : my_spaceship->ship_template->external_dock_classes)
                                                    {
                                                        LOG(WARNING) << "Authorized: " << auth;
                                                    }
                                                    //Still defend order
                                                }
                                            }
                                            else
                                            {
                                                order = AI_Attack;
                                            }
                                        }
                                    }
                                    
                                    my_spaceship->commandOrderSquadronTarget(order, squadron_index,target);
                                }
                            }
                            else
                            {
                                glm::vec2 upper_bound(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
                                glm::vec2 lower_bound(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
                                lower_bound.x = std::min(lower_bound.x, sel_squadron_leader->getPosition().x);
                                lower_bound.y = std::min(lower_bound.y, sel_squadron_leader->getPosition().y);
                                upper_bound.x = std::max(upper_bound.x, sel_squadron_leader->getPosition().x);
                                upper_bound.y = std::max(upper_bound.y, sel_squadron_leader->getPosition().y);
                                glm::vec2 objects_center = (upper_bound + lower_bound) / 2.0f;

                                if(shift_down)
                                {
                                    my_spaceship->commandOrderSquadronPosition(AI_FlyTowardsBlind, squadron_index,position + (sel_squadron_leader->getPosition() - objects_center));
                                }
                                else
                                {
                                    my_spaceship->commandOrderSquadronPosition(AI_FlyTowards, squadron_index,position + (sel_squadron_leader->getPosition() - objects_center));
                                }
                            }
                        }
                    }

                //target_for_ai.setToClosestTo(position, 1000, TargetsContainer::Selectable, my_spaceship);
                }
                break;
            case WaypointPlacement:
                if (my_spaceship)
                    my_spaceship->commandAddWaypoint(position);
                mode = TargetSelection;
                option_buttons->show();
                bp_layout->show();
                break;
            case MoveWaypoint:
                mode = TargetSelection;
                target_for_ai.setWaypointIndex(drag_waypoint_index);
                break;
            }
        }
    );

    if (my_spaceship)
        radar->setViewPosition(my_spaceship->getPosition());

    auto sidebar = new GuiElement(this, "SIDE_BAR");
    sidebar->setPosition(-20, 150, sp::Alignment::TopRight)->setSize(250, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    info_callsign = new GuiKeyValueDisplay(sidebar, "CIC_CALLSIGN", 0.5, tr("Callsign"), "");
    info_callsign->setSize(GuiElement::GuiSizeMax, 30);
    info_distance_to_ship = new GuiKeyValueDisplay(sidebar, "CIC_DISTANCE", 0.5, tr("Distance to ship"), "");
    info_distance_to_ship->setSize(GuiElement::GuiSizeMax, 30);
    info_bearing_to_ship = new GuiKeyValueDisplay(sidebar, "CIC_BEARING", 0.5, tr("Bearing to ship"), "");
    info_bearing_to_ship->setSize(GuiElement::GuiSizeMax, 30);

    // Controls for the radar view
    view_controls = new GuiElement(this, "VIEW_CONTROLS");
    view_controls->setPosition(20, -70, sp::Alignment::BottomLeft)->setSize(250, GuiElement::GuiSizeMax)->setAttribute("layout", "verticalbottom");;
    zoom_slider = new GuiSlider(this, "ZOOM_SLIDER", max_distance, min_distance, 50000.0f, [this](float value) {
        zoom_label->setText(tr("Zoom: {zoom}x").format({{"zoom", string(max_distance / value, 1.0f)}}));
        radar->setDistance(value);
    });
    zoom_slider->setPosition(20, -70, sp::Alignment::BottomLeft)->setSize(GuiElement::GuiSizeMax, 50);
    zoom_slider->setVisible(false);
    zoom_label = new GuiLabel(zoom_slider, "", "Zoom: " + string(max_distance / radar->getDistance(), 1.0f) + "x", 30);
    zoom_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

   
    // Option buttons for waypoints.
    option_buttons = new GuiElement(this, "BUTTONS");
    option_buttons->setPosition(20, 50, sp::Alignment::TopLeft)->setSize(250, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    (new GuiLabel(option_buttons, "", " ", 30))->setSize(GuiElement::GuiSizeMax, 50);

    // Manage waypoints.
    waypoints_layout = new GuiElement(option_buttons, "WAYPOINTS_LAYOUT");
    waypoints_layout -> setSize(GuiElement::GuiSizeMax, 50)->setAttribute("layout", "horizontal");;

    (new GuiLabel(waypoints_layout, "", tr("Place Waypoint"), 30))->setAlignment(sp::Alignment::Center)->setSize(150, 50);
    add_waypoint_button = new GuiButton(waypoints_layout, "WAYPOINT_PLACE_BUTTON", "+", [this]() {
        mode = WaypointPlacement;
        option_buttons->hide();

    });
    add_waypoint_button->setSize(50, 50);

    delete_waypoint_button = new GuiButton(waypoints_layout, "WAYPOINT_DELETE_BUTTON", "-", [this]() {
        if (my_spaceship && target_for_ai.getWaypointIndex() >= 0)
        {
            my_spaceship->commandRemoveWaypoint(target_for_ai.getWaypointIndex());
        }
    });
    delete_waypoint_button->setSize(50, 50);

    // Center screen
    center_screen_button = new GuiButton(option_buttons, "CENTER_SCREEN_BUTTON", tr("Recenter radar"), [this]() {
        if (my_spaceship)
        {
            radar->setViewPosition(my_spaceship->getPosition());
            //position_text->setText(getStringFromPosition(my_spaceship->getPosition(), my_spaceship->correction_x, my_spaceship->correction_y));
        }
    });
    center_screen_button->setSize(GuiElement::GuiSizeMax, 50);
    center_screen_button->setIcon("gui/icons/lock");;

    // Scenario clock display.
    info_clock = new GuiKeyValueDisplay(option_buttons, "INFO_CLOCK", 0.67f, tr("Clock") + ":", "");
    info_clock->setSize(GuiElement::GuiSizeMax, 40);

    nbr_squadrons = new GuiKeyValueDisplay(option_buttons, "NBR_SQUADRON", 0.67f, tr("cic","In flight") + ":", "");
    nbr_squadrons->setSize(GuiElement::GuiSizeMax, 40);

    //bp_layout = new GuiElement(this, "BLUEPRINTS_LAYOUT");
    bp_layout = new GuiBlueprintsControls(this, "BLUEPRINTS_LAYOUT", my_spaceship);
    bp_layout->setPosition(20, 300, sp::Alignment::TopLeft)->setSize(250, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    sqc_layout = new GuiLaunchSquadronControls(this, "SQUADRON_CONTROL", my_spaceship);
    sqc_layout->setPosition(20, 500, sp::Alignment::TopLeft)->setSize(250, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    launched_squadron_layout = new GuiSquadronControls(this, "LAUNCHED_SQUADRON_LAYOUT", my_spaceship, radar, targets_squadron);
    launched_squadron_layout->setPosition(-120, 300, sp::Alignment::TopRight)->setSize(250, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    //(new GuiAlertLevelSelect(this, ""))->setPosition(-20, -70, sp::Alignment::BottomRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "verticalbottom");

    (new GuiCustomShipFunctions(this, cagOfficer, "", my_spaceship))->setPosition(-20, 350, sp::Alignment::TopRight)->setSize(250, GuiElement::GuiSizeMax);

    new ShipsLog(this,"generic");
    if (allow_comms)
    {
        (new GuiCommsOverlay(this))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    }
}

void CicScreen::onDraw(sp::RenderTarget& renderer)
{
    ///Handle mouse wheel
    float mouse_wheel_delta = keys.zoom_in.getValue() - keys.zoom_out.getValue();
    if (mouse_wheel_delta != 0.0f)
    {
        float view_distance = radar->getDistance() * (1.0f - (mouse_wheel_delta * 0.1f));
        zoom_slider->setValue(view_distance);
        view_distance = zoom_slider->getValue();
        radar->setDistance(view_distance);
        zoom_label->setText("Zoom: " + string(max_distance / view_distance, 1.0f) + "x");
    }
    ///!

    GuiOverlay::onDraw(renderer);

    // if (target_for_ai.get() && my_spaceship)
    // {
    //     // Check each object to determine whether the target is still within
    //     // shared radar range of a friendly object.
    //     P<SpaceObject> target = target_for_ai.get();
    //     bool near_friendly = false;

    //     // For each SpaceObject on the map...
    //     foreach(SpaceObject, obj, space_object_list)
    //     {
    //         // POUR DAID VERSION
    //         // Consider the object only if it is:
    //         // - Any ShipTemplateBasedObject (ship or station)
    //         // - A SpaceObject belonging to a friendly faction
    //         // - The player's ship
    //         // - A scan probe owned by the player's ship
    //         // This check is duplicated from GuiRadarView::drawObjects.

    //         // C'est different ici 

    //         P<ShipTemplateBasedObject> stb_obj = obj;

    //         if ((!stb_obj && !P<WormHole>(obj) && !P<Planet>(obj)) 
    //             || (!obj->isFriendly(my_spaceship) && obj != my_spaceship))
    //         {
    //             P<ScanProbe> sp = obj;

    //             if (!sp || sp->owner_id != my_spaceship->getMultiplayerId())
    //             {
    //                 continue;
    //             }
    //         }

    //         //On ne peut sélectionner que des vaisseaux qui sont dans la liste des escadres
            
    //         // Set the targetable radius to getShortRangeRadarRange() if the
    //         // object's a ShipTemplateBasedObject. Otherwise, default to 5U.
    //         float r = stb_obj ? stb_obj->getShortRangeRadarRange() : 5000.0f;

    //         // If the target is within the short-range radar range/5U of the
    //         // object, consider it near a friendly object.
    //         if (glm::length2(obj->getPosition() - target->getPosition()) < r * r)
    //         {
    //             near_friendly = true;
    //             break;
    //         }
    //     }

    //     if (!near_friendly)
    //     {
    //         // If the target is no longer near a friendly object, unset it as
    //         // the target.
    //         target_for_ai.clear();
    //     }
    // }

    // if (target_for_ai.get() && my_spaceship)
    // {
    //     P<SpaceObject> obj = target_for_ai.get();
    //     P<SpaceShip> ship = obj;
    //     P<SpaceStation> station = obj;
    //     P<ScanProbe> probe = obj;

    //     distance = glm::length(obj->getPosition() - my_spaceship->getPosition()) / 1000.0f;
    //     info_distance -> setValue(string(distance, 1.0f) + " U");

    //     info_callsign->setValue(obj->getCallSign());

    //     if (factionInfo[obj->getFactionId()]) {
    //         if (ship)
    //         {
    //             if (ship->getScannedStateFor(my_spaceship) >= SS_SimpleScan)
    //             {
    //                 info_faction->setValue(factionInfo[obj->getFactionId()]->getLocaleName());
    //             }
    //         }else{
    //             info_faction->setValue(factionInfo[obj->getFactionId()]->getLocaleName());
    //         }
    //     }

    // }
    // else
    P<CpuShip> sel_squadron_leader = targets_squadron.get();
    if(sel_squadron_leader)
    {
        info_callsign->setValue(sel_squadron_leader->getCallSign());
        float distance = glm::length(sel_squadron_leader->getPosition() - my_spaceship->getPosition()) / 1000.0f;
        info_distance_to_ship->setValue(string(distance, 1.0f) + " U");
        float heading = vec2ToAngle(sel_squadron_leader->getPosition() - my_spaceship->getPosition()) - 90;
        while(heading < 0) heading += 360;
        info_bearing_to_ship->setValue(int(heading));
    }
    else
    {
        info_callsign->setValue("-");
        info_distance_to_ship->setValue("-");
        info_bearing_to_ship->setValue("-");
    }
    if (my_spaceship)
    {
        //info_reputation->setValue(string(my_spaceship->getReputationPoints(), 0)); //tsht : TODO voir si on peut reajouter
        info_clock->setValue(gameGlobalInfo->getMissionTime());
    }
      if (my_spaceship)
    {
        const unsigned int total = my_spaceship->getLaunchedSquadronsCount();
        nbr_squadrons->setValue(string(total) + " / " + string(my_spaceship->getMaximumNumberOfSquadronsInFlight()));
    }

    if (target_for_ai.getWaypointIndex() >= 0)
    {
        delete_waypoint_button->enable();
        //distance = glm::length(my_spaceship->waypoints[target_for_ai.getWaypointIndex()] - my_spaceship->getPosition()) / 1000.0f;
        //info_distance -> setValue(string(distance, 1.0f) + " U");
    }
    else
        delete_waypoint_button->disable();

  
}

void CicScreen::onUpdate()
{
    if (my_spaceship)
    {
        
        if (keys.relay_add_waypoint.getDown())
        {
            mode = WaypointPlacement;
            option_buttons->hide();
            bp_layout->hide();
        }
        if (keys.relay_delete_waypoint.getDown())
        {
            if (target_for_ai.getWaypointIndex() >= 0)
                my_spaceship->commandRemoveWaypoint(target_for_ai.getWaypointIndex());
        }
        //TODO check
        // if (key.hotkey == "INCREASE_ZOOM")
        // {
        //     float view_distance = radar->getDistance() * 1.1f;
        //     if (view_distance > max_distance)
        //         view_distance = max_distance;
        //     if (view_distance < min_distance)
        //         view_distance = min_distance;
        //     radar->setDistance(view_distance);
        //     // Keep the zoom slider in sync.
        //     zoom_slider->setValue(view_distance);
        //     zoom_label->setText("Zoom: " + string(max_distance / view_distance, 1.0f) + "x");
        // }
        // if (key.hotkey == "DECREASE_ZOOM")
        // {
        //     float view_distance = radar->getDistance() * 0.9f;
        //     if (view_distance > max_distance)
        //         view_distance = max_distance;
        //     if (view_distance < min_distance)
        //         view_distance = min_distance;
        //     radar->setDistance(view_distance);
        //     // Keep the zoom slider in sync.
        //     zoom_slider->setValue(view_distance);
        //     zoom_label->setText("Zoom: " + string(max_distance / view_distance, 1.0f) + "x");
        // }
        // if (keys.relay_alert_normal.getDown())
        // {
        //     my_spaceship->commandSetAlertLevel(AL_Normal);
        //     for(GuiButton* button : alert_level_buttons)
        //         button->setVisible(false);
        //     alert_level_button->setValue(false);
        // }
        // if (keys.relay_alert_yellow.getDown())
        // {
        //     my_spaceship->commandSetAlertLevel(AL_YellowAlert);
        //     for(GuiButton* button : alert_level_buttons)
        //         button->setVisible(false);
        //     alert_level_button->setValue(false);
        // }
        // if (keys.relay_alert_red.getDown())
        // {
        //     my_spaceship->commandSetAlertLevel(AL_RedAlert);
        //     for(GuiButton* button : alert_level_buttons)
        //         button->setVisible(false);
        //     alert_level_button->setValue(false);
        // }
    }
}
