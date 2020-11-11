#include <i18n.h>
#include "databaseView.h"
#include "scienceDatabase.h"

#include "gui/gui2_listbox.h"
#include "gui/gui2_image.h"
#include "gui/gui2_autolayout.h"
#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_scrolltext.h"

#include "screenComponents/rotatingModelView.h"

DatabaseViewComponent::DatabaseViewComponent(GuiContainer* owner)
: GuiElement(owner, "DATABASE_VIEW")
{
    database_entry = nullptr;
    selected_entry = nullptr;

    item_list = new GuiListbox(this, "DATABASE_ITEM_LIST", [this](int index, string value) {
        P<ScienceDatabase> entry;

        unsigned int id = std::stoul(value, nullptr, 10);
        selected_entry = findEntryById(id);
        display();
    });
    item_list->setPosition(0, 0, ATopLeft)->setMargins(20, 20, 20, 130)->setSize(navigation_width, GuiElement::GuiSizeMax);
    display();
}

P<ScienceDatabase> DatabaseViewComponent::findEntryById(unsigned int id)
{
    if (id == 0)
    {
        return nullptr;
    }
    foreach(ScienceDatabase, sd, ScienceDatabase::science_databases)
    {
        if (sd->getId() == id)
        {
            return sd;
        }
    }
    return nullptr;
}

bool DatabaseViewComponent::findAndDisplayEntry(string name)
{
    foreach(ScienceDatabase, sd, ScienceDatabase::science_databases)
    {
        if (sd->getName() == name)
        {
            selected_entry = sd;
            display();
            return true;
        }
    }
    return false;
}

void DatabaseViewComponent::fillListBox()
{
    item_list->setOptions({});
    item_list->setSelectionIndex(-1);

    // indices of child or sibling pages in the science_databases vector
    std::vector<unsigned> children_idx;
    std::vector<unsigned> siblings_idx;
    P<ScienceDatabase> parent_entry;

    for (unsigned idx=0; idx<ScienceDatabase::science_databases.size(); idx++)
    {
        P<ScienceDatabase> sd = ScienceDatabase::science_databases[idx];
        if (!sd) continue;

        if(selected_entry)
        {
            if(sd->getId() == selected_entry->getParentId())
            {
                parent_entry = sd;
            }
            if(sd->getParentId() == selected_entry->getParentId())
            {
                siblings_idx.push_back(idx);
            }
            if(sd->getParentId() == selected_entry->getId())
            {
                children_idx.push_back(idx);
            }
        }
        else
        {
            if(sd->getParentId() == 0)
            {
                siblings_idx.push_back(idx);
            }
        }
    }

    if(selected_entry)
    {
        if (children_idx.size() != 0)
        {
            item_list->addEntry(tr("Back"), std::to_string(selected_entry->getParentId()));
        }
        else if(parent_entry)
        {
            item_list->addEntry(tr("Back"), std::to_string(parent_entry->getParentId()));
        }
    }
    if(children_idx.size() > 0)
    {
        for (unsigned i=0; i<children_idx.size(); i++)
        {
            P<ScienceDatabase> sd = ScienceDatabase::science_databases[children_idx[i]];
            item_list->addEntry(sd->getName(), std::to_string(sd->getId()));
        }
    }
    else if(siblings_idx.size() > 0)
    {
        for (unsigned i=0; i<siblings_idx.size(); i++)
        {
            P<ScienceDatabase> sd = ScienceDatabase::science_databases[siblings_idx[i]];
            int item_list_idx = item_list->addEntry(sd->getName(), std::to_string(sd->getId()));
            if (selected_entry && selected_entry->getId() == sd->getId())
            {
                item_list->setSelectionIndex(item_list_idx);
            }
        }
    }
}

void DatabaseViewComponent::display()
{
    if (database_entry)
        database_entry->destroy();

    database_entry = new GuiElement(this, "DATABASE_ENTRY");
    database_entry->setPosition(navigation_width, 0, ATopLeft)->setMargins(20)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    fillListBox();

    if (!selected_entry)
        return;

    bool has_key_values = selected_entry->keyValuePairs.size() > 0;
    bool has_image_or_model = selected_entry->hasModelData() || selected_entry->image != "";
    bool has_text = selected_entry->longDescription.length() > 0;

    int left_column_width = 0;
    if (has_key_values)
    {
        left_column_width = 400;
    }
    GuiAutoLayout* right = new GuiAutoLayout(database_entry, "DATABASE_ENTRY_RIGHT", GuiAutoLayout::LayoutHorizontalRows);
    right->setPosition(left_column_width, 0, ATopLeft)->setMargins(0)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    if (has_image_or_model)
    {
        GuiElement* visual = (new GuiElement(right, "DATABASE_VISUAL_ELEMENT"))->setMargins(0, 0, 0, 40)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

        if (selected_entry->hasModelData())
        {
            (new GuiRotatingModelView(visual, "DATABASE_MODEL_VIEW", selected_entry->getModelData()))->setMargins(-100, -50)->setSize(GuiElement::GuiSizeMax, has_text ? GuiElement::GuiSizeMax : 450);
        }

        if(selected_entry->image != "")
        {
            (new GuiImage(visual, "DATABASE_IMAGE", selected_entry->image))->setScaleUp(false)->setMargins(0)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
        }
    }
    if (has_text)
    {
        if (!has_image_or_model)
        {
            // make sure station and main screen buttons don't overlay the text
            if (!has_key_values)
            {
                right->setMargins(0, 10, 270, 0);
            } else {
                right->setMargins(0, 120, 0, 0);
            }
        }
        (new GuiScrollText(right, "DATABASE_LONG_DESCRIPTION", selected_entry->longDescription))->setTextSize(24)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    }

    // we render the left column second so it overlays the rotating 3D model
    if (has_key_values)
    {
        GuiAutoLayout* left = new GuiAutoLayout(database_entry, "DATABASE_ENTRY_LEFT", GuiAutoLayout::LayoutVerticalTopToBottom);
        left->setPosition(0, 0, ATopLeft)->setMargins(0, 0, 20, 0)->setSize(left_column_width, GuiElement::GuiSizeMax);

        for(unsigned int n=0; n<selected_entry->keyValuePairs.size(); n++)
        {
            (new GuiKeyValueDisplay(left, "", 0.37, selected_entry->keyValuePairs[n].key, selected_entry->keyValuePairs[n].value))->setSize(GuiElement::GuiSizeMax, 40);
        }
    }
}
