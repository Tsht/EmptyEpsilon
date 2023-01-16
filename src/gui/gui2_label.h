#ifndef GUI2_LABEL_H
#define GUI2_LABEL_H

#include "gui2_element.h"

class GuiLabel : public GuiElement
{
protected:
    string text;
    float text_size;
    sf::Color text_color;
    sp::Alignment text_alignment;
    bool background;
    bool bold;
    bool vertical;
public:
    GuiLabel(GuiContainer* owner, string id, string text, float text_size);

    virtual void onDraw(sp::RenderTarget& target);

    GuiLabel* setText(string text);
    string getText() const;
    GuiLabel* setAlignment(sp::Alignment alignment);
    GuiLabel* addBackground();
    GuiLabel* setVertical();
    GuiLabel* setHorizontal();
    GuiLabel* setBold(bool bold=true);
};

#endif//GUI2_LABEL_H
