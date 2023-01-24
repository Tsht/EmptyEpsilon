#ifndef GUI2_OVERLAY_H
#define GUI2_OVERLAY_H

#include "gui2_element.h"

class GuiOverlay : public GuiElement
{
private:
    glm::u8vec4 color;
    enum ETextureMode
    {
        TM_None,
        TM_Tiled,
    } texture_mode;
    string texture;
    bool blocking;
public:
    GuiOverlay(GuiContainer* owner, string id, glm::u8vec4 color);

    virtual void onDraw(sp::RenderTarget& target) override;

    GuiOverlay* setColor(glm::u8vec4 color);
    GuiOverlay* setBlocking(bool blocking){ this->blocking = blocking; return this;}
    GuiOverlay* setAlpha(int alpha);
    GuiOverlay* setTextureTiled(string texture);
    GuiOverlay* setTextureNone();
    virtual bool onMouseDown(sp::io::Pointer::Button button, glm::vec2 position, int id);

};

#endif//GUI2_OVERLAY_H
