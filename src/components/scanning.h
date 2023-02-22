#pragma once

#include "ecs/entity.h"
#include <vector>


class ScanState
{
public:
    enum class State {
        NotScanned,
        FriendOrFoeIdentified,
        SimpleScan,
        FullScan
    };

    /*!
     * Scan state per FactionInfo.
     * When the required faction is not in the vector, the scan state
     * is SS_NotScanned
     */
    std::vector<std::pair<sp::ecs::Entity, ScanState::State>> per_faction;

    bool allow_simple_scan = false; // Does the first scan go to a full scan or a simple scan.
    int complexity = -1; //Amount of bars each minigame has (-1 for default)
    int depth = -1;      //Amount of minigames that need to be finished (-1 for default)

    State getStateFor(sp::ecs::Entity entity);
    void setStateFor(sp::ecs::Entity entity, State state);
};

class ScienceScanner
{
public:
    float delay = 0.0f; // When a delay based scan is done, this will count down.
    sp::ecs::Entity target;
};