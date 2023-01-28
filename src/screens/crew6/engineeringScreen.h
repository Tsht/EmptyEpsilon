#ifndef ENGINEERING_SCREEN_H
#define ENGINEERING_SCREEN_H

#include "gui/gui2_overlay.h"
#include "shipTemplate.h"
#include "playerInfo.h"

class GuiSelfDestructButton;
class GuiKeyValueDisplay;
class GuiLabel;
class GuiSlider;
class GuiAutoLayout;
class GuiImage;
class GuiArrow;
class GuiToggleButton;
class GuiButton; 
class GuiProgressbar;
class GuiProgressSlider;
class GuiPowerDamageIndicator;

class EngineeringScreen : public GuiOverlay
{
private:
    GuiOverlay* background_crosses;

    GuiKeyValueDisplay* energy_display;
    GuiKeyValueDisplay* hull_display;
    GuiKeyValueDisplay* shields_display;
    GuiKeyValueDisplay* oxygen_display;
    GuiKeyValueDisplay* coolant_display;
    GuiKeyValueDisplay* repair_display;
    GuiSelfDestructButton* self_destruct_button;
    GuiLabel* power_label;
    GuiSlider* power_slider;
    GuiLabel* coolant_label;
    GuiSlider* coolant_slider;
    GuiLabel* repair_label;
    GuiSlider* repair_slider;
    GuiToggleButton* presets_button;
    std::vector<GuiButton*> presets_buttons;

    class SystemRow
    {
    public:
        GuiAutoLayout* layout;
        GuiToggleButton* button;
        GuiPowerDamageIndicator* state;
        GuiProgressbar* max_health_bar;
        GuiProgressbar* damage_bar;
        GuiImage* damage_icon;
        GuiLabel* damage_label;
        GuiProgressbar* heat_bar;
        GuiArrow* heat_arrow;
        GuiImage* heat_icon;
        GuiLabel* heat_label;
        GuiProgressSlider* power_bar;
        GuiLabel* power_label;
        GuiProgressSlider* coolant_bar;
        GuiLabel* coolant_label;
        GuiProgressSlider* repair_bar;
        GuiLabel* repair_label;
        GuiImage* coolant_max_indicator;
    };
    std::vector<SystemRow> system_rows;
    GuiAutoLayout* system_effects_container;
    std::vector<GuiKeyValueDisplay*> system_effects;
    unsigned int system_effects_index;
    ESystem selected_system;

    float previous_energy_measurement;
    float previous_energy_level;
    float average_energy_delta;

    void addSystemEffect(string key, string value, glm::u8vec4 color = glm::u8vec4(255,255,255,255));
    void selectSystem(ESystem system);

    string toNearbyIntString(float value);
public:
    EngineeringScreen(GuiContainer* owner, ECrewPosition crew_position=engineering);

    virtual void onDraw(sp::RenderTarget& target) override;
    virtual void onUpdate() override;
    //Ajout tdelc/tsht
    virtual void applyPreset(int preset);
    virtual void updatePreset(int preset);
};

#endif//ENGINEERING_SCREEN_H
