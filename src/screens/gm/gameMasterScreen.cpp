#include "gameMasterScreen.h"
#include "shipTemplate.h"
#include "main.h"
#include "gameGlobalInfo.h"
#include "GMActions.h"
#include "gameMasterScreen.h"
#include "objectCreationView.h"
#include "globalMessageEntryView.h"
#include "tweak.h"
#include "clipboard.h"
#include "chatDialog.h"
#include "spaceObjects/cpuShip.h"
#include "spaceObjects/spaceStation.h"
#include "spaceObjects/explosionEffect.h"
#include "spaceObjects/zone.h"

#include "screenComponents/radarView.h"

#include "gui/gui2_togglebutton.h"
#include "gui/gui2_selector.h"
#include "gui/gui2_slider.h"
#include "gui/gui2_listbox.h"
#include "gui/gui2_label.h"
#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_textentry.h"

GameMasterScreen::GameMasterScreen(RenderLayer* render_layer)
: GuiCanvas(render_layer), click_and_drag_state(CD_None)
{
    my_spaceship = nullptr;

    main_radar = new GuiRadarView(this, "MAIN_RADAR", 50000.0f, &targets, my_spaceship); // my_spaceship === nullptr
    main_radar->setStyle(GuiRadarView::Rectangular)->longRange()->gameMaster()->enableTargetProjections(nullptr)->setAutoCentering(false);
    main_radar->setPosition(0, 0, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    main_radar->setCallbacks(
        [this](sp::io::Pointer::Button button, glm::vec2 position) { this->onMouseDown(button, position); },
        [this](glm::vec2 position) { this->onMouseDrag(position); },
        [this](glm::vec2 position) { this->onMouseUp(position); }
    );
    box_selection_overlay = new GuiOverlay(main_radar, "BOX_SELECTION", glm::u8vec4(255, 255, 255, 32));
    box_selection_overlay->layout.fill_height = false;
    box_selection_overlay->layout.fill_width = false;
    box_selection_overlay->hide();

//    pause_button = new GuiToggleButton(this, "PAUSE_BUTTON", tr("button", "Pause"), [this](bool value) {
//         if (!value)
//             engine->setGameSpeed(1.0f);
//         else
//             engine->setGameSpeed(0.0f);
//     });
//     pause_button->setValue(engine->getGameSpeed() == 0.0f)->setPosition(20, 20, sp::Alignment::TopLeft)->setSize(250, 50);

    pause_button = new GuiSlider(this, "PAUSE_BUTTON", 0.0, 10.0, 1.0, [](float value) {
                                 gameMasterActions->commandSetGameSpeed(value);
    });

    pause_button->setPosition(20, 20, sp::Alignment::TopLeft)->setSize(250, 50);
    pause_button->addSnapValue(0.0, 0.5);
    pause_button->addSnapValue(1.0, 0.5);
    pause_button->addSnapValue(2.0, 0.5);
    pause_button->addSnapValue(5.0, 0.5);
    pause_button->addSnapValue(10.0, 0.5);


    if (gameGlobalInfo->intercept_all_comms_to_gm < CGI_Always){
        intercept_comms_button = new GuiToggleButton(this, "INTERCEPT_COMMS_BUTTON",  tr("button", "Intercept all comms"), [this](bool value) {
            gameMasterActions->commandInterceptAllCommsToGm(value);
        });
        intercept_comms_button->setValue((int)gameGlobalInfo->intercept_all_comms_to_gm)->setTextSize(20)->setPosition(300, 20, sp::Alignment::TopLeft)->setSize(200, 25);
    }

    faction_selector = new GuiSelector(this, "FACTION_SELECTOR", [this](int index, string value) {
        gameMasterActions->commandSetFactionId(index, targets.getTargets());
    });
    for(P<FactionInfo> info : factionInfo)
        if (info)
            faction_selector->addEntry(info->getLocaleName(), info->getName());
    faction_selector->setPosition(20, 70, sp::Alignment::TopLeft)->setSize(250, 50);

    personality_selector = new GuiSelector(this, "FACTION_SELECTOR", [this](int index, string value) {
        for(P<SpaceObject> obj : targets.getTargets())
        {
            gameMasterActions->commandSetPersonalityId(index, targets.getTargets());
//            obj->setPersonalityId(index);
        }
    });
    personality_selector->addEntry("Normal","Normal");
    personality_selector->addEntry("Pacifique","Pacifique");
    personality_selector->addEntry("Hostile","Hostile");
    personality_selector->addEntry("Solo","Solo");
    personality_selector->setPosition(20, 120, sp::Alignment::TopLeft)->setSize(250, 50);

    global_message_button = new GuiButton(this, "GLOBAL_MESSAGE_BUTTON", tr("button", "Global message"), [this]() {
        global_message_entry->show();
    });
    global_message_button->setPosition(20, -20, sp::Alignment::BottomLeft)->setSize(250, 50);

    player_ship_selector = new GuiSelector(this, "PLAYER_SHIP_SELECTOR", [this](int index, string value) {
        P<SpaceObject> ship = gameGlobalInfo->getPlayerShip(value.toInt());
        if (ship)
        {
            target = ship;
            main_radar->setViewPosition(ship->getPosition());
            if(!position_text_custom)
                position_text->setText(getStringFromPosition(ship->getPosition()));
            targets.set(ship);
        }
    });
    player_ship_selector->setPosition(270, -20, sp::Alignment::BottomLeft)->setSize(250, 50);

    position_text_custom = false;
    position_text = new GuiTextEntry(this, "SECTOR_NAME_TEXT", "");
    position_text->setPosition(620, -20, sp::Alignment::BottomLeft)->setSize(250, 50);
    position_text->callback([this](string text){
        position_text_custom = true;
    });
    position_text->validator(isValidPositionString);
    position_text->enterCallback([this](string text){
        position_text_custom = false;
        if (position_text->isValid())
        {
            glm::vec2 pos = getPositionFromSring(text);
            main_radar->setViewPosition(pos);
        }
    });
    position_text->setText(getStringFromPosition(main_radar->getViewPosition()));

    CPU_ship_selector = new GuiSelector(this, "CPU_SHIP_SELECTOR", [this](int index, string value) {
        P<SpaceShip> ship = space_object_list[value.toInt()];
        if (ship)
        {
            target = ship;
            main_radar->setViewPosition(ship->getPosition());
            targets.set(ship);
        }
    });
    CPU_ship_selector->setPosition(270, -70, sp::Alignment::BottomLeft)->setSize(250, 50);

    space_station_selector = new GuiSelector(this, "SPACE_STATION_SELECTOR", [this](int index, string value) {
        P<SpaceObject> station = space_object_list[value.toInt()];
        if (station)
        {
            target = station;
            main_radar->setViewPosition(station->getPosition());
            targets.set(station);
        }
    });
    space_station_selector->setPosition(270, -120, sp::Alignment::BottomLeft)->setSize(250, 50);

    //create_button = new GuiButton(this, "CREATE_OBJECT_BUTTON", "Creer...", [this]() {
    //    object_creation_screen->show();
    //});
    //create_button->setPosition(20, -70, sp::Alignment::BottomLeft)->setSize(250, 50);

    copy_scenario_button = new GuiButton(this, "COPY_SCENARIO_BUTTON", tr("button", "Copy scenario"), [this]() {
        Clipboard::setClipboard(getScriptExport(false));
    });
    copy_scenario_button->setTextSize(20)->setPosition(-20, -20, sp::Alignment::BottomRight)->setSize(125, 25);

    copy_selected_button = new GuiButton(this, "COPY_SELECTED_BUTTON", tr("button", "Copy selected"), [this]() {
        Clipboard::setClipboard(getScriptExport(true));
    });
    copy_selected_button->setTextSize(20)->setPosition(-20, -45, sp::Alignment::BottomRight)->setSize(125, 25);

    create_button = new GuiButton(this, "CREATE_OBJECT_BUTTON", tr("button", "Create..."), [this]() {
        object_creation_view->show();
    });
    create_button->setPosition(20, -70, sp::Alignment::BottomLeft)->setSize(250, 50);
      
    cancel_action_button = new GuiButton(this, "CANCEL_CREATE_BUTTON", tr("button", "Cancel"), [this]() {
        gameGlobalInfo->on_gm_click = nullptr;
    });
    cancel_action_button->setPosition(20, -70, sp::Alignment::BottomLeft)->setSize(250, 50)->hide();

    tweak_button = new GuiButton(this, "TWEAK_OBJECT", tr("button", "Tweak"), [this]() {
        for(P<SpaceObject> obj : targets.getTargets())
        {
            if (P<PlayerSpaceship>(obj))
            {
                player_tweak_dialog->open(obj);
                break;
            }
            else if (P<SpaceShip>(obj))
            {
                ship_tweak_dialog->open(obj);
                break;
            }
			else if (P<ShipTemplateBasedObject>(obj))
			{
                template_tweak_dialog->open(obj);
                break;
            }
			else if (P<Planet>(obj))
			{
                planet_tweak_dialog->open(obj);
                break;
            }
            else if (P<SpaceStation>(obj))
            {
                station_tweak_dialog->open(obj);
                break;
            }
            else if (P<WarpJammer>(obj))
            {
                jammer_tweak_dialog->open(obj);
            }
            else if (P<Asteroid>(obj))
            {
                asteroid_tweak_dialog->open(obj);
            }
            else /*if(obj)*/
            {
                object_tweak_dialog->open(obj);
                break;
            }
        }
    });
    // tweaks only work on the server
    tweak_button->setPosition(20, -120, sp::Alignment::BottomLeft)->setSize(250, 50)->setEnable(bool(game_server))->hide();

    player_comms_hail = new GuiButton(this, "HAIL_PLAYER", tr("button", "Hail ship"), [this]() {
        for(P<SpaceObject> obj : targets.getTargets())
        {
            if (P<PlayerSpaceship>(obj))
            {
                int idx = gameGlobalInfo->findPlayerShip(obj);
                chat_dialog_per_ship[idx]->show()->setPosition(main_radar->worldToScreen(obj->getPosition()))->setSize(300, 300);
            }
        }
    });
    player_comms_hail->setPosition(20, -170, sp::Alignment::BottomLeft)->setSize(250, 50)->hide();

    info_layout = new GuiElement(this, "INFO_LAYOUT");
    info_layout->setPosition(-20, 20, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    info_clock = new GuiKeyValueDisplay(info_layout, "INFO_CLOCK", 0.5, tr("Clock"), "");
    info_clock->setSize(GuiElement::GuiSizeMax, 30);

    gm_script_options = new GuiListbox(this, "GM_SCRIPT_OPTIONS", [this](int index, string value)
    {
        gm_script_options->setSelectionIndex(-1);
        gameMasterActions->commandCallGmScript(index, getSelection());
    });
    gm_script_options->setPosition(20, 160, sp::Alignment::TopLeft)->setSize(250, 500);

    order_layout = new GuiElement(this, "ORDER_LAYOUT");
    order_layout->setPosition(-20, -90, sp::Alignment::BottomRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "verticalbottom");

    (new GuiButton(order_layout, "ORDER_DEFEND_LOCATION", tr("Defend location"), [this]() {
        for(P<SpaceObject> obj : targets.getTargets())
            if (P<CpuShip>(obj))
                P<CpuShip>(obj)->orderDefendLocation(obj->getPosition());
    }))->setTextSize(20)->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(order_layout, "ORDER_STAND_GROUND", tr("Stand ground"), [this]() {
        for(P<SpaceObject> obj : targets.getTargets())
            if (P<CpuShip>(obj))
                P<CpuShip>(obj)->orderStandGround();
    }))->setTextSize(20)->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(order_layout, "ORDER_ROAMING", tr("Roaming"), [this]() {
        for(P<SpaceObject> obj : targets.getTargets())
            if (P<CpuShip>(obj))
                P<CpuShip>(obj)->orderRoaming();
    }))->setTextSize(20)->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(order_layout, "ORDER_IDLE", tr("Idle"), [this]() {
        for(P<SpaceObject> obj : targets.getTargets())
            if (P<CpuShip>(obj))
                P<CpuShip>(obj)->orderIdle();
    }))->setTextSize(20)->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(order_layout, "ORDERS_LABEL", tr("Orders:"), 20))->addBackground()->setSize(GuiElement::GuiSizeMax, 30);

    chat_layer = new GuiElement(this, "");
    chat_layer->setPosition(0, 0)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    for(int n=0; n<GameGlobalInfo::max_player_ships; n++)
    {
        chat_dialog_per_ship.push_back(new GameMasterChatDialog(chat_layer, main_radar, n));
        chat_dialog_per_ship[n]->hide();
    }

    player_tweak_dialog = new GuiObjectTweak(this, TW_Player);
    player_tweak_dialog->hide();
	template_tweak_dialog = new GuiObjectTweak(this, TW_Template);
    template_tweak_dialog->hide();
    ship_tweak_dialog = new GuiObjectTweak(this, TW_Ship);
    ship_tweak_dialog->hide();
    object_tweak_dialog = new GuiObjectTweak(this, TW_Object);
    object_tweak_dialog->hide();
    planet_tweak_dialog = new GuiObjectTweak(this, TW_Planet);
    planet_tweak_dialog->hide();
    station_tweak_dialog = new GuiObjectTweak(this, TW_Station);
    station_tweak_dialog->hide();
    jammer_tweak_dialog = new GuiObjectTweak(this, TW_Jammer);
    jammer_tweak_dialog->hide();
    asteroid_tweak_dialog = new GuiObjectTweak(this, TW_Asteroid);
    asteroid_tweak_dialog->hide();

    global_message_entry = new GuiGlobalMessageEntryView(this);
    global_message_entry->hide();
    object_creation_view = new GuiObjectCreationView(this);
    object_creation_view->hide();

    message_frame = new GuiPanel(this, "");
    message_frame->setPosition(0, 0, sp::Alignment::TopCenter)->setSize(900, 230)->hide();

    message_text = new GuiScrollText(message_frame, "", "");
    message_text->setTextSize(20)->setPosition(20, 20, sp::Alignment::TopLeft)->setSize(900 - 40, 200 - 40);
    message_close_button = new GuiButton(message_frame, "", tr("button", "Close"), []() {
        if (!gameGlobalInfo->gm_messages.empty())
        {
            gameGlobalInfo->gm_messages.pop_front();
        }

    });
    message_close_button->setTextSize(30)->setPosition(-20, -20, sp::Alignment::BottomRight)->setSize(300, 30);
}

//due to a suspected compiler bug this deconstructor needs to be explicitly defined
GameMasterScreen::~GameMasterScreen()
{
}

void GameMasterScreen::update(float delta)
{
    float mouse_wheel_delta = keys.zoom_in.getValue() - keys.zoom_out.getValue();
    if (mouse_wheel_delta != 0.0f)
    {
        float view_distance = main_radar->getDistance() * (1.0f - (mouse_wheel_delta * 0.1f));
        if (view_distance > max_distance)
            view_distance = max_distance;
        if (view_distance < min_distance)
            view_distance = min_distance;
        main_radar->setDistance(view_distance);
        if (view_distance < 10000)
            main_radar->shortRange();
        else
            main_radar->longRange();
    }

    if (keys.gm_delete.getDown())
    {
        gameMasterActions->commandDestroy(targets.getTargets());
    }
    if (keys.gm_clipboardcopy.getDown())
    {
        Clipboard::setClipboard(getScriptExport(false));
    }
    if (keys.spectator_show_callsigns.getDown())
    {
        // Toggle callsigns.
        main_radar->showCallsigns(!main_radar->getCallsigns());
    }

    if (keys.escape.getDown())
    {
        destroy();
        returnToShipSelection(getRenderLayer());
    }
    if (keys.pause.getDown())
    {
        if (engine->getGameSpeed() == 0.0f)
            gameMasterActions->commandSetGameSpeed(1.0f);
        else
            gameMasterActions->commandSetGameSpeed(0.0f);
    }
    if (engine->getGameSpeed() == 0.0f) {
        pause_button->setValue(true);
    } else {
        pause_button->setValue(false);
    }

    bool has_object = false;
    bool has_cpu_ship = false;
    bool has_player_ship = false;

    // Add and remove entries from the player ship list.
    for(int n=0; n<GameGlobalInfo::max_player_ships; n++)
    {
        P<PlayerSpaceship> ship = gameGlobalInfo->getPlayerShip(n);
        if (ship)
        {
            if (player_ship_selector->indexByValue(string(n)) == -1)
            {
                player_ship_selector->addEntry(ship->getTypeName() + " " + ship->getCallSign(), string(n));
            } else {
                player_ship_selector->setEntryName(player_ship_selector->indexByValue(string(n)),ship->getCallSign());
            }

            if (ship->isCommsBeingHailedByGM() || ship->isCommsChatOpenToGM())
            {
                if (!chat_dialog_per_ship[n]->isVisible())
                {
                    chat_dialog_per_ship[n]->show()->setPosition(main_radar->worldToScreen(ship->getPosition()))->setSize(300, 300);
                }
            }
        }else{
            if (player_ship_selector->indexByValue(string(n)) != -1)
                player_ship_selector->removeEntry(player_ship_selector->indexByValue(string(n)));
        }
    }

    // Add and remove entries from the CPU ship and space station list.
    int n = 0;
    foreach(SpaceObject, obj, space_object_list)
    {
        P<SpaceShip> ship = obj;
        P<SpaceStation> station = obj;
        if (ship)
        {
            if (CPU_ship_selector->indexByValue(string(n)) == -1)
                CPU_ship_selector->addEntry(ship->getTypeName() + " " + ship->getCallSign(), string(n));
        }else{
            if (CPU_ship_selector->indexByValue(string(n)) != -1)
                CPU_ship_selector->removeEntry(CPU_ship_selector->indexByValue(string(n)));
        }
        if (station)
        {
            if (space_station_selector->indexByValue(string(n)) == -1)
                space_station_selector->addEntry(station->getTypeName() + " " + station->getCallSign(), string(n));
        }else{
            if (space_station_selector->indexByValue(string(n)) != -1)
                space_station_selector->removeEntry(space_station_selector->indexByValue(string(n)));
        }
    n += 1;
    }

    // Record object type.
    for(P<SpaceObject> obj : targets.getTargets())
    {
        has_object = true;
        if (P<CpuShip>(obj))
            has_cpu_ship = true;
        else if (P<PlayerSpaceship>(obj))
            has_player_ship = true;
    }

    // Show selector only if there are objects.
    player_ship_selector->setVisible(player_ship_selector->entryCount() > 0);
    CPU_ship_selector->setVisible(CPU_ship_selector->entryCount() > 0);
    space_station_selector->setVisible(space_station_selector->entryCount() > 0);

    // Show tweak button.
    tweak_button->setVisible(has_object);

    order_layout->setVisible(has_cpu_ship);
    player_comms_hail->setVisible(has_player_ship);

    // Update mission clock
    info_clock->setValue(gameGlobalInfo->getMissionTime());

    std::unordered_map<string, string> selection_info;

    // For each selected object, determine and report their type.
    for(P<SpaceObject> obj : targets.getTargets())
    {
        std::unordered_map<string, string> info = obj->getGMInfo();
        for(std::unordered_map<string, string>::iterator i = info.begin(); i != info.end(); i++)
        {
            if (selection_info.find(i->first) == selection_info.end())
            {
                selection_info[i->first] = i->second;
            }
            else if (selection_info[i->first] != i->second)
            {
                selection_info[i->first] = tr("*mixed*");
            }
        }
    }

    if (targets.getTargets().size() == 1)
    {
        P<SpaceObject> target = targets.getTargets()[0];
        selection_info[trMark("gm_info", "Position")] = string(targets.getTargets()[0]->getPosition().x, 0) + "," + string(targets.getTargets()[0]->getPosition().y, 0);

        for(int n=0; n<GameGlobalInfo::max_player_ships; n++)
        {
            P<PlayerSpaceship> ship = gameGlobalInfo->getPlayerShip(n);
            if (ship)
            {
                float distance = glm::length(targets.getTargets()[0]->getPosition() - ship->getPosition());
                selection_info["Distance " + ship->callsign] = string(distance / 1000.0f,0) + " U";
            }
        }
    }

    unsigned int cnt = 0;
    for(std::unordered_map<string, string>::iterator i = selection_info.begin(); i != selection_info.end(); i++)
    {
        if (cnt == info_items.size())
        {
            info_items.push_back(new GuiKeyValueDisplay(info_layout, "INFO_" + string(cnt), 0.5, i->first, i->second));
            info_items[cnt]->setSize(GuiElement::GuiSizeMax, 30);
        }else{
            info_items[cnt]->show();
            info_items[cnt]->setKey(tr("gm_info", i->first))->setValue(i->second);
        }
        cnt++;
    }
    while(cnt < info_items.size())
    {
        info_items[cnt]->hide();
        cnt++;
    }

    bool gm_functions_changed = gm_script_options->entryCount() != int(gameGlobalInfo->gm_callback_names.size());
    auto it = gameGlobalInfo->gm_callback_names.begin();
    for(int n=0; !gm_functions_changed && n<gm_script_options->entryCount(); n++)
    {
        if (gm_script_options->getEntryName(n) != *it)
            gm_functions_changed = true;
        it++;
    }
    if (gm_functions_changed)
    {
        gm_script_options->setOptions({});
        for(const string& callbackName : gameGlobalInfo->gm_callback_names)
        {
            gm_script_options->addEntry(callbackName, callbackName);
        }
    }

    pause_button->setValue(engine->getGameSpeed());
    if (gameGlobalInfo->intercept_all_comms_to_gm < CGI_Always)
        intercept_comms_button->setValue(gameGlobalInfo->intercept_all_comms_to_gm);
    if (!gameGlobalInfo->gm_messages.empty())
    {
        GMMessage* message = &gameGlobalInfo->gm_messages.front();
        message_text->setText(message->text);
        message_frame->show();
    } else {
        message_frame->hide();
    }

    if (gameGlobalInfo->on_gm_click)
    {
        create_button->hide();
        object_creation_view->hide();
        cancel_action_button->show();
    }
    else
    {
        create_button->show();
        cancel_action_button->hide();
    }
}

void GameMasterScreen::onMouseDown(sp::io::Pointer::Button button, glm::vec2 position)
{
    if (click_and_drag_state != CD_None)
        return;
    if (button == sp::io::Pointer::Button::Right)
    {
        click_and_drag_state = CD_DragViewOrOrder;
    }
    else
    {
        if (gameGlobalInfo->on_gm_click)
        {
            gameGlobalInfo->on_gm_click(position);
        }else{
            click_and_drag_state = CD_BoxSelect;

            float min_drag_distance = main_radar->getDistance() / 450 * 10;

            for(P<SpaceObject> obj : targets.getTargets())
            {
                if (glm::length(obj->getPosition() - position) < std::max(min_drag_distance, obj->getRadius()))
                    click_and_drag_state = CD_DragObjects;
            }
        }
    }
    drag_start_position = position;
    drag_previous_position = position;
}

void GameMasterScreen::onMouseDrag(glm::vec2 position)
{
    switch(click_and_drag_state)
    {
    case CD_DragViewOrOrder:
    case CD_DragView:
        click_and_drag_state = CD_DragView;
        main_radar->setViewPosition(main_radar->getViewPosition() - (position - drag_previous_position));
        if(!position_text_custom)
            position_text->setText(getStringFromPosition(main_radar->getViewPosition()));
        position -= (position - drag_previous_position);
        break;
    case CD_DragObjects:
        gameMasterActions->commandMoveObjects(position - drag_previous_position, targets.getTargets());
        break;
    case CD_BoxSelect:
        {
            auto p0 = main_radar->worldToScreen(drag_start_position);
            auto p1 = main_radar->worldToScreen(position);
            if (p0.x > p1.x) std::swap(p0.x, p1.x);
            if (p0.y > p1.y) std::swap(p0.y, p1.y);
            box_selection_overlay->show();
            box_selection_overlay->setPosition(p0, sp::Alignment::TopLeft);
            box_selection_overlay->setSize(p1 - p0);
        }
        break;
    default:
        break;
    }
    drag_previous_position = position;
}

void GameMasterScreen::onMouseUp(glm::vec2 position)
{
    switch(click_and_drag_state)
    {
    case CD_DragViewOrOrder:
        {
            //Right click
            bool shift_down = SDL_GetModState() & KMOD_SHIFT;
            gameMasterActions->commandContextualGoTo(position, shift_down, targets.getTargets()); //for ai, order attack etc.
           
        }
        break;
    case CD_BoxSelect:
        {
            bool shift_down = SDL_GetModState() & KMOD_SHIFT;
            bool ctrl_down = SDL_GetModState() & KMOD_CTRL;
            bool alt_down = SDL_GetModState() & KMOD_ALT;
            PVector<Collisionable> objects = CollisionManager::queryArea(drag_start_position, position);
            PVector<SpaceObject> space_objects;
            foreach(Collisionable, c, objects)
            {
                if (P<Zone>(c))
                    continue;
                if (ctrl_down && !P<ShipTemplateBasedObject>(c))
                    continue;
                if (alt_down && (!P<SpaceObject>(c) || (int)(P<SpaceObject>(c))->getFactionId() != faction_selector->getSelectionIndex()))
                    continue;
                space_objects.push_back(c);
            }
            if (shift_down)
            {
                foreach(SpaceObject, s, space_objects)
                {
                    targets.add(s);
                }
            } else {
                targets.set(space_objects);
            }


            if (space_objects.size() > 0)
            {
                faction_selector->setSelectionIndex(space_objects[0]->getFactionId());
                personality_selector->setSelectionIndex(space_objects[0]->getPersonalityId());
            }
        }
        break;
    default:
        break;
    }
    click_and_drag_state = CD_None;
    box_selection_overlay->hide();
}

PVector<SpaceObject> GameMasterScreen::getSelection()
{
    return targets.getTargets();
}

string GameMasterScreen::getScriptExport(bool selected_only)
{
    string output;
    PVector<SpaceObject> objs;
    if (selected_only)
    {
        objs = targets.getTargets();
    }else{
        objs = space_object_list;
    }

    foreach(SpaceObject, obj, objs)
    {
        string line = obj->getExportLine();
        if (line == "")
            continue;
        output += "    " + line + "\n";
    }
    return output;
}
