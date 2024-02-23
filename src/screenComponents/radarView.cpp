#include <graphics/opengl.h>

#include "main.h"
#include "gameGlobalInfo.h"
#include "spaceObjects/nebula.h"
#include "spaceObjects/blackHole.h"
#include "spaceObjects/wormHole.h"
#include "spaceObjects/scanProbe.h"
#include "spaceObjects/planet.h"
#include "playerInfo.h"
#include "radarView.h"
#include "missileTubeControls.h"
#include "targetsContainer.h"

namespace
{
    enum class RadarStencil : uint8_t
    {
        None = 0,
        RadarBounds = 1 << 0,
        VisibleSpace = 1 << 1,
        InBoundsAndVisible = RadarBounds | VisibleSpace,
        All = InBoundsAndVisible
    };

    constexpr std::underlying_type_t<RadarStencil> as_mask(RadarStencil mask)
    {
        return static_cast<std::underlying_type_t<RadarStencil>>(mask);
    }
}


GuiRadarView::GuiRadarView(GuiContainer* owner, string id, TargetsContainer* targets, P<PlayerSpaceship> targetSpaceship)
: SectorsView(owner, id, 5000.0f, targets), 
    //auto_rotate_on_my_ship(false),
    auto_distance(true),
    //next_ghost_dot_update(0.0), 
    //targets(targets) //only for merge
    //missile_tube_controls(nullptr), 
    //auto_center_on_my_ship(true),
    //distance(5000.0f) //only for merge
    // long_range(false), 
    // show_ghost_dots(false),
    // show_waypoints(false), 
    // show_target_projection(false), 
    // show_missile_tubes(false), 
    // show_callsigns(false), 
    // show_heading_indicators(false), 
    // show_game_master_data(false), 
    // range_indicator_step_size(0.0f), 
    // background_alpha(255),
    // style(Circular), 
    // fog_style(NoFogOfWar), 
    // mouse_down_func(nullptr), 
    // mouse_drag_func(nullptr), 
    // mouse_up_func(nullptr),
    target_spaceship(targetSpaceship)
{
}

GuiRadarView::GuiRadarView(GuiContainer* owner, string id, float distance, TargetsContainer* targets, P<PlayerSpaceship> targetSpaceship)
: SectorsView(owner, id, distance, targets), 
    //next_ghost_dot_update(0.0), 
    //targets(targets), //only for merge
    //missile_tube_controls(nullptr), 
    //auto_center_on_my_ship(true),
    //auto_rotate_on_my_ship(false),
    //distance(distance), //only for merge
    // long_range(false), 
    // show_ghost_dots(false),
    // show_waypoints(false), 
    // show_target_projection(false), 
    // show_missile_tubes(false), 
    // show_callsigns(false), 
    // show_heading_indicators(false), 
    // show_game_master_data(false), 
    // range_indicator_step_size(0.0f), 
    // background_alpha(255),
    // style(Circular), 
    // fog_style(NoFogOfWar), 
    // mouse_down_func(nullptr), 
    // mouse_drag_func(nullptr), 
    // mouse_up_func(nullptr),
    target_spaceship(targetSpaceship)
{
}

void GuiRadarView::onDraw(sp::RenderTarget& renderer)
{
    //Hacky, when not relay and we have a ship, center on it.
    if (target_spaceship && auto_center_on_my_ship) {
        setViewPosition(target_spaceship->getPosition());
    }
    if (target_spaceship && auto_rotate_on_my_ship) {
        setViewRotation(target_spaceship->getRotation() + 90);
    }
    
    if (auto_distance)
    {
        setDistance(long_range ? 30000.0f : 5000.0f);
        if (target_spaceship)
        {
            if (long_range)
                setDistance(target_spaceship->getLongRangeRadarRange());
            else
                setDistance(target_spaceship->getShortRangeRadarRange());
        }
    }

    // Make sure all the drawing up till now is no longer queued and passed to the GPU.
    renderer.finish();

    //We must take some care to not overstep our bounds,
    // quite literally.
    // We use scissoring to define a 'box' in which all draw operations can happen.
    // This allows the side main screen to work correctly even when falling back in the non-render texture path.
    auto origin = renderer.virtualToPixelPosition(rect.position);
    auto extents = renderer.virtualToPixelPosition(rect.position + rect.size);

    glEnable(GL_SCISSOR_TEST);
    glScissor(origin.x, renderer.getPhysicalSize().y - extents.y, extents.x - origin.x, extents.y - origin.y);

    // Draw the initial background 'clear' color.
    if (style == Rectangular)
    {
        drawBackground(renderer);
    }
    
    if ((style == CircularMasked || style == Circular))
    {
        // Draw the radar's outline. First, and before any stencil kicks in.
        // this way, the outline is not even a part of the rendering area.
        float r = std::min(rect.size.x, rect.size.y) * 0.5f;
        renderer.drawCircleOutline(getCenterPoint(), r, 2.0f, colorConfig.radar_outline);
    }

    // Stencil setup.
    renderer.finish();
    glEnable(GL_STENCIL_TEST);
    glStencilMask(as_mask(RadarStencil::InBoundsAndVisible));
    
    // By default, nothing's visible.
    auto clear_mask = as_mask(RadarStencil::None);
    if (style == Rectangular)
    {
        // Rectangular shape, radar bounds is the entire texture target.
        clear_mask |= as_mask(RadarStencil::RadarBounds);

        // Without fog of war (ie GM), everything is deemed visible :)
        if (fog_style == NoFogOfWar)
            clear_mask |= as_mask(RadarStencil::VisibleSpace);
    }
    glClearStencil(clear_mask);
    glClear(GL_STENCIL_BUFFER_BIT);

    glDepthMask(GL_FALSE); // Nothing in this process writes in the depth.
    
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    if ((style == CircularMasked || style == Circular))
    {
        // When drawing the radar 'scope', mark the area as "in sight" and "visible".
        glStencilFunc(GL_ALWAYS, as_mask(RadarStencil::InBoundsAndVisible), 0);

        // Draws the radar circle shape.
        // Note that this draws both in the stencil and the color buffer!
        renderer.fillCircle(getCenterPoint(), std::min(rect.size.x, rect.size.y) / 2.0f - 2.0f, glm::u8vec4{ 20, 20, 20, background_alpha });
        renderer.finish();
    }

    if (fog_style == NebulaFogOfWar)
    {
        // Draw the *blocked* areas.
        // In this cas, we want to clear the 'visible' bit,
        // for all the stencil that has the radar one.
        glStencilFunc(GL_EQUAL, as_mask(RadarStencil::RadarBounds), as_mask(RadarStencil::RadarBounds));
        drawNebulaBlockedAreas(renderer);
    }
    else if (fog_style == FriendlysShortRangeFogOfWar || fog_style == FriendlysLongRangeFogOfWar)
    {
        // Draws the *visible* areas.
        // Add the visible states to anything that's in friendly sight (and still in bounds)
        glStencilFunc(GL_EQUAL, as_mask(RadarStencil::InBoundsAndVisible), as_mask(RadarStencil::RadarBounds));
        // LongRangeFogOfWar -> friendly short range is still used for friends
        drawNoneFriendlyBlockedAreas(renderer);
    }
    // Stencil is setup!
    renderer.finish();
    glStencilMask(as_mask(RadarStencil::None)); // disable writes.
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Back to defaults.

    // These always draw within the radar's confine.
    glStencilFunc(GL_EQUAL, as_mask(RadarStencil::RadarBounds), as_mask(RadarStencil::RadarBounds));
    drawSectorGrid(renderer);
    drawRangeIndicators(renderer);
    if (show_target_projection)
        drawTargetProjections(renderer);
    if (show_missile_tubes)
        drawMissileTubes(renderer);

    ///Start drawing of foreground
    // Foreground is radar confine + not blocked out.
    
    //Draw things that are masked out by fog-of-war
    if (show_ghost_dots)
    {
        updateGhostDots();
        drawGhostDots(renderer);
    }

    drawObjects(renderer);

    // Post masking
    renderer.finish();
    glStencilFunc(GL_EQUAL, as_mask(RadarStencil::RadarBounds), as_mask(RadarStencil::RadarBounds));
    if (show_game_master_data)
        drawObjectsGM(renderer);

    if (show_waypoints)
        drawWaypoints(renderer);
    if (show_heading_indicators)
        drawHeadingIndicators(renderer);
    drawTargets(renderer);

    if (style == Rectangular && target_spaceship)
    {
        auto ship_offset = (target_spaceship->getPosition() - getViewPosition()) / getDistance() * std::min(rect.size.x, rect.size.y) / 2.0f;
        if (ship_offset.x < -rect.size.x / 2.0f || ship_offset.x > rect.size.x / 2.0f || ship_offset.y < -rect.size.y / 2.0f || ship_offset.y > rect.size.y / 2.0f)
        {
            glm::vec2 position(rect.position.x + rect.size.x / 2.0f, rect.position.y + rect.size.y / 2.0f);
            position += ship_offset / glm::length(ship_offset) * std::min(rect.size.x, rect.size.y) * 0.4f;

            renderer.drawRotatedSprite("waypoint.png", position, 32, vec2ToAngle(ship_offset) - 90);
        }
    }
    // Done with the stencil.
    renderer.finish();
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);
}

void GuiRadarView::updateGhostDots()
{
    if (next_ghost_dot_update < engine->getElapsedTime())
    {
        next_ghost_dot_update = engine->getElapsedTime() + 5.0f;
        foreach(SpaceObject, obj, space_object_list)
        {
            P<SpaceShip> ship = obj;
            if (ship && glm::length(obj->getPosition() - getViewPosition()) < getDistance())
            {
                ghost_dots.push_back(GhostDot(obj->getPosition()));
            }
        }

        for(unsigned int n=0; n < ghost_dots.size(); n++)
        {
            if (ghost_dots[n].end_of_life <= engine->getElapsedTime())
            {
                ghost_dots.erase(ghost_dots.begin() + n);
                n--;
            }
        }
    }
}

void GuiRadarView::drawBackground(sp::RenderTarget& renderer)
{
    uint8_t tint = fog_style == NoFogOfWar ? 20 : 0;
    // When drawing a non-rectangular radar (ie circle),
    // we need full transparency on the outer edge.
    // We then use the stencil mask to allow the actual drawing.
    if (style == Rectangular)
        renderer.fillRect(rect, {tint, tint, tint, background_alpha});
    else
        renderer.fillRect(rect, {0, 0, 0, 0});
}

void GuiRadarView::drawNoneFriendlyBlockedAreas(sp::RenderTarget& renderer)
{
    if (my_spaceship)
    {
        float r_probe = my_spaceship->getProbeRangeRadarRange();
        r_probe *= getScale();
        float r_default = 5000.0f * getScale();

        foreach(SpaceObject, obj, space_object_list)
        {
            if(obj->id_galaxy != my_spaceship->id_galaxy)
                continue;
            
            P<ShipTemplateBasedObject> stb_obj = obj;
            
            if (obj == my_spaceship)
            {
                float r_stb = (fog_style == FriendlysLongRangeFogOfWar) ? stb_obj->getLongRangeRadarRange() : stb_obj->getShortRangeRadarRange();
                r_stb *= getScale();
                renderer.fillCircle(worldToScreen(obj->getPosition()), r_stb, glm::u8vec4{ 20, 20, 20, background_alpha });
            }
            if ((stb_obj && (obj->faction_id == my_spaceship->faction_id || obj->personality_id == 1)))
            {
                float r_stb = stb_obj->getShortRangeRadarRange();
                r_stb *= getScale();
                renderer.fillCircle(worldToScreen(obj->getPosition()), r_stb, glm::u8vec4{ 20, 20, 20, background_alpha });
            }
            else if (P<Planet>(obj)
                 && (obj->faction_id == my_spaceship->faction_id || obj->personality_id == 1))
            {
                renderer.fillCircle(worldToScreen(obj->getPosition()), r_default, glm::u8vec4{ 20, 20, 20, background_alpha });
            }

            P<ScanProbe> sp = obj;

            if (sp && sp->owner_id == my_spaceship->getMultiplayerId())
            {
                renderer.fillCircle(worldToScreen(obj->getPosition()), r_probe, glm::u8vec4{ 20, 20, 20, background_alpha });
            }
        }
    }
}

void GuiRadarView::drawNebulaBlockedAreas(sp::RenderTarget& renderer)
{
    if (!my_spaceship)
        return;
    auto scan_center = my_spaceship->getPosition();
    
    PVector<Nebula> nebulas = Nebula::getNebulas();
    foreach(Nebula, n, nebulas)
    {
        if (my_spaceship && n->id_galaxy != my_spaceship->id_galaxy)
            continue;

        auto diff = n->getPosition() - scan_center;
        float diff_len = glm::length(diff);

        if (diff_len < n->getRadius() + getDistance())
        {
            if (diff_len < n->getRadius())
            {
                // Inside a nebula - everything is blocked out.
                renderer.fillRect(rect, glm::u8vec4(0, 0, 0, 255));
                
                // Leave the loop here: there's no point adding more blocked areas.
                break;
            }else{
                float r = n->getRadius() * getScale();
                renderer.fillCircle(worldToScreen(n->getPosition()), r, glm::u8vec4(0, 0, 0, 255));

                float diff_angle = vec2ToAngle(diff);
                float angle = glm::degrees(acosf(n->getRadius() / diff_len));

                auto pos_a = n->getPosition() - vec2FromAngle(diff_angle + angle) * n->getRadius();
                auto pos_b = n->getPosition() - vec2FromAngle(diff_angle - angle) * n->getRadius();
                auto pos_c = scan_center + glm::normalize(pos_a - scan_center) * getDistance() * 3.0f;
                auto pos_d = scan_center + glm::normalize(pos_b - scan_center) * getDistance() * 3.0f;
                auto pos_e = scan_center + diff / diff_len * getDistance() * 3.0f;

                renderer.drawTriangleStrip({worldToScreen(pos_a), worldToScreen(pos_b), worldToScreen(pos_c), worldToScreen(pos_d), worldToScreen(pos_e)}, glm::u8vec4(0, 0, 0, 255));
            }
        }
    }

    // ship's short radar range is always visible.
    if (my_spaceship)
    {
        auto r = my_spaceship->getShortRangeRadarRange() * getScale();;
        
        renderer.fillCircle(worldToScreen(my_spaceship->getPosition()), r, glm::u8vec4{ 20, 20, 20, background_alpha });
    }
}

void GuiRadarView::drawGhostDots(sp::RenderTarget& renderer)
{
    for(unsigned int n=0; n<ghost_dots.size(); n++)
    {
        renderer.drawPoint(worldToScreen(ghost_dots[n].position), glm::u8vec4(255, 255, 255, 255 * std::max(((ghost_dots[n].end_of_life - engine->getElapsedTime()) / GhostDot::total_lifetime), 0.f)));
    }
}

void GuiRadarView::drawWaypoints(sp::RenderTarget& renderer)
{
    if (!my_spaceship)
        return;

    glm::vec2 radar_screen_center(rect.position.x + rect.size.x / 2.0f, rect.position.y + rect.size.y / 2.0f);

    for(unsigned int n=0; n<my_spaceship->waypoints.size(); n++)
    {
        auto screen_position = worldToScreen(my_spaceship->waypoints[n]);

        renderer.drawSprite("waypoint.png", screen_position - glm::vec2(0, 10), 20, colorConfig.ship_waypoint_background);
        renderer.drawText(sp::Rect(screen_position.x, screen_position.y - 10, 0, 0), string(n + 1), sp::Alignment::Center, 14, bold_font, colorConfig.ship_waypoint_text);

        if (style != Rectangular && glm::length(screen_position - radar_screen_center) > std::min(rect.size.x, rect.size.y) * 0.5f)
        {
            screen_position = radar_screen_center + ((screen_position - radar_screen_center) / glm::length(screen_position - radar_screen_center) * std::min(rect.size.x, rect.size.y) * 0.4f);

            renderer.drawRotatedSprite("waypoint.png", screen_position, 20, vec2ToAngle(screen_position - radar_screen_center) - 90, colorConfig.ship_waypoint_background);
            renderer.drawText(sp::Rect(screen_position.x, screen_position.y, 0, 0), string(n + 1), sp::Alignment::Center, 14, bold_font, colorConfig.ship_waypoint_text);
        }
    }
}

void GuiRadarView::drawRangeIndicators(sp::RenderTarget& renderer)
{
    if (range_indicator_step_size < 1.0f)
        return;

    glm::vec2 radar_screen_center(rect.position.x + rect.size.x / 2.0f, rect.position.y + rect.size.y / 2.0f);

    for(float circle_size=range_indicator_step_size; circle_size < getDistance(); circle_size+=range_indicator_step_size)
    {
        float s = circle_size * getScale();
        
        renderer.drawCircleOutline(radar_screen_center, s, 2.0, glm::u8vec4(255, 255, 255, 16));
        renderer.drawText(sp::Rect(radar_screen_center.x, radar_screen_center.y - s - 20, 0, 0), string(int(circle_size / 1000.0f + 0.1f)) + DISTANCE_UNIT_1K, sp::Alignment::Center, 20, bold_font, glm::u8vec4(255, 255, 255, 32));

    }
}

void GuiRadarView::drawTargetProjections(sp::RenderTarget& renderer)
{
    const float seconds_per_distance_tick = 5.0f;

    if (target_spaceship && missile_tube_controls)
    {
        for(int n=0; n<target_spaceship->weapon_tube_count; n++)
        {
            if (!target_spaceship->weapon_tube[n].isLoaded())
                continue;
            auto fire_position = target_spaceship->getPosition() + rotateVec2(target_spaceship->ship_template->model_data->getTubePosition2D(n), target_spaceship->getRotation());

            const MissileWeaponData& data = MissileWeaponData::getDataFor(target_spaceship->weapon_tube[n].getLoadType());
            float fire_angle = target_spaceship->weapon_tube[n].getDirection() + (target_spaceship->getRotation());
            float missile_target_angle = fire_angle;
            float speed = data.speed * target_spaceship->getSystemEffectiveness(SYS_MissileSystem);
            if (data.turnrate > 0.0f)
            {
                if (missile_tube_controls->getManualAim())
                {
                    missile_target_angle = missile_tube_controls->getMissileTargetAngle();
                }else{
                    float firing_solution = target_spaceship->weapon_tube[n].calculateFiringSolution(target_spaceship->getTarget());
                    if (firing_solution != std::numeric_limits<float>::infinity())
                        missile_target_angle = firing_solution;
                }
            }

            float angle_diff = angleDifference(missile_target_angle, fire_angle);
            float turn_radius = ((360.0f / data.turnrate) * speed) / (2.0f * float(M_PI));
            if (data.turnrate == 0.0f)
                turn_radius = 0.0f;

            float left_or_right = 90;
            if (angle_diff > 0)
                left_or_right = -90;

            auto turn_center = vec2FromAngle(fire_angle + left_or_right) * turn_radius;
            auto turn_exit = turn_center + vec2FromAngle(missile_target_angle - left_or_right) * turn_radius;

            float turn_distance = fabs(angle_diff) / 360.0f * (turn_radius * 2.0f * float(M_PI));
            float lifetime_after_turn = data.lifetime - turn_distance / speed;
            float length_after_turn = speed * lifetime_after_turn;

            std::vector<glm::vec2> missile_path;
            missile_path.push_back(worldToScreen(fire_position));
            for(int cnt=0; cnt<10; cnt++)
                missile_path.push_back(worldToScreen(fire_position + (turn_center + vec2FromAngle(fire_angle - angle_diff / 10.0f * cnt - left_or_right) * turn_radius)));
            missile_path.push_back(worldToScreen(fire_position + turn_exit));
            missile_path.push_back(worldToScreen(fire_position + (turn_exit + vec2FromAngle(missile_target_angle) * length_after_turn)));
            renderer.drawLine(missile_path, glm::u8vec4(255, 255, 255, 128));

            float offset = seconds_per_distance_tick * speed;
            for(int cnt=0; cnt<floor(data.lifetime / seconds_per_distance_tick); cnt++)
            {
                glm::vec2 p;
                glm::vec2 n{};
                if (offset < turn_distance)
                {
                    n = vec2FromAngle(fire_angle - (angle_diff * offset / turn_distance) - left_or_right);
                    p = worldToScreen(fire_position + (turn_center + n * turn_radius));
                }else{
                    p = worldToScreen(fire_position + (turn_exit + vec2FromAngle(missile_target_angle) * (offset - turn_distance)));
                    n = vec2FromAngle(missile_target_angle + 90.0f);
                }
                n = rotateVec2(n, -getViewRotation());
                n = glm::normalize(n);

                renderer.drawLine(p - glm::vec2(n.x, n.y) * 10.0f, p + glm::vec2(n.x, n.y) * 10.0f, glm::u8vec4{255,255,255,255});

                offset += seconds_per_distance_tick * speed;
            }
        }
    }

    if (getTargets())
    {
        for(P<SpaceObject> obj : getTargets()->getTargets())
        {
            if (glm::length2(obj->getVelocity()) < 1.0f)
                continue;

            auto start = worldToScreen(obj->getPosition());
            renderer.drawLine(start, worldToScreen(obj->getPosition() + obj->getVelocity() * 60.0f), glm::u8vec4(255, 255, 255, 128), glm::u8vec4(255, 255, 255, 0));
            glm::vec2 n = glm::normalize(rotateVec2(glm::vec2(-obj->getVelocity().y, obj->getVelocity().x), -getViewRotation())) * 10.0f;
            for(int cnt=0; cnt<5; cnt++)
            {
                auto p = rotateVec2(obj->getVelocity() * (seconds_per_distance_tick * (cnt + 1.0f) * getScale()), -getViewRotation());
                renderer.drawLine(start + p + n, start + p - n, glm::u8vec4(255, 255, 255, 128 - cnt * 20));
            }
        }
    }
}

void GuiRadarView::drawMissileTubes(sp::RenderTarget& renderer)
{
    if (target_spaceship)
    {
        //sf::VertexArray a(sf::Lines, target_spaceship->weapon_tube_count * 2);
        for(int n=0; n<target_spaceship->weapon_tube_count; n++)
        {
            auto fire_position = target_spaceship->getPosition() + rotateVec2(target_spaceship->ship_template->model_data->getTubePosition2D(n), target_spaceship->getRotation());
            auto fire_draw_position = worldToScreen(fire_position);

            float fire_angle = target_spaceship->getRotation() + target_spaceship->weapon_tube[n].getDirection() - getViewRotation();
            
            renderer.drawLine(fire_draw_position, fire_draw_position + (vec2FromAngle(fire_angle) * 1000.0f * getScale()), glm::u8vec4(128, 128, 128, 128), glm::u8vec4(128, 128, 128, 0));
        }
    }
}

void GuiRadarView::drawObjects(sp::RenderTarget& renderer)
{

    std::unordered_set<SpaceObject*> visible_objects;
    visible_objects.reserve(space_object_list.size());
    switch(fog_style)
    {
    case NoFogOfWar:
        foreach(SpaceObject, obj, space_object_list)
        {
            visible_objects.emplace(*obj);
        }
        break;
    
    case FriendlysLongRangeFogOfWar:
    case FriendlysShortRangeFogOfWar:
        // Reveal objects if they are within short-range radar range (or 5U) of
        // a friendly ship, station, or scan probe.

        // Continue only if the player's ship exists.
        if (!my_spaceship)
        {
            return;
        }

        // For each SpaceObject on the map...
        foreach(SpaceObject, obj, space_object_list)
        {
            // If the object can't hide in a nebula, it's considered visible.
            if (!obj->canHideInNebula())
            {
                visible_objects.emplace(*obj);
            }
            // IN DAID VERSION
            // Consider the object only if it is:
            // - Any ShipTemplateBasedObject (ship or station)
            // - A SpaceObject belonging to a friendly faction
            // - The player's ship
            // - A scan probe owned by the player's ship
            // This check is duplicated in RelayScreen::onDraw.
            // Here this is other thing :

            if (target_spaceship && (obj->faction_id != target_spaceship->faction_id) && (obj->personality_id != 1))
                continue;

            // Get the radar range, either ship template based, probe or ourself
            P<ShipTemplateBasedObject> stb_obj = obj;
            
            if (!stb_obj && !P<ScanProbe>(obj) && !P<WormHole>(obj) && !P<Planet>(obj))
                continue;

            if (target_spaceship && (obj->id_galaxy != target_spaceship->id_galaxy))
                continue;

            // Set the radius to reveal as getShortRangeRadarRange() if the
            // object's a ShipTemplateBasedObject. Otherwise, default to 5U.
            float radar_range = stb_obj ? stb_obj->getShortRangeRadarRange() : 5000.0f;

            P<ScanProbe> sp = obj;
            if (sp && sp->owner_id == my_spaceship->getMultiplayerId())
            {
                radar_range = my_spaceship->getProbeRangeRadarRange();
            } 
            else if(my_spaceship == obj) 
            {
                radar_range = (fog_style == FriendlysLongRangeFogOfWar) ? target_spaceship->getLongRangeRadarRange() : target_spaceship->getShortRangeRadarRange();
            }     

            // Query for objects within short-range radar/5U of this object.
            auto position = obj->getPosition();
            PVector<Collisionable> obj_list = CollisionManager::queryArea(position - glm::vec2(radar_range, radar_range), position + glm::vec2(radar_range, radar_range));
            foreach(Collisionable, c_obj, obj_list)
            {
                P<SpaceObject> obj2 = c_obj;
                if (obj2 && glm::length2(obj->getPosition() - obj2->getPosition()) < (radar_range + obj2->getRadius())*(radar_range + obj2->getRadius()))
                {
                    if (obj2->canHideInNebula() && Nebula::blockedByNebula(obj2->getPosition(), obj->getPosition(), my_spaceship->getShortRangeRadarRange()))
                        continue;
                    visible_objects.emplace(*obj2);
                }
            }
        }

        break;
    case NebulaFogOfWar:
        if (!target_spaceship)
            return;
            
        foreach(SpaceObject, obj, space_object_list)
        {
            if (target_spaceship && (obj->id_galaxy != target_spaceship->id_galaxy))
                continue;
            //Here : blockedByNebula can be extremely costly. This is a "Nebula fog of war and radar range" then.
            if (glm::length(target_spaceship->getPosition() - obj->getPosition()) > getDistance() + obj->getRadius())
                continue;

            if (obj->canHideInNebula() && target_spaceship && Nebula::blockedByNebula(target_spaceship->getPosition(), obj->getPosition(), my_spaceship->getShortRangeRadarRange()))
                continue;
            visible_objects.emplace(*obj);
        }
        break;
    case NoObjects:
        foreach(SpaceObject, obj, space_object_list)
        {
            if (P<Nebula>(obj) || P<BlackHole>(obj))
                visible_objects.insert(*obj);
        }
        break;
    }
    const float scale = getScale();
    //TODO verifier les perfos de ce truc
    std::vector objects_to_draw(std::begin(visible_objects), std::end(visible_objects));
    std::sort(std::begin(objects_to_draw), std::end(objects_to_draw), [](const auto& lhs, const auto& rhs)
    {
        const auto lhsLayer = lhs->getRadarLayer();
        const auto rhsLayer = rhs->getRadarLayer();
        if (lhsLayer < rhsLayer)
            return true;
        if (lhsLayer > rhsLayer)
            return false;
        if (lhs->canHideInNebula() && !rhs->canHideInNebula())
            return true;
        if (!lhs->canHideInNebula() && rhs->canHideInNebula())
            return false;
        return lhs->getMultiplayerId() < rhs->getMultiplayerId();
    });

    auto draw_object = [&renderer, this, scale](SpaceObject* obj)
    {
        if (target_spaceship && (obj->id_galaxy != target_spaceship->id_galaxy))
            return;

        auto object_position_on_screen = worldToScreen(obj->getPosition());
        float r = obj->getRadius() * getScale();
        sp::Rect object_rect(object_position_on_screen.x - r, object_position_on_screen.y - r, r * 2, r * 2);
        if (obj != *target_spaceship && rect.overlaps(object_rect))
        {
            if (obj->getTransparency() < 0.5f)
            {
                P<SpaceObject> spobj(obj);
                P<SpaceShip> this_spaceship(spobj);
                if(this_spaceship && (this_spaceship->docked_style != DockStyle::Internal))
                {
                    // Draw beam arcs on short-range radar only, and only for fully scanned
                    // ships.
                    if ((!long_range && (!my_spaceship || (this_spaceship->getScannedStateFor(my_spaceship) == SS_FullScan)))
                        || (my_spaceship && show_squadron_beam_arcs && my_spaceship->isInSquadron(this_spaceship))
                        )
                    {
                        obj->drawBeamArcsOnRadar(renderer, object_position_on_screen, getScale(), getViewRotation());
                    }
                }
                obj->drawOnRadar(renderer, object_position_on_screen, getScale(), getViewRotation(), long_range);
            }
            if (show_callsigns && obj->getCallSign() != "" && obj->getTransparency() < 0.2f && obj->getDockedStyle() != DockStyle::Internal)
                renderer.drawText(sp::Rect(object_position_on_screen.x, object_position_on_screen.y - 15, 0, 0), obj->getCallSign(), sp::Alignment::Center, 15, bold_font);
        }
    };

    glStencilFunc(GL_EQUAL, as_mask(RadarStencil::RadarBounds), as_mask(RadarStencil::RadarBounds));
    for(SpaceObject* obj : objects_to_draw)
    {
        draw_object(obj);
    }
    if (target_spaceship)
    {
        auto object_position_on_screen = worldToScreen(target_spaceship->getPosition());
        target_spaceship->drawOnRadar(renderer, object_position_on_screen, getScale(), getViewRotation(), long_range);
        if (long_range && show_callsigns && target_spaceship->getCallSign() != "")
            renderer.drawText(sp::Rect(object_position_on_screen.x, object_position_on_screen.y - 15, 0, 0), target_spaceship->getCallSign(), sp::Alignment::Center, 15, bold_font);
    }
}

void GuiRadarView::drawObjectsGM(sp::RenderTarget& renderer)
{
    foreach(SpaceObject, obj, space_object_list)
    {
        auto object_position_on_screen = worldToScreen(obj->getPosition());
        float r = obj->getRadius() * getScale();
        sp::Rect object_rect(object_position_on_screen.x - r, object_position_on_screen.y - r, r * 2, r * 2);
        if (rect.overlaps(object_rect))
        {
            if (obj->getTransparency() >= 0.5f)
                obj->drawOnRadar(renderer, object_position_on_screen, getScale(), getViewRotation(), long_range);
            obj->drawOnGMRadar(renderer, object_position_on_screen, getScale(), getViewRotation(), long_range);
        }
    }
}

void GuiRadarView::drawHeadingIndicators(sp::RenderTarget& renderer)
{
    float boundingRadius = std::min(rect.size.x, rect.size.y) / 2.0f; //NOT divided by distance here, attention. Do not use getScale() !

    auto radar_screen_center = rect.center();
    // If radar is 600-800px then tigs run every 20 degrees, small tigs every 5.
    // So if radar is 400-600x then the tigs should run every 45 degrees and smalls every 5.
    // If radar is <400px, tigs every 90, smalls every 10.
    unsigned int tig_interval = 20;
    unsigned int small_tig_interval = 5;

    if (boundingRadius >= 300.0f)
    {
        tig_interval = 20;
        small_tig_interval = 5;
    }
    else if (boundingRadius > 200.0f && boundingRadius <= 300.0f)
    {
        tig_interval = 45;
        small_tig_interval = 5;
    }
    else if (boundingRadius <= 200.0f)
    {
        tig_interval = 90;
        small_tig_interval = 10;
    }

    // Main radar tigs
    for(unsigned int n = 0; n < 360; n += tig_interval)
    {
        renderer.drawLine(
            radar_screen_center + vec2FromAngle(float(n) - 90 - getViewRotation()) * (boundingRadius - 20),
            radar_screen_center + vec2FromAngle(float(n) - 90 - getViewRotation()) * (boundingRadius - 40),
            {255, 255, 255, 255});
    }

    for(unsigned int n = 0; n < 360; n += small_tig_interval)
    {
        renderer.drawLine(
            radar_screen_center + vec2FromAngle(float(n) - 90 - getViewRotation()) * (boundingRadius - 20),
            radar_screen_center + vec2FromAngle(float(n) - 90 - getViewRotation()) * (boundingRadius - 30),
            {255, 255, 255, 255});
    }

    for(unsigned int n = 0; n < 360; n += tig_interval)
    {
        renderer.drawRotatedText(
            radar_screen_center + vec2FromAngle(float(n) - 90 - getViewRotation()) * (boundingRadius - 50), n-getViewRotation(),
            string(n), 15.0f, sp::RenderTarget::getDefaultFont(), {255, 255, 255, 255});
    }
}

bool GuiRadarView::onMouseDown(sp::io::Pointer::Button button, glm::vec2 position, sp::io::Pointer::ID id)
{
    if (style == Circular || style == CircularMasked)
    {
        float radius = std::min(rect.size.x, rect.size.y) / 2.0f;
        if (glm::length(position - getCenterPoint()) > radius)
            return false;
    }
    return SectorsView::onMouseDown(button, position, id);
}
