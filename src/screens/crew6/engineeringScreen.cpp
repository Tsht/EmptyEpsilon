#include "playerInfo.h"
#include "gameGlobalInfo.h"
#include "engineeringScreen.h"
#include "preferenceManager.h"

#include "screenComponents/shipInternalView.h"
#include "screenComponents/selfDestructButton.h"
#include "screenComponents/alertOverlay.h"
#include "screenComponents/customShipFunctions.h"
#include "screenComponents/powerDamageIndicator.h"
#include "screenComponents/shipsLogControl.h"

#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_togglebutton.h"
#include "gui/gui2_slider.h"
#include "gui/gui2_progressbar.h"
#include "gui/gui2_progressslider.h"
#include "gui/gui2_arrow.h"
#include "gui/gui2_image.h"
#include "gui/gui2_panel.h"

EngineeringScreen::EngineeringScreen(GuiContainer* owner, ECrewPosition crew_position)
: GuiOverlay(owner, "ENGINEERING_SCREEN", colorConfig.background), selected_system(SYS_None)
{
    // Render the background decorations.
    background_crosses = new GuiOverlay(this, "BACKGROUND_CROSSES", glm::u8vec4{255,255,255,255});
    background_crosses->setTextureTiled("gui/background/crosses.png");

    // Render the alert level color overlay.
    (new AlertLevelOverlay(this));


    auto stats = new GuiElement(this, "ENGINEER_STATS");
    stats->setPosition(20, 100, sp::Alignment::TopLeft)->setSize(240, 200)->setAttribute("layout", "vertical");

    energy_display = new GuiKeyValueDisplay(stats, "ENERGY_DISPLAY", 0.45, tr("Energy"), "");
    energy_display->setIcon("gui/icons/energy")->setTextSize(20)->setSize(240, 40);
    hull_display = new GuiKeyValueDisplay(stats, "HULL_DISPLAY", 0.45, tr("health","Hull"), "");
    hull_display->setIcon("gui/icons/hull")->setTextSize(20)->setSize(240, 40);
    shields_display = new GuiKeyValueDisplay(stats, "SHIELDS_DISPLAY", 0.45, tr("Shields"), "");
    shields_display->setIcon("gui/icons/shields")->setTextSize(20)->setSize(240, 40);
    oxygen_display = new GuiKeyValueDisplay(stats, "OXYGEN_DISPLAY", 0.45, tr("Oxygen"), "");
    oxygen_display->setIcon("gui/icons/oxygen")->setTextSize(20)->setSize(240, 40);
    coolant_display = new GuiKeyValueDisplay(stats, "COOLANT_DISPLAY", 0.45, tr("total","Coolant"), "");
    coolant_display->setIcon("gui/icons/coolant")->setTextSize(20)->setSize(240, 40);
    if (gameGlobalInfo->use_nano_repair_crew && gameGlobalInfo->use_system_damage)
    {
        repair_display = new GuiKeyValueDisplay(stats, "REPAIR_DISPLAY", 0.45, tr("total","Repair"), "");
        //repair_display->setIcon("gui/icons/system_health")->setTextSize(20)->setPosition(20, 300, sp::Alignment::TopLeft)->setSize(240, 40);
        repair_display->setIcon("gui/icons/system_health")->setTextSize(20)->setSize(240, 40);
    }
    
    self_destruct_button = new GuiSelfDestructButton(this, "SELF_DESTRUCT");
    self_destruct_button->setPosition(20, 20, sp::Alignment::TopLeft)->setSize(240, 100)->setVisible(my_spaceship && my_spaceship->getCanSelfDestruct());

    GuiElement* system_config_container = new GuiElement(this, "");
    system_config_container->setPosition(0, -50, sp::Alignment::BottomCenter)->setSize(750 + 300, GuiElement::GuiSizeMax);
    GuiElement* system_row_layouts = new GuiElement(system_config_container, "SYSTEM_ROWS");
    system_row_layouts->setPosition(0, 0, sp::Alignment::BottomLeft)->setAttribute("layout", "verticalbottom");
    system_row_layouts->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    float column_width = gameGlobalInfo->use_system_damage ? 100 : 150;
    for(int n=0; n<SYS_COUNT; n++)
    {
        string id = "SYSTEM_ROW_" + getSystemName(ESystem(n));
        SystemRow info;
        info.row = new GuiElement(system_row_layouts, id);
        info.row->setAttribute("layout", "horizontal");
        info.row->setSize(GuiElement::GuiSizeMax, 50);

        info.button = new GuiToggleButton(info.row, id + "_SELECT", getLocaleSystemName(ESystem(n)), [this, n](bool value){
            selectSystem(ESystem(n));
        });
        info.button->setSize(300, GuiElement::GuiSizeMax);

        if (my_spaceship)
        {
            info.state = new GuiPowerDamageIndicator(info.button, id + "_INDICATOR", ESystem(n), sp::Alignment::CenterLeft, my_spaceship);
            info.state ->setPosition(0, 0, sp::Alignment::BottomLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
        }

        info.damage_bar = new GuiProgressbar(info.row, id + "_DAMAGE", 0.0f, 1.0f, 0.0f);
        info.damage_bar->setSize(150, GuiElement::GuiSizeMax);
        info.damage_icon = new GuiImage(info.damage_bar, "", "gui/icons/hull");
        info.damage_icon->setColor(colorConfig.overlay_damaged)->setPosition(0, 0, sp::Alignment::CenterRight)->setSize(GuiElement::GuiSizeMatchHeight, GuiElement::GuiSizeMax);
        info.damage_label = new GuiLabel(info.damage_bar, id + "_DAMAGE_LABEL", "...", 20);
        info.damage_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
        info.heat_bar = new GuiProgressbar(info.row, id + "_HEAT", 0.0f, 1.0f, 0.0f);
        info.heat_bar->setSize(column_width, GuiElement::GuiSizeMax);
        info.heat_arrow = new GuiArrow(info.heat_bar, id + "_HEAT_ARROW", 0);
        info.heat_arrow->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
        info.heat_icon = new GuiImage(info.heat_bar, "", "gui/icons/status_overheat");
        info.heat_icon->setColor(colorConfig.overlay_overheating)->setPosition(0, 0, sp::Alignment::Center)->setSize(GuiElement::GuiSizeMatchHeight, GuiElement::GuiSizeMax);
        info.heat_label = new GuiLabel(info.heat_bar, id + "_HEAT_LABEL", "...", 20);
        info.heat_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
        info.power_bar = new GuiProgressSlider(info.row, id + "_POWER", 0.0f, 3.0f, 0.0f, [this,n](float value){
            if (my_spaceship)
                my_spaceship->commandSetSystemPowerRequest(ESystem(n), value);
        });
        info.power_bar->setColor(glm::u8vec4(192, 192, 32, 128))->setSize(column_width, GuiElement::GuiSizeMax);
        info.power_label = new GuiLabel(info.power_bar, id + "_POWER_LABEL", "...", 20);
        info.power_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
        info.coolant_bar = new GuiProgressSlider(info.row, id + "_COOLANT", 0.0f, 10.0f, 0.0f, [this,n](float value){
            if (my_spaceship)
                my_spaceship->commandSetSystemCoolantRequest(ESystem(n), value);
        });
        info.coolant_bar->setColor(glm::u8vec4(32, 128, 128, 128))->setSize(column_width, GuiElement::GuiSizeMax);
        info.coolant_label = new GuiLabel(info.coolant_bar, id + "_COOLANT_LABEL", "...", 20);
        info.coolant_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
        if (!gameGlobalInfo->use_system_damage)
        {
            info.damage_bar->hide();
            info.heat_bar->setSize(150, GuiElement::GuiSizeMax);
            info.power_bar->setSize(150, GuiElement::GuiSizeMax);
            info.coolant_bar->setSize(150, GuiElement::GuiSizeMax);
        }
        info.coolant_max_indicator = new GuiImage(info.coolant_bar, "", "gui/widget/SliderTick.png");
        info.coolant_max_indicator->setSize(40, 40);
        info.coolant_max_indicator->setAngle(90);
        info.coolant_max_indicator->setColor({255,255,255,0});

        if (gameGlobalInfo->use_nano_repair_crew)
        {
            info.damage_bar->setSize(150, GuiElement::GuiSizeMax);
            info.heat_bar->setSize(150, GuiElement::GuiSizeMax);
            info.power_bar->setSize(150, GuiElement::GuiSizeMax);
            info.coolant_bar->setSize(150, GuiElement::GuiSizeMax);
            if (gameGlobalInfo->use_system_damage)
            {
                info.repair_bar = new GuiProgressSlider(info.row, id + "_REPAIR", 0.0f, 10.0f, 0.0f, [this,n](float value){
                    if (my_spaceship)
                        my_spaceship->commandSetSystemRepairRequest(ESystem(n), value);
                });
                info.repair_bar->setColor(glm::u8vec4(32, 128, 32, 128))->setSize(150, GuiElement::GuiSizeMax);
                info.repair_label = new GuiLabel(info.repair_bar, id + "_REPAIR_LABEL", "...", 20);
                info.repair_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
                info.repair_max_indicator = new GuiImage(info.repair_bar, "", "gui/widget/SliderTick.png");
                info.repair_max_indicator->setSize(40, 40);
                info.repair_max_indicator->setAngle(90);
                info.repair_max_indicator->setColor({255,255,255,0});
        
            }
        }
        

        info.row->moveToBack();
        system_rows.push_back(info);
    }

    GuiElement* icon_layout = new GuiElement(system_row_layouts, "");
    icon_layout->setSize(GuiElement::GuiSizeMax, 48)->setAttribute("layout", "horizontal");
    (new GuiElement(icon_layout, "FILLER"))->setSize(300, GuiElement::GuiSizeMax);
    
    auto coolant_progress_func = [](float requested_unused_coolant)
    {
        float total_requested = 0.0f;
        float new_max_total = my_spaceship->max_coolant - requested_unused_coolant;
        for(int n=0; n<SYS_COUNT; n++)
            total_requested += my_spaceship->systems[n].coolant_request;
        if (new_max_total < total_requested) { // Drain systems
            for(int n=0; n<SYS_COUNT; n++)
                my_spaceship->commandSetSystemCoolantRequest((ESystem)n, my_spaceship->systems[n].coolant_request * new_max_total / total_requested);
        } else 
        { // Put coolant into systems
            int system_count = 0;
            for(int n=0; n<SYS_COUNT; n++)
                if (my_spaceship->hasSystem((ESystem)n))
                    system_count += 1;
            float add = (new_max_total - total_requested) / float(system_count);
            for(int n=0; n<SYS_COUNT; n++)
                if (my_spaceship->hasSystem((ESystem)n))
                    my_spaceship->commandSetSystemCoolantRequest((ESystem)n, std::min(my_spaceship->systems[n].coolant_request + add, 10.0f));
        }
    };

    auto repair_progress_func = [](float requested_unused_repair)
    {
        float total_requested = 0.0f;
        float new_max_total = my_spaceship->max_repair - requested_unused_repair;
        for(int n=0; n<SYS_COUNT; n++)
            total_requested += my_spaceship->systems[n].repair_request;
        if (new_max_total < total_requested) { // Drain systems
            for(int n=0; n<SYS_COUNT; n++)
                my_spaceship->commandSetSystemRepairRequest((ESystem)n, my_spaceship->systems[n].repair_request * new_max_total / total_requested);
        } else 
        { // Put nanobots into systems
            int system_count = 0;
            for(int n=0; n<SYS_COUNT; n++)
                if (my_spaceship->hasSystem((ESystem)n))
                    system_count += 1;
            float add = (new_max_total - total_requested) / float(system_count);
            for(int n=0; n<SYS_COUNT; n++)
                if (my_spaceship->hasSystem((ESystem)n))
                    my_spaceship->commandSetSystemRepairRequest((ESystem)n, std::min(my_spaceship->systems[n].repair_request + add, 10.0f));
        }
    };
    
    
    if (gameGlobalInfo->use_system_damage){
        if (gameGlobalInfo->use_nano_repair_crew)
        {
            (new GuiKeyValueDisplay(icon_layout, "SYSTEM_HEALTH", 0.9, tr("health"), ""))->setIcon("gui/icons/hull")->setTextSize(30)->setSize(150, GuiElement::GuiSizeMax);
            (new GuiKeyValueDisplay(icon_layout, "SYSTEM_HEAT", 0.9, tr("heat"), ""))->setIcon("gui/icons/status_overheat")->setTextSize(30)->setSize(150, GuiElement::GuiSizeMax);
            (new GuiKeyValueDisplay(icon_layout, "SYSTEM_POWER", 0.9, tr("power"), ""))->setIcon("gui/icons/energy")->setTextSize(30)->setSize(150, GuiElement::GuiSizeMax);
            coolant_remaining_bar = new GuiProgressSlider(icon_layout, "", 0, 10.0, 10.0, coolant_progress_func);
            coolant_remaining_bar->setColor(glm::u8vec4(32, 128, 128, 178))->setDrawBackground(false)->setSize(150, GuiElement::GuiSizeMax);
            (new GuiKeyValueDisplay(coolant_remaining_bar, "SYSTEM_COOLANT", 0.9, tr("coolant"), ""))->setIcon("gui/icons/coolant")->setTextSize(30)->setSize(150, GuiElement::GuiSizeMax);
            
            repair_remaining_bar = new GuiProgressSlider(icon_layout, "", 0, 10.0, 10.0, repair_progress_func);
            repair_remaining_bar->setColor(glm::u8vec4(32, 128, 32, 178))->setDrawBackground(false)->setSize(150, GuiElement::GuiSizeMax);
            (new GuiKeyValueDisplay(repair_remaining_bar, "SYSTEM_REPAIR", 0.9, tr("repair"), ""))->setIcon("gui/icons/system_health")->setTextSize(30)->setSize(150, GuiElement::GuiSizeMax);
        } else 
        {
            (new GuiImage(icon_layout, "SYSTEM_HEALTH_ICON", "gui/icons/hull"))->setSize(150, GuiElement::GuiSizeMax);
        }
    }
    if(!(gameGlobalInfo->use_nano_repair_crew))
    {
        (new GuiImage(icon_layout, "HEAT_ICON", "gui/icons/status_overheat"))->setSize(column_width, GuiElement::GuiSizeMax);
        (new GuiImage(icon_layout, "POWER_ICON", "gui/icons/energy"))->setSize(column_width, GuiElement::GuiSizeMax);
        coolant_remaining_bar = new GuiProgressSlider(icon_layout, "", 0, 10.0, 10.0, coolant_progress_func);
        coolant_remaining_bar->setColor(glm::u8vec4(32, 128, 128, 128))->setDrawBackground(false)->setSize(column_width, GuiElement::GuiSizeMax);
        (new GuiImage(coolant_remaining_bar, "COOLANT_ICON", "gui/icons/coolant"))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    }
    
    
    

    system_rows[SYS_Reactor].button->setIcon("gui/icons/system_reactor");
    system_rows[SYS_Cloaking].button->setIcon("gui/icons/system_cloaking");
    system_rows[SYS_BeamWeapons].button->setIcon("gui/icons/system_beam");
    system_rows[SYS_MissileSystem].button->setIcon("gui/icons/system_missile");
    system_rows[SYS_Maneuver].button->setIcon("gui/icons/system_maneuver");
    system_rows[SYS_Impulse].button->setIcon("gui/icons/system_impulse");
    system_rows[SYS_Warp].button->setIcon("gui/icons/system_warpdrive");
    system_rows[SYS_JumpDrive].button->setIcon("gui/icons/system_jumpdrive");
    system_rows[SYS_FrontShield].button->setIcon("gui/icons/shields-fore");
    system_rows[SYS_RearShield].button->setIcon("gui/icons/shields-aft");
    system_rows[SYS_Docks].button->setIcon("gui/icons/docking");
    system_rows[SYS_Drones].button->setIcon("gui/icons/heading");
    system_rows[SYS_Hangar].button->setIcon("gui/icons/door");

    // if (gameGlobalInfo->use_nano_repair_crew)
    // {
    //     system_effects_container = new GuiElement(this, "");
    //     system_effects_container->setPosition(-20, 100, sp::Alignment::TopRight)->setSize(270, 340)->setAttribute("layout", "vertical");
    // } else {
        system_effects_container = new GuiElement(system_config_container, "");
        system_effects_container->setPosition(0, -400, sp::Alignment::BottomRight)->setSize(270, 400)->setAttribute("layout", "vertical");
    //}
    
    GuiPanel* box = new GuiPanel(system_config_container, "POWER_COOLANT_BOX");
    box->setPosition(0, 0, sp::Alignment::BottomRight)->setSize(270, 400);
    power_label = new GuiLabel(box, "POWER_LABEL", tr("slider", "Power"), 20);
    power_label->setVertical()->setAlignment(sp::Alignment::Center)->setPosition(20, 20, sp::Alignment::TopLeft)->setSize(30, 360);
    coolant_label = new GuiLabel(box, "COOLANT_LABEL", tr("slider", "Coolant"), 20);
    coolant_label->setVertical()->setAlignment(sp::Alignment::Center)->setPosition(110, 20, sp::Alignment::TopLeft)->setSize(30, 360);

    power_slider = new GuiSlider(box, "POWER_SLIDER", 3.0, 0.0, 1.0, [this](float value) {
        if (my_spaceship && selected_system != SYS_None)
            my_spaceship->commandSetSystemPowerRequest(selected_system, value);
    });
    power_slider->setPosition(50, 20, sp::Alignment::TopLeft)->setSize(60, 360);
    for(float snap_point = 0.0f; snap_point <= 3.0f; snap_point += 0.5f)
        power_slider->addSnapValue(snap_point, snap_point == 1.0f ? 0.1f : 0.01f);
    power_slider->disable();
    coolant_slider = new GuiSlider(box, "COOLANT_SLIDER", 10.0, 0.0, 0.0, [this](float value) {
        if (my_spaceship && selected_system != SYS_None)
            my_spaceship->commandSetSystemCoolantRequest(selected_system, value);
    });
    coolant_slider->setPosition(140, 20, sp::Alignment::TopLeft)->setSize(60, 360);
    coolant_slider->disable();

    if (!gameGlobalInfo->use_nano_repair_crew)
        (new GuiShipInternalView(system_row_layouts, "SHIP_INTERNAL_VIEW", 48.0f))->setShip(my_spaceship)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
        
    if (gameGlobalInfo->use_nano_repair_crew)
    {
        box->setPosition(0, 20, sp::Alignment::TopCenter)->setSize(450, 220);
        power_label->setHorizontal()->setPosition(20, 20, sp::Alignment::TopLeft)->setSize(400, 20);
        power_slider->setRange(0.0, 3.0)->setPosition(20, 40, sp::Alignment::TopLeft)->setSize(400, 40);
        coolant_label->setHorizontal()->setPosition(20, 80, sp::Alignment::TopLeft)->setSize(400, 20);
        coolant_slider->setRange(0.0, 10.0)->setPosition(20, 100, sp::Alignment::TopLeft)->setSize(400, 40);
        
        if (gameGlobalInfo->use_system_damage)
        {
            repair_label = new GuiLabel(box, "COOLANT_LABEL", tr("slider", "Repair"), 20);
            repair_label->setAlignment(sp::Alignment::CenterLeft)->setPosition(20, 140, sp::Alignment::TopLeft)->setSize(400, 20);
            repair_slider = new GuiSlider(box, "REPAIR_SLIDER", 0.0, 10.0, 0.0, [this](float value) {
            if (my_spaceship && selected_system != SYS_None)
                my_spaceship->commandSetSystemRepairRequest(selected_system, value);
            });
            repair_slider->setPosition(20, 160, sp::Alignment::TopLeft)->setSize(400, 40);
            repair_slider->disable();
        }
    }

    (new GuiCustomShipFunctions(this, crew_position, "", my_spaceship))->setPosition(-20, 100, sp::Alignment::TopRight)->setSize(250, GuiElement::GuiSizeMax);

    new ShipsLog(this,"intern");
    // Bottom layout.
    auto layout = new GuiElement(this, "");
    layout->setPosition(-20, -20, sp::Alignment::BottomRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "verticalbottom");;
    std::vector<GuiElement*> presets_buttons_layouts;

    // Presets buttons.
    presets_button = new GuiToggleButton(layout, "PRESET", tr("preset", "presets"), [this](bool value)
    {
        for(int buttonId = 0 ; buttonId < my_spaceship->max_engineer_presets_number*2 ; buttonId++)
        {
            if (buttonId < my_spaceship->active_engineer_presets_number*2)
                presets_buttons.at(buttonId)->setVisible(value);
            else
                presets_buttons.at(buttonId)->setVisible(false);
        }
    });
    presets_button->setValue(false);
    presets_button->setTextSize(20);
    presets_button->setSize(125, 25);

    for(int presetId=1; presetId < my_spaceship->max_engineer_presets_number + 1; presetId++)
    {

        auto preset_button_layout = new GuiElement(layout, "");
        preset_button_layout->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "horizontal");;
        GuiButton* preset_button_apply = new GuiButton(preset_button_layout, "", tr("preset", "preset") + " " + std::to_string(presetId), [this, presetId]()
        {
            applyPreset(presetId);
            for(GuiButton* button : presets_buttons)
                button->setVisible(false);
            presets_button->setValue(false);
        });
        preset_button_apply->setVisible(false);
        preset_button_apply->setTextSize(20);
        preset_button_apply->setSize(100, 25);   
        presets_buttons.push_back(preset_button_apply);
        GuiButton* preset_button_update = new GuiButton(preset_button_layout, "", "", [this, presetId]()
        {
            updatePreset(presetId);
            //my_spaceship->saveToPreferencesEngineerPresets();
            for(GuiButton* button : presets_buttons)
                button->setVisible(false);
            presets_button->setValue(false);
        });
        preset_button_update->setVisible(false);
        preset_button_update->setTextSize(20);
        preset_button_update->setIcon("gui/icons/save-arrow");
        preset_button_update->setSize(25, 25);   
        presets_buttons.push_back(preset_button_update);

        //preset_button_layout->setVisible(false);
        preset_button_layout->setSize(125, 25);   
        presets_buttons_layouts.push_back(preset_button_layout);
    }


    new ShipsLog(this, crew_position);

    previous_energy_level = 0.0f;
    average_energy_delta = 0.0f;
    previous_energy_measurement = 0.0f;
}

void EngineeringScreen::applyPreset(int preset)
{
    if (my_spaceship)
    {
        for(int n = 0; n < SYS_COUNT; n++)
        {
            ESystem system = ESystem(n);
            my_spaceship->commandSetSystemPowerRequest(system, my_spaceship->power_presets[n][preset-1]);
            my_spaceship->commandSetSystemCoolantRequest(system, my_spaceship->coolant_presets[n][preset-1]);
        }
        my_spaceship->addToSpecificShipLog(tr("preset","Preset {id_preset} loaded").format({{"id_preset", string(preset)}}), colorConfig.log_receive_neutral, "intern");
    }
}

void EngineeringScreen::updatePreset(int preset)
{
    if (my_spaceship)
    {
        for(int n = 0; n < SYS_COUNT; n++)
        {
            ESystem system = ESystem(n);
            my_spaceship->commandSetSystemPowerPreset(system, preset-1, my_spaceship->systems[n].power_request);
            my_spaceship->commandSetSystemCoolantPreset(system, preset-1, my_spaceship->systems[n].coolant_request);
        }
    }
    // Save preset to options.ini file
    string power_saved_presets = "";
    string coolant_saved_presets = "";
    for(int n = 0; n < SYS_COUNT-1; n++)
    {
        power_saved_presets += string(my_spaceship->systems[n].power_request, 2) + "|";
        coolant_saved_presets += string(my_spaceship->systems[n].coolant_request, 2) + "|";
    }
    power_saved_presets += string(my_spaceship->systems[SYS_COUNT-1].power_request, 2);
    coolant_saved_presets += string(my_spaceship->systems[SYS_COUNT-1].coolant_request, 2);
    
    PreferencesManager::set("ENGINEERING.PRESET_POWER_"+string(preset), power_saved_presets);
    PreferencesManager::set("ENGINEERING.PRESET_COOLANT_"+string(preset), coolant_saved_presets);

    //Save preferences
    if (getenv("HOME"))
        PreferencesManager::save(string(getenv("HOME")) + "/.emptyepsilon/options.ini");
    else
        PreferencesManager::save("options.ini");
}

void EngineeringScreen::onDraw(sp::RenderTarget& renderer)
{
    if (my_spaceship)
    {
        // Update presets number
        for(int buttonId = 0 ; buttonId < my_spaceship->max_engineer_presets_number*2; buttonId++)
        {
            if (buttonId < my_spaceship->active_engineer_presets_number*2)
                presets_buttons.at(buttonId)->setVisible(presets_button->getValue());
            else
                presets_buttons.at(buttonId)->setVisible(false);
        }
        presets_button->setVisible(my_spaceship->active_engineer_presets_number > 0);

        // Update the energy usage.
        if (previous_energy_measurement == 0.0f)
        {
            previous_energy_level = my_spaceship->energy_level;
            previous_energy_measurement = engine->getElapsedTime();
        }else{
            if (previous_energy_measurement != engine->getElapsedTime())
            {
                float delta_t = engine->getElapsedTime() - previous_energy_measurement;
                float delta_e = my_spaceship->energy_level - previous_energy_level;
                float delta_e_per_second = delta_e / delta_t;
                average_energy_delta = average_energy_delta * 0.99f + delta_e_per_second * 0.01f;

                previous_energy_level = my_spaceship->energy_level;
                previous_energy_measurement = engine->getElapsedTime();
            }
        }

        energy_display->setValue(toNearbyIntString(my_spaceship->energy_level) + " (" + tr("{energy}/min").format({{"energy", toNearbyIntString(average_energy_delta * 60.0f)}}) + ")");
        if (my_spaceship->energy_level < 100.0f)
            energy_display->setColor(glm::u8vec4(255, 0, 0, 255));
        else
            energy_display->setColor(glm::u8vec4{255,255,255,255});
        hull_display->setValue(toNearbyIntString(100.0f * my_spaceship->hull_strength / my_spaceship->hull_max) + "%");
        if (my_spaceship->hull_strength < my_spaceship->hull_max / 4.0f)
            hull_display->setColor(glm::u8vec4(255, 0, 0, 255));
        else
            hull_display->setColor(glm::u8vec4{255,255,255,255});

        if (my_spaceship->getShieldCount() > 0)
        {
            shields_display->show();
            string shields_info = "";
            for(int n=0; n<my_spaceship->getShieldCount(); n++)
                shields_info += string(my_spaceship->getShieldPercentage(n)) + "% ";
            shields_display->setValue(shields_info);
        }
        else
            shields_display->hide();

        string text_oxygen = "";
        for(int n = 0; n < my_spaceship->oxygen_zones; n++)
            text_oxygen += toNearbyIntString(100.0f * my_spaceship->getOxygenPoints(n) / my_spaceship->getOxygenMax(n)) + "% ";
        oxygen_display->setValue(text_oxygen);
        if (my_spaceship->getOxygenTotal() < 0.2f)
            oxygen_display->setColor(glm::u8vec4(255,0,0,255));
        else
            oxygen_display->setColor(glm::u8vec4(255,255,255,255));
        oxygen_display->setVisible(my_spaceship->getOxygenMaxTotal() > 0);
        coolant_display->setValue(toNearbyIntString(my_spaceship->max_coolant * 10) + "%");
        if (gameGlobalInfo->use_nano_repair_crew)
        {
            coolant_display->setValue(toNearbyIntString(my_spaceship->max_coolant));
            if (gameGlobalInfo->use_system_damage)
                repair_display->setValue(toNearbyIntString(my_spaceship->max_repair));
        }
        
        float total_coolant_used = 0.0f;
        float total_repair_used = 0.0f;
        for(int n=0; n<SYS_COUNT; n++)
        {
            SystemRow info = system_rows[n];
            auto& system = my_spaceship->systems[n];
            info.row->setVisible(my_spaceship->hasSystem(ESystem(n)));

            float health = system.health;
            if (health < 0.0f)
                info.damage_bar->setValue(-health)->setColor(glm::u8vec4(128, 32, 32, 192));
            else
                info.damage_bar->setValue(health)->setColor(glm::u8vec4(64, 128 * health, 64 * health, 192));
            info.damage_label->setText(toNearbyIntString(health * 100) + "%");
            info.heat_label->setText(toNearbyIntString(system.heat_level * 100) + "%");
            info.heat_label->setVisible(system.heat_level > 0.f);
            info.power_label->setText(toNearbyIntString(system.power_level * 100) + "%");
            info.coolant_label->setText(string(system.coolant_level, 1));
            info.coolant_label->setVisible(system.coolant_level > 0.f);
            if (gameGlobalInfo->use_nano_repair_crew and gameGlobalInfo->use_system_damage)
            {
                info.repair_label->setText(string(system.repair_level, 1));
                info.repair_label->setVisible(system.repair_level > 0.f);
            }
            float health_max = system.health_max;
            if (health_max < 1.0f)
                info.damage_icon->show();
            else
                info.damage_icon->hide();

            float heat = system.heat_level;
            info.heat_bar->setValue(heat)->setColor(glm::u8vec4(128, 32 + 96 * (1.0f - heat), 32, 192));
            float heating_diff = system.getHeatingDelta();
            if (heating_diff > 0)
                info.heat_arrow->setAngle(90);
            else
                info.heat_arrow->setAngle(-90);
            info.heat_arrow->setVisible(heat > 0);
            info.heat_arrow->setColor(glm::u8vec4(255, 255, 255, std::min(255, int(255.0f * fabs(heating_diff)))));
            if (heat > 0.9f && fmod(engine->getElapsedTime(), 0.5f) < 0.25f)
                info.heat_icon->show();
            else
                info.heat_icon->hide();

            info.power_bar->setValue(system.power_level);
            info.coolant_bar->setValue(system.coolant_level);
            info.coolant_bar->setRange(0.0, my_spaceship->max_coolant_per_system);
            if (system.coolant_request > 0.0f) {
                float f = system.coolant_request / 10.f;
                info.coolant_max_indicator->setPosition(-20 + info.coolant_bar->getSize().x * f, 5);
                info.coolant_max_indicator->setColor({255,255,255,255});
            } 
            else 
            {
                info.coolant_max_indicator->setColor({255,255,255,0});
            }
            if (gameGlobalInfo->use_nano_repair_crew && gameGlobalInfo->use_system_damage)
            {
                info.repair_bar->setRange(0.0, my_spaceship->max_repair_per_system);
                ///info.repair_bar->setValue(system.repair_level);
                info.repair_bar->setValue(std::min(system.repair_level, my_spaceship->max_repair_per_system));

                if (system.repair_request > 0.0f) 
                {
                    float f = system.repair_request / my_spaceship->max_repair_per_system;
                    info.repair_max_indicator->setPosition(-20 + info.repair_bar->getSize().x * f, 5);
                    info.repair_max_indicator->setColor({255,255,255,255});
                } 
                else 
                {
                    info.repair_max_indicator->setColor({255,255,255,0});
                }

                if(system.repair_level > my_spaceship->max_repair_per_system)
                        my_spaceship->commandSetSystemRepairRequest(ESystem(n), my_spaceship->max_repair_per_system);
                else if(system.repair_level > my_spaceship->max_repair)
                        my_spaceship->commandSetSystemRepairRequest(ESystem(n), my_spaceship->max_repair);
                total_repair_used += system.repair_level;
            }
            total_coolant_used += system.coolant_level;
        }
        coolant_remaining_bar->setRange(0, my_spaceship->max_coolant);
        coolant_remaining_bar->setValue(my_spaceship->max_coolant - total_coolant_used);

        repair_remaining_bar->setRange(0, my_spaceship->max_repair);
        repair_remaining_bar->setValue(my_spaceship->max_repair - total_repair_used);

        if (selected_system != SYS_None)
        {
            ShipSystem& system = my_spaceship->systems[selected_system];
            power_label->setText(tr("Power") +": " + toNearbyIntString(system.power_level * 100) + "%/" + toNearbyIntString(system.power_request * 100) + "%");
            power_slider->setValue(system.power_request);
            coolant_label->setText(tr("Coolant") +": " + toNearbyIntString(system.coolant_level / my_spaceship->max_coolant_per_system * 100) + "%/" + toNearbyIntString(std::min(system.coolant_request, my_spaceship->max_coolant) / my_spaceship->max_coolant_per_system * 100) + "%");
            coolant_slider->setEnable(!my_spaceship->auto_coolant_enabled);
            coolant_slider->setRange(0.0, my_spaceship->max_coolant_per_system);
            coolant_slider->setValue(std::min(system.coolant_request, my_spaceship->max_coolant));
            if (gameGlobalInfo->use_nano_repair_crew)
            {
                coolant_label->setText(tr("Coolant") +": " + string(system.coolant_level, 1) + " / " + string(my_spaceship->max_coolant_per_system, 1) + "\t\t ("+ tr("Target") +": " + string(system.coolant_request, 1)+")");
                power_label->setText(tr("Power") +": " + toNearbyIntString(system.power_level * 100) + "% \t\t ("+ tr("Target") +": " + toNearbyIntString(system.power_request * 100) + "%)");
                if (gameGlobalInfo->use_system_damage)
                {
                    repair_slider->setEnable(!my_spaceship->auto_repair_enabled);
                    repair_slider->setRange(0.0, my_spaceship->max_repair_per_system);
                    repair_slider->setValue(std::min(system.repair_request, my_spaceship->max_repair_per_system));
                    repair_slider->addSnapValue(my_spaceship->max_repair, 0.1);
                    repair_label->setText(tr("Repair") +": " + string(system.repair_level, 1) + " / " + string(my_spaceship->max_repair_per_system, 1) + "\t\t ("+ tr("Target") +": " + string(system.repair_request, 1)+")");
                }
            }

            system_effects_index = 0;
            float effectiveness = my_spaceship->getSystemEffectiveness(selected_system);
            float health_max = my_spaceship->getSystemHealthMax(selected_system);
            if (health_max < 1.0f)
            {
                addSystemEffect(tr("Necessary intervention"), tr("see log"),glm::u8vec4(255,0,0,255)); //TODO TRAD
                addSystemEffect(tr("Maximal health"), toNearbyIntString(health_max * 100) + "%"); //TODO TRAD
            }
            switch(selected_system)
            {
            case SYS_Reactor:
                if (effectiveness > 1.0f)
                    effectiveness = (1.0f + effectiveness) / 2.0f;
                addSystemEffect(tr("Energy production"),  tr("{energy}/min").format({{"energy", string(effectiveness * -my_spaceship->systems[SYS_Reactor].power_factor * 60.0f, 1)}}));
                // Oxygene
                for(int n = 0; n < my_spaceship->oxygen_zones; n++)
                    addSystemEffect(tr("Zone ") + string(n+1) + ", oxygene", string(my_spaceship->getOxygenRechargeRate(n) * 60.0f, 1) + "/m");
                break;
            case SYS_Cloaking:
                addSystemEffect(tr("Cloaking grade"), string(my_spaceship->getCloakingDegree() * 100.0f, 1) + "%");
                if (my_spaceship->getCloakingDegree() > 0.5f)
                    addSystemEffect(tr("Radar detection"), tr("no"));
                else
                    addSystemEffect(tr("Radar detection"), tr("yes"));
                break;
            case SYS_BeamWeapons:
                addSystemEffect(tr("Firing rate"), toNearbyIntString(effectiveness * 100) + "%");
                // If the ship has a turret, also note that the rotation rate
                // is affected.
                for(int n = 0; n < max_beam_weapons; n++)
                {
                    if (my_spaceship->beam_weapons[n].getTurretArc() > 0)
                    {
                        addSystemEffect(tr("Beam rotation"), toNearbyIntString(effectiveness * 100) + "%");
                        break;
                    }
                }
                break;
            case SYS_MissileSystem:
                addSystemEffect(tr("missile","Reload rate"), toNearbyIntString(effectiveness * 100) + "%");
                addSystemEffect(tr("Missiles speed"), toNearbyIntString(effectiveness * 100) + "%");
                break;
            case SYS_Maneuver:
                addSystemEffect(tr("Turning speed"), toNearbyIntString(effectiveness * 100) + "%");
                if (my_spaceship->combat_maneuver_boost_speed > 0.0f || my_spaceship->combat_maneuver_strafe_speed)
                    addSystemEffect(tr("Combat recharge rate"), toNearbyIntString(((my_spaceship->getSystemEffectiveness(SYS_Maneuver) + my_spaceship->getSystemEffectiveness(SYS_Impulse)) / 2.0f) * 100) + "%");
                break;
            case SYS_Impulse:
                addSystemEffect(tr("Impulse speed"), toNearbyIntString(effectiveness * 100) + "%");
                if (my_spaceship->combat_maneuver_boost_speed > 0.0f || my_spaceship->combat_maneuver_strafe_speed)
                    addSystemEffect(tr("Combat recharge rate"), toNearbyIntString(((my_spaceship->getSystemEffectiveness(SYS_Maneuver) + my_spaceship->getSystemEffectiveness(SYS_Impulse)) / 2.0f) * 100) + "%");
                break;
            case SYS_Warp:
                addSystemEffect(tr("Warp drive speed"), toNearbyIntString(effectiveness * 100) + "%");
                break;
            case SYS_JumpDrive:
                addSystemEffect(tr("Jump drive recharge rate"), toNearbyIntString(my_spaceship->getJumpDriveRechargeRate() * 100) + "%");
                addSystemEffect(tr("Jump drive jump speed"), toNearbyIntString(effectiveness * 100) + "%");
                //if (effectiveness > 1.0)
                //    addSystemEffect("Range JUMP", toNearbyIntString(effectiveness * 100) + "%");
                break;
            case SYS_FrontShield:
                if (gameGlobalInfo->use_beam_shield_frequencies)
                    addSystemEffect(tr("shields","Calibration speed"), toNearbyIntString((my_spaceship->getSystemEffectiveness(SYS_FrontShield) + my_spaceship->getSystemEffectiveness(SYS_RearShield)) / 2.0f * 100) + "%");
                addSystemEffect(tr("shields","Charge rate"), toNearbyIntString(effectiveness * 100) + "%");
                {
                    DamageInfo di;
                    di.type = DT_Kinetic;
                    float damage_negate = 1.0f - my_spaceship->getShieldDamageFactor(di, 0);
                    if (damage_negate < 0.0f)
                        addSystemEffect(tr("Extra damage"), toNearbyIntString(-damage_negate * 100) + "%");
                    else
                        addSystemEffect(tr("Damage negate"), toNearbyIntString(damage_negate * 100) + "%");
                }
                break;
            case SYS_RearShield:
                if (gameGlobalInfo->use_beam_shield_frequencies)
                    addSystemEffect(tr("shields","Calibration speed"), toNearbyIntString((my_spaceship->getSystemEffectiveness(SYS_FrontShield) + my_spaceship->getSystemEffectiveness(SYS_RearShield)) / 2.0f * 100) + "%");
                addSystemEffect(tr("shields","Charge rate"), toNearbyIntString(effectiveness * 100) + "%");
                {
                    DamageInfo di;
                    di.type = DT_Kinetic;
                    float damage_negate = 1.0f - my_spaceship->getShieldDamageFactor(di, my_spaceship->shield_count - 1);
                    if (damage_negate < 0.0f)
                        addSystemEffect(tr("Extra damage"), toNearbyIntString(-damage_negate * 100) + "%");
                    else
                        addSystemEffect(tr("Damage negate"), toNearbyIntString(damage_negate * 100) + "%");
                }
                break;
            case SYS_Docks:
                addSystemEffect(tr("Airship transfer speed"), toNearbyIntString(effectiveness * 100) + "%");
                addSystemEffect(tr("Energy transfer speed"), string(effectiveness * PlayerSpaceship::energy_transfer_per_second) + "/s");
                break;
            case SYS_Drones:
                addSystemEffect(tr("Drone control"), string(my_spaceship->getDronesControlRange() / 1000.0f,1) + "U");
                addSystemEffect(tr("Short range radar"), string(my_spaceship->getShortRangeRadarRange() / 1000.0f,1) + "U");
                addSystemEffect(tr("Long range radar"), string( my_spaceship->getLongRangeRadarRange() / 1000.0f,1) + "U");
                if(my_spaceship->getSystemEffectiveness(SYS_Drones) >= 0.1f)
                {
                    addSystemEffect(tr("Power factor"), " x" + string(std::sqrt(my_spaceship->getSystemEffectiveness(SYS_Drones)) ,1));
                }
                else
                {
                    addSystemEffect(tr("Auxiliary generator"), " x" + string(std::sqrt(0.1f), 1) ,glm::u8vec4(255,255,0,255));
                    
                }
                
                break;
            case SYS_Hangar:
            {
                
                if(my_spaceship->getSystemEffectiveness(SYS_Hangar) >= 0.3f)
                {
                    addSystemEffect(tr("Working"),"");
                }
                else
                {
                    addSystemEffect(tr("Not enough energy"),"", glm::u8vec4(255,0,0,255));
                }
            }
                break;
            default:
                break;
            }
            for(unsigned int idx=system_effects_index; idx<system_effects.size(); idx++)
                system_effects[idx]->hide();
        }
    }
    GuiOverlay::onDraw(renderer);
}

void EngineeringScreen::onUpdate()
{
    if (my_spaceship && isVisible())
    {
        for(unsigned int n=0; n<SYS_COUNT; n++) {
            if (keys.engineering_select_system[n].getDown()) selectSystem(static_cast<ESystem>(n));

            float set_value = keys.engineering_set_power_for_system[n].getValue() * 3.0f;
            if (set_value != my_spaceship->systems[n].power_request && (set_value != 0.0f || set_power_active[n]))
            {
                my_spaceship->commandSetSystemPowerRequest(selected_system, set_value);
                set_power_active[n] = set_value != 0.0f; //Make sure the next update is send, even if it is back to zero.
            }
            set_value = keys.engineering_set_coolant_for_system[n].getValue() * my_spaceship->max_coolant_per_system;
            if (set_value != my_spaceship->systems[n].coolant_request && (set_value != 0.0f || set_coolant_active[n]))
            {
                my_spaceship->commandSetSystemCoolantRequest(selected_system, set_value);
                set_coolant_active[n] = set_value != 0.0f; //Make sure the next update is send, even if it is back to zero.
            }
        }

        if (selected_system != SYS_None)
        {
            // Note the code duplication with extra/powerManagement
            if (keys.engineering_set_power_000.getDown())
            {
                power_slider->setValue(0.0f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }
            if (keys.engineering_set_power_030.getDown())
            {
                power_slider->setValue(0.3f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }
            if (keys.engineering_set_power_050.getDown())
            {
                power_slider->setValue(0.5f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }
            if (keys.engineering_set_power_100.getDown())
            {
                power_slider->setValue(1.0f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }
            if (keys.engineering_set_power_150.getDown())
            {
                power_slider->setValue(1.5f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }
            if (keys.engineering_set_power_200.getDown())
            {
                power_slider->setValue(2.0f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }
            if (keys.engineering_set_power_250.getDown())
            {
                power_slider->setValue(2.5f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }
            if (keys.engineering_set_power_300.getDown())
            {
                power_slider->setValue(3.0f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }

            auto power_adjust = (keys.engineering_increase_power.getValue() - keys.engineering_decrease_power.getValue()) * 0.1f;
            if (power_adjust != 0.0f)
            {
                power_slider->setValue(my_spaceship->systems[selected_system].power_request + power_adjust);
                my_spaceship->commandSetSystemPowerRequest(selected_system, power_slider->getValue());
            }
            auto coolant_adjust = (keys.engineering_increase_coolant.getValue() - keys.engineering_decrease_coolant.getValue()) * 0.5f;
            if (coolant_adjust != 0.0f)
            {
                coolant_slider->setValue(my_spaceship->systems[selected_system].coolant_request + coolant_adjust);
                my_spaceship->commandSetSystemCoolantRequest(selected_system, coolant_slider->getValue());
            }
            if (keys.engineering_set_minimum_coolant.getDown())
            {
                coolant_slider->setValue(0.0f);
                my_spaceship->commandSetSystemCoolantRequest(selected_system, coolant_slider->getValue());
            }
            if (keys.engineering_reset.getDown())
            {
                power_slider->setValue(1.0f);
                coolant_slider->setValue(0.0f);
                my_spaceship->commandSetSystemPowerRequest(selected_system, 1.0f);
                my_spaceship->commandSetSystemCoolantRequest(selected_system, 0.0f);
            }

            float set_value = keys.engineering_set_power.getValue() * 3.0f;
            if (set_value != my_spaceship->systems[selected_system].power_request && (set_value != 0.0f || set_power_active[selected_system]))
            {
                my_spaceship->commandSetSystemPowerRequest(selected_system, set_value);
                set_power_active[selected_system] = set_value != 0.0f; //Make sure the next update is send, even if it is back to zero.
            }
            set_value = keys.engineering_set_coolant.getValue() * my_spaceship->max_coolant_per_system;
            if (set_value != my_spaceship->systems[selected_system].coolant_request && (set_value != 0.0f || set_coolant_active[selected_system]))
            {
                my_spaceship->commandSetSystemCoolantRequest(selected_system, set_value);
                set_coolant_active[selected_system] = set_value != 0.0f; //Make sure the next update is send, even if it is back to zero.
            }
        }

        for(int presetId=1; presetId < 10; presetId++) 
        {
            if (presetId <= my_spaceship->active_engineer_presets_number)
            {
                if (keys.engineering_apply_preset[presetId-1].getDown())
                    applyPreset(presetId);
                if (keys.engineering_update_preset[presetId-1].getDown())
                    updatePreset(presetId);
            }
        }
    }
}

void EngineeringScreen::selectSystem(ESystem system)
{
    if (my_spaceship && !my_spaceship->hasSystem(system))
        return;

    for(int idx=0; idx<SYS_COUNT; idx++)
    {
        system_rows[idx].button->setValue(idx == system);
    }
    selected_system = system;
    power_slider->enable();
    if (my_spaceship)
    {
        power_slider->setValue(my_spaceship->systems[system].power_request);
        coolant_slider->setValue(my_spaceship->systems[system].coolant_request);
        if (gameGlobalInfo->use_nano_repair_crew && gameGlobalInfo->use_system_damage)
            repair_slider->setValue(my_spaceship->systems[system].repair_request);
    }
}

void EngineeringScreen::addSystemEffect(string key, string value, glm::u8vec4 color)
{
    if (system_effects_index == system_effects.size())
    {
        GuiKeyValueDisplay* item = new GuiKeyValueDisplay(system_effects_container, "", 0.7, key, value);
        item->setTextSize(20)->setSize(GuiElement::GuiSizeMax, 40);
        system_effects.push_back(item);
    }else{
        system_effects[system_effects_index]->setKey(key);
        system_effects[system_effects_index]->setValue(value);
        system_effects[system_effects_index]->setColor(color);
        system_effects[system_effects_index]->show();
    }
    system_effects_index++;
}

string EngineeringScreen::toNearbyIntString(float value)
{
    return string(int(nearbyint(value)));
}
