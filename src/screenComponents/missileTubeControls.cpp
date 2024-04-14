#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "missileTubeControls.h"
#include "powerDamageIndicator.h"

#include "gui/gui2_button.h"
#include "gui/gui2_progressbar.h"
#include "gui/gui2_label.h"
#include "gui/gui2_togglebutton.h"

GuiMissileTubeControls::GuiMissileTubeControls(GuiContainer* owner, string id, P<PlayerSpaceship> targetSpaceship)
: GuiElement(owner, id), target_spaceship(targetSpaceship)
{
    setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    setAttribute("layout", "verticalbottom");

    rows.resize(max_weapon_tubes);
    load_type_rows.resize(MW_Count);

    for (int n = max_weapon_tubes - 1; n >= 0; n--)
    {
        TubeRow row;
        row.layout = new GuiElement(this, id + "_ROW_" + string(n));
        row.layout->setSize(GuiElement::GuiSizeMax, 50)->setAttribute("layout", "horizontal");
        row.load_button = new GuiButton(row.layout, id + "_" + string(n) + "_LOAD_BUTTON", tr("missile","Load"), [this, n]() {
            if (!target_spaceship)
                return;
            if (target_spaceship->weapon_tube[n].isEmpty())
            {
                if (load_type != string(MW_None))
                {
                    target_spaceship->commandLoadTube(n, load_type);
                }
            }
            else
            {
                target_spaceship->commandUnloadTube(n);
            }
        });
        row.load_button->setSize(130, 50);
        row.fire_button = new GuiButton(row.layout, id + "_" + string(n) + "_FIRE_BUTTON", tr("missile","Fire"), [this, n]() {
            if (!target_spaceship)
                return;
            if (target_spaceship->weapon_tube[n].isLoaded())
            {
                float target_angle = missile_target_angle;
                if (!manual_aim)
                {
                    target_angle = target_spaceship->weapon_tube[n].calculateFiringSolution(target_spaceship->getTarget());
                    if (target_angle == std::numeric_limits<float>::infinity())
                        target_angle = target_spaceship->getRotation() + target_spaceship->weapon_tube[n].getDirection();
                }
                target_spaceship->commandFireTube(n, target_angle);
            }
        });
        row.fire_button->setSize(200, 50);

        pdi = new GuiPowerDamageIndicator(row.fire_button, id + "_" + string(n) + "_PDI", SYS_MissileSystem, sp::Alignment::CenterRight, target_spaceship);
        pdi->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

        row.loading_bar = new GuiProgressbar(row.layout, id + "_" + string(n) + "_PROGRESS", 0, 1.0, 0);
        row.loading_bar->setColor(glm::u8vec4(128, 128, 128, 255))->setSize(200, 50);
        row.loading_label = new GuiLabel(row.loading_bar, id + "_" + string(n) + "_PROGRESS_LABEL", tr("missile","Loading"), 35);
        row.loading_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

        rows[n] = row;
    }

    auto createLoadTypeRow = [this](TypeRow &tr, const string &id, const string &name) 
    {
        tr.layout = new GuiElement(this, id + "_ROW_" + string(name));
        tr.layout->setSize(GuiElement::GuiSizeMax, 40)->setAttribute("layout", "horizontal");;
        //const string& str = kv.first;

        tr.bp_button = new GuiToggleButton(tr.layout, id + "_BPMW_" + name, " ", [this, name](bool value) {
            if (!target_spaceship)
                return;
            target_spaceship->commandSetAmmoBlueprintActivation(name, value);
            
        });
        tr.bp_button->setIcon("gui/icons/station-weapons.png");
        tr.bp_button->setTextSize(28)->setSize(40, 40);


        tr.button = new GuiToggleButton(tr.layout, id + "_MW_" + name, name, [this, name](bool value) {
            if (value)
                load_type = name;
            else
                load_type = MW_None;
            for(unsigned int idx = 0; idx < load_type_rows.size(); idx++)
                load_type_rows[idx].button->setValue(string(idx) == load_type);
        });
        tr.button->setTextSize(28)->setSize(150, 40);

        pdi = new GuiPowerDamageIndicator(tr.bp_button, id + "_" + name + "_PDI", SYS_MissileSystem, sp::Alignment::CenterRight, target_spaceship);
        pdi->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

        tr.loading_bar = new GuiProgressbar(tr.layout, id + "_" + name + "_PROGRESS", 0, 1.0, 0);
        tr.loading_bar->setColor(glm::u8vec4(128, 128, 128, 255))->setSize(50, 40);
        tr.loading_label = new GuiLabel(tr.loading_bar, id + "_" + name + "_PROGRESS_LABEL", "", 25);
        tr.loading_label->setAlignment(sp::Alignment::Center)->setSize(50, 50);
    };

    for (int n = MW_Count-1; n >= 0; n--)
    {
        createLoadTypeRow(load_type_rows[n], id, getLocaleMissileWeaponName(EMissileWeapons(n)));
    }
    load_type_rows[MW_Homing].button->setIcon("gui/icons/weapon-homing.png");
    load_type_rows[MW_Mine].button->setIcon("gui/icons/weapon-mine.png");
    load_type_rows[MW_EMP].button->setIcon("gui/icons/weapon-emp.png");
    load_type_rows[MW_Nuke].button->setIcon("gui/icons/weapon-nuke.png");
    load_type_rows[MW_HVLI].button->setIcon("gui/icons/weapon-hvli.png");

    for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
        TypeRow tr;
        createLoadTypeRow(tr, id, kv.first);
        tr.button->setIcon(load_type_rows[kv.second.basetype].button->getIcon());
        load_type_rows.push_back(tr);
    }
}

void GuiMissileTubeControls::setTargetSpaceship(P<PlayerSpaceship> targetSpaceship){
    target_spaceship = targetSpaceship;
    pdi->setTargetSpaceship(target_spaceship);
}

void GuiMissileTubeControls::onUpdate()
{

    if (!target_spaceship || !isVisible())
        return;
    for (int n = 0; n < MW_Count; n++)
    {
        load_type_rows[n].button->setText(getLocaleMissileWeaponName(EMissileWeapons(n)));
        load_type_rows[n].loading_label->setText(string(target_spaceship->weapon_storage[n]) + "/" + string(target_spaceship->weapon_storage_max[n]));
        load_type_rows[n].layout->setVisible(target_spaceship->weapon_storage_max[n] > 0);

        unsigned int cur_ammo = target_spaceship->weapon_storage[n];
        unsigned int max_ammo = target_spaceship->weapon_storage_max[n];
        if(cur_ammo >= max_ammo)
        {
            load_type_rows[n].loading_bar->show();
            load_type_rows[n].loading_bar->setValue(0);
        }
        else
        {
            load_type_rows[n].loading_bar->show();
            load_type_rows[n].loading_bar->setValue(target_spaceship->getAmmoCreationProgression(getLocaleMissileWeaponName(EMissileWeapons(n))));
        }

    }

    unsigned int n = MW_Count;
    for(auto& kv : CustomMissileWeaponRegistry::getCustomMissileWeapons())
    {
        assert(n < load_type_rows.size() && "out of bound for custom weapons");
        load_type_rows[n].button->setText(kv.first);
        load_type_rows[n].loading_label->setText(string(target_spaceship->custom_weapon_storage[kv.first]) + "/" + string(target_spaceship->custom_weapon_storage_max[kv.first]));
        load_type_rows[n].layout->setVisible(target_spaceship->custom_weapon_storage_max[kv.first] > 0);

        unsigned int cur_ammo = target_spaceship->custom_weapon_storage[kv.first];
        unsigned int max_ammo = target_spaceship->custom_weapon_storage_max[kv.first];
        if(cur_ammo >= max_ammo)
        {
            load_type_rows[n].loading_bar->show();
            load_type_rows[n].loading_bar->setValue(0);
        }
        else
        {
            load_type_rows[n].loading_bar->show();
            load_type_rows[n].loading_bar->setValue(target_spaceship->getAmmoCreationProgression(kv.first));
        }

        n++;
    }

    for (int n = 0; n < target_spaceship->weapon_tube_count; n++)
    {
        WeaponTube& tube = target_spaceship->weapon_tube[n];
        rows[n].layout->show();
        if (tube.canOnlyLoad(MW_Mine))
            rows[n].fire_button->setIcon("gui/icons/weapon-mine", sp::Alignment::CenterLeft);
        else
            rows[n].fire_button->setIcon("gui/icons/missile", sp::Alignment::CenterLeft, target_spaceship->getRotation() + tube.getDirection() + 90);
        if(tube.isEmpty())
        {
            rows[n].load_button->setEnable(tube.canLoad(load_type));
            rows[n].load_button->setText(tr("missile","Load"));
            rows[n].fire_button->disable()->show();
            rows[n].fire_button->setText(tube.getTubeName() + ": " + tr("missile","Empty"));
            rows[n].loading_bar->hide();
        }else if(tube.isLoaded())
        {
            rows[n].load_button->enable();
            rows[n].load_button->setText(tr("missile","Unload"));
            rows[n].fire_button->enable()->show();
            rows[n].fire_button->setText(tube.getTubeName() + ": " + getLocaleMissileWeaponName(tube.getLoadType()));
            rows[n].loading_bar->hide();
        }else if(tube.isLoading())
        {
            rows[n].load_button->disable();
            rows[n].load_button->setText(tr("missile","Load"));
            rows[n].fire_button->hide();
            rows[n].fire_button->setText(tube.getTubeName() + ": " + getLocaleMissileWeaponName(tube.getLoadType()));
            rows[n].loading_bar->show();
            rows[n].loading_bar->setValue(tube.getLoadProgress());
            rows[n].loading_label->setText(tr("missile","Loading"));
        }else if(tube.isUnloading())
        {
            rows[n].load_button->disable();
            rows[n].load_button->setText(tr("missile","Unload"));
            rows[n].fire_button->hide();
            rows[n].fire_button->setText(getLocaleMissileWeaponName(tube.getLoadType()));
            rows[n].loading_bar->show();
            rows[n].loading_bar->setValue(tube.getUnloadProgress());
            rows[n].loading_label->setText(tr("missile","Unloading"));
        }else if(tube.isFiring())
        {
            rows[n].load_button->disable();
            rows[n].load_button->setText(tr("missile","Load"));
            rows[n].fire_button->disable()->show();
            rows[n].fire_button->setText(tr("missile","Firing"));
            rows[n].loading_bar->hide();
        }

        if (my_spaceship->current_warp > 0.0f)
        {
            rows[n].fire_button->disable();
        }
    }
    for(int n=target_spaceship->weapon_tube_count; n<max_weapon_tubes; n++)
        rows[n].layout->hide();

    if (keys.weapons_select_homing.getDown())
        selectMissileWeapon(MW_Homing);
    if (keys.weapons_select_nuke.getDown())
        selectMissileWeapon(MW_Nuke);
    if (keys.weapons_select_mine.getDown())
        selectMissileWeapon(MW_Mine);
    if (keys.weapons_select_emp.getDown())
        selectMissileWeapon(MW_EMP);
    if (keys.weapons_select_hvli.getDown())
        selectMissileWeapon(MW_HVLI);

    for(int n=0; n<my_spaceship->weapon_tube_count; n++)
    {
        if (keys.weapons_load_tube[n].getDown())
            my_spaceship->commandLoadTube(n, load_type);
        if (keys.weapons_unload_tube[n].getDown())
            my_spaceship->commandUnloadTube(n);
        if (keys.weapons_fire_tube[n].getDown())
        {
            float target_angle = missile_target_angle;
            if (!manual_aim)
            {
                target_angle = my_spaceship->weapon_tube[n].calculateFiringSolution(my_spaceship->getTarget());
                if (target_angle == std::numeric_limits<float>::infinity())
                    target_angle = my_spaceship->getRotation() + my_spaceship->weapon_tube[n].getDirection();
            }
            my_spaceship->commandFireTube(n, target_angle);
        }
    }

    for(auto &row : load_type_rows)
    {
        if(target_spaceship->isAmmoBlueprintAvailable(row.button->getText()) == false)
        {
            row.bp_button->hide();
        }
        else
        {
            row.bp_button->show();
        }
        row.bp_button->setValue(target_spaceship->isAmmoBlueprintActivated(row.button->getText()));
        
    }

}

void GuiMissileTubeControls::setMissileTargetAngle(float angle)
{
    missile_target_angle = angle;
}

float GuiMissileTubeControls::getMissileTargetAngle()
{
    return missile_target_angle;
}

void GuiMissileTubeControls::setManualAim(bool manual)
{
    manual_aim = manual;
}

bool GuiMissileTubeControls::getManualAim()
{
    return manual_aim;
}

void GuiMissileTubeControls::selectMissileWeapon(EMissileWeapons type)
{
    load_type = type;
    for(int idx = 0; idx < MW_Count; idx++)
        load_type_rows[idx].button->setValue(idx == type);
}
