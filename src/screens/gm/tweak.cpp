#include <i18n.h>
#include "tweak.h"
#include "playerInfo.h"
#include "spaceObjects/spaceship.h"
#include "spaceObjects/spaceObject.h"
#include "shipCargo.h"
#include "gameGlobalInfo.h"

#include "gui/gui2_listbox.h"
#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_label.h"
#include "gui/gui2_textentry.h"
#include "gui/gui2_selector.h"
#include "gui/gui2_slider.h"
#include "gui/gui2_togglebutton.h"

GuiObjectTweak::GuiObjectTweak(GuiContainer* owner, ETweakType tweak_type)
: GuiPanel(owner, "GM_TWEAK_DIALOG")
{
    //setPosition(0, 0, sp::Alignment::Center);
    //setSize(1000, 800);
    setPosition(0, -100, sp::Alignment::BottomCenter);
    setSize(1000, 700);

    GuiListbox* list = new GuiListbox(this, "", [this](int index, string value)
    {
        for(GuiTweakPage* page : pages)
            page->hide();
        pages[index]->show();
    });

    list->setSize(300, GuiElement::GuiSizeMax);
    list->setPosition(25, 25, sp::Alignment::TopLeft);

    pages.push_back(new GuiObjectTweakBase(this));
    list->addEntry(tr("tab", "Base"), "");
    
    pages.push_back(new GuiShipTweakInfos(this));
    list->addEntry("Infos", "");

    if (tweak_type == TW_Template || tweak_type == TW_Ship || tweak_type == TW_Player || tweak_type == TW_Station)
    {
        pages.push_back(new GuiTemplateTweak(this));
        list->addEntry(tr("tab","Template-based"), "");
    }

    if (tweak_type == TW_Asteroid)
    {
        pages.push_back(new GuiAsteroidTweak(this));
        list->addEntry(tr("tab", "Asteroid"), "");
    }

    if (tweak_type == TW_Jammer)
    {
        pages.push_back(new GuiJammerTweak(this));
        list->addEntry(tr("tab", "Jammer"), "");
    }

    if (tweak_type == TW_Ship || tweak_type == TW_Player || tweak_type == TW_Station)
    {
        pages.push_back(new GuiShipTweakShields(this));
        list->addEntry(tr("tab", "Shields"), "");
    }

    if (tweak_type == TW_Ship || tweak_type == TW_Player)
    {
        pages.push_back(new GuiShipTweak(this));
        list->addEntry(tr("tab", "Ship"), "");
        pages.push_back(new GuiShipTweakMissileTubes(this));
        list->addEntry(tr("tab", "Tubes"), "");
        pages.push_back(new GuiShipTweakMissileWeapons(this));
        list->addEntry(tr("tab", "Missiles"), "");
        pages.push_back(new GuiShipTweakBeamweapons(this));
        list->addEntry(tr("tab", "Beams"), "");
        pages.push_back(new GuiShipTweakSystems(this));
        list->addEntry(tr("tab", "Systems"), "");
        pages.push_back(new GuiShipTweakSystemPowerFactors(this));
        list->addEntry(tr("tab", "Power"), "");
        pages.push_back(new GuiShipTweakSystemRates(this, GuiShipTweakSystemRates::Type::Coolant));
        list->addEntry(tr("tab", "Coolant Rate"), "");
        pages.push_back(new GuiShipTweakSystemRates(this, GuiShipTweakSystemRates::Type::Heat));
        list->addEntry(tr("tab", "Heat Rate"), "");
        pages.push_back(new GuiShipTweakSystemRates(this, GuiShipTweakSystemRates::Type::Power));
        list->addEntry(tr("tab", "Power Rate"), "");
    }

    if (tweak_type == TW_Player)
    {
        pages.push_back(new GuiShipTweakPlayer(this));
        list->addEntry(tr("tab", "Player"), "");

        pages.push_back(new GuiShipTweakCrew(this));
        list->addEntry(tr("tab", "Crew"), "");

        // pages.push_back(new GuiShipTweakOxygen(this));
        // list->addEntry("Oxygene", "");
        //Maybe later, this would be available for NPC ships too
        pages.push_back(new GuiShipTweakDock(this));
        list->addEntry(tr("tab","Dock"), "");

        pages.push_back(new GuiShipTweakMessages(this));
        list->addEntry(tr("tab","Messages"), "");

        pages.push_back(new GuiModifierTweak(this));
        list->addEntry(tr("tab", "Modifiers"), "");

        pages.push_back(new GuiBlueprintTweak(this));
        list->addEntry(tr("tab", "Blueprints"), "");
    }

    if (tweak_type == TW_Planet)
    {
        pages.push_back(new GuiShipTweakPlanet(this));
        list->addEntry(tr("tab","Planet"), "");
    }

    for(GuiTweakPage* page : pages)
    {
        //page->setSize(700, 800)->setPosition(0, 0, sp::Alignment::BottomRight)->hide();
        page->setSize(700, 700)->setPosition(0, 0, sp::Alignment::BottomRight)->hide();
    }

    pages[0]->show();
    list->setSelectionIndex(0);

    (new GuiButton(this, "CLOSE_BUTTON", tr("button", "Close"), [this]() {
        hide();
    }))->setTextSize(20)->setPosition(-10, 0, sp::Alignment::TopRight)->setSize(70, 30);
}

void GuiObjectTweak::open(P<SpaceObject> target)
{
    this->target = target;

    for(GuiTweakPage* page : pages)
        page->open(target);

    show();
}

void GuiObjectTweak::onDraw(sp::RenderTarget& renderer)
{
    GuiPanel::onDraw(renderer);

    if (!target)
        hide();
}

GuiObjectTweakBase::GuiObjectTweakBase(GuiContainer* owner)
: GuiTweakPage(owner)
{
    GuiElement* left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    GuiElement* right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    // Left column
    (new GuiLabel(left_col, "", tr("Callsign:"), 30))->setSize(GuiElement::GuiSizeMax, 50);

    callsign = new GuiTextEntry(left_col, "", "");
    callsign->setSize(GuiElement::GuiSizeMax, 50);
    callsign->callback([this](string text) {
        if(target)
            target->callsign = text;
    });

    // Edit object's description.
    // TODO: Fix long strings in GuiTextEntry, or make a new GUI element for
    // editing long strings.
    (new GuiLabel(left_col, "", tr("Unscanned description:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    description = new GuiTextEntry(left_col, "", "");
    description->setSize(GuiElement::GuiSizeMax, 50);
    description->callback([this](string text) {
        if(target)
            target->setDescriptionForScanState(SS_NotScanned, text);
        //target->setDescription(text);
    });
    (new GuiLabel(left_col, "", tr("Friend or Foe Description:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    description_fof = new GuiTextEntry(left_col, "", "");
    description_fof->setSize(GuiElement::GuiSizeMax, 50);
    description_fof->callback([this](string text) {
        if(target)
            target->setDescriptionForScanState(SS_FriendOrFoeIdentified,text);
    });
    (new GuiLabel(left_col, "", tr("Simple Scan Description:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    description_scan = new GuiTextEntry(left_col, "", "");
    description_scan->setSize(GuiElement::GuiSizeMax, 50);
    description_scan->callback([this](string text) {
        if(target)
            target->setDescriptionForScanState(SS_SimpleScan, text);
    });
    (new GuiLabel(left_col, "", tr("Full Scan Description:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    description_full_scan = new GuiTextEntry(left_col, "", "");
    description_full_scan->setSize(GuiElement::GuiSizeMax, 50);
    description_full_scan->callback([this](string text) {
        if(target)
            target->setDescriptionForScanState(SS_FullScan, text);
    });

    (new GuiLabel(left_col, "", tr("Hull current:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    hull_slider = new GuiSlider(left_col, "", 0.0, 500, 0.0, [this](float value) {
        if(target)
            target->hull = value;
    });
    hull_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(left_col, "", tr("Z-Axis:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    zaxis_slider = new GuiSlider(left_col, "", -100.0, 100, 0.0, [this](float value) {
        if(target)
            target->translate_z = value;
    });
    zaxis_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    // Right column
	// Radar signature
	//(new GuiLabel(right_col, "", "Radar Signature", 30))->setSize(GuiElement::GuiSizeMax, 50);
	(new GuiLabel(right_col, "", tr("Gravitational Signature:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
	gravity_s_slider = new GuiSlider(right_col, "", -100.0, 100.0, 0.0, [this](float value) {
        if(target)
            target->radar_signature.gravity = value / 100.0f;
    });
    gravity_s_slider->addSnapValue(0.0f, 5.0f);
    gravity_s_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 50);
 	(new GuiLabel(right_col, "", tr("Energy Signature:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
	electrical_s_slider = new GuiSlider(right_col, "", -100.0, 100.0, 0.0, [this](float value) {
        if(target)
            target->radar_signature.electrical = value / 100.0f;
    });
    electrical_s_slider->addSnapValue(0.0f, 5.0f);
    electrical_s_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 50);
 	(new GuiLabel(right_col, "", tr("Biological Signature:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
	biological_s_slider = new GuiSlider(right_col, "", -100.0, 100.0, 0.0, [this](float value) {
        if(target)
            target->radar_signature.biological = value / 100.0f;
    });
    biological_s_slider->addSnapValue(0.0f, 5.0f);
    biological_s_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 50);

    // Scanning complexity
	(new GuiLabel(right_col, "", tr("Scanning Complexity:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    scanning_complexity_selector = new GuiSlider(right_col, "", 0, 4, 0, [this](int value) {
        if(target)
            target->setScanningParameters(value,target->scanning_depth_value);
//        target->scanning_complexity_value = value;
    });
    scanning_complexity_selector->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
 	// Scanning Channel Depth
	(new GuiLabel(right_col, "", tr("Scanning Depth:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    scanning_channel_depth_selector = new GuiSlider(right_col, "", 0, 4, 0, [this](int value) {
        if(target)
            target->setScanningParameters(target->scanning_complexity_value,value);
//        target->scanning_depth_value = value;
    });
    scanning_channel_depth_selector->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("Size:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    radius_slider = new GuiSlider(right_col, "", 0, 50000, 0.0, [this](float value) {
        if(target)
            target->setRadius(value);
    });
    radius_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

}

void GuiObjectTweakBase::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    hull_slider->setValue(target->hull);

    scanning_complexity_selector->setValue(target->scanning_complexity_value);
	scanning_channel_depth_selector->setValue(target->scanning_depth_value);

    callsign->setText(target->callsign);
    description->setText(target->object_description.not_scanned);
    description_scan->setText(target->object_description.simple_scan);
    description_full_scan->setText(target->object_description.full_scan);

    gravity_s_slider->setValue(target->radar_signature.gravity * 100.0f);
    electrical_s_slider->setValue(target->getRadarSignatureElectrical() * 100.0f);
	biological_s_slider->setValue(target->getRadarSignatureBiological() * 100.0f);

	scanning_complexity_selector->setValue(target->scanning_complexity_value);
	scanning_channel_depth_selector->setValue(target->scanning_depth_value);

	zaxis_slider->setValue(target->translate_z);
	radius_slider->setValue(target->getRadius());
	radius_slider->setRange(target->getRadius()/10,target->getRadius()*10);
}

void GuiObjectTweakBase::open(P<SpaceObject> target)
{
    this->target = target;
}

GuiTemplateTweak::GuiTemplateTweak(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;
    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    (new GuiLabel(left_col, "", tr("Heading:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    heading_slider = new GuiSlider(left_col, "", 0.0, 359.9, 0.0, [this](float value) {
        if(target)
            target->setHeading(value);

        // If the target is a player, also set its target rotation.
        P<PlayerSpaceship> player = target;
        if (player)
            player->commandTargetRotation(value - 90.0f);
    });
    heading_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(left_col, "", tr("Rotation:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    rotation_slider = new GuiSlider(left_col, "", -100.0, 100.0, 0.0, [this](float value) {
        if(target)
            target->setRotationSpeed(value/10.f);
    });
    rotation_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    // Transparency slider
    (new GuiLabel(left_col, "", tr("Transparency:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    transparency_slider = new GuiSlider(left_col, "", 0.0, 100.0, 0.0, [this](float value) {
       if(target)
        target->setTransparency(value / 100.f);
    });
    transparency_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(left_col, "", tr("Short range radar:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    short_range_radar_slider = new GuiSlider(left_col, "", 100.0, 20000.0, 0.0, [this](float value) {
        P<PlayerSpaceship> player = target;
        if (player)
        {
            player->setShortRangeRadarRange(value);
        }
    });
    short_range_radar_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(left_col, "", tr("Long range radar:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    long_range_radar_slider = new GuiSlider(left_col, "", 100.0, 100000.0, 0.0, [this](float value) {
        P<PlayerSpaceship> player = target;
        if (player)
        {
            player->setLongRangeRadarRange(value);
        }
    });
    long_range_radar_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    // Right column
    // Set type name. Does not change ship type.
    (new GuiLabel(right_col, "", tr("Type name:"), 30))->setSize(GuiElement::GuiSizeMax, 50);

    type_name = new GuiTextEntry(right_col, "", "");
    type_name->setSize(GuiElement::GuiSizeMax, 50);
    type_name->callback([this](string text) {
        if(target)
            target->setTypeName(text);
    });

    // Right column
    // Hull max and state sliders
    (new GuiLabel(right_col, "", tr("Hull max:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    hull_max_slider = new GuiSlider(right_col, "", 0.0, 4000, 0.0, [this](float value) {
        if(target)
        {
            target->hull_max = round(value);
            target->hull_strength = std::min(target->hull_strength, target->hull_max);
        }
    });
    hull_max_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("Hull current:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    hull_slider = new GuiSlider(right_col, "", 0.0, 4000, 0.0, [this](float value) {
        if(target)
            target->hull_strength = std::min(roundf(value), target->hull_max);
    });
    hull_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

   // Can be destroyed bool
   can_be_destroyed_toggle = new GuiToggleButton(right_col, "", tr("Could be destroyed"), [this](bool value) {
       if(target)
        target->setCanBeDestroyed(value);
   });
   can_be_destroyed_toggle->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("System damage ratio :"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    system_damage_ratio_slider = new GuiSlider(right_col, "", 0.0, 100.0, 0.0, [this](float value) {
        if(target)
            target->system_damage_ratio = value / 100.0f;
    });
    system_damage_ratio_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("Min% hull before system damage :"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    system_damage_hull_threshold_slider = new GuiSlider(right_col, "", 0.0, 100.0, 0.0, [this](float value) {
        if(target)
            target->system_damage_hull_threshold = value / 100.0f;
    });
    system_damage_hull_threshold_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
}
 void GuiTemplateTweak::onDraw(sp::RenderTarget& renderer)
{
    if(target)
    {
        heading_slider->setValue(target->getHeading());
        rotation_slider->setValue(target->getRotationSpeed()*10.f);
        hull_slider->setValue(target->hull_strength);
        transparency_slider->setValue(target->getTransparency() * 100.f);
        system_damage_ratio_slider->setValue(target->system_damage_ratio * 100.f);
        system_damage_hull_threshold_slider->setValue(target->system_damage_hull_threshold * 100.f);

        short_range_radar_slider->setValue(target->getShortRangeRadarRange());
        long_range_radar_slider->setValue(target->getLongRangeRadarRange());

    }
}

 void GuiTemplateTweak::open(P<SpaceObject> target)
{
    P<ShipTemplateBasedObject> shipTemplate = target;
    this->target = shipTemplate;

    type_name->setText(shipTemplate->getTypeName());
    hull_max_slider->setValue(shipTemplate->hull_max);
    hull_max_slider->clearSnapValues()->addSnapValue(shipTemplate->ship_template->hull, 5.0f);
    can_be_destroyed_toggle->setValue(shipTemplate->getCanBeDestroyed());
}

GuiShipTweakShields::GuiShipTweakShields(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    for(int n=0; n<max_shield_count; n++)
    {
        (new GuiLabel(left_col, "", tr("Shield {id_shield} max:").format({{"id_shield", string(n + 1)}}), 20))->setSize(GuiElement::GuiSizeMax, 30);
        shield_max_slider[n] = new GuiSlider(left_col, "", 0.0, 2000, 0.0, [this, n](float value) {
            if(target)
            {
                target->shield_max[n] = roundf(value);
                target->shield_level[n] = std::min(target->shield_level[n], target->shield_max[n]);

                int actual_shield_count = 0;
                for(int k=0; k<max_shield_count; k++)
                    if (target->shield_max[k] > 0)
                        actual_shield_count = k+1;
                target->setShieldCount(actual_shield_count);
            }
        });
        shield_max_slider[n]->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
    }

    for(int n=0; n<max_shield_count; n++)
    {
        (new GuiLabel(right_col, "", tr("Shield {id_shield}:").format({{"id_shield", string(n + 1)}}), 20))->setSize(GuiElement::GuiSizeMax, 30);
        shield_slider[n] = new GuiSlider(right_col, "", 0.0, 2000, 0.0, [this, n](float value) {
            if(target)
                target->shield_level[n] = std::min(roundf(value), target->shield_max[n]);
        });
        shield_slider[n]->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
    }

    (new GuiLabel(left_col, "", tr("Shields recharge rate :"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    shield_recharge_slider = new GuiSlider(left_col, "", 0.0, 200, 30, [this](float value) {
        if(target)
            target->shield_recharge_rate = value/100.0f;
    });
    shield_recharge_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
}

void GuiShipTweakShields::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    for(int n=0; n<max_shield_count; n++)
    {
        shield_max_slider[n]->setValue(target->shield_max[n]);
        shield_slider[n]->setValue(target->shield_level[n]);
    }
    shield_recharge_slider->setValue(target->shield_recharge_rate * 100);
}

void GuiShipTweakShields::open(P<SpaceObject> target)
{
    P<ShipTemplateBasedObject> ship = target;
    this->target = ship;

    if (ship)
    {
        for(int n = 0; n < max_shield_count; n++)
        {
            shield_max_slider[n]->clearSnapValues()->addSnapValue(ship->ship_template->shield_level[n], 5.0f);
        }
        shield_recharge_slider->setValue(ship->shield_recharge_rate * 100);
    }

}

GuiShipTweak::GuiShipTweak(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;
    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    // Left column
    (new GuiLabel(left_col, "",  tr("Impulse speed:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    impulse_speed_slider = new GuiSlider(left_col, "", 0.0, 250, 0.0, [this](float value) {
        if(target)
            target->impulse_max_speed = value;
    });
    impulse_speed_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(left_col, "", tr("Impulse acceleration:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    impulse_acceleration_slider = new GuiSlider(left_col, "", 0.0, 250, 0.0, [this](float value) {
        target->setAcceleration(value, target->getAcceleration().reverse);
    });
    impulse_acceleration_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(left_col, "", tr("Impulse reverse speed:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    impulse_reverse_speed_slider = new GuiSlider(left_col, "", 0.0, 250, 0.0, [this](float value) {
        if(target)
            target->impulse_max_reverse_speed = value;
    });
    impulse_reverse_speed_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

     (new GuiLabel(left_col, "", tr("Impulse reverse acceleration:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    impulse_reverse_acceleration_slider = new GuiSlider(left_col, "", 0.0, 250, 0.0, [this](float value) {
        target->setAcceleration(target->getAcceleration().forward, value);
    });
    impulse_reverse_acceleration_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(left_col, "", tr("Turn speed:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    turn_speed_slider = new GuiSlider(left_col, "", 0.0, 35, 0.0, [this](float value) {
        if(target)
            target->turn_speed = value;
    });
    turn_speed_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    // Display Boost/Strafe speed sliders
    (new GuiLabel(left_col, "", tr("Boost Speed:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    combat_maneuver_boost_speed_slider = new GuiSlider(left_col, "", 0.0, 1000, 0.0, [this](float value) {
        if(target)
            target->combat_maneuver_boost_speed = value;
    });
    combat_maneuver_boost_speed_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(left_col, "", tr("Strafe Speed:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    combat_maneuver_strafe_speed_slider = new GuiSlider(left_col, "", 0.0, 1000, 0.0, [this](float value) {
        if(target)
            target->combat_maneuver_strafe_speed = value;
    });
    combat_maneuver_strafe_speed_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    // Warp and jump drive toggles
    (new GuiLabel(left_col, "", tr("Special Engines:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    warp_toggle = new GuiToggleButton(left_col, "", tr("Warp Drive"), [this](bool value) {
        if(target)
            target->setWarpDrive(value);
    });
    warp_toggle->setSize(GuiElement::GuiSizeMax, 35);

    jump_toggle = new GuiToggleButton(right_col, "", tr("Jump Drive"), [this](bool value) {
        if(target)
            target->setJumpDrive(value);
    });
    jump_toggle->setSize(GuiElement::GuiSizeMax, 35);

    reactor_toggle = new GuiToggleButton(left_col, "", tr("Reactor"), [this](bool value) {
        if(target)
            target->setReactor(value);
    });
    reactor_toggle->setSize(GuiElement::GuiSizeMax, 35);

    cloaking_toggle = new GuiToggleButton(left_col, "", tr("Cloaking"), [this](bool value) {
        if(target)
            target->setCloaking(value);
    });
    cloaking_toggle->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(left_col, "", tr("Engineering presets:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    engineering_presets_slider = new GuiSlider(left_col, "", 0, 9, 0, [this](int value) {
        P<PlayerSpaceship> player = target;
        if(player)
            player->active_engineer_presets_number = value;
    });
    engineering_presets_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(right_col, "", tr("Min jump dist:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    jump_drive_min_distance_slider = new GuiSlider(right_col, "", 0.0, 50000, 0.0, [this](float value) {
        if(target)
        {
            target->jump_drive_min_distance = round(value / 1000) * 1000000;
            target->jump_drive_max_distance = std::max(target->jump_drive_min_distance, target->jump_drive_max_distance);
        }
    });
    jump_drive_min_distance_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(right_col, "", tr("Max jump dist:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    jump_drive_max_distance_slider = new GuiSlider(right_col, "", 0.0, 100000, 0.0, [this](float value) {
        if(target)
        {
            target->jump_drive_max_distance = round(value / 1000) * 1000000;
            target->jump_drive_min_distance = std::min(target->jump_drive_min_distance, target->jump_drive_max_distance);
        }
   });
    jump_drive_max_distance_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(right_col, "", tr("Jump charge:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    jump_drive_charge_slider = new GuiSlider(right_col, "", 0.0, 100.0, 0.0, [this](float value) {
        if(target)
            target->jump_drive_charge = value / 100.f * target->jump_drive_max_distance;
    });
    jump_drive_charge_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(right_col, "", tr("Jump charge complete:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    jump_drive_charge_time_slider = new GuiSlider(right_col, "", 0.0, 90*60, 0.0, [this](float value) {
        if(target)
            target->jump_drive_charge_time = value;
    });
    jump_drive_charge_time_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(right_col, "", tr("Jump, energy:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    jump_drive_energy_slider = new GuiSlider(right_col, "", 0.0, 100, 0.0, [this](float value) {
        if(target)
            target->jump_drive_energy_per_km_charge = value;
    });
    jump_drive_energy_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(right_col, "", tr("Jump, delay:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    jump_delay_slider = new GuiSlider(right_col, "", 0.0, 60*20, 0.0, [this](float value) {
        if (target->jump_delay > 0)
            target->jump_delay = value;
    });
    jump_delay_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(right_col, "", tr("Warp, speed:"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    warp_speed_slider = new GuiSlider(right_col, "", 0.0, 90, 0.0, [this](float value) {
        if(target)
            target->warp_speed_per_warp_level = value*16.667f;
    });
    warp_speed_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

}
 void GuiShipTweak::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    jump_drive_charge_slider->setValue(target->jump_drive_charge / target->jump_drive_max_distance * 100.f);
    jump_drive_min_distance_slider->setValue(round(target->jump_drive_min_distance / 1000000)*1000);
    jump_drive_max_distance_slider->setValue(round(target->jump_drive_max_distance / 1000000)*1000);
    jump_delay_slider->setValue(target->jump_delay);

    jump_drive_charge_slider->setVisible(target->hasJumpDrive());
    jump_drive_min_distance_slider->setVisible(target->hasJumpDrive());
    jump_drive_max_distance_slider->setVisible(target->hasJumpDrive());
    jump_delay_slider->setVisible(target->hasJumpDrive());
    jump_drive_charge_time_slider->setVisible(target->hasJumpDrive());
    jump_drive_energy_slider->setVisible(target->hasJumpDrive());
    warp_speed_slider->setVisible(target->has_warp_drive);

    warp_toggle->setValue(target->has_warp_drive);
    jump_toggle->setValue(target->hasJumpDrive());
    reactor_toggle->setValue(target->hasReactor());
    cloaking_toggle->setValue(target->hasCloaking());
    impulse_speed_slider->setValue(target->impulse_max_speed);
    impulse_acceleration_slider->setValue(target->getAcceleration().forward);
    impulse_reverse_speed_slider->setValue(target->impulse_max_reverse_speed);
    impulse_reverse_acceleration_slider->setValue(target->getAcceleration().reverse);
    turn_speed_slider->setValue(target->turn_speed);
    combat_maneuver_boost_speed_slider->setValue(target->combat_maneuver_boost_speed);
    combat_maneuver_strafe_speed_slider->setValue(target->combat_maneuver_strafe_speed);
    jump_drive_charge_time_slider->setValue(target->jump_drive_charge_time);
    jump_drive_energy_slider->setValue(target->jump_drive_energy_per_km_charge);
    warp_speed_slider->setValue(target->warp_speed_per_warp_level/16.667f);

    P<PlayerSpaceship> player = target;
    if (player)
    {
        engineering_presets_slider->setValue(player->active_engineer_presets_number);
    }
}

 void GuiShipTweak::open(P<SpaceObject> target)
{
    P<SpaceShip> ship = target;
    this->target = ship;

    impulse_speed_slider->clearSnapValues()->addSnapValue(ship->ship_template->impulse_speed, 5.0f);
    impulse_acceleration_slider->clearSnapValues()->addSnapValue(ship->ship_template->impulse_acceleration, 5.0f);
    impulse_reverse_speed_slider->clearSnapValues()->addSnapValue(ship->ship_template->impulse_reverse_speed, 5.0f);
    impulse_reverse_acceleration_slider->clearSnapValues()->addSnapValue(ship->ship_template->impulse_reverse_acceleration, 5.0f);
    turn_speed_slider->clearSnapValues()->addSnapValue(ship->ship_template->turn_speed, 1.0f);

    combat_maneuver_boost_speed_slider->clearSnapValues()->addSnapValue(ship->combat_maneuver_boost_speed, 20.0f);

    combat_maneuver_strafe_speed_slider->clearSnapValues()->addSnapValue(ship->combat_maneuver_strafe_speed, 20.0f);
}

GuiShipTweakMissileWeapons::GuiShipTweakMissileWeapons(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    // Left column
    (new GuiLabel(left_col, "", tr("missiles", "Storage space:"), 30))->setSize(GuiElement::GuiSizeMax, 40);


    for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
        custom_missile_storage_labels.push_back(new GuiLabel(left_col, "", getMissileWeaponName(kv.first) + ":", 20));
        custom_missile_storage_labels.back()->setSize(GuiElement::GuiSizeMax, 30);
        missile_storage_amount_slider.push_back(new GuiSlider(left_col, "", 0.0, 300, 0.0, [this, kv](float value) {
            if(target)
            {
                target->custom_weapon_storage_max[kv.first] = int(round(value));
                target->custom_weapon_storage[kv.first] = std::min(target->custom_weapon_storage[kv.first], target->custom_weapon_storage_max[kv.first]);
            }
        }));
        missile_storage_amount_slider.back()->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
    }

    for(int n=0; n<MW_Count; n++)
    {
        (new GuiLabel(left_col, "", getLocaleMissileWeaponName(EMissileWeapons(n)) + ":", 20))->setSize(GuiElement::GuiSizeMax, 30);
        missile_storage_amount_slider.push_back(new GuiSlider(left_col, "", 0.0, 300, 0.0, [this, n](float value) {
            if(target)
            {
                target->weapon_storage_max[n] = int(round(value));
                target->weapon_storage[n] = std::min(target->weapon_storage[n], target->weapon_storage_max[n]);
            }
        }));
        missile_storage_amount_slider.back()->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
    }

    // Right column
    (new GuiLabel(right_col, "",  tr("missiles", "Stored amount:"), 30))->setSize(GuiElement::GuiSizeMax, 40);

    for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
        custom_missile_current_labels.push_back(new GuiLabel(right_col, "", getMissileWeaponName(kv.first) + ":", 20));
        custom_missile_current_labels.back()->setSize(GuiElement::GuiSizeMax, 30);
        missile_current_amount_slider.push_back(new GuiSlider(right_col, "", 0.0, 300, 0.0, [this, kv](float value) {
            if(target)
                target->custom_weapon_storage[kv.first] = std::min(int(round(value)), target->custom_weapon_storage_max[kv.first]);
        }));
        missile_current_amount_slider.back()->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
    }

    for(int n=0; n<MW_Count; n++)
    {
        (new GuiLabel(right_col, "", getLocaleMissileWeaponName(EMissileWeapons(n)) + ":", 20))->setSize(GuiElement::GuiSizeMax, 30);
        missile_current_amount_slider.push_back(new GuiSlider(right_col, "", 0.0, 300, 0.0, [this, n](float value) {
            if(target)
                target->weapon_storage[n] = std::min(int(round(value)), target->weapon_storage_max[n]);
        }));
        missile_current_amount_slider.back()->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
    }


}

GuiJammerTweak::GuiJammerTweak(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    (new GuiLabel(left_col, "", tr("Jammer Range:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    jammer_range_slider = new GuiSlider(left_col, "", 0, 20000, 0, [this](float value) {
        if(target)
            target->setRange(value);
    });
    jammer_range_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("Hull current:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    hull_slider = new GuiSlider(right_col, "", 0.0, 500, 0.0, [this](float value) {
        target->setHull(roundf(value));
    });
    hull_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
}

void GuiJammerTweak::open(P<SpaceObject> target)
{
    P<WarpJammer> jammer = target;
    this->target = jammer;
}

void GuiJammerTweak::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    jammer_range_slider->setValue(target->getRange());
    hull_slider->setValue(target->getHull());
}

GuiAsteroidTweak::GuiAsteroidTweak(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    (new GuiLabel(left_col, "", tr("Asteroid Size:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    asteroid_size_slider = new GuiSlider(left_col, "", 10, 500, 0, [this](float value) {
        target->setSize(value);
    });
    asteroid_size_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
}

void GuiAsteroidTweak::open(P<SpaceObject> target)
{
    P<Asteroid> asteroid = target;
    this->target = asteroid;
}

void GuiAsteroidTweak::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    asteroid_size_slider->setValue(target->getSize());
}

void GuiShipTweakMissileWeapons::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    unsigned int n = 0;
    for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
        custom_missile_storage_labels[n]->setVisible((target->custom_weapon_storage_max[kv.first]>0)
        || (target->ship_template->custom_weapon_storage.find(kv.first)!=target->ship_template->custom_weapon_storage.end()));
        custom_missile_current_labels[n]->setVisible((target->custom_weapon_storage_max[kv.first]>0)
        || (target->ship_template->custom_weapon_storage.find(kv.first)!=target->ship_template->custom_weapon_storage.end()));
        missile_current_amount_slider[n]->setVisible((target->custom_weapon_storage_max[kv.first]>0)
        || (target->ship_template->custom_weapon_storage.find(kv.first)!=target->ship_template->custom_weapon_storage.end()));
        missile_storage_amount_slider[n]->setVisible((target->custom_weapon_storage_max[kv.first]>0)
        || (target->ship_template->custom_weapon_storage.find(kv.first)!=target->ship_template->custom_weapon_storage.end()));
        missile_current_amount_slider[n]->setRange(0, float(target->custom_weapon_storage_max[kv.first]));

        if (target->custom_weapon_storage[kv.first] != int(missile_current_amount_slider[n]->getValue()))
        {
            missile_current_amount_slider[n]->setValue(float(target->custom_weapon_storage[kv.first]));
        }
        n++;
    }

    unsigned int wsIdx = 0;
    while(n<missile_current_amount_slider.size())
    {

        missile_current_amount_slider[n]->setRange(0, float(target->weapon_storage_max[wsIdx]));

        if (target->weapon_storage[wsIdx] != int(missile_current_amount_slider[n]->getValue()))
        {
            missile_current_amount_slider[n]->setValue(float(target->weapon_storage[wsIdx]));
        }
        n++; wsIdx++;
    }
}

void GuiShipTweakMissileWeapons::open(P<SpaceObject> target)
{
    P<SpaceShip> ship = target;
    this->target = ship;


    unsigned int n = 0;
    for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
        missile_storage_amount_slider[n]->setValue(float(ship->custom_weapon_storage_max[kv.first]));
        n++;
    }

    unsigned int wsIdx = 0;
    while(n<missile_storage_amount_slider.size())
    {
        missile_storage_amount_slider[n]->setValue(float(ship->weapon_storage_max[wsIdx]));
        n++; wsIdx++;

    }

}

GuiShipTweakMissileTubes::GuiShipTweakMissileTubes(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    // Left column
    (new GuiLabel(left_col, "", tr("Tube count:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    missile_tube_amount_selector = new GuiSelector(left_col, "", [this](int index, string value) {
        if(target)
            target->weapon_tube_count = index;
    });
    for(int n=0; n<max_weapon_tubes; n++)
        missile_tube_amount_selector->addEntry(string(n), "");
    missile_tube_amount_selector->setSize(GuiElement::GuiSizeMax, 40);

    // Right column
    tube_index = 0;
    index_selector = new GuiSelector(right_col, "", [this](int index, string value)
    {
        if(target)
        {
            if (index >= target->weapon_tube_count)
            {
                if (index == max_weapon_tubes - 1)
                    index = std::max(0, target->weapon_tube_count - 1);
                else
                    index = 0;
                index_selector->setSelectionIndex(index);
            }
            tube_index = index;
        }
    });
    index_selector->setSize(GuiElement::GuiSizeMax, 40);
    for(int n=0; n<max_weapon_tubes; n++)
        index_selector->addEntry(tr("Tube: {id_tube}").format({{"id_tube", string(n + 1)}}), "");
    index_selector->setSelectionIndex(0);

    (new GuiLabel(right_col, "", tr("tube", "Direction:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    direction_slider = new GuiSlider(right_col, "", -180.0, 180, 0.0, [this](float value) {
        if(target)
            target->weapon_tube[tube_index].setDirection(roundf(value));
    });
    direction_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("tube", "Load time:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    load_time_slider = new GuiSlider(right_col, "", 0.0, 60.0, 0.0, [this](float value) {
        if(target)
            target->weapon_tube[tube_index].setLoadTimeConfig(roundf(value * 10) / 10);
    });
    load_time_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("tube", "Size:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    size_selector=new GuiSelector(right_col, "", [this](int index, string value)
    {
        if(target)
            target->weapon_tube[tube_index].setSize(EMissileSizes(index));
    });
    size_selector->addEntry(tr("tube", "Small"),MS_Small);
    size_selector->addEntry(tr("tube", "Medium"),MS_Medium);
    size_selector->addEntry(tr("tube", "Large"),MS_Large);
    size_selector->setSelectionIndex(MS_Medium);
    size_selector->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("tube", "Allowed use:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    int n=0;
    for(n=0; n<MW_Count; n++)
    {
        allowed_use.push_back(new GuiToggleButton(right_col, "", getLocaleMissileWeaponName(EMissileWeapons(n)), [this, n](bool value) {
            if (value && target)
                target->weapon_tube[tube_index].allowLoadOf(EMissileWeapons(n));
            else if (target)
                target->weapon_tube[tube_index].disallowLoadOf(EMissileWeapons(n));
        }));
        allowed_use[n]->setSize(GuiElement::GuiSizeMax, 40);
    }
    for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
         allowed_use.push_back(new GuiToggleButton(right_col, "", getLocaleMissileWeaponName(kv.first), [this, kv](bool value) {
            if (value && target)
                target->weapon_tube[tube_index].allowLoadOf(kv.first);
            else if (target)
                target->weapon_tube[tube_index].disallowLoadOf(kv.first);
        }));
        allowed_use[n]->setSize(GuiElement::GuiSizeMax, 40);
        n++;
    }

}

void GuiShipTweakMissileTubes::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    direction_slider->setValue(angleDifference(0.0f, target->weapon_tube[tube_index].getDirection()));
    load_time_slider->setValue(target->weapon_tube[tube_index].getLoadTimeConfig());
    int n=0;
    for(n=0; n<MW_Count; n++)
    {
        allowed_use[n]->setValue(target->weapon_tube[tube_index].canLoad(EMissileWeapons(n)));
    }
    for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
        allowed_use[n]->setValue(target->weapon_tube[tube_index].canLoad(kv.first));
        n++;
    }
      
    size_selector->setSelectionIndex(target->weapon_tube[tube_index].getSize());
}

void GuiShipTweakMissileTubes::open(P<SpaceObject> target)
{
    P<SpaceShip> ship = target;
    this->target = ship;

    missile_tube_amount_selector->setSelectionIndex(ship->weapon_tube_count);
}

GuiShipTweakBeamweapons::GuiShipTweakBeamweapons(GuiContainer* owner)
: GuiTweakPage(owner)
{
    beam_index = 0;

    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    GuiSelector* index_selector = new GuiSelector(left_col, "", [this](int index, string value)
    {
        beam_index = index;
    });
    index_selector->setSize(GuiElement::GuiSizeMax, 40);
    for(int n=0; n<max_beam_weapons; n++)
        index_selector->addEntry(tr("Beam: {id_beam}").format({{"id_beam", string(n + 1)}}), "");
    index_selector->setSelectionIndex(0);

    valid_toggle = new GuiToggleButton(left_col, "", "Valide", [this](bool value) {
        if(target)
            target->beam_weapons[beam_index].setIsValid(value);
    });
    valid_toggle->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("beam", "Arc:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    arc_slider = new GuiSlider(right_col, "", 0.0, 360.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setArc(roundf(value));
    });
    arc_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("beam", "Direction:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    direction_slider = new GuiSlider(right_col, "", -180.0, 180.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setDirection(roundf(value));
    });
    direction_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("beam", "Turret arc:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    turret_arc_slider = new GuiSlider(right_col, "", 0.0, 360.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setTurretArc(roundf(value));
    });
    turret_arc_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("beam", "Turret direction:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    turret_direction_slider = new GuiSlider(right_col, "", -180.0, 180.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setTurretDirection(roundf(value));
    });
    turret_direction_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("beam", "Turret rotation rate:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    // 25 is an arbitrary limit to add granularity; values greater than 25
    // result in practicaly instantaneous turret rotation anyway.
    turret_rotation_rate_slider = new GuiSlider(right_col, "", 0.0, 250.0, 0.0, [this](float value) {
        // Divide a large value for granularity.
        if ((value > 0) && target)
            target->beam_weapons[beam_index].setTurretRotationRate(value / 10.0f);
        else if (target)
            target->beam_weapons[beam_index].setTurretRotationRate(0.0f);
    });
    turret_rotation_rate_slider->setSize(GuiElement::GuiSizeMax, 30);
    // Override overlay label.
    turret_rotation_rate_overlay_label = new GuiLabel(turret_rotation_rate_slider, "", "", 30);
    turret_rotation_rate_overlay_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    (new GuiLabel(right_col, "", tr("beam", "Range:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    range_slider = new GuiSlider(right_col, "", 0.0, 5000.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setRange(roundf(value / 100) * 100);
    });
    range_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("beam", "Cycle time:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    cycle_time_slider = new GuiSlider(right_col, "", 0.1, 20.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setCycleTime(value);
    });
    cycle_time_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("beam", "Energy used per fire:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    energy_per_fire_slider = new GuiSlider(right_col, "", 0.0, 20.0, 0.0, [this](float value) {
        target->beam_weapons[beam_index].setEnergyPerFire(value);
    });
    energy_per_fire_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("beam", "Heat generated per fire:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    heat_per_fire_slider = new GuiSlider(right_col, "", 0.0, 250.0, 0.0, [this](float value) {
        // Divide a large value for granularity.
        if (value > 0)
            target->beam_weapons[beam_index].setHeatPerFire(value / 100.0f);
        else
            target->beam_weapons[beam_index].setHeatPerFire(0.0f);
    });
    heat_per_fire_slider->setSize(GuiElement::GuiSizeMax, 30);
    // Override overlay label.
    heat_per_fire_overlay_label = new GuiLabel(heat_per_fire_slider, "", "", 30);
    heat_per_fire_overlay_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    (new GuiLabel(right_col, "", tr("beam", "Damage:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    damage_slider = new GuiSlider(right_col, "", 0.1, 50.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setDamage(value);
    });
    damage_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("Overheat by shot (/100):"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    heat_slider = new GuiSlider(right_col, "", 0.1, 50.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setHeatPerFire(value / 100.0f);
    });
    heat_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiLabel(right_col, "", tr("Energy by shot:"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    energy_slider = new GuiSlider(right_col, "", 0.1, 50.0, 0.0, [this](float value) {
        if(target)
            target->beam_weapons[beam_index].setEnergyPerFire(value);
    });
    energy_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);
}

void GuiShipTweakBeamweapons::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    target->drawOnRadar(renderer, glm::vec2(rect.position.x - 150.0f + rect.size.x / 2.0f, rect.position.y + rect.size.y * 0.66f), 300.0f / 5000.0f, 0, false);

    arc_slider->setValue(target->beam_weapons[beam_index].getArc());
    direction_slider->setValue(angleDifference(0.0f, target->beam_weapons[beam_index].getDirection()));
    range_slider->setValue(target->beam_weapons[beam_index].getRange());
    turret_arc_slider->setValue(target->beam_weapons[beam_index].getTurretArc());
    turret_direction_slider->setValue(angleDifference(0.0f, target->beam_weapons[beam_index].getTurretDirection()));
    turret_rotation_rate_slider->setValue(target->beam_weapons[beam_index].getTurretRotationRate() * 10.0f);
    turret_rotation_rate_overlay_label->setText(string(target->beam_weapons[beam_index].getTurretRotationRate()));
    cycle_time_slider->setValue(target->beam_weapons[beam_index].getCycleTime());
    energy_per_fire_slider->setValue(target->beam_weapons[beam_index].getEnergyPerFire());
    heat_per_fire_slider->setValue(target->beam_weapons[beam_index].getHeatPerFire() * 100.0f);
    heat_per_fire_overlay_label->setText(string(target->beam_weapons[beam_index].getHeatPerFire()));
    damage_slider->setValue(target->beam_weapons[beam_index].getDamage());
    heat_slider->setValue(target->beam_weapons[beam_index].getHeatPerFire() * 100.0f);
    energy_slider->setValue(target->beam_weapons[beam_index].getEnergyPerFire());
    valid_toggle->setValue(target->beam_weapons[beam_index].getIsValid());
}

void GuiShipTweakBeamweapons::open(P<SpaceObject> target)
{
    P<SpaceShip> ship = target;
    this->target = ship;
}

GuiShipTweakSystems::GuiShipTweakSystems(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto col_1 = new GuiElement(this, "LAYOUT_1");
    col_1->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(150, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    auto col_2 = new GuiElement(this, "LAYOUT_2");
    col_2->setPosition(210, 25, sp::Alignment::TopLeft)->setSize(100, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    auto col_3 = new GuiElement(this, "LAYOUT_3");
    col_3->setPosition(320, 25, sp::Alignment::TopLeft)->setSize(100, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    auto col_4 = new GuiElement(this, "LAYOUT_4");
    col_4->setPosition(430, 25, sp::Alignment::TopLeft)->setSize(100, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    auto col_5 = new GuiElement(this, "LAYOUT_5");
    col_5->setPosition(540, 25, sp::Alignment::TopLeft)->setSize(100, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    (new GuiLabel(col_1, "", tr("State"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(col_2, "", tr("Max"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(col_3, "", tr("Heat"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(col_4, "", tr("Hack"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(col_5, "", tr("Coolant"), 20))->setSize(GuiElement::GuiSizeMax, 30);

    for(int n=0; n<SYS_COUNT; n++)
    {
        ESystem system = ESystem(n);

        system_name[n] = new GuiLabel(col_1, "", getLocaleSystemName(system), 20);
        system_name[n]->setSize(GuiElement::GuiSizeMax, 30);

        temp_1[n] = new GuiLabel(col_2, "", " ", 20);
        temp_1[n]->setSize(GuiElement::GuiSizeMax, 30);

        temp_2[n] = new GuiLabel(col_3, "", " ", 20);
        temp_2[n]->setSize(GuiElement::GuiSizeMax, 30);

        temp_3[n] = new GuiLabel(col_4, "", " ", 20);
        temp_3[n]->setSize(GuiElement::GuiSizeMax, 30);

        temp_4[n] = new GuiLabel(col_5, "", " ", 20);
        temp_4[n]->setSize(GuiElement::GuiSizeMax, 30);

        system_damage[n] = new GuiSlider(col_1, "", -1.0, 1.0, 0.0, [this, n](float value) {
            if(target)
                target->systems[n].health = std::min(value,target->systems[n].health_max);
        });
        system_damage[n]->setSize(GuiElement::GuiSizeMax, 30);
        system_damage[n]->addSnapValue(-1.0, 0.01);
        system_damage[n]->addSnapValue(-0.5, 0.01);
        system_damage[n]->addSnapValue( 0.0, 0.01);
        system_damage[n]->addSnapValue( 0.5, 0.01);
        system_damage[n]->addSnapValue( 1.0, 0.01);

        //(new GuiLabel(col_2, "", tr("{system} health max").format({{"system", getLocaleSystemName(system)}}), 20))->setSize(GuiElement::GuiSizeMax, 30);
        system_health_max[n] = new GuiSlider(col_2, "", -1.0, 1.0, 1.0, [this, n](float value) {
            if(target)
            {
                target->systems[n].health_max = value;
                target->systems[n].health = std::min(value,target->systems[n].health);
            }
        });
        system_health_max[n]->setSize(GuiElement::GuiSizeMax, 30);
        system_health_max[n]->addSnapValue(-1.0, 0.01);
        system_health_max[n]->addSnapValue( 0.0, 0.01);
        system_health_max[n]->addSnapValue( 1.0, 0.01);

        //(new GuiLabel(col_3, "", tr("{system} heat").format({{"system", getLocaleSystemName(system)}}), 20))->setSize(GuiElement::GuiSizeMax, 30);
        system_heat[n] = new GuiSlider(col_3, "", 0.0, 1.0, 0.0, [this, n](float value) {
            if(target)
                target->systems[n].heat_level = value;
        });
        system_heat[n]->setSize(GuiElement::GuiSizeMax, 30);
        system_heat[n]->addSnapValue( 0.0, 0.01);
        system_heat[n]->addSnapValue( 0.5, 0.01);
        system_heat[n]->addSnapValue( 1.0, 0.01);

        system_hack[n] = new GuiSlider(col_4, "", 0.0, 1.0, 0.0, [this, n](float value) {
            if(target)
                target->systems[n].hacked_level = value;
        });
        system_hack[n]->setSize(GuiElement::GuiSizeMax, 30);
        system_hack[n]->addSnapValue( 0.0, 0.01);
        system_hack[n]->addSnapValue( 0.5, 0.01);
        system_hack[n]->addSnapValue( 1.0, 0.01);

        system_coolant[n] = new GuiSlider(col_5, "", 0.0, 2.0, 0.0, [this, n](float value) {
            P<PlayerSpaceship> player = target;
            if (player)
                player->setSystemCoolantRequest(ESystem(n), player->systems[n].coolant_request);
        });
        system_coolant[n]->setSize(GuiElement::GuiSizeMax, 30);
        system_coolant[n]->addSnapValue( 0.0, 0.01);
        system_coolant[n]->addSnapValue( 0.5, 0.01);
        system_coolant[n]->addSnapValue( 1.0, 0.01);
        system_coolant[n]->addSnapValue( 1.5, 0.01);
        system_coolant[n]->addSnapValue( 2.0, 0.01);
    }
}

void GuiShipTweakSystems::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    for(int n=0; n<SYS_COUNT; n++)
    {
        system_damage[n]->setValue(target->systems[n].health);
        system_health_max[n]->setValue(target->systems[n].health_max);
        system_heat[n]->setValue(target->systems[n].heat_level);
        system_hack[n]->setValue(target->systems[n].hacked_level);

        if (!target->hasSystem(ESystem(n)))
        {
            system_name[n]->hide();
            temp_1[n]->hide();
            temp_2[n]->hide();
            temp_3[n]->hide();
            temp_4[n]->hide();
            system_damage[n]->hide();
            system_health_max[n]->hide();
            system_heat[n]->hide();
            system_hack[n]->hide();
            system_coolant[n]->hide();
        }
        else
        {
            system_name[n]->show();
            temp_1[n]->show();
            temp_2[n]->show();
            temp_3[n]->show();
            temp_4[n]->show();
            system_damage[n]->show();
            system_health_max[n]->show();
            system_heat[n]->show();
            system_hack[n]->show();
            system_coolant[n]->show();
        }
    }
}

void GuiShipTweakSystems::open(P<SpaceObject> target)
{
    P<SpaceShip> ship = target;
    this->target = ship;
}

GuiShipTweakCrew::GuiShipTweakCrew(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    // Count and list ship positions and whether they're occupied.
    position_count = new GuiLabel(right_col, "", tr("Positions occupied: "), 30);
    position_count->setSize(GuiElement::GuiSizeMax, 50);

    for(int n = 0; n < max_crew_positions; n++)
    {
        string position_name = getCrewPositionName(ECrewPosition(n));

        position[n] = new GuiKeyValueDisplay(right_col, "CREW_POSITION_" + position_name, 0.5, position_name, "-");
        position[n]->setSize(GuiElement::GuiSizeMax, 30);
    }

}

void GuiShipTweakCrew::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    // Update position list.
    int position_counter = 0;

    // Update the status of each crew position.
    for(int n = 0; n < max_crew_positions; n++)
    {
        string position_name = getCrewPositionName(ECrewPosition(n));
        string position_state = "-";

         std::vector<string> players;
         foreach(PlayerInfo, i, player_info_list)
         {
            if (i->ship_id == target->getMultiplayerId() && i->crew_position[n])
            {
                players.push_back(i->name);
            }
        }
        if (target->hasPlayerAtPosition(ECrewPosition(n)))
        {
            position_state = tr("position", string(", ").join(players));
            position_counter += 1;
        }

        position[n]->setValue(position_state);
    }

    // Update the total occupied position count.
    position_count->setText(tr("Positions occupied: ") + string(position_counter));

}

void GuiShipTweakCrew::open(P<SpaceObject> target)
{
    P<PlayerSpaceship> player = target;
    this->target = player;
}

GuiShipTweakSystemRates::GuiShipTweakSystemRates(GuiContainer* owner, Type type)
    : GuiTweakPage(owner), type{type}
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    auto center_col = new GuiElement(this, "CENTER_LAYOUT");
    center_col->setPosition(10, 25, sp::Alignment::TopCenter)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    // Header
    (new GuiLabel(left_col, "", "", 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(center_col, "", tr("current rate"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(right_col, "", tr("desired rate"), 20))->setSize(GuiElement::GuiSizeMax, 30);

    for (int n = 0; n < SYS_COUNT; n++)
    {
        ESystem system = ESystem(n);
        (new GuiLabel(left_col, "", tr("{system}").format({ {"system", getLocaleSystemName(system)} }), 20))->setSize(GuiElement::GuiSizeMax, 30);
        current_rates[n] = new GuiLabel(center_col, "", "", 20);
        current_rates[n]->setSize(GuiElement::GuiSizeMax, 30);

        desired_rates[n] = new GuiTextEntry(right_col, "", "");
        desired_rates[n]->setSize(GuiElement::GuiSizeMax, 30);
        desired_rates[n]->enterCallback([this, n](const string& text)
            {
                // Perform safe conversion (typos can happen).
                char* end = nullptr;
                auto converted = strtof(text.c_str(), &end);
                if (converted == 0.f && end == text.c_str())
                {
                    // failed - reset text to current value.
                    desired_rates[n]->setText(string(getRateValue(ESystem(n), this->type), 2));
                }
                else
                {
                    // apply!
                    setRateValue(ESystem(n), this->type, converted);
                }
            });
    }
    // Footer
    (new GuiLabel(center_col, "", tr("Applies on [Enter]"), 20))->setSize(GuiElement::GuiSizeMax, 30);
}

void GuiShipTweakSystemRates::open(P<SpaceObject> target)
{
    P<SpaceShip> ship = target;
    this->target = ship;
    for (int n = 0; n < SYS_COUNT; n++)
    {
        current_rates[n]->setText(string(getRateValue(ESystem(n), type), 2));
    }
}

void GuiShipTweakSystemRates::onDraw(sp::RenderTarget& target)
{
    for (int n = 0; n < SYS_COUNT; n++)
    {
        current_rates[n]->setText(string(getRateValue(ESystem(n), type), 2));
    }
}


float GuiShipTweakSystemRates::getRateValue(ESystem system, Type type) const
{
    if(!target)
        return 0.0;
    switch (type)
    {
    case Type::Coolant:
        return target->getSystemCoolantRate(system);
    case Type::Heat:
        return target->getSystemHeatRate(system);
    case Type::Power:
        return target->getSystemPowerRate(system);
    }

    LOG(ERROR) << "Unknown rate type " << static_cast<std::underlying_type_t<Type>>(type);
    return 0.f;
}

void GuiShipTweakSystemRates::setRateValue(ESystem system, Type type, float value)
{
    if(!target)
        return;
    switch (type)
    {
    case Type::Coolant:
        target->setSystemCoolantRate(system, value);
        break;
    case Type::Heat:
        target->setSystemHeatRate(system, value);
        break;
    case Type::Power:
        target->setSystemPowerRate(system, value);
        break;
    default:
        LOG(ERROR) << "Unknown rate type " << static_cast<std::underlying_type_t<Type>>(type);
    }
}

GuiShipTweakPlayer::GuiShipTweakPlayer(GuiContainer* owner)
: GuiTweakPage(owner)
{
    // TODO: Add more player ship tweaks here.
    // -   Ship-to-ship player transfer
    // -   Reputation

    // Add two columns.
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    // Left column
    // Edit control code.
    (new GuiLabel(left_col, "", tr("Control code:"), 25))->setSize(GuiElement::GuiSizeMax, 45);

    control_code = new GuiTextEntry(left_col, "", "");
    control_code->setSize(GuiElement::GuiSizeMax, 45);
    control_code->callback([this](string text) {
        if(target)
            target->control_code = text.upper();
    });

    // Edit reputation.
    if(gameGlobalInfo->use_nano_repair_crew)
    {
        (new GuiLabel(left_col, "", tr("Repair capacity:"), 25))->setSize(GuiElement::GuiSizeMax, 45);
        repair_total_slider = new GuiSlider(left_col, "", 0, 15, 0, [this](int value) {
            if(target)
                target->setRepairCrewCount(value);
        });
    }
    else
    {
        (new GuiLabel(left_col, "", tr("Droid number:"), 25))->setSize(GuiElement::GuiSizeMax, 45);
        repair_total_slider = new GuiSlider(left_col, "", 0, 15, 0, [this](int value) {
            if(target)
                target->setRepairCrewCount(value);
        });
    }
    repair_total_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);
    //(new GuiLabel(left_col, "", tr("Reputation:"), 30))->setSize(GuiElement::GuiSizeMax, 50);

    auto_repair_toogle = new GuiToggleButton(left_col, "", tr("button", "Auto repair"), [this](bool value) {
        if(target)
            target->commandSetAutoRepair(value);
    });
    auto_repair_toogle->setSize(GuiElement::GuiSizeMax, 35);

    // Edit energy level.
    (new GuiLabel(left_col, "", tr("Max energy:"), 25))->setSize(GuiElement::GuiSizeMax, 45);

    max_energy_level_slider = new GuiSlider(left_col, "", 0.0, 5000, 0.0, [this](float value) {
        if(target)
        {
            target->max_energy_level = value;
            target->energy_level = std::min(target->energy_level, target->max_energy_level);
        }
    });
    max_energy_level_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(left_col, "", tr("Current energy:"), 25))->setSize(GuiElement::GuiSizeMax, 45);

    energy_level_slider = new GuiSlider(left_col, "", 0.0, 5000, 0.0, [this](float value) {
        if(target)
            target->energy_level = std::min(value, target->max_energy_level);
    });
    energy_level_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(left_col, "", tr("Energy consumption ratio:"), 25))->setSize(GuiElement::GuiSizeMax, 45);
    energy_conso_ratio_slider = new GuiSlider(left_col, "", 0.0, 200, 0.0, [this](float value) {
        if(target)
            target->energy_consumption_ratio = value/100.0f;
    });
    energy_conso_ratio_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(left_col, "", tr("Coolant:"), 25))->setSize(GuiElement::GuiSizeMax, 45);

    max_coolant_slider = new GuiSlider(left_col, "", 0, 30, 0, [this](int value) {
        if(target)
            target->max_coolant = value;
        //P<PlayerSpaceship> player = target;
        //if (player)
        //    player->setSystemCoolantRequest(ESystem(0), player->systems[0].coolant_request);
    });
    max_coolant_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);
    
    (new GuiLabel(left_col, "", tr("Coolant (per system / max):"), 25))->setSize(GuiElement::GuiSizeMax, 45);

    coolant_per_system_slider = new GuiSlider(left_col, "", 0, 20, 0, [this](int value) {
        if(target)
            target->setMaxCoolantPerSystem(value);
    });
    coolant_per_system_slider->addSnapValue(10,1)->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(left_col, "", tr("Repair (per system / max):"), 30))->setSize(GuiElement::GuiSizeMax, 45);

    repair_per_system_slider = new GuiSlider(left_col, "", 0, 10, 0, [this](int value) {
        if(target)
            target->setMaxRepairPerSystem(value);
    });
    repair_per_system_slider->addSnapValue(2,1)->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);
    
    auto_coolant_toogle = new GuiToggleButton(left_col, "", tr("button", "Auto coolant"), [this](bool value) {
        if(target)
            target->setAutoCoolant(value);
    });
    auto_coolant_toogle->setSize(GuiElement::GuiSizeMax, 35);

    // Right column

    // Radar Capabilities
    (new GuiLabel(right_col, "", tr("Radar Capabilities:"), 25))->setSize(GuiElement::GuiSizeMax, 45);
    gravity_toggle = new GuiToggleButton(right_col, "", tr("Gravity Sensor"), [this](bool value) {
        if(target)
            target->setGravitySensor(value);
    });
    gravity_toggle->setSize(GuiElement::GuiSizeMax, 35);
    electrical_toggle = new GuiToggleButton(right_col, "", tr("Electrical Sensor"), [this](bool value) {
        if(target)
            target->setElectricalSensor(value);
    });
    electrical_toggle->setSize(GuiElement::GuiSizeMax, 35);
    biological_toggle = new GuiToggleButton(right_col, "", tr("Biological Sensor"), [this](bool value) {
        if(target)
            target->setBiologicalSensor(value);
    });
    biological_toggle->setSize(GuiElement::GuiSizeMax, 35);

    (new GuiLabel(right_col, "", tr("Max number of probes:"), 25))->setSize(GuiElement::GuiSizeMax, 45);
    probe_max_slider = new GuiSlider(right_col, "", 0, 20, 0, [this](int value) {
        if(target)
            target->setMaxScanProbeCount(value);
    });
    probe_max_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 35);



    //tsht : probleme de place sur l'ecran... MAYBEFIX
    // {
    //     (new GuiLabel(right_col, "", "Ajouter un drone:", 30))->setSize(GuiElement::GuiSizeMax, 50);

    //     std::vector<string> drones_names = ShipTemplate::getTemplateNameList(ShipTemplate::Drone);
    //     std::sort(drones_names.begin(), drones_names.end());
    //     GuiListbox* listDronesBox = new GuiListbox(right_col, "CREATE_SHIPS", [this](int index, string value)
    //     {
    //         P<ShipTemplate> drone_ship_template = ShipTemplate::getTemplate(value);
    //         P<SpaceShip> ship = target;
    //         Dock* dock = Dock::findOpenForDocking(ship->docks, max_docks_count);
    //         if (dock)
    //         {
    //             P<ShipCargo> cargo = new ShipCargo(drone_ship_template);
    //             dock->dock(cargo);
    //         }
    //     });
    //     listDronesBox->setTextSize(20)->setButtonHeight(30)->setPosition(-20, 20, sp::Alignment::TopRight)->setSize(300, 200);
    //     for(string drones_name : drones_names)
    //     {
    //         listDronesBox->addEntry(drones_name, drones_name);
    //     }
    // }
    {

        (new GuiLabel(right_col, "", tr("Add to dock :"), 30))->setSize(GuiElement::GuiSizeMax, 50);

        list_ships_box = new GuiListbox(right_col, "CREATE_SHIPS2", [this](int index, string value)
        {
            P<ShipTemplate> drone_ship_template = ShipTemplate::getTemplate(value);
            P<SpaceShip> ship = target;
            Dock* dock = Dock::findOpenForDocking(ship->docks, max_docks_count);
            if (dock)
            {
                P<ShipCargo> cargo = new ShipCargo(drone_ship_template);
                dock->dock(cargo);
                cargo->setBaseCarrier(ship);
            }
        });
        list_ships_box->setTextSize(20)->setButtonHeight(30)->setPosition(-20, 20, sp::Alignment::TopRight)->setSize(300, 200);


    }
    {
        (new GuiLabel(right_col, "", tr("Capabilities :"), 30))->setSize(GuiElement::GuiSizeMax, 50);
        // Can scan bool
        can_scan = new GuiToggleButton(right_col, "", tr("button", "Can scan"), [this](bool value) {
            if(target)
                target->setCanScan(value);
        });
        can_scan->setSize(GuiElement::GuiSizeMax, 30);

        // Can hack bool
        can_hack = new GuiToggleButton(right_col, "", tr("button", "Can hack"), [this](bool value) {
            if(target)
                target->setCanHack(value);
        });
        can_hack->setSize(GuiElement::GuiSizeMax, 30);

        // Can dock bool
        can_dock = new GuiToggleButton(right_col, "", tr("button", "Can dock"), [this](bool value) {
            if(target)
                target->setCanDock(value);
        });
        can_dock->setSize(GuiElement::GuiSizeMax, 30);

        // Can combat maneuver bool
        can_combat_maneuver = new GuiToggleButton(right_col, "", tr("button", "Can combat maneuver"), [this](bool value) {
            if(target)
                target->setCanCombatManeuver(value);
        });
        can_combat_maneuver->setSize(GuiElement::GuiSizeMax, 30);

        // Can self destruct bool
        can_self_destruct = new GuiToggleButton(right_col, "", tr("button", "Can self destruct"), [this](bool value) {
            if(target)
                target->setCanSelfDestruct(value);
        });
        can_self_destruct->setSize(GuiElement::GuiSizeMax, 30);

        // Can launch probe bool
        can_launch_probe = new GuiToggleButton(right_col, "", tr("button", "Can launch probes"), [this](bool value) {
            if(target)
                target->setCanLaunchProbe(value);
        });
        can_launch_probe->setSize(GuiElement::GuiSizeMax, 30);
    }

}

void GuiShipTweakPlayer::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    // Update the ship's energy level.
    energy_level_slider->setValue(target->energy_level);
    max_energy_level_slider->setValue(target->max_energy_level);
    energy_conso_ratio_slider->setValue(target->energy_consumption_ratio * 100);
    coolant_per_system_slider->setValue(target->max_coolant_per_system);
    repair_per_system_slider->setValue(target->max_repair_per_system);

    // Update Max of coolant level
    max_coolant_slider->setValue(target->max_coolant);

    // Update reputation points.
    if(gameGlobalInfo->use_nano_repair_crew)
        repair_total_slider->setValue(target->max_repair);
    else
        repair_total_slider->setValue(target->getRepairCrewCount());
    probe_max_slider->setValue(target->getMaxScanProbeCount());

}

void GuiShipTweakPlayer::open(P<SpaceObject> target)
{
    P<PlayerSpaceship> player = target;
    this->target = player;

    if (player)
    {
        // Read ship's control code.
        control_code->setText(player->control_code);

        gravity_toggle->setValue(player->has_gravity_sensor);
        electrical_toggle->setValue(player->has_electrical_sensor);
        biological_toggle->setValue(player->has_biological_sensor);

        auto_repair_toogle->setValue(player->auto_repair_enabled);
        auto_coolant_toogle->setValue(player->auto_coolant_enabled);
        if(!list_ships_box->entryCount())
        {
            for (auto &droneTemplate : player->ship_template->drones) // access by reference to avoid copying
            {
                list_ships_box->addEntry(droneTemplate.template_name,droneTemplate.template_name);
            }
        }
        energy_conso_ratio_slider->setValue(player->energy_consumption_ratio * 100);
    }
}

GuiShipTweakDock::GuiShipTweakDock(GuiContainer* owner)
: GuiTweakPage(owner)
{
    // Add two columns, hangar and flying.
    auto dock_col = new GuiElement(this, "DOCK_LAYOUT");
    dock_col->setPosition(50, 30, sp::Alignment::TopLeft)->setSize(80, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;
    auto type_col = new GuiElement(this, "DOCK_TYPE_LAYOUT");
    type_col->setPosition(130, 30, sp::Alignment::TopLeft)->setSize(170, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;
    auto content_col = new GuiElement(this, "DOCK_TYPE_LAYOUT");
    content_col->setPosition(300, 30, sp::Alignment::TopLeft)->setSize(150, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;


    auto envol_col = new GuiElement(this, "RIGHT_LAYOUT");
    envol_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    (new GuiLabel(dock_col, "", tr("Dock:"), 30))->setSize(GuiElement::GuiSizeMax, 40);
    (new GuiLabel(envol_col, "", tr("Recall:"), 30))->setSize(GuiElement::GuiSizeMax, 40);
    (new GuiLabel(type_col, "", "", 30))->setSize(GuiElement::GuiSizeMax, 40);
    (new GuiLabel(content_col, "", "", 30))->setSize(GuiElement::GuiSizeMax, 40);

    for (int n = 0; n < max_docks_count; n++)
    {
        {
            GuiLabel *label = new GuiLabel(dock_col, "", tr("Dock") + std::to_string(n+1), 20);
            label->setSize(GuiElement::GuiSizeMax, 40);
        }
        {
            type_selector.push_back(new GuiSelector(type_col, "", [this, n](int index, string value)
            {
                if(target)
                    target->docks[n].dock_type = getDockTypeEnum(value);
            }));

            type_selector[n]->setSize(GuiElement::GuiSizeMax, 40);
            int nbDockTypes = 0;
            for(; nbDockTypes < EDockType::nbElems; nbDockTypes++)
            {
                type_selector[n]->addEntry(getDockTypeName((EDockType)nbDockTypes), getDockTypeName((EDockType)nbDockTypes));
            }
            type_selector[n]->setSelectionIndex(nbDockTypes-1);

        }

        {

            content_button.push_back(new GuiButton(content_col, "", "", [this, n]() {
                if(target && target->docks[n].getCargo())
                {
                    target->docks[n].getCargo()->onLaunch(target->docks[n]);
                    target->docks[n].getCargo()->destroy();
                    target->docks[n].empty();
                }
            }));
            content_button[n]->setSize(150, 40);
        }

    }
    list_envol_box = new GuiListbox(envol_col, "ENVOL", [this](int index, string value)
    {
        P<SpaceShip> ship = gameGlobalInfo->getPlayerShip(value.toInt());
        if(ship && target)
        {
            Dock* dock = Dock::findOpenForDocking(target->docks, max_docks_count);
            if (dock)
            {
                P<ShipCargo> cargo = new ShipCargo(ship); //should keep current parameters
                dock->dock(cargo);
                ship->destroy();
            }
        }
    });
    //TODO : selectionner รงa puis un dock pour choisir le dock
    //list_envol_box->setTextSize(20)->setButtonHeight(30)->setPosition(-20, 20, sp::Alignment::TopRight)->setSize(300, 200);
    list_envol_box->setSize(150, GuiElement::GuiSizeMax);
}

void GuiShipTweakDock::open(P<SpaceObject> target)
{

    P<PlayerSpaceship> player = target;
    this->target = player;

    if (player)
    {
        for(int i=0; i < max_docks_count; i++)
        {
            type_selector[i]->setSelectionIndex((int)player->docks[i].dock_type);
            std::string text = (player->docks[i].getCargo()) ? player->docks[i].getCargo()->getCallSign():"Vide";
            content_button[i]->setText(text);
        }
    }
}

void GuiShipTweakDock::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    P<PlayerSpaceship> player = target;
    if (player)
    {
        for(int i=0; i < max_docks_count; i++)
        {
            std::string text = (player->docks[i].getCargo()) ? player->docks[i].getCargo()->getCallSign():"Vide";
            content_button[i]->setText(text);
        }


        for(int n = 0; n < GameGlobalInfo::max_player_ships; n++)
        {

            P<PlayerSpaceship> ship = gameGlobalInfo->getPlayerShip(n);

            int index_in_list = list_envol_box->indexByValue(string(n)); //at the end, we should have scanned the whole list, [0,max_player_ship[ only possible values
            if(ship)
            {
                if(target->canBeLandedOn(ship)) //no check on position here.
                {
                    if (index_in_list != -1)
                    {

                        if(ship && ship->getCallSign() == list_envol_box->getEntryName(index_in_list)) //ugly, suppose callsign is unique... should do with some kind of uuid (multiplayerid for entry name ?)
                            continue;
                        else
                        {
                            list_envol_box->removeEntry(index_in_list); //we are not synchronized, correct one will be added just after
                        }
                    }

                    list_envol_box->addEntry(ship->getCallSign(), std::to_string(n));
                }
                else if (index_in_list != -1) //should never happen, suppose canbelandedonby property changed over time
                {
                    list_envol_box->removeEntry(index_in_list);
                }
            }
            else if (index_in_list != -1) //no ship in game but still ship in list
            {
                list_envol_box->removeEntry(index_in_list);
            }
        }
    }


}

GuiShipTweakOxygen::GuiShipTweakOxygen(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    auto center_col = new GuiElement(this, "CENTER_LAYOUT");
    center_col->setPosition(0, 25, sp::Alignment::TopCenter)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    (new GuiLabel(left_col, "", tr("Oxygen Max zone"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    (new GuiLabel(center_col, "", tr("Oxygen zone"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    (new GuiLabel(right_col, "", tr("Recharge"), 30))->setSize(GuiElement::GuiSizeMax, 50);

    for(int n = 0; n < max_oxygen_zones; n++)
    {
        // (new GuiLabel(left_col, "", "Zone : " + string(n + 1), 30))->setSize(GuiElement::GuiSizeMax, 50);

        // Edit oxygen max
        oxygen_max_slider[n] = new GuiSlider(left_col, "", 0, 5000, 0, [this, n](int value) {
            if (target)
            {
                target->oxygen_max[n] = value;
                target->oxygen_points[n] = std::min(target->oxygen_points[n], target->oxygen_max[n]);
                int actual_zones_count = 0;
                for(int k=0; k<max_oxygen_zones; k++)
                    if (target->oxygen_max[k] > 0)
                        actual_zones_count = k+1;
                target->oxygen_zones = actual_zones_count;
            }
        });
        oxygen_max_slider[n]->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

        // Edit oxygen.
         oxygen_point_slider[n] = new GuiSlider(center_col, "", 0, 5000, 0, [this, n](int value) {
            if(target)
                target->oxygen_points[n] = std::min(float(value), target->oxygen_max[n]);
        });
        oxygen_point_slider[n]->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

        // Edit oxygen rate.
         oxygen_rate_slider[n] = new GuiSlider(right_col, "", -15, 2, 0, [this, n](int value) {
            if(target)
                target->oxygen_rate[n] = value;
        });
        oxygen_rate_slider[n]->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
        oxygen_rate_slider[n]->addSnapValue(-15, 0.05);
        oxygen_rate_slider[n]->addSnapValue(-10, 0.05);
        oxygen_rate_slider[n]->addSnapValue(-9, 0.05);
        oxygen_rate_slider[n]->addSnapValue(-8, 0.05);
        oxygen_rate_slider[n]->addSnapValue(-6, 0.05);
        oxygen_rate_slider[n]->addSnapValue(0, 0.05);
        oxygen_rate_slider[n]->addSnapValue(2, 0.05);
    }

    (new GuiLabel(left_col, "", tr("Passengers:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    passagers_slider = new GuiSlider(left_col, "", 0.0, 20.0, 0.0, [this](float value) {
        if(target)
            target->setPassagersCount(value);
    });
    passagers_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);

    (new GuiLabel(right_col, "", tr("Max passagers:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    max_passagers_slider = new GuiSlider(right_col, "", 0.0, 40.0, 0.0, [this](float value) {
        if(target)
            target->setMaxPassagersCount(value);
    });
    max_passagers_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
}

void GuiShipTweakOxygen::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    // Update oxygen points.
    for(int n = 0; n < max_oxygen_zones; n++)
    {
        oxygen_point_slider[n]->setValue(target->getOxygenPoints(n));
        oxygen_max_slider[n]->setValue(target->getOxygenMax(n));
        oxygen_rate_slider[n] ->setValue(target->getOxygenRate(n));
    }
}

void GuiShipTweakOxygen::open(P<SpaceObject> target)
{
    P<SpaceShip> ship = target;
    this->target = ship;

    passagers_slider->setValue(ship->getPassagersCount());
    max_passagers_slider->setValue(ship->getMaxPassagersCount());
}

GuiShipTweakMessages::GuiShipTweakMessages(GuiContainer* owner)
: GuiTweakPage(owner)
{
    color_message = glm::u8vec4(255,255,255,255);
    type_log = "generic";
    message = "";

    (new GuiLabel(this, "", tr("Message:"), 30))->setPosition(0, 30, sp::Alignment::TopCenter);
    message_entry = new GuiTextEntry(this, "", "");
    message_entry->setSize(650, 50);
    message_entry->setPosition(0, 50, sp::Alignment::TopCenter);
    message_entry->callback([this](string text) {
        message = text;
    });

    (new GuiButton(this, "", tr("Delete message"), [this]() {
        message_entry -> setText("");
        message = "";
    }))->setPosition(0, 100, sp::Alignment::TopCenter)->setSize(300, 50);

    // Add two columns.
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 150, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 150, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    // Left column

    (new GuiLabel(left_col, "", tr("In log"), 30))->setSize(GuiElement::GuiSizeMax, 100);

    // Choose the color
    (new GuiLabel(left_col, "", tr("Color:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    GuiSelector* color_selector = new GuiSelector(left_col, "", [this](int index, string value)
    {
        color_message = glm::u8vec4(255,255,255,255);
        if (value == "white")
            color_message = glm::u8vec4(255,255,255,255);
        if (value == "black")
            color_message = glm::u8vec4(0,0,0,255);
        if (value == "red")
            color_message = glm::u8vec4(255,0,0,255);
        if (value == "green")
            color_message = glm::u8vec4(0,255,0,255);
        if (value == "blue")
            color_message = glm::u8vec4(0,0,255,255);
        if (value == "yellow")
            color_message = glm::u8vec4(255,255,0,255);
        if (value == "magenta")
            color_message = glm::u8vec4(255,0,255,255);
        if (value == "cyan")
            color_message = glm::u8vec4(0,255,255,255);
    });
    color_selector->setSize(GuiElement::GuiSizeMax, 40);
    color_selector->addEntry("white", "white");
    color_selector->addEntry("black", "black");
    color_selector->addEntry("red", "red");
    color_selector->addEntry("green", "green");
    color_selector->addEntry("blue", "blue");
    color_selector->addEntry("yellow", "yellow");
    color_selector->addEntry("magenta", "magenta");
    color_selector->addEntry("cyan", "cyan");
    color_selector->setSelectionIndex(0);

    // Choose the color
    (new GuiLabel(left_col, "", tr("Log:"), 30))->setSize(GuiElement::GuiSizeMax, 50);
    GuiSelector* log_selector = new GuiSelector(left_col, "", [this](int index, string value)
    {
        type_log = value;
    });
    log_selector->setSize(GuiElement::GuiSizeMax, 40);
    log_selector->addEntry("generic", "generic");
    log_selector->addEntry("intern", "intern");
    log_selector->addEntry("docks", "docks");
    log_selector->addEntry("science", "science");
    log_selector->setSelectionIndex(0);

    // Send to all player
    message_all_toggle = new GuiToggleButton(left_col, "", tr("Message broadcast"), [this](bool value) {

   });
   message_all_toggle->setSize(GuiElement::GuiSizeMax, 40);

    // Send the message
    send_message_log = new GuiButton(left_col, "", tr("Send message"), [this]() {
       if (message_all_toggle->getValue())
       {
            for(int n=0; n<GameGlobalInfo::max_player_ships; n++)
            {
                P<PlayerSpaceship> ship = gameGlobalInfo->getPlayerShip(n);
                if (ship)
                    ship -> addToSpecificShipLog(message,color_message,type_log);
            }
       }
        else if(target)
            target -> addToSpecificShipLog(message,color_message,type_log);
    });
    send_message_log->setSize(GuiElement::GuiSizeMax, 40);

    // Right column

    (new GuiLabel(right_col, "", tr("On a screen"), 30))->setSize(GuiElement::GuiSizeMax, 100);

    (new GuiButton(right_col, "", tr("station","Helms"), [this]() {
        if(target)
            target->addCustomMessage(helmsOfficer,"helms_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(right_col, "", tr("station","Weapons"), [this]() {
        if(target)
            target->addCustomMessage(weaponsOfficer,"weapons_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(right_col, "", tr("station","Engineering"), [this]() {
        if(target)
            target->addCustomMessage(engineering,"engineering_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(right_col, "", tr("station","Science"), [this]() {
        if(target)
            target->addCustomMessage(scienceOfficer,"science_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(right_col, "", tr("station","Relay"), [this]() {
        if(target)
            target->addCustomMessage(relayOfficer,"relay_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(right_col, "", tr("station","Cic"), [this]() {
        if(target)
            target->addCustomMessage(cagOfficer,"cic_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(right_col, "", tr("Docks"), [this]() {
        if(target)
            target->addCustomMessage(dockMaster,"docks_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiButton(right_col, "", tr("Drone"), [this]() {
        if(target)
            target->addCustomMessage(dronePilot,"drone_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);
        (new GuiButton(right_col, "", tr("station","Single Pilot"), [this]() {
        if(target)
            target->addCustomMessage(singlePilot,"single_message", message);
    }))->setSize(GuiElement::GuiSizeMax, 30);

    (new GuiButton(right_col, "", tr("Take of screen message"), [this]() {
        if(target)
        {
            target->removeCustom("helms_message");
            target->removeCustom("weapons_message");
            target->removeCustom("engineering_message");
            target->removeCustom("science_message");
            target->removeCustom("relay_message");
            target->removeCustom("cic_message");
            target->removeCustom("docks_message");
            target->removeCustom("drone_message");
            target->removeCustom("single_message");

            message_entry -> setText("");
        }
    }))->setSize(GuiElement::GuiSizeMax, 30);

}

void GuiShipTweakMessages::onDraw(sp::RenderTarget& renderer)
{
}

void GuiShipTweakMessages::open(P<SpaceObject> target)
{
    P<PlayerSpaceship> player = target;
    this->target = player;

    if (player)
    {

    }
}


GuiShipTweakPlanet::GuiShipTweakPlanet(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto center_col = new GuiElement(this, "CENTER_LAYOUT");
    center_col->setPosition(0, 25, sp::Alignment::TopCenter)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    (new GuiLabel(left_col, "", "Texture", 30))->setSize(GuiElement::GuiSizeMax, 50);
    texture_selector = new GuiSelector(left_col, "", [this](int index, string value)
    {
        if(target)
            target->setPlanetSurfaceTexture(value);
    });
    texture_selector->setPosition(0, 0, sp::Alignment::TopRight)->setSize(GuiElement::GuiSizeMax, 50);

    std::vector<string> texture_filenames_jpg = findResources("planets/*.jpg");
    std::sort(texture_filenames_jpg.begin(), texture_filenames_jpg.end());
    std::vector<string> texture_filenames_png = findResources("planets/*.png");
    std::sort(texture_filenames_png.begin(), texture_filenames_png.end());
    for(string filename : texture_filenames_jpg)
        texture_selector->addEntry(filename.substr(filename.rfind("/") + 1, filename.rfind(".")), filename);
    for(string filename : texture_filenames_png)
        texture_selector->addEntry(filename.substr(filename.rfind("/") + 1, filename.rfind(".")), filename);
}

void GuiShipTweakPlanet::onDraw(sp::RenderTarget& renderer)
{

}

void GuiShipTweakPlanet::open(P<SpaceObject> target)
{
    P<Planet> planet = target;
    this->target = planet;
    string texture = planet->getPlanetSurfaceTexture();
    int id_texture = texture_selector->indexByValue(texture);
    texture_selector->setSelectionIndex(id_texture);
}

GuiShipTweakInfos::GuiShipTweakInfos(GuiContainer* owner)
: GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    (new GuiLabel(left_col, "", tr("Label"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    (new GuiLabel(right_col, "", tr("Value"), 30))->setSize(GuiElement::GuiSizeMax, 45);

    for(int n = 0; n < 10; n++)
    {
        infos_label[n] = new GuiTextEntry(left_col, "", "");
        infos_label[n]->setSize(GuiElement::GuiSizeMax, 45);
        infos_label[n]->callback([this, n](string text) {
            if(target)
                target->infos_label[n] = text;
        });

        infos_value[n] = new GuiTextEntry(right_col, "", "");
        infos_value[n]->setSize(GuiElement::GuiSizeMax, 45);
        infos_value[n]->callback([this, n](string text) {
            if(target)
                target->infos_value[n] = text;
        });
    }
    (new GuiLabel(left_col, "", tr("Private labels"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    (new GuiLabel(right_col, "", tr("Private values"), 30))->setSize(GuiElement::GuiSizeMax, 45);
    for(int n = 10; n < 15; n++)
    {
        infos_label[n] = new GuiTextEntry(left_col, "", "");
        infos_label[n]->setSize(GuiElement::GuiSizeMax, 45);
        infos_label[n]->callback([this, n](string text) {
            if(target)
                target->infos_label[n] = text;
        });

        infos_value[n] = new GuiTextEntry(right_col, "", "");
        infos_value[n]->setSize(GuiElement::GuiSizeMax, 45);
        infos_value[n]->callback([this, n](string text) {
            if(target)
                target->infos_value[n] = text;
        });
    }

}

void GuiShipTweakInfos::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    // Update infos.
    for(int n = 0; n < 15; n++)
    {
        infos_label[n]->setText(target->infos_label[n]);
        infos_value[n]->setText(target->infos_value[n]);
    }
}

void GuiShipTweakInfos::open(P<SpaceObject> target)
{
    this->target = target;
}

string GuiShipTweakSystemPowerFactors::powerFactorToText(float power)
{
    return string(power, 1);
}

GuiShipTweakSystemPowerFactors::GuiShipTweakSystemPowerFactors(GuiContainer* owner)
    : GuiTweakPage(owner)
{
    auto left_col = new GuiElement(this, "LEFT_LAYOUT");
    left_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;
    auto center_col = new GuiElement(this, "CENTER_LAYOUT");
    center_col->setPosition(10, 25, sp::Alignment::TopCenter)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;
    auto right_col = new GuiElement(this, "RIGHT_LAYOUT");
    right_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(200, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");;

    // Header
    (new GuiLabel(left_col, "", "", 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(center_col, "", tr("current factor"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(right_col, "", tr("desired factor"), 20))->setSize(GuiElement::GuiSizeMax, 30);

    for (int n = 0; n < SYS_COUNT; n++)
    {
        ESystem system = ESystem(n);
        (new GuiLabel(left_col, "", tr("{system}").format({ {"system", getLocaleSystemName(system)} }), 20))->setSize(GuiElement::GuiSizeMax, 30);
        system_current_power_factor[n] = new GuiLabel(center_col, "", "", 20);
        system_current_power_factor[n]->setSize(GuiElement::GuiSizeMax, 30);

        system_power_factor[n] = new GuiTextEntry(right_col, "", "");
        system_power_factor[n]->setSize(GuiElement::GuiSizeMax, 30);
        system_power_factor[n]->enterCallback([this, n](const string& text)
            {
                if (target)
                {
                    // Perform safe conversion (typos can happen).
                    char* end = nullptr;
                    auto converted = strtof(text.c_str(), &end);
                    if (converted == 0.f && end == text.c_str())
                    {
                        // failed - reset text to current value.
                        system_power_factor[n]->setText(string(target->systems[n].power_factor, 1));
                    }
                    else
                    {
                        // apply!
                        target->systems[n].power_factor = converted;
                    }
                }
            });
    }
    // Footer
    (new GuiLabel(center_col, "", tr("Applies on [Enter]"), 20))->setSize(GuiElement::GuiSizeMax, 30);
}

void GuiShipTweakSystemPowerFactors::open(P<SpaceObject> target)
{
    if(!target)
        return;
    P<SpaceShip> ship = target;
    this->target = ship;
    for (int n = 0; n < SYS_COUNT; n++)
    {
        system_power_factor[n]->setText(string(this->target->systems[n].power_factor, 1));
    }
}

void GuiShipTweakSystemPowerFactors::onDraw(sp::RenderTarget& renderer)
{
    if(!target)
        return;
    for (int n = 0; n < SYS_COUNT; n++)
    {
        system_current_power_factor[n]->setText(string(this->target->systems[n].power_factor, 1));
    }
}

GuiModifierTweak::GuiModifierTweak(GuiContainer* owner)
    : GuiTweakPage(owner)
{
    // Add two columns.
    button_col = new GuiElement(this, "LEFT_LAYOUT");
    button_col->setPosition(50, 25, sp::Alignment::TopLeft)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    information_col = new GuiElement(this, "RIGHT_LAYOUT");
    information_col->setPosition(-25, 25, sp::Alignment::TopRight)->setSize(300, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");

    (new GuiLabel(button_col, "", tr("Modifier"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(information_col, "", tr("Information"), 20))->setSize(GuiElement::GuiSizeMax, 30);
}

void GuiModifierTweak::open(P<SpaceObject> target)
{
    if(!target)
        return;
    if(this->target)
        return;
    P<PlayerSpaceship> ship = target;
    this->target = ship;

    std::map<string, std::vector<string>> category_indexed_modifiers;
    
    for (const auto & [key, modifier] : this->target->modifiers_to_data)
    {
        auto list_of_modifiers_it = category_indexed_modifiers.find(modifier.category);
        if(list_of_modifiers_it == category_indexed_modifiers.end())
        {
            category_indexed_modifiers.emplace(std::make_pair(modifier.category, std::vector<string>{key}));
        }
        else
        {
            list_of_modifiers_it->second.push_back(key);
        }
    }

    for (const auto &[category, vec_of_keys] : category_indexed_modifiers)
    {
        (new GuiLabel(button_col, "", tr(category), 20))->setSize(GuiElement::GuiSizeMax, 30);
        (new GuiLabel(information_col, "", "", 20))->setSize(GuiElement::GuiSizeMax, 30);

        for (const auto & key : vec_of_keys)
        {
            PlayerSpaceship::Modifier &modifier{this->target->modifiers_to_data[key]};
        
            modifier_buttons.push_back(new GuiToggleButton(button_col, "", key, [this, key](bool value) {
                    if(this->target)
                    {
                        if(true == value)
                        {
                            this->target->activateModifier(key);
                        }
                        else
                        {
                            this->target->deActivateModifier(key);
                        }
                    }
                }));
            modifier_buttons.back()->setSize(GuiElement::GuiSizeMax, 40);
            
            modifier_information.push_back(new GuiLabel(information_col, "", modifier.information, 20));
            modifier_information.back()->setSize(GuiElement::GuiSizeMax, 40);
            
        }
    }

}

void GuiModifierTweak::onDraw(sp::RenderTarget& renderer)
{

}

GuiBlueprintTweak::GuiBlueprintTweak(GuiContainer* owner)
    : GuiTweakPage(owner)
{
    // Add four columns.
    bp_available_col = new GuiElement(this, "LAYOUT_1");
    bp_available_col->setPosition(50, 125, sp::Alignment::TopLeft)->setSize(150, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    bp_in_use_col = new GuiElement(this, "LAYOUT_2");
    bp_in_use_col->setPosition(210, 125, sp::Alignment::TopLeft)->setSize(100, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    bp_wait_col = new GuiElement(this, "LAYOUT_3");
    bp_wait_col->setPosition(320, 125, sp::Alignment::TopLeft)->setSize(100, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    bp_max_col = new GuiElement(this, "LAYOUT_4");
    bp_max_col->setPosition(430, 125, sp::Alignment::TopLeft)->setSize(100, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    bp_duration_col = new GuiElement(this, "LAYOUT_5");
    bp_duration_col->setPosition(540, 125, sp::Alignment::TopLeft)->setSize(100, GuiElement::GuiSizeMax)->setAttribute("layout", "vertical");
    
    (new GuiLabel(this, "", tr("Squadron Delay ratio"), 20))->setSize(GuiElement::GuiSizeMax, 30);

    sq_bp_delay_factor = new GuiSlider(this, "", 0.0f, 200.0f, 100.0f, [this](float value) {
            if(this->target)
            {
                this->target->sq_bp_delay_factor = value / 100.0f;
            }
    });
    sq_bp_delay_factor->setPosition(50, 25, sp::Alignment::TopLeft);
    sq_bp_delay_factor->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
    sq_bp_delay_factor->addSnapValue(100, 0.05);

    (new GuiLabel(this, "", tr("Ammo Delay ratio"), 20))->setPosition(50, 50, sp::Alignment::TopLeft)->setSize(GuiElement::GuiSizeMax, 30);
    ammo_bp_delay_factor = new GuiSlider(this, "", 0.0f, 200.0f, 100.0f, [this](float value) {
            if(this->target)
            {
                this->target->ammo_bp_delay_factor = value / 100.0f;
            }
    });
    ammo_bp_delay_factor->setPosition(50, 75, sp::Alignment::TopLeft);
    ammo_bp_delay_factor->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
    ammo_bp_delay_factor->addSnapValue(100, 0.05);

    (new GuiLabel(bp_available_col, "", tr("Blueprint availability"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(bp_in_use_col, "", tr("Current in use"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(bp_wait_col, "", tr("Current waiting"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(bp_max_col, "", tr("Max"), 20))->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiLabel(bp_duration_col, "", tr("Time to finish"), 20))->setSize(GuiElement::GuiSizeMax, 30);
}

void GuiBlueprintTweak::open(P<SpaceObject> target)
{
    if(!target)
        return;
    if(this->target)
        return;
    P<PlayerSpaceship> ship = target;
    this->target = ship;
    sq_bp_delay_factor->setValue(this->target->sq_bp_delay_factor * 100.0f);
    ammo_bp_delay_factor->setValue(this->target->ammo_bp_delay_factor * 100.0f);

    
    {
        int n=0;
        for(auto &sqt : ship->getSquadronCompositions())
        {
            GuiToggleButton* sq_blueprint = new GuiToggleButton(bp_available_col, "", tr("cic",sqt.template_name), [n, this](bool value) {
                if (!this->target)
                    return;
                this->target->squadron_bp_available[n] = value;});
            sq_blueprint->setValue(this->target->squadron_bp_available[n]);
            sq_blueprint->setSize(130, 40);
            sq_blueprint->setTextSize(30);

            sq_bp_available.push_back(sq_blueprint);

            GuiLabel* in_use = new GuiLabel(bp_in_use_col, "", this->target->getLaunchedSquadronsCount(sqt.template_name), 30);
            in_use->setSize(GuiElement::GuiSizeMax, 40);

            sq_bp_in_use.push_back(in_use);

            GuiSlider* wait_instances = new GuiSlider(bp_wait_col, "", 0, sqt.max_created - this->target->getLaunchedSquadronsCount(sqt.template_name), 0, [this, n](int value) {
                if(this->target)
                {
                    this->target->number_of_waiting_squadron_for_bp[n] = value;
                }
            });
            wait_instances->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
            sq_bp_wait_instances.push_back(wait_instances);

            GuiSlider* max_instances = new GuiSlider(bp_max_col, "", 0, sqt.max_created *5, 0, [this, n](int value) {
                if(this->target)
                {
                    this->target->squadron_bp_max_created[n] = value;
                }
            });
            max_instances->setValue(sqt.max_created);
            max_instances->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
            max_instances->addSnapValue(sqt.max_created, 0.25);
            
            sq_bp_max_instances.push_back(max_instances);

            GuiSlider* duration = new GuiSlider(bp_duration_col, "",0, sqt.construction_duration *2 , sqt.construction_duration, [this, n](int value)
            {
                if(this->target)
                {
                    this->target->squadron_delay_to_next_creation[n] = value;
                    this->target->ship_template->squadrons_compositions[n].construction_duration = value;
                }
            });
            duration->setValue(sqt.construction_duration);
            duration->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
            duration->addSnapValue(sqt.construction_duration, 0.25);

            sq_bp_duration.push_back(duration);

            n++;
        }
    }

    {
        for(auto &amt : ship->ship_template->ammo_blueprints)
        {
            GuiToggleButton* ammo_blueprint = new GuiToggleButton(bp_available_col, "", tr("weapons",amt.first), [amt, this](bool value) {
                if (!this->target)
                    return;
                this->target->ammo_bp_available[amt.first] = value;});
            ammo_blueprint->setValue(this->target->ammo_bp_available[amt.first]);
            ammo_blueprint->setSize(130, 40);
            ammo_blueprint->setTextSize(30);

            ammo_bp_available.push_back(ammo_blueprint);

            GuiSlider* duration = new GuiSlider(bp_duration_col, "",0, amt.second.construction_duration *2 , amt.second.construction_duration, [this, amt](int value)
            {
                if(this->target)
                {
                    this->target->ammo_delay_to_next_creation[amt.first] = value;
                    this->target->ship_template->ammo_blueprints[amt.first].construction_duration = value;
                }
            });
            duration->setValue(amt.second.construction_duration);
            duration->addOverlay()->setSize(GuiElement::GuiSizeMax, 40);
            duration->addSnapValue(amt.second.construction_duration, 0.25);
            ammo_bp_duration.push_back(duration);
        }
    }

}

void GuiBlueprintTweak::onDraw(sp::RenderTarget& renderer)
{
    if(!this->target)
        return;
    for(size_t n = 0; n< sq_bp_available.size() ; n++)
    {
        auto& sqt = this->target->getSquadronCompositions()[n];
        sq_bp_in_use[n]->setText(this->target->getLaunchedSquadronsCount(sqt.template_name));
        sq_bp_wait_instances[n]->setRange(0, std::max(this->target->squadron_bp_max_created[n] - this->target->getLaunchedSquadronsCount(sqt.template_name), this->target->number_of_waiting_squadron_for_bp[n]));
        sq_bp_wait_instances[n]->setValue(this->target->number_of_waiting_squadron_for_bp[n]);
        sq_bp_max_instances[n]->setValue(this->target->squadron_bp_max_created[n]);
        sq_bp_duration[n]->setValue(this->target->squadron_delay_to_next_creation[n]);
    }
    sq_bp_delay_factor->setValue(this->target->sq_bp_delay_factor * 100.0f);

    int n = 0;
    for(auto &amt : this->target->ship_template->ammo_blueprints)
    {
        ammo_bp_duration[n]->setValue(this->target->ammo_delay_to_next_creation[amt.first]);
        n++;
    }
    ammo_bp_delay_factor->setValue(this->target->ammo_bp_delay_factor * 100.0f);
}