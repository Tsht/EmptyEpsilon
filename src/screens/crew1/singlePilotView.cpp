#include "singlePilotView.h"

#include "engine.h"
#include "preferenceManager.h"
#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"

#include "screenComponents/viewport3d.h"

#include "screenComponents/alertOverlay.h"
#include "screenComponents/combatManeuver.h"
#include "screenComponents/radarView.h"
#include "screenComponents/impulseControls.h"
#include "screenComponents/warpControls.h"
#include "screenComponents/jumpControls.h"
#include "screenComponents/dockingButton.h"
#include "screenComponents/landingButton.h"

#include "screenComponents/missileTubeControls.h"
#include "screenComponents/aimLock.h"
#include "screenComponents/shieldsEnableButton.h"
#include "screenComponents/beamFrequencySelector.h"
#include "screenComponents/beamTargetSelector.h"

#include "screenComponents/openCommsButton.h"
#include "screenComponents/commsOverlay.h"

#include "screenComponents/customShipFunctions.h"

#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_rotationdial.h"
#include "gui/gui2_image.h"

SinglePilotView::SinglePilotView(GuiContainer* owner, P<PlayerSpaceship> targetSpaceship)
: GuiElement(owner, "SINGLE_PILOT_VIEW"), target_spaceship(targetSpaceship)
{
    // Render the radar shadow and background decorations.
    (new GuiImage(this, "BACKGROUND_GRADIENT", "gui/background/gradientSingle.png"))->setPosition(glm::vec2(0, 0), sp::Alignment::Center)->setSize(1200, 900);

    // 5U tactical radar with piloting features.
    radar = new GuiRadarView(this, "TACTICAL_RADAR", &targets, (P<SpaceShip>)target_spaceship);
    radar->setPosition(0, 0, sp::Alignment::Center)->setSize(GuiElement::GuiSizeMatchHeight, 750);
    radar->setRangeIndicatorStepSize(1000.0)->shortRange()->enableGhostDots()->enableWaypoints()->enableCallsigns()->enableHeadingIndicators()->setStyle(GuiRadarView::Circular);
    radar->setCallbacks(
        [this](sp::io::Pointer::Button button, glm::vec2 position) {
            targets.setToClosestTo(position, 250, TargetsContainer::Targetable, target_spaceship);
            if (target_spaceship && targets.get())
                target_spaceship->commandSetTarget(targets.get());
            else if (target_spaceship)
                target_spaceship->commandTargetRotation(vec2ToAngle(position - target_spaceship->getPosition()));
        },
        [this](glm::vec2 position) {
            if (target_spaceship)
                target_spaceship->commandTargetRotation(vec2ToAngle(position - target_spaceship->getPosition()));
        },
        [this](glm::vec2 position) {
            if (target_spaceship)
                target_spaceship->commandTargetRotation(vec2ToAngle(position - target_spaceship->getPosition()));
        }
    );
    radar->setAutoRotating(PreferencesManager::get("single_pilot_radar_lock","0")=="1");
    
    // Ship stats and combat maneuver at bottom right corner of left panel.
    combat_maneuver = new GuiCombatManeuver(this, "COMBAT_MANEUVER", target_spaceship);
    combat_maneuver->setPosition(-20, -240, sp::Alignment::BottomRight)->setSize(200, 150)->setVisible(target_spaceship && target_spaceship->getCanCombatManeuver());

    auto stats = new GuiElement(this, "STATS");
    stats->setPosition(-20, -80, sp::Alignment::BottomRight)->setSize(240, 160)->setAttribute("layout", "vertical");;
    heat_display = new GuiKeyValueDisplay(stats, "HEAT_DISPLAY", 0.45, tr("Overheat"), "");
    heat_display->setIcon("gui/icons/status_overheat")->setTextSize(20)->setSize(240, 40);
    hull_display = new GuiKeyValueDisplay(stats, "HULL_DISPLAY", 0.45, tr("Hull"), "");
    hull_display->setIcon("gui/icons/hull")->setTextSize(20)->setSize(240, 40);
    energy_display = new GuiKeyValueDisplay(stats, "ENERGY_DISPLAY", 0.45, tr("Energy"), "");
    energy_display->setIcon("gui/icons/energy")->setTextSize(20)->setSize(240, 40);
    heading_display = new GuiKeyValueDisplay(stats, "HEADING_DISPLAY", 0.45, tr("Heading"), "");
    heading_display->setIcon("gui/icons/heading")->setTextSize(20)->setSize(240, 40);
    velocity_display = new GuiKeyValueDisplay(stats, "VELOCITY_DISPLAY", 0.45, tr("Speed"), "");
    velocity_display->setIcon("gui/icons/speed")->setTextSize(20)->setSize(240, 40);
    shields_display = new GuiKeyValueDisplay(stats, "SHIELDS_DISPLAY", 0.45, tr("Shields"), "");
    shields_display->setIcon("gui/icons/shields")->setTextSize(20)->setSize(240, 40);

    // Unlocked missile aim dial and lock controls.
    missile_aim = new AimLock(this, "MISSILE_AIM", radar, -90, 360 - 90, 0, [this](float value){
        tube_controls->setMissileTargetAngle(value);
    });
    missile_aim->setPosition(0, 0, sp::Alignment::Center)->setSize(GuiElement::GuiSizeMatchHeight, 700);

    // Weapon tube controls.
    tube_controls = new GuiMissileTubeControls(this, "MISSILE_TUBES", target_spaceship);
    tube_controls->setPosition(20, -20, sp::Alignment::BottomLeft);
    radar->enableTargetProjections(tube_controls);

     // Beam controls beneath the radar.
    if (gameGlobalInfo->use_beam_shield_frequencies || gameGlobalInfo->use_system_damage)
    {
        GuiElement* beam_info_box = new GuiElement(this, "BEAM_INFO_BOX");
        beam_info_box->setPosition(0, -20, sp::Alignment::BottomCenter)->setSize(500, 50);
        (new GuiLabel(beam_info_box, "BEAM_INFO_LABEL", tr("Beams"), 30))->addBackground()->setPosition(0, 0, sp::Alignment::BottomLeft)->setSize(80, 50);
        (new GuiBeamFrequencySelector(beam_info_box, "BEAM_FREQUENCY_SELECTOR"))->setPosition(80, 0, sp::Alignment::BottomLeft)->setSize(132, 50);
        (new GuiPowerDamageIndicator(beam_info_box, "", SYS_BeamWeapons, sp::Alignment::CenterLeft))->setPosition(0, 0, sp::Alignment::BottomLeft)->setSize(212, 50);
        (new GuiBeamTargetSelector(beam_info_box, "BEAM_TARGET_SELECTOR"))->setPosition(0, 0, sp::Alignment::BottomRight)->setSize(288, 50);
    }

    // Engine layout in top left corner of left panel.
    auto engine_layout = new GuiElement(this, "ENGINE_LAYOUT");
    engine_layout->setPosition(20, 120, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, 250)->setAttribute("layout", "horizontal");

    impulse_controls = new GuiImpulseControls(engine_layout, "IMPULSE", target_spaceship);
    impulse_controls->setSize(100, GuiElement::GuiSizeMax);
    warp_controls = new GuiWarpControls(engine_layout, "WARP", target_spaceship);
    warp_controls->setSize(100, GuiElement::GuiSizeMax);
    jump_controls = new GuiJumpControls(engine_layout, "JUMP", target_spaceship);
    jump_controls->setSize(100, GuiElement::GuiSizeMax);

    // Docking, comms, and shields buttons across top.
    docking_button = new GuiDockingButton(this, "DOCKING", target_spaceship);
    docking_button->setPosition(20, 20, sp::Alignment::TopLeft)->setSize(250, 50);
    landing_button = new GuiLandingButton(this, "LANDING", target_spaceship);
    landing_button->setPosition(20, 60, sp::Alignment::TopLeft)->setSize(250, 50);
    if (target_spaceship == my_spaceship)
    {
        (new GuiOpenCommsButton(this, "OPEN_COMMS_BUTTON", tr("Open Comms"), &targets))->setPosition(270, 20, sp::Alignment::TopLeft)->setSize(250, 50);
        (new GuiCommsOverlay(this))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    }
    shields_enable_button = new GuiShieldsEnableButton(this, "SHIELDS_ENABLE", target_spaceship);
    shields_enable_button->setPosition(-20, 80, sp::Alignment::TopRight)->setSize(250, 50);


    lock_fire = new GuiToggleButton(this, "TOOGLE_FIRE", tr("Fire"), [this](bool value){
        if (target_spaceship)
            target_spaceship->lock_fire = value;
    });
    lock_fire->setIcon("gui/icons/lock");
    //lock_fire->setPosition(-70, -10, sp::Alignment::BottomCenter)->setSize(200, 50);
    lock_fire->setPosition(-20, 150, sp::Alignment::TopRight)->setSize(150, 50);
    lock_fire->setValue(false);

    // Missile lock button near top right of left panel.
    lock_aim = new AimLockButton(this, "LOCK_AIM", tube_controls, missile_aim, target_spaceship);
    //lock_aim->setPosition(130, -10, sp::Alignment::BottomCenter)->setSize(200, 50);
    lock_aim->setPosition(-20, 200, sp::Alignment::TopRight)->setSize(150, 50);

    custom_ship_functions = new GuiCustomShipFunctions(this, singlePilot, "", target_spaceship);
    custom_ship_functions->setPosition(-20, 120, sp::Alignment::TopRight)->setSize(250, GuiElement::GuiSizeMax);
}

void SinglePilotView::setTargetSpaceship(P<PlayerSpaceship> targetSpaceship){
    target_spaceship = targetSpaceship;
    radar->setTargetSpaceship((P<SpaceShip>)target_spaceship);
    combat_maneuver->setTargetSpaceship(target_spaceship);
    tube_controls->setTargetSpaceship(target_spaceship);
    impulse_controls->setTargetSpaceship(target_spaceship);
    warp_controls->setTargetSpaceship(target_spaceship);
    jump_controls->setTargetSpaceship(target_spaceship);
    docking_button->setTargetSpaceship(target_spaceship);
    landing_button->setTargetSpaceship(target_spaceship);
    shields_enable_button->setTargetSpaceship(target_spaceship);
    lock_aim->setTargetSpaceship(target_spaceship);
    custom_ship_functions->setTargetSpaceship(target_spaceship);
}

void SinglePilotView::onDraw(sp::RenderTarget& renderer)
{
    if (target_spaceship)
    {
        float totalHeat = 0;
        for(unsigned int n=0; n<SYS_COUNT; n++)
            totalHeat += target_spaceship->getSystemHeat(ESystem(n));
        heat_display->setValue(string(totalHeat, 2));
        hull_display->setValue(string(int(100 * target_spaceship->hull_strength / target_spaceship->hull_max)) + "%");
        energy_display->setValue(string(int(target_spaceship->energy_level)));
        heading_display->setValue(string(target_spaceship->getHeading(), 1));
        float velocity = glm::length(target_spaceship->getVelocity()) / 1000 * 60;
        velocity_display->setValue(tr("{value} {unit}/min").format({{"value", string(velocity, 1)}, {"unit", DISTANCE_UNIT_1K}}));

        lock_aim->setVisible(target_spaceship->getWeaponTubeCount() > 0);

        warp_controls->setVisible(target_spaceship->has_warp_drive);
        jump_controls->setVisible(target_spaceship->has_jump_drive);

//        shields_display->setValue(string(target_spaceship->getShieldPercentage(0)) + "% " + string(target_spaceship->getShieldPercentage(1)) + "%");
        if (target_spaceship->getShieldCount() > 0)
        {
            shields_display->show();
            string shields_info = "";
            for(int n=0; n<target_spaceship->getShieldCount(); n++)
                shields_info += string(target_spaceship->getShieldPercentage(n)) + "% ";
            shields_display->setValue(shields_info);
        }
        else
            shields_display->hide();

        missile_aim->setVisible(tube_controls->getManualAim());

        targets.set(target_spaceship->getTarget());
    }
    GuiElement::onDraw(renderer);
}

void SinglePilotView::onUpdate()
{
    if (isVisible()){
        if (target_spaceship && isVisible())
        {
            auto angle = (keys.helms_turn_right.getValue() - keys.helms_turn_left.getValue()) * 5.0f;
            if (angle != 0.0f)
            {
                my_spaceship->commandTargetRotation(my_spaceship->getRotation() + angle);
            }
            if (keys.weapons_enemy_next_target.getDown())
            {
                bool current_found = false;
                foreach(SpaceObject, obj, space_object_list)
                {
                    if (obj == my_spaceship)
                        continue;
                    if (obj == targets.get())
                    {
                        current_found = true;
                        continue;
                    }
                    if (current_found && glm::length(obj->getPosition() - target_spaceship->getPosition()) < target_spaceship->getShortRangeRadarRange() && target_spaceship->isEnemy(obj) && target_spaceship->getScannedStateFor(obj) >= SS_FriendOrFoeIdentified && obj->canBeTargetedBy(target_spaceship))
                    {
                        targets.set(obj);
                        target_spaceship->commandSetTarget(targets.get());
                        return;
                    }
                }
                foreach(SpaceObject, obj, space_object_list)
                {
                    if (obj == targets.get())
                    {
                        continue;
                    }
                    if (target_spaceship->isEnemy(obj) && glm::length(obj->getPosition() - target_spaceship->getPosition()) < target_spaceship->getShortRangeRadarRange() && target_spaceship->getScannedStateFor(obj) >= SS_FriendOrFoeIdentified && obj->canBeTargetedBy(target_spaceship))
                    {
                        targets.set(obj);
                        target_spaceship->commandSetTarget(targets.get());
                        return;
                    }
                }
            }
            if (keys.weapons_next_target.getDown())
            {
                bool current_found = false;
                foreach(SpaceObject, obj, space_object_list)
                {
                    if (obj == targets.get())
                    {
                        current_found = true;
                        continue;
                    }
                    if (obj == target_spaceship)
                        continue;
                    if (current_found && glm::length(obj->getPosition() - target_spaceship->getPosition()) < target_spaceship->getShortRangeRadarRange() && obj->canBeTargetedBy(target_spaceship))
                    {
                        targets.set(obj);
                        target_spaceship->commandSetTarget(targets.get());
                        return;
                    }
                }
                foreach(SpaceObject, obj, space_object_list)
                {
                    if (obj == targets.get() || obj == target_spaceship)
                        continue;
                    if (glm::length(obj->getPosition() - target_spaceship->getPosition()) < target_spaceship->getShortRangeRadarRange() && obj->canBeTargetedBy(target_spaceship))
                    {
                        targets.set(obj);
                        target_spaceship->commandSetTarget(targets.get());
                        return;
                    }
                }
            }
            auto aim_adjust = keys.weapons_aim_left.getValue() - keys.weapons_aim_right.getValue();
            if (aim_adjust != 0.0f)
            {
                missile_aim->setValue(missile_aim->getValue() - 5.0f * aim_adjust);
                tube_controls->setMissileTargetAngle(missile_aim->getValue());
            }
        }
    }
}
   
