#include <i18n.h>
#include "engine.h"
#include "mainMenus.h"
#include "main.h"
#include "preferenceManager.h"
#include "epsilonServer.h"
#include "playerInfo.h"
#include "gameGlobalInfo.h"
#include "spaceObjects/spaceship.h"
#include "menus/serverCreationScreen.h"
#include "menus/optionsMenu.h"
#include "menus/tutorialMenu.h"
#include "menus/serverBrowseMenu.h"
#include "screens/gm/gameMasterScreen.h"
#include "screenComponents/rotatingModelView.h"

#include "gui/gui2_image.h"
#include "gui/gui2_label.h"
#include "gui/gui2_button.h"
#include "gui/gui2_textentry.h"

class DebugAllModelView : public GuiCanvas
{
public:
    DebugAllModelView()
    {
        new GuiOverlay(this, "", colorConfig.background);
        (new GuiOverlay(this, "", glm::u8vec4{255,255,255,255}))->setTextureTiled("gui/background/crosses.png");

        std::vector<string> names = ModelData::getModelDataNames();
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.startswith("transport_"); }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.startswith("artifact"); }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.startswith("SensorBuoyMK"); }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.startswith("space_station_"); }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name == "ammo_box"; }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name == "shield_generator"; }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.endswith("Blue"); }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.endswith("Green"); }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.endswith("Grey"); }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.endswith("Red"); }), names.end());
        names.erase(std::remove_if(names.begin(), names.end(), [](const string& name) { return name.endswith("White"); }), names.end());
        int col_count = sqrtf(names.size()) + 1;
        int row_count = ceil(names.size() / col_count) + 1;
        int x = 0;
        int y = 0;
        float w = 1600 / col_count;
        float h = 900 / row_count;
        for(string name : names)
        {
            (new GuiRotatingModelView(this, "", ModelData::getModel(name)))->setPosition(x * w, y * h, sp::Alignment::TopLeft)->setSize(w, h);
            x++;
            if (x == col_count)
            {
                x = 0;
                y++;
            }
        }
    }
};

MainMenu::MainMenu()
{
    constexpr float logo_size = 256;
    constexpr float logo_size_y = 256;
    constexpr float logo_size_x = 1024;
    constexpr float title_y = 160;

    new GuiOverlay(this, "", colorConfig.background);
    (new GuiOverlay(this, "", glm::u8vec4{255,255,255,255}))->setTextureTiled("gui/background/crosses.png");

    (new GuiImage(this, "LOGO", "logo_full.png"))->setPosition(0, title_y, sp::Alignment::TopCenter)->setSize(logo_size_x, logo_size_y);
    (new GuiLabel(this, "VERSION", tr("Version: {version}").format({{"version", string(VERSION_NUMBER)}}), 20))->setPosition(0, title_y + logo_size, sp::Alignment::TopCenter)->setSize(0, 20);

    (new GuiLabel(this, "", tr("Your name:"), 30))->setAlignment(sp::Alignment::CenterLeft)->setPosition({50, -400}, sp::Alignment::BottomLeft)->setSize(300, 50);
    (new GuiTextEntry(this, "USERNAME", PreferencesManager::get("username")))->callback([](string text) {
        PreferencesManager::set("username", text);
    })->setPosition({50, -350}, sp::Alignment::BottomLeft)->setSize(300, 50);

    (new GuiButton(this, "START_SERVER", tr("Start server"), [this]() {
        new ServerSetupScreen();
        destroy();
    }))->setPosition({50, -230}, sp::Alignment::BottomLeft)->setSize(300, 50);

    (new GuiButton(this, "START_CLIENT", tr("Start client"), [this]() {
        new ServerBrowserMenu(ServerBrowserMenu::Local);
        destroy();
    }))->setPosition({50, -170}, sp::Alignment::BottomLeft)->setSize(300, 50);

    (new GuiButton(this, "OPEN_OPTIONS", tr("Options"), [this]() {
        new OptionsMenu();
        destroy();
    }))->setPosition({50, -110}, sp::Alignment::BottomLeft)->setSize(300, 50);

    (new GuiButton(this, "QUIT", tr("Quit"), []() {
        engine->shutdown();
    }))->setPosition({50, -50}, sp::Alignment::BottomLeft)->setSize(300, 50);

    (new GuiButton(this, "START_TUTORIAL", tr("Tutorials"), [this]() {
        new TutorialMenu();
        destroy();
    }))->setPosition({370, -50}, sp::Alignment::BottomLeft)->setSize(300, 50);

    float y = 25;
    (new GuiLabel(this, "CREDITS", "Credits", 25))->setAlignment(sp::Alignment::CenterRight)->setPosition(-50, y, sp::Alignment::TopRight)->setSize(0, 25); y += 25;
    (new GuiLabel(this, "CREDITS1", "Empty Epsilon", 20))->setAlignment(sp::Alignment::CenterRight)->setPosition(-50, y, sp::Alignment::TopRight)->setSize(0, 20); y += 20;

    if (PreferencesManager::get("instance_name") != "")
    {
        (new GuiLabel(this, "", PreferencesManager::get("instance_name"), 25))->setAlignment(sp::Alignment::CenterLeft)->setPosition(20, 20, sp::Alignment::TopLeft)->setSize(0, 18);
    }

#ifdef DEBUG
    (new GuiButton(this, "", "TO DA GM!", [this]() {
        new EpsilonServer(defaultServerPort);
        if (game_server)
        {
            gameGlobalInfo->startScenario("scenario_10_empty.lua");

            my_player_info->commandSetShipId(-1);
            destroy();
            new GameMasterScreen();
        }
    }))->setPosition({370, -150}, sp::Alignment::BottomLeft)->setSize(300, 50);

    (new GuiButton(this, "", "MODELS!", [this]() {
        destroy();
        new DebugAllModelView();
    }))->setPosition({370, -200}, sp::Alignment::BottomLeft)->setSize(300, 50);
#endif
}
