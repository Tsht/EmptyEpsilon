#ifndef RADAR_VIEW_H
#define RADAR_VIEW_H

#include "gui/gui2_element.h"
#include "sectorsView.h"
#include "spaceObjects/playerSpaceship.h"
#include "gui/gui2_overlay.h"

class GuiMissileTubeControls;

class GuiRadarView : public SectorsView
{
public:
    enum ERadarStyle
    {
        Rectangular,
        Circular,
        CircularMasked
    };
    enum EFogOfWarStyle
    {
        NoFogOfWar,
        NebulaFogOfWar,
        FriendlysShortRangeFogOfWar,
        NoObjects
    };

    typedef std::function<void(sp::io::Pointer::Button button, glm::vec2 position)> bpfunc_t;
    typedef std::function<void(glm::vec2 position)> pfunc_t;
    typedef std::function<void(float position)>     ffunc_t;
private:
    class GhostDot
    {
    public:
        constexpr static float total_lifetime = 60.0f;

        glm::vec2 position{};
        float end_of_life;

        GhostDot(glm::vec2 pos) : position(pos), end_of_life(engine->getElapsedTime() + total_lifetime) {}
    };
    std::vector<GhostDot> ghost_dots;
    float next_ghost_dot_update;

    GuiMissileTubeControls* missile_tube_controls;

    bool auto_rotate_on_my_ship;
    bool auto_distance = false;
    bool auto_center_on_my_ship;
    bool long_range;
    bool show_ghost_dots;
    bool show_waypoints;
    bool show_target_projection;
    bool show_missile_tubes;
    bool show_callsigns;
    bool show_heading_indicators;
    bool show_game_master_data;
    float range_indicator_step_size;
    uint8_t background_alpha;
    ERadarStyle style;
    EFogOfWarStyle fog_style;
    bpfunc_t mouse_down_func;
    pfunc_t mouse_drag_func;
    pfunc_t mouse_up_func;
    P<PlayerSpaceship> target_spaceship;

public:
    GuiRadarView(GuiContainer* owner, string id, TargetsContainer* targets, P<PlayerSpaceship> targetSpaceship);
    GuiRadarView(GuiContainer* owner, string id, float distance, TargetsContainer* targets, P<PlayerSpaceship> targetSpaceship);

    virtual void onDraw(sp::RenderTarget& target) override;

    virtual GuiRadarView* setDistance(float distance) { SectorsView::setDistance(distance); return this; }
    GuiRadarView* setRangeIndicatorStepSize(float step) { range_indicator_step_size = step; return this; }
    GuiRadarView* longRange() { long_range = true; return this; }
    GuiRadarView* shortRange() { long_range = false; return this; }
    GuiRadarView* enableGhostDots() { show_ghost_dots = true; return this; }
    GuiRadarView* disableGhostDots() { show_ghost_dots = false; return this; }
    GuiRadarView* enableWaypoints() { show_waypoints = true; return this; }
    GuiRadarView* disableWaypoints() { show_waypoints = false; return this; }
    GuiRadarView* enableTargetProjections(GuiMissileTubeControls* missile_tube_controls) { show_target_projection = true; this->missile_tube_controls = missile_tube_controls; return this; }
    GuiRadarView* disableTargetProjections() { show_target_projection = false; return this; }
    GuiRadarView* enableMissileTubeIndicators() { show_missile_tubes = true; return this; }
    GuiRadarView* disableMissileTubeIndicators() { show_missile_tubes = false; return this; }
    GuiRadarView* enableCallsigns() { show_callsigns = true; return this; }
    GuiRadarView* disableCallsigns() { show_callsigns = false; return this; }
    GuiRadarView* showCallsigns(bool value) { show_callsigns = value; return this; }
    bool getCallsigns() { return show_callsigns; }
    GuiRadarView* enableHeadingIndicators() { show_heading_indicators = true; return this; }
    GuiRadarView* disableHeadingIndicators() { show_heading_indicators = false; return this; }
    GuiRadarView* gameMaster() { show_game_master_data = true; return this; }
    GuiRadarView* setBackgroundAlpha(uint8_t background_alpha) { this->background_alpha = background_alpha; return this; }
    GuiRadarView* setStyle(ERadarStyle style) { this->style = style; return this; }
    GuiRadarView* setFogOfWarStyle(EFogOfWarStyle style) { this->fog_style = style; return this; }
    bool getAutoCentering() { return auto_center_on_my_ship; }
    GuiRadarView* setAutoCentering(bool value) { this->auto_center_on_my_ship = value; return this; }
    bool getAutoRotating() { return auto_rotate_on_my_ship; }
    GuiRadarView* setAutoRotating(bool value) { this->auto_rotate_on_my_ship = value; return this; }
   
    virtual GuiRadarView* setCallbacks(bpfunc_t mouse_down_func, pfunc_t mouse_drag_func, pfunc_t mouse_up_func) { SectorsView::setCallbacks(mouse_down_func, mouse_drag_func, mouse_up_func); return this; }
    virtual GuiRadarView* setViewPosition(glm::vec2 view_position) { SectorsView::setViewPosition(view_position); return this; }

    virtual bool onMouseDown(sp::io::Pointer::Button button, glm::vec2 position, sp::io::Pointer::ID id);
    void setTargetSpaceship(P<PlayerSpaceship> targetSpaceship){target_spaceship = targetSpaceship;}

private:
    void updateGhostDots();
    void drawBackground(sp::RenderTarget& window);
    void drawNebulaBlockedAreas(sp::RenderTarget& window);
    void drawNoneFriendlyBlockedAreas(sp::RenderTarget& window);
    void drawFriendlyNotVisibleAreas(sp::RenderTarget& window);
    void drawGhostDots(sp::RenderTarget& window);
    void drawWaypoints(sp::RenderTarget& window);
    void drawRangeIndicators(sp::RenderTarget& window);
    void drawTargetProjections(sp::RenderTarget& window);
    void drawMissileTubes(sp::RenderTarget& window);
    void drawObjects(sp::RenderTarget& window);
    void drawObjectsGM(sp::RenderTarget& window);
    void drawHeadingIndicators(sp::RenderTarget& window);
};

#endif//RADAR_VIEW_H
