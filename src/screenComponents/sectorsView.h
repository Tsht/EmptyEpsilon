#ifndef SECTORS_VIEW_H
#define SECTORS_VIEW_H

#include "gui/gui2_element.h"

class TargetsContainer;
class SectorsView : public GuiElement
{
    public:
        typedef std::function<void(glm::vec2 position)> pfunc_t;
        typedef std::function<void(float position)> ffunc_t;
        static const int grid_scale_size = 5;

    private:
        sf::Color grid_colors[SectorsView::grid_scale_size];
        const float sub_sectors_count = 8;
        float distance;
        glm::vec2 view_position;
        float view_rotation;

        TargetsContainer *targets;
        pfunc_t mouse_down_func;
        pfunc_t mouse_drag_func;
        pfunc_t mouse_up_func;
    public:
        SectorsView(GuiContainer *owner, string id, float distance, TargetsContainer *targets);
        virtual SectorsView *setDistance(float distance)
        {
            this->distance = distance;
            return this;
        }
        float getDistance() { return distance; }
        virtual SectorsView* setViewPosition(glm::vec2 view_position)
        {
            this->view_position = view_position;
            return this;
        }
        virtual glm::vec2 getViewPosition() { return view_position; }
        virtual void setViewRotation(float view_rotation) 
        { 
            this->view_rotation = view_rotation; 
        }
        virtual float getViewRotation() { return view_rotation; }
        virtual sf::Vector2f worldToScreen(glm::vec2 world_position);
        virtual glm::vec2 screenToWorld(sf::Vector2f screen_position);
        virtual float getScale() { return std::min(rect.width, rect.height) / 2.0f / distance; };
        void drawSectorGrid(sf::RenderTarget &window);
        virtual bool onMouseDown(sf::Vector2f position);
        virtual void onMouseDrag(sf::Vector2f position);
        virtual void onMouseUp(sf::Vector2f position);
        virtual SectorsView *setCallbacks(pfunc_t mouse_down_func, pfunc_t mouse_drag_func, pfunc_t mouse_up_func)
        {
            this->mouse_down_func = mouse_down_func;
            this->mouse_drag_func = mouse_drag_func;
            this->mouse_up_func = mouse_up_func;
            return this;
        }
    protected:
        TargetsContainer * getTargets(){return targets;};
        void drawTargets(sf::RenderTarget &window);
        void drawTerrain(sf::RenderTarget &window);
    private:
        int calcGridScaleMagnitude(int scale_magnitude, int position);
};
 #endif //SECTORS_VIEW_H