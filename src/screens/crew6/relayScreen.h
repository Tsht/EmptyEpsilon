#ifndef RELAY_SCREEN_H
#define RELAY_SCREEN_H

#include "screenComponents/targetsContainer.h"
#include "gui/gui2_overlay.h"

class GuiRadarView;
class GuiKeyValueDisplay;
class GuiAutoLayout;
class GuiButton;
class GuiToggleButton;
class GuiSlider;
class GuiLabel;
class GuiHackingDialog;
class GuiHackDialog;
class GuiTextEntry;
class GuiSelector;
class GuiProgressbar;

class RelayScreen : public GuiOverlay
{
private:
    enum EMode
    {
        TargetSelection,
        WaypointPlacement,
        LaunchProbe,
        MoveWaypoint
    };

    EMode mode;
    TargetsContainer targets;
    P<SpaceObject> target;
    int drag_waypoint_index;
    GuiRadarView* radar;

    GuiKeyValueDisplay* info_distance;
    GuiKeyValueDisplay* info_radar_range;
    GuiKeyValueDisplay* info_callsign;
    GuiKeyValueDisplay* info_faction;
    GuiKeyValueDisplay* info_probe;

    //GuiKeyValueDisplay* info_reputation;
    GuiButton* center_screen_button;
    GuiSelector* station_selector;
    GuiSelector* probe_selector;
    //GuiKeyValueDisplay* info_reputation;
    GuiKeyValueDisplay* info_clock;
    GuiAutoLayout* option_buttons;
//    GuiButton* comm_icon;
    GuiButton* hack_target_button;
    GuiToggleButton* link_to_science_button;
    GuiToggleButton* link_to_3D_port_button;
    GuiAutoLayout* waypoints_layout;
    GuiButton* add_waypoint_button;
    GuiButton* delete_waypoint_button;
    GuiButton* launch_probe_button;
    GuiProgressbar* progress_probe;
    GuiAutoLayout* view_controls;
    bool position_text_custom;
    GuiTextEntry* position_text;
    GuiElement* position_entry;

    GuiToggleButton* alert_level_button;
    std::vector<GuiButton*> alert_level_buttons;

    GuiSlider* zoom_slider;
    GuiLabel* zoom_label;

    //GuiHackingDialog* hacking_dialog;
    GuiHackDialog* hacking_dialog;

    glm::vec2 mouse_down_position;
    const float max_distance = 10000000.0f;
    const float min_distance = 6250.0f;
    float distance;
public:
    RelayScreen(GuiContainer* owner, bool allow_comms);

    virtual void onDraw(sf::RenderTarget& window);
    virtual void onHotkey(const HotkeyResult& key) override;
};

#endif //RELAY_SCREEN_H
