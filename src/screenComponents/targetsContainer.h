#ifndef TARGETS_CONTAINER_H
#define TARGETS_CONTAINER_H

#include "spaceObjects/spaceObject.h"

class PlayerSpaceship;
class TargetsContainer
{
private:
    PVector<SpaceObject> entries;
    bool allow_waypoint_selection;
    int waypoint_selection_index;
    glm::vec2 waypoint_selection_position{};

public:
    enum ESelectionType
    {
        Targetable,
        Selectable
    };

    TargetsContainer();
    //P<PlayerSpaceship>& target_spaceship;

    void setAllowWaypointSelection() { allow_waypoint_selection = true; }

    void clear();
    void add(P<SpaceObject> obj);
    void set(P<SpaceObject> obj);
    void set(PVector<SpaceObject> objs);
    PVector<SpaceObject> getTargets() { entries.update(); return entries; }
    P<SpaceObject> get() { entries.update(); if (entries.size() > 0) return entries[0]; return nullptr; }
    int getWaypointIndex();
    void setWaypointIndex(int index);
    glm::vec2 getWaypointPosition();

    void setToClosestTo(glm::vec2 position, float max_range, ESelectionType selection_type, P<PlayerSpaceship> target_spaceship);
};

#endif//TARGETS_CONTAINER_H
