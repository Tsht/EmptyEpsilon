#include "droneOperatorScreen.h"

#include "playerInfo.h"
#include "gameGlobalInfo.h"

#include "screenComponents/alertOverlay.h"
#include "screenComponents/customShipFunctions.h"

#include "spaceObjects/scanProbe.h"

#include "gui/gui2_overlay.h"
#include "gui/gui2_panel.h"
#include "gui/gui2_label.h"
#include "gui/gui2_listbox.h"
#include "screenComponents/powerDamageIndicator.h"

const ECrewPosition crewPosition = ECrewPosition::singlePilot;
DroneOperatorScreen::DroneOperatorScreen(GuiContainer *owner)
    : GuiOverlay(owner, "DRONE_PILOT_SCREEN", colorConfig.background), mode(DroneSelection)
{
    background_crosses = new GuiOverlay(this, "BACKGROUND_CROSSES", glm::u8vec4(255,255,255,255));
    background_crosses->setTextureTiled("gui/background/crosses.png");

    // Render the alert level color overlay.
    (new AlertLevelOverlay(this));

    // Draw a container for drone selection UI
    droneSelection = new GuiElement(this, "");
    droneSelection->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setAttribute("layout", "horizontal");;

    // Drone list
    drone_list = new GuiListbox(droneSelection, "PLAYER_SHIP_LIST", [this](int index, string value) {
        P<PlayerSpaceship> ship;
        if (game_server)
            ship = game_server->getObjectById(value.toInt());
        else if (game_client)
            ship = game_client->getObjectById(value.toInt());
        // If the selected item is a ship ...
        if (ship)
        // TODO :  check if occupied
        {
            mode = Piloting;
            selected_drone = ship;
            single_pilot_screen->setTargetSpaceship(selected_drone);
        }
    });
    drone_list->setPosition(0, -100, sp::Alignment::BottomCenter)->setSize(500, 1000);

    // single pilot UI
    single_pilot_screen = new SinglePilotScreen(this, selected_drone);
    single_pilot_screen->setPosition(0, 0, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    connection_label = new GuiLabel(this, "CONNECTION_LABEL", "0%", 30);
    connection_label->setPosition(0, -50, sp::Alignment::BottomCenter)->setSize(460, 50);

    disconnect_button = new GuiButton(this, "DISCONNECT_BUTTON", "Se deconnecter", [this]() {disconnected();});
    disconnect_button->setPosition(0, 0, sp::Alignment::BottomCenter)->setSize(400, 50);
    disconnect_button->moveToFront();
    // label for when there are no drones
    no_drones_label = new GuiLabel(this, "SHIP_SELECTION_NO_SHIPS_LABEL", "Aucun drone actif dans la zone de portee de radar", 30);
    no_drones_label->setPosition(0, 100, sp::Alignment::TopCenter)->setSize(460, 50);
    // Prep the alert overlay.
    (new GuiPowerDamageIndicator(this, "DOCKS_DPI", SYS_Drones, sp::Alignment::TopCenter, my_spaceship))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    custom_functions = new GuiCustomShipFunctions(this, dronePilot, "", my_spaceship);
    custom_functions->setPosition(-20, 120, sp::Alignment::TopRight)->setSize(250, GuiElement::GuiSizeMax);

    for(auto &drone_template : my_spaceship->ship_template->drones)
    {
        drone_and_ship_template_names.insert(drone_template.template_name);
    }

}
void DroneOperatorScreen::disconnected()
{
    mode = drone_list->entryCount() == 0 ? NoDrones : DroneSelection;
    selected_drone = NULL;
    single_pilot_screen->setTargetSpaceship(selected_drone);
}
bool DroneOperatorScreen::isOperated(P<PlayerSpaceship> ship)
{
    return (isConnectableDrone(ship) || isShip(ship));
}

bool DroneOperatorScreen::isConnectableDrone(P<PlayerSpaceship> ship)
{
     return ship
    && ship->ship_template
    && ship->ship_template->getType() == ShipTemplate::TemplateType::Drone
    && ship->getFactionId() == my_spaceship->getFactionId()
    && (ship->getBaseCarrier() == my_spaceship)
    && getConnectionQuality(ship) >= 0.01f;
}

bool DroneOperatorScreen::isShip(P<PlayerSpaceship> ship)
{
    return ship
    && ship->ship_template
    && ((ship->ship_template->getType() == ShipTemplate::TemplateType::Ship) || (ship->ship_template->getType() == ShipTemplate::TemplateType::PlayerShip)) 
    && ship->getFactionId() == my_spaceship->getFactionId()
    //&& drone_and_ship_template_names.find(ship->ship_template->getName()) != drone_and_ship_template_names.end();
    && (ship->getBaseCarrier() == my_spaceship); //only control ships or drones based on this carrier
}

float DroneOperatorScreen::getConnectionQuality(P<PlayerSpaceship> ship)
{
    float distance_min = length(ship->getPosition() - my_spaceship->getPosition());
    foreach(SpaceObject, obj, space_object_list)
    {
        P<ScanProbe> probe = obj;
        if (probe && my_spaceship-> getMultiplayerId() == probe->owner_id)
            if (length(ship->getPosition() - obj->getPosition()) < distance_min)
                distance_min = length(ship->getPosition() - obj->getPosition());
    }
    float rangeFactor = 1 - std::min(1.0f, (distance_min / my_spaceship->getDronesControlRange()));

    //float droneStateFactor = std::min(1.0f, ship->getSystemEffectiveness(SYS_Drones));
    float droneStateFactor = 1.0f;
    return rangeFactor * droneStateFactor;
}
void DroneOperatorScreen::onDraw(sp::RenderTarget& renderer)
{
    if (my_spaceship)
    {
        // Update the player ship list with all player ships.
        std::vector<string> options;
        std::vector<string> values;
        no_drones_label->setText("Aucun drone ni chasseur actif dans la zone de portee de radar (" + string(my_spaceship->getDronesControlRange() / 1000.0,1) + "U)");
        for (int n = 0; n < GameGlobalInfo::max_player_ships; n++)
        {
            P<PlayerSpaceship> ship = gameGlobalInfo->getPlayerShip(n);
            if (isConnectableDrone(ship)) {
                options.push_back("Drone - " + ship->getTypeName() + " " + ship->getCallSign() + " (Connexion radar : " + string(int(getConnectionQuality(ship) * 100)) + "%)");
                values.push_back(ship->getMultiplayerId());
            }
            else if(isShip(ship))
            {
                options.push_back("Chasseur - " + ship->getTypeName() + " " + ship->getCallSign());
                values.push_back(ship->getMultiplayerId());
            }
        }
        drone_list->setOptions(options, values);
        // automatically change mode if needed
        if (!selected_drone || !isOperated(selected_drone) || selected_drone->isDestroyed())
        {
           disconnected();
        }
        // update display according to mode
        switch (mode)
        {
        case DroneSelection:
            no_drones_label->hide();
            droneSelection->show();
            single_pilot_screen->hide();
            disconnect_button->hide();
            custom_functions->hide();
            connection_label->hide();
            break;
        case Piloting:
            no_drones_label->hide();
            droneSelection->hide();
            single_pilot_screen->show();
            disconnect_button->setText("Se deconnecter de " + selected_drone->callsign);
            disconnect_button->show();
            custom_functions->show();
            if(isConnectableDrone(selected_drone))
            {
                connection_label->setText("Connexion radar : " + string(int(getConnectionQuality(selected_drone) * 100)) + "%");
                connection_label->show();
            }
            break;
        case NoDrones:
            no_drones_label->show();
            droneSelection->hide();
            single_pilot_screen->hide();
            disconnect_button->hide();
            custom_functions->hide();
            connection_label->hide();
            break;
        }
    }
}
