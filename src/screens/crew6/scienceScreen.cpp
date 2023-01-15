#include "playerInfo.h"
#include "gameGlobalInfo.h"
#include "scienceScreen.h"
#include "scienceDatabase.h"
#include "spaceObjects/nebula.h"
#include "spaceObjects/asteroid.h"
#include "spaceObjects/mine.h"
#include "preferenceManager.h"
#include "shipTemplate.h"

#include "screenComponents/radarView.h"
#include "screenComponents/rawScannerDataRadarOverlay.h"
#include "screenComponents/scanTargetButton.h"
#include "screenComponents/frequencyCurve.h"
#include "screenComponents/scanningDialog.h"
#include "screenComponents/databaseView.h"
#include "screenComponents/alertOverlay.h"
#include "screenComponents/customShipFunctions.h"
#include "screenComponents/shipsLogControl.h"
#include "screenComponents/noRadarPopup.h"

#include "gui/gui2_autolayout.h"
#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_togglebutton.h"
#include "gui/gui2_selector.h"
#include "gui/gui2_scrolltext.h"
#include "gui/gui2_listbox.h"
#include "gui/gui2_slider.h"

ScienceScreen::ScienceScreen(GuiContainer* owner, ECrewPosition crew_position)
: GuiOverlay(owner, "SCIENCE_SCREEN", colorConfig.background)
{
    targets.setAllowWaypointSelection();

    // Render the radar shadow and background decorations.
    background_gradient = new GuiOverlay(this, "BACKGROUND_GRADIENT", sf::Color::White);
    background_gradient->setTextureCenter("gui/background/gradientOffset.png");

    background_crosses = new GuiOverlay(this, "BACKGROUND_CROSSES", sf::Color::White);
    background_crosses->setTextureTiled("gui/background/crosses.png");

    // Render the alert level color overlay.
    (new AlertLevelOverlay(this));

    // Draw the radar.
    radar_view = new GuiElement(this, "RADAR_VIEW");
    radar_view->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    // Draw the science radar.
    science_radar = new GuiRadarView(radar_view, "SCIENCE_RADAR", my_spaceship ? my_spaceship->getLongRangeRadarRange() : 30000.0, &targets, my_spaceship);
    science_radar->setPosition(-270, 0, ACenterRight)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    science_radar->setRangeIndicatorStepSize(5000.0)->longRange()->enableWaypoints()->enableCallsigns()->enableHeadingIndicators()->setStyle(GuiRadarView::Circular)->setFogOfWarStyle(GuiRadarView::NebulaFogOfWar);
    science_radar->setCallbacks(
        [this](glm::vec2 position) {
            if (!my_spaceship || my_spaceship->scanning_delay > 0.0)
                return;

            targets.setToClosestTo(position, 1000, TargetsContainer::Selectable, my_spaceship);
        }, nullptr, nullptr
    );
    science_radar->setAutoRotating(PreferencesManager::get("science_radar_lock","0")=="1");
    new RawScannerDataRadarOverlay(science_radar, "", my_spaceship ? my_spaceship->getLongRangeRadarRange() : 30000.0f);

    // Draw and hide the probe radar.
    probe_radar = new GuiRadarView(radar_view, "PROBE_RADAR", my_spaceship->getProbeRangeRadarRange(), &targets, my_spaceship);
    probe_radar->setPosition(-270, 0, ACenterRight)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->hide();
    probe_radar->setAutoCentering(false)->longRange()->enableWaypoints()->enableCallsigns()->enableHeadingIndicators()->setStyle(GuiRadarView::Circular)->setFogOfWarStyle(GuiRadarView::NoFogOfWar);
    probe_radar->setCallbacks(
        [this](glm::vec2 position) {
            if (!my_spaceship || my_spaceship->scanning_delay > 0.0)
                return;

            targets.setToClosestTo(position, 1000, TargetsContainer::Selectable, my_spaceship);
        }, nullptr, nullptr
    );
    new RawScannerDataRadarOverlay(probe_radar, "", my_spaceship->getProbeRangeRadarRange()); 

    sidebar_selector = new GuiSelector(radar_view, "", [this](int index, string value)
    {
        info_sidebar->setVisible(index == 0);
        custom_function_sidebar->setVisible(index == 1);
    });
    sidebar_selector->setOptions({"Scanning", "Other"});
    sidebar_selector->setSelectionIndex(0);
    sidebar_selector->setPosition(-20, 120, ATopRight)->setSize(250, 50);

    // Target scan data sidebar.
    info_sidebar = new GuiAutoLayout(radar_view, "SIDEBAR", GuiAutoLayout::LayoutVerticalTopToBottom);
    info_sidebar->setPosition(-20, 100, ATopRight)->setSize(250, GuiElement::GuiSizeMax);

    custom_function_sidebar = new GuiCustomShipFunctions(radar_view, crew_position, "", my_spaceship);
    custom_function_sidebar->setPosition(-270, 20, ATopRight)->setSize(200, GuiElement::GuiSizeMax)->hide();

    // Scan button.
    scan_button = new GuiScanTargetButton(info_sidebar, "SCAN_BUTTON", &targets);
    scan_button->setSize(GuiElement::GuiSizeMax, 50)->setVisible(my_spaceship && my_spaceship->getCanScan());

    // Simple scan data.
    info_callsign = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_CALLSIGN", 0.4, tr("Callsign"), "");
    info_callsign->setSize(GuiElement::GuiSizeMax, 30);
    info_distance = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_DISTANCE", 0.4, tr("science","Distance"), "");
    info_distance->setSize(GuiElement::GuiSizeMax, 30);
    info_heading = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_HEADING", 0.4, tr("Bearing"), "");
    info_heading->setSize(GuiElement::GuiSizeMax, 30);
    info_relspeed = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_REL_SPEED", 0.4, tr("Rel. Speed"), "");
    info_relspeed->setSize(GuiElement::GuiSizeMax, 30);
    info_faction = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_FACTION", 0.4, tr("Faction"), "");
    info_faction->setSize(GuiElement::GuiSizeMax, 30);
    info_type = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_TYPE", 0.4, tr("science","Type"), "");
    info_type->setSize(GuiElement::GuiSizeMax, 30);
    info_type_button = new GuiButton(info_type, "SCIENCE_TYPE_BUTTON", tr("database", "DB"), [this]() {
        P<SpaceShip> ship = targets.get();
        P<SpaceStation> station = targets.get();
        if (ship)
        {
            P<ShipTemplate> st = ship->ship_template;
            if (database_view->findAndDisplayEntry(st->getLocaleName()))
            {
                view_mode_selection->setSelectionIndex(1);
                radar_view->hide();
                background_gradient->hide();
                database_view->show();
            }
        }
        if (station)
        {
            P<ShipTemplate> st = station->ship_template;
            if (database_view->findAndDisplayEntry(st->getLocaleName()))
            {
                view_mode_selection->setSelectionIndex(1);
                radar_view->hide();
                background_gradient->hide();
                database_view->show();
            }
        }
    });
    info_type_button->setTextSize(20)->setPosition(0, 1, ATopLeft)->setSize(50, 28);
    info_shields = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_SHIELDS", 0.4, tr("science", "Shields"), "");
    info_shields->setSize(GuiElement::GuiSizeMax, 30);
    info_hull = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_HULL", 0.4, tr("science", "Hull"), "");
    info_hull->setSize(GuiElement::GuiSizeMax, 30);
    info_signatures = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_SIGNATURES", 0.4, "Signatures", "");
    info_signatures->setSize(GuiElement::GuiSizeMax, 30);

    // Full scan data

    // Draw and hide the sidebar pager.
    sidebar_pager = new GuiSelector(info_sidebar, "SIDEBAR_PAGER", [this](int index, string value) {});
    sidebar_pager->setSize(GuiElement::GuiSizeMax, 50)->show();

    // If the server uses frequencies, add the Tactical sidebar page.
    if (gameGlobalInfo->use_beam_shield_frequencies)
    {
        sidebar_pager->addEntry("Tactique", "Tactique");
    }

    // Add sidebar page for systems.
    sidebar_pager->addEntry("Systemes", "Systemes");

    // Add sidebar page for a description.
    sidebar_pager->addEntry("Description", "Description");

    // Add sidebar page for informations.
    sidebar_pager->addEntry("Informations", "Informations");

    // Default the pager to the first item.
    sidebar_pager->setSelectionIndex(0);

    // Prep and hide the frequency graphs.
    info_shield_frequency = new GuiFrequencyCurve(info_sidebar, "SCIENCE_SHIELD_FREQUENCY", false, true);
    info_shield_frequency->setSize(GuiElement::GuiSizeMax, 150);
    info_beam_frequency = new GuiFrequencyCurve(info_sidebar, "SCIENCE_BEAM_FREQUENCY", true, false);
    info_beam_frequency->setSize(GuiElement::GuiSizeMax, 150);

    // Show shield and beam frequencies only if enabled by the server.
    if (!gameGlobalInfo->use_beam_shield_frequencies)
    {
        info_shield_frequency->hide();
        info_beam_frequency->hide();
    }

    // List each system's status.
    for(int n = 0; n < SYS_COUNT; n++)
    {
        info_system[n] = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_SYSTEM_" + string(n), 0.75, getLocaleSystemName(ESystem(n)), "-");
        info_system[n]->setSize(GuiElement::GuiSizeMax, 30);
        info_system[n]->hide();
    }

    info_oxygen = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_SYSTEM_OXYGEN", 0.75, "Oxygene", "-");
    info_oxygen->setSize(GuiElement::GuiSizeMax, 30);
    info_oxygen->hide();

    // Prep and hide the description text area.
    info_description = new GuiScrollText(info_sidebar, "SCIENCE_DESC", "");
    info_description->setTextSize(28)->setMargins(20, 20, 0, 0)->setSize(GuiElement::GuiSizeMax, 350)->hide();

    for(int n = 0; n < 10; n++)
    {
        info_other[n] = new GuiKeyValueDisplay(info_sidebar, "SCIENCE_INFOS_" + string(n), 0.6, "-", "-");
        info_other[n]->setSize(GuiElement::GuiSizeMax, 30);
        info_other[n]->hide();
    }

    // Prep and hide the database view.
    database_view = new DatabaseViewComponent(this);
    database_view->hide()->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    // Probe view button
    probe_view_button = new GuiToggleButton(radar_view, "PROBE_VIEW", tr("button", "Probe View"), [this](bool value){
        P<ScanProbe> probe;

        if (game_server)
            probe = game_server->getObjectById(my_spaceship->linked_science_probe_id);
        else
            probe = game_client->getObjectById(my_spaceship->linked_science_probe_id);

        if (value && probe)
        {
            auto probe_position = probe->getPosition();
            science_radar->hide();
            probe_radar->show();
            probe_radar->setViewPosition(probe_position)->show();

            zoom_slider->hide(); //FIXME plus bas pour afficher la bonne chose avec la sonde
            zoom_label->hide();
        }else{
            probe_view_button->setValue(false);
            science_radar->show();
            probe_radar->hide();

            zoom_slider->show();
            zoom_label->show();
        }
    });
    probe_view_button->setPosition(20, -160, ABottomLeft)->setSize(200, 50)->disable();

    // Link target to analysis screen.
    link_to_analysis_button = new GuiToggleButton(radar_view, "LINK_TO_ANALYSIS", tr("Link to Analysis"), [this](bool value){
        if (value)
            my_spaceship->commandSetAnalysisLink(targets.get()->getMultiplayerId());
        else
            my_spaceship->commandSetAnalysisLink(-1);
    });
    link_to_analysis_button->setPosition(-20, -120, ABottomRight)->setSize(250, 50);

    // Draw the zoom slider.
    zoom_slider = new GuiSlider(radar_view, "", my_spaceship ? my_spaceship->getLongRangeRadarRange() : 30000.0f, my_spaceship ? my_spaceship->getShortRangeRadarRange() : 5000.0f, my_spaceship ? my_spaceship->getLongRangeRadarRange() : 30000.0f, [this](float value)
    {
        if (my_spaceship)
            zoom_label->setText(tr("Zoom: {zoom}x").format({{"zoom", string(my_spaceship->getLongRangeRadarRange() / value, 1)}}));
        science_radar->setDistance(value);
    });
    zoom_slider->setPosition(-20, -60, ABottomRight)->setSize(250, 50);
    zoom_label = new GuiLabel(zoom_slider, "", "Zoom: 1.0x", 30);
    zoom_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    // Radar/database view toggle.
    view_mode_selection = new GuiListbox(this, "VIEW_SELECTION", [this](int index, string value) {
        radar_view->setVisible(index == 0);
        background_gradient->setVisible(index == 0);
        database_view->setVisible(index == 1);
    });
    view_mode_selection->setOptions({tr("button", "Radar"), tr("button", "Database")})->setSelectionIndex(0)->setPosition(20, -20, ABottomLeft)->setSize(200, 100);

    // Scanning dialog.
    new GuiScanningDialog(this, "SCANNING_DIALOG");

    new ShipsLog(this,"science");

    new GuiNoRadarPopup(this);
}

void ScienceScreen::onDraw(sf::RenderTarget& window)
{
    GuiOverlay::onDraw(window);
    P<ScanProbe> probe;

    if (!my_spaceship)
        return;

    if(my_spaceship->getLongRangeRadarRange() <= 0)
    {
        zoom_label->setText("PAS D'AUSPEX");
        return;
        //and popup shown
    }

    float view_distance = science_radar->getDistance();
    float mouse_wheel_delta=InputHandler::getMouseWheelDelta();
    if (mouse_wheel_delta!=0)
    {
        view_distance *= (1.0 - (mouse_wheel_delta * 0.1f));
    }
    view_distance = std::min(view_distance,my_spaceship->getLongRangeRadarRange());
    view_distance = std::max(view_distance,my_spaceship->getShortRangeRadarRange());
    if (view_distance!=science_radar->getDistance() || previous_long_range_radar != my_spaceship->getLongRangeRadarRange() || previous_short_range_radar != my_spaceship->getShortRangeRadarRange())
    {
        previous_short_range_radar=my_spaceship->getShortRangeRadarRange();
        previous_long_range_radar=my_spaceship->getLongRangeRadarRange();
        science_radar->setDistance(view_distance);
        // Keep the zoom slider in sync.
        zoom_slider->setValue(view_distance)->setRange(my_spaceship->getLongRangeRadarRange(),my_spaceship->getShortRangeRadarRange());
        zoom_label->setText(tr("Zoom: {zoom}x").format({{"zoom", string(my_spaceship->getLongRangeRadarRange() / view_distance, 1)}}));
    }

    if (game_server)
        probe = game_server->getObjectById(my_spaceship->linked_science_probe_id);
    else
        probe = game_client->getObjectById(my_spaceship->linked_science_probe_id);

    float radar_range = my_spaceship->getShortRangeRadarRange();
    if (my_spaceship->hasSystem(SYS_Drones))
        radar_range = radar_range * my_spaceship->getSystemEffectiveness(SYS_Drones);

    if (probe_view_button->getValue() && probe)
    {
        if (targets.get() && glm::length2(probe->getPosition() - targets.get()->getPosition()) > (radar_range + targets.get()->getRadius())*(radar_range + targets.get()->getRadius()))
            targets.clear();
    }else{
        if (targets.get() && !P<Nebula>(targets.get()) && Nebula::blockedByNebula(my_spaceship->getPosition(), targets.get()->getPosition(), my_spaceship->getShortRangeRadarRange()))
            targets.clear();
    }

    sidebar_selector->setVisible(sidebar_selector->getSelectionIndex() > 0 || (custom_function_sidebar && custom_function_sidebar->hasEntries()));

    info_callsign->setValue("-");
    info_distance->setValue("-");
    info_heading->setValue("-");
    info_relspeed->setValue("-");
    info_faction->setValue("-");
    info_type->setValue("-");
    info_shields->setValue("-");
    info_hull->setValue("-");
    info_signatures->setValue("-");
    info_shield_frequency->setFrequency(-1)->hide();
    info_beam_frequency->setFrequency(-1)->hide();
    info_description->hide();
    info_type_button->hide();
//    sidebar_pager->hide();

    for(int n = 0; n < SYS_COUNT; n++)
        info_system[n]->setValue("-")->hide();
    info_oxygen->setValue("-")->hide();
    for(int n = 0; n < 10; n++)
    {
        info_other[n]->setValue("-")->hide();
        info_other[n]->setKey("-")->hide();
    }
    link_to_analysis_button->disable();

    if (probe)
    {
        probe_view_button->enable();
        probe_radar->setViewPosition(probe->getPosition());
        //probe_radar->setDistance(radar_range);
        probe_radar->setDistance(my_spaceship->getProbeRangeRadarRange());
    }
    else
    {
        probe_view_button->disable();
        probe_view_button->setValue(false);
        science_radar->show();
        probe_radar->hide();
    }

    if (targets.get())
    {
        P<SpaceObject> obj = targets.get();
        P<SpaceShip> ship = obj;
        P<SpaceStation> station = obj;
        P<Asteroid> asteroid = obj;
        P<Mine> mine = obj;

        // Info laterale

        // Toujours :
            // ID
            // Distance
            // Rel. Speed
            // Description de base

        // Si Scan simple ou allie :
            // Faction
            // Type
            // Boucliers
            // Carlingue
            // Description scan simple

        // Si scan ameliore
            // Description ameliore
            // Frequences
            // Systemes
        
        if (my_spaceship and obj->getScannedStateFor(my_spaceship) >= SS_FullScan)
        {
            link_to_analysis_button->setValue(my_spaceship->linked_analysis_object_id == obj->getMultiplayerId());
            link_to_analysis_button->enable();
        }
        else
        {
            link_to_analysis_button->setValue(false);
            link_to_analysis_button->disable();
        }

        auto position_diff = obj->getPosition() - my_spaceship->getPosition();
        float distance = glm::length(position_diff);
        float heading = vec2ToAngle(position_diff) - 270;

        while(heading < 0) heading += 360;

        float rel_velocity = dot(obj->getVelocity(), position_diff / distance) - dot(my_spaceship->getVelocity(), position_diff / distance);

        if (fabs(rel_velocity) < 0.01)
            rel_velocity = 0.0;

        string duration = "";
        if (fabs(rel_velocity) > 0.01)
        {
            if (fabs(my_spaceship->getHeading()-heading) < 10)
            {
                int seconds = fabs(distance/rel_velocity);
                int minutes = seconds / 60;
                 duration += " (";
                if (minutes < 10)
                    duration += "0";
                duration += string(minutes);
                duration += ":";
                if (seconds%60 < 10)
                    duration += "0";
                duration += string(seconds%60);
                duration += ")";
            }
        }

        info_callsign->setValue(obj->getCallSign());
        info_distance->setValue(string(distance / 1000.0f, 1) + DISTANCE_UNIT_1K);
        info_heading->setValue(string(int(heading)));
        info_relspeed->setValue(string(rel_velocity / 1000.0f * 60.0f, 1) + DISTANCE_UNIT_1K + "/min" + duration);

        // En un coup, recuperation de toute les descriptions possibles
        string description = obj->getDescriptionFor(my_spaceship);
        string sidebar_pager_selection = sidebar_pager->getSelectionValue();

        info_description->setText(description)->show();

        if (obj->getScannedStateFor(my_spaceship) == SS_NotScanned)
            sidebar_pager->setSelectionIndex(sidebar_pager->indexByValue("Description"));

//        if (description.size() > 0)
//        {
//
//            if (!sidebar_pager->indexByValue("Description") < 0)
//                sidebar_pager->addEntry("Description", "Description");
//        }
//        else
//        {
//            sidebar_pager->removeEntry(sidebar_pager->indexByValue("Description"));
//            if (sidebar_pager->getSelectionIndex() < 0)
//              sidebar_pager->setSelectionIndex(0);
//        }
//        string sidebar_pager_selection = sidebar_pager->getSelectionValue();
//        if (obj->infos_label[0] != "")
//        {
//            if (!sidebar_pager->indexByValue("Informations"))
//                sidebar_pager->addEntry("Informations", "Informations");
//        }
//        else
//            sidebar_pager->removeEntry(sidebar_pager->indexByValue("Informations"));
//
//        if (ship)
//        {
//            if (!sidebar_pager->indexByValue("Tactique"))
//                sidebar_pager->addEntry("Tactique", "Tactique");
//            if (!sidebar_pager->indexByValue("Systemes"))
//                sidebar_pager->addEntry("Systemes", "Systemes");
//        }
//        else
//        {
//            sidebar_pager->removeEntry(sidebar_pager->indexByValue("Tactique"));
//            sidebar_pager->removeEntry(sidebar_pager->indexByValue("Systemes"));
//        }
//
//        // De base, pas de sidebar
//        sidebar_pager->hide();

        // On a simple scan or deeper, show the faction, ship type, shields,
        // hull integrity, and database reference button.
        if (obj->getScannedStateFor(my_spaceship) >= SS_SimpleScan)
        {

            string show_signature_gravity = "-";
            string show_signature_biological = "-";
            string show_signature_electrical = "-";
            
            RawRadarSignatureInfo info;

            if (P<SpaceShip>(ship))
            {
                // Use dynamic signatures for ships.
                info = ship->getDynamicRadarSignatureInfo();
            } else 
            {
                // Otherwise, use the baseline only.
                info = obj->getRadarSignatureInfo();
            }
            
            if(my_spaceship->has_gravity_sensor)
                show_signature_gravity = string(std::max(0.0f,info.gravity));
            if(my_spaceship->has_biological_sensor)
                show_signature_biological = string(std::max(0.0f,info.biological));
            if(my_spaceship->has_electrical_sensor)
                show_signature_electrical = string(std::max(0.0f,info.electrical));
            string radarSignatureString = show_signature_gravity + " / " + show_signature_biological + " / "  + show_signature_electrical;
            info_signatures->setValue(radarSignatureString);

            info_hull->setValue(int(obj->hull));

            if (ship)
            {
                P<ShipTemplate> st = ship->ship_template;
                info_faction->setValue(factionInfo[ship->getFactionId()]->getLocaleName());
//                if (factionInfo[ship->getFactionId()]->getIcon() != "")
//                    info_faction->setIcon("gui/icons/" + factionInfo[ship->getFactionId()]->getIcon());
//                else
//                    info_faction->setIcon("");
                info_type_button->show();
                info_type->setValue(st->getLocaleName());
                info_shields->setValue(ship->getShieldDataString());
                info_hull->setValue(int(ceil(ship->getHull())));
            }

            if (station)
            {
                P<ShipTemplate> st = station->ship_template;
                info_faction->setValue(factionInfo[station->getFactionId()]->getLocaleName());
                info_type_button->show();
                info_type->setValue(st->getLocaleName());
                info_shields->setValue(station->getShieldDataString());
                info_hull->setValue(int(station->getHull()));
            }
        }

        // On commence par le pager
        if (obj->getScannedStateFor(my_spaceship) >= SS_FullScan)
        {
            info_shield_frequency->hide();
            info_beam_frequency->hide();

            for(int n = 0; n < SYS_COUNT; n++)
                info_system[n]->hide();
            info_oxygen->hide();

            info_description->hide();

            for(int n = 0; n < 10; n++)
                info_other[n]->hide();

            if (sidebar_pager_selection == "Informations")
                for(int n = 0; n < 10; n++)
                {
                    if (obj->infos_label[n] == "")
                        continue;
                    info_other[n]->show();
                    info_other[n]->setKey(obj->infos_label[n]);
                    info_other[n]->setValue(obj->infos_value[n]);
                }
            if (sidebar_pager_selection == "Description")
                info_description->show();

            if (ship)
            {
                if (sidebar_pager_selection == "Tactique")
                {
                    info_shield_frequency->show();
                    info_beam_frequency->show();
                    if (gameGlobalInfo->use_beam_shield_frequencies)
                    {
                        info_shield_frequency->setFrequency(ship->shield_frequency);
                        info_beam_frequency->setFrequency(ship->beam_frequency);
                    }
                }
                if (sidebar_pager_selection == "Systemes")
                {
                    for(int n = 0; n < SYS_COUNT; n++)
                    {
                        info_system[n]->setVisible(ship->hasSystem(ESystem(n)));
                        float system_health = ship->systems[n].health;
                        info_system[n]->setValue(string(int(system_health * 100.0f)) + "%")->setColor(sf::Color(255, 127.5 * (system_health + 1), 127.5 * (system_health + 1), 255));
                    }
                    info_oxygen->setVisible(obj->getOxygenMax() > 0);
                    info_oxygen->setValue(string(int(100.0f * obj->getOxygenTotal())) + "%");
                    if (obj->getOxygenTotal() < 0.20)
                        info_oxygen->setColor(sf::Color::Red);
                    else
                        info_oxygen->setColor(sf::Color::White);
                }
        // Tsht : copie en commentaire depuis le merge du master. A voir si on veut reutiliser le module oxygene de tdelc larp et revenir a un ecran plus proche du master
        //         else
        //         {
        //             LOG(WARNING) << "Invalid pager state: " << sidebar_pager_selection;
        //         }

        //         // If beam and shield frequencies are enabled on the server,
        //         // populate their graphs.
        //         if (gameGlobalInfo->use_beam_shield_frequencies)
        //         {
        //             info_shield_frequency->setFrequency(ship->shield_frequency);
        //             info_beam_frequency->setFrequency(ship->beam_frequency);
        //         }

        //         // Show the status of each subsystem.
        //         for(int n = 0; n < SYS_COUNT; n++)
        //         {
        //             float system_health = ship->systems[n].health;
        //             info_system[n]->setValue(string(int(system_health * 100.0f)) + "%")->setColor(sf::Color(255, 127.5 * (system_health + 1), 127.5 * (system_health + 1), 255));
        //         }
        //     }
        // }

        // // If the target isn't a ship, show basic info.
        // else
        // {
        //     sidebar_pager->hide();
        //     info_faction->setValue(factionInfo[obj->getFactionId()]->getLocaleName());

        //     // If the target is a station, show basic tactical info.
        //     if (station)
        //     {
        //         info_type->setValue(station->template_name);
        //         info_shields->setValue(station->getShieldDataString());
        //         info_hull->setValue(int(ceil(station->getHull())));
            }
//            sidebar_pager->setVisible(sidebar_pager->entryCount() > 1);
        }
        // On a full scan : frequencies
        // Only for ship
//        if (obj->getScannedStateFor(my_spaceship) >= SS_FullScan && ship)
//        if (obj->getScannedStateFor(my_spaceship) >= SS_FullScan)
//        {
//            sidebar_pager->setVisible(sidebar_pager->entryCount() > 1);
//
//            if (sidebar_pager_selection == "Informations")
//            {
//                info_shield_frequency->hide();
//                info_beam_frequency->hide();
//
//                for(int n = 0; n < SYS_COUNT; n++)
//                {
//                    info_system[n]->hide();
//                }
//                info_oxygen->hide();
//
//                info_description->hide();
//
//                for(int n = 0; n < 10; n++)
//                    info_other[n]->show();
//            }
//
//            if (ship)
//            {
//
//                // Check sidebar pager state.
//                if (sidebar_pager_selection == "Tactique")
//                {
//                    info_shield_frequency->show();
//                    info_beam_frequency->show();
//
//                    for(int n = 0; n < SYS_COUNT; n++)
//                    {
//                        info_system[n]->hide();
//                    }
//                    info_oxygen->hide();
//
//                    info_description->hide();
//                }
//                else if (sidebar_pager_selection == "Systemes")
//                {
//                    info_shield_frequency->hide();
//                    info_beam_frequency->hide();
//
//                    for(int n = 0; n < SYS_COUNT; n++)
//                        info_system[n]->setVisible(ship->hasSystem(ESystem(n)));
//
//                    info_oxygen->setVisible(obj->getOxygenMax() > 0);
//
//                    info_description->hide();
//                }
//                else if (sidebar_pager_selection == "Description")
//                {
//                    info_shield_frequency->hide();
//                    info_beam_frequency->hide();
//
//                    for(int n = 0; n < SYS_COUNT; n++)
//                    {
//                        info_system[n]->hide();
//                    }
//                    info_oxygen->hide();
//
//                    info_description->show();
//                }
//                else
//                {
//                    LOG(WARNING) << "Invalid pager state: " << sidebar_pager_selection;
//                }
//
//            // Infos extra
////            for(int n = 0; n < 10; n++)
////            {
////                if (obj->infos_label.size() < n)
////                    break;
////                info_other[n]->setKey(obj->infos_label[n]);
////                info_other[n]->setValue(obj->infos_value[n]);
////            }
//
//
//                // If beam and shield frequencies are enabled on the server,
//                // populate their graphs.
//                if (gameGlobalInfo->use_beam_shield_frequencies)
//                {
//                    info_shield_frequency->setFrequency(ship->shield_frequency);
//                    info_beam_frequency->setFrequency(ship->beam_frequency);
//                }
//
//                // Show the status of each subsystem.
//                for(int n = 0; n < SYS_COUNT; n++)
//                {
//                    float system_health = ship->systems[n].health;
//                    info_system[n]->setValue(string(int(system_health * 100.0f)) + "%")->setColor(sf::Color(255, 127.5 * (system_health + 1), 127.5 * (system_health + 1), 255));
//                }
//                info_oxygen->setValue(string(int(100.0f * obj->getOxygenTotal())) + "%");
//                if (obj->getOxygenTotal() < 0.20)
//                    info_oxygen->setColor(sf::Color::Red);
//                else
//                    info_oxygen->setColor(sf::Color::White);
//            }
//        }
    }

    // If the target is a waypoint, show its heading and distance, and our
    // velocity toward it.
    else if (targets.getWaypointIndex() >= 0)
    {
        sidebar_pager->hide();
        auto position_diff = my_spaceship->waypoints[targets.getWaypointIndex()] - my_spaceship->getPosition();
        float distance = glm::length(position_diff);
        float heading = vec2ToAngle(position_diff) - 270;

        while(heading < 0) heading += 360;

        float rel_velocity = -dot(my_spaceship->getVelocity(), position_diff / distance);

        if (fabs(rel_velocity) < 0.01)
            rel_velocity = 0.0;

        info_distance->setValue(string(distance / 1000.0f, 1) + DISTANCE_UNIT_1K);
        info_heading->setValue(string(int(heading)));
        info_relspeed->setValue(string(rel_velocity / 1000.0f * 60.0f, 1) + DISTANCE_UNIT_1K + "/min");
    }
}

void ScienceScreen::onHotkey(const HotkeyResult& key)
{
    if (key.category == "SCIENCE" && my_spaceship)
    {
         // Initiate a scan on scannable objects.
        if (key.hotkey == "SCAN_OBJECT" &&
            my_spaceship->getCanScan() &&
            my_spaceship->scanning_delay == 0.0)
        {
            P<SpaceObject> obj = targets.get();

            // Allow scanning only if the object is scannable, and if the player
            // isn't already scanning something.
            if (obj &&
                obj->canBeScannedBy(my_spaceship))
            {
                my_spaceship->commandScan(obj);
                return;
            }
        }

        // Cycle selection through scannable objects.
        if (key.hotkey == "NEXT_SCANNABLE_OBJECT" &&
            my_spaceship->scanning_delay == 0.0)
        {
            bool current_found = false;
            for (P<SpaceObject> obj : space_object_list)
            {
                // If this object is the current object, flag and skip it.
                if (obj == targets.get())
                {
                    current_found = true;
                    continue;
                }

                // If this object is my ship or not visible due to a Nebula,
                // skip it.
                if (obj == my_spaceship ||
                    Nebula::blockedByNebula(my_spaceship->getPosition(), obj->getPosition(), my_spaceship->getShortRangeRadarRange()))
                    continue;

                // If this is a scannable object and the currently selected
                // object, and it remains in radar range, continue to set it.
                if (current_found &&
                    glm::length(obj->getPosition() - my_spaceship->getPosition()) < science_radar->getDistance() &&
                    obj->canBeScannedBy(my_spaceship))
                {
                    targets.set(obj);
                    return;
                }
            }

            // Advance to the next object.
            for (P<SpaceObject> obj : space_object_list)
            {
                if (obj == targets.get() ||
                    obj == my_spaceship ||
                    Nebula::blockedByNebula(my_spaceship->getPosition(), obj->getPosition(), my_spaceship->getShortRangeRadarRange()))
                    continue;

                if (glm::length(obj->getPosition() - my_spaceship->getPosition()) < science_radar->getDistance() &&
                    obj->canBeScannedBy(my_spaceship))
                {
                    targets.set(obj);
                    return;
                }
            }
        }
         
        if (key.hotkey == "SHOW_PROBE")
        {
            P<ScanProbe> probe;
             if (game_server)
                probe = game_server->getObjectById(my_spaceship->linked_science_probe_id);
            else
                probe = game_client->getObjectById(my_spaceship->linked_science_probe_id);
             if (probe && !probe_view_button->getValue())
            {
                probe_view_button->setValue(true);
                glm::vec2 probe_position = probe->getPosition();
                science_radar->hide();
                probe_radar->show();
                probe_radar->setViewPosition(probe_position)->show();
            }else{
                probe_view_button->setValue(false);
                science_radar->show();
                probe_radar->hide();
            }
        }
        if (key.hotkey == "SHOW_DATABASE")
        {
            P<SpaceShip> ship = targets.get();
            if (ship && ship->getScannedStateFor(my_spaceship) >= SS_SimpleScan)
                database_view->findAndDisplayEntry(ship->getLocaleName());

            view_mode_selection->setSelectionIndex(1);
            radar_view->hide();
            background_gradient->hide();
            database_view->show();
        }
        if (key.hotkey == "SHOW_RADAR")
        {
            view_mode_selection->setSelectionIndex(0);
            radar_view->show();
            background_gradient->show();
            database_view->hide();
        }
        if (key.hotkey == "DECREASE_ZOOM")
        {
            float view_distance = science_radar->getDistance() + 1500.0f;
            if (view_distance > my_spaceship->getLongRangeRadarRange())
                view_distance = my_spaceship->getLongRangeRadarRange();
            if (view_distance < my_spaceship->getShortRangeRadarRange() )
                view_distance = my_spaceship->getShortRangeRadarRange();
            science_radar->setDistance(view_distance);
            // Keep the zoom slider in sync.
            zoom_slider->setValue(view_distance);
            zoom_label->setText("Zoom: " + string(my_spaceship->getLongRangeRadarRange() / view_distance, 1) + "x");
        }
        if (key.hotkey == "INCREASE_ZOOM")
        {
            float view_distance = science_radar->getDistance() - 1500.0f;
            if (view_distance > my_spaceship->getLongRangeRadarRange())
                view_distance = my_spaceship->getLongRangeRadarRange();
            if (view_distance < my_spaceship->getShortRangeRadarRange() )
                view_distance = my_spaceship->getShortRangeRadarRange();
            science_radar->setDistance(view_distance);
            // Keep the zoom slider in sync.
            zoom_slider->setValue(view_distance);
            zoom_label->setText("Zoom: " + string(my_spaceship->getLongRangeRadarRange() / view_distance, 1) + "x");
        }
        
    }
}
