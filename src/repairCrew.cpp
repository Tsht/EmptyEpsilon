#include "repairCrew.h"
#include "gameGlobalInfo.h"
#include "random.h"
#include "multiplayer_client.h"
#include "multiplayer_server.h"
#include <SDL_assert.h>

const static int16_t CMD_SET_TARGET_POSITION = 0x0000;

PVector<RepairCrew> repairCrewList;

REGISTER_MULTIPLAYER_CLASS(RepairCrew, "RepairCrew");
RepairCrew::RepairCrew()
: MultiplayerObject("RepairCrew")
{
    ship_id = -1;
    position = {-1, -1};
    target_position = {0, 0};
    action = RC_Idle;
    direction = ERepairCrewDirection(irandom(RC_Up, RC_Right + 1));

    selected = false;

    registerMemberReplication(&ship_id);
    registerMemberReplication(&position, 1.0);
    registerMemberReplication(&target_position);

    repairCrewList.push_back(this);
}

//due to a suspected compiler bug this deconstructor needs to be explicitly defined
RepairCrew::~RepairCrew()
{
}

/* struct PathNode
{
    ERepairCrewDirection arrive_direction;
    bool right, down;
};
*/

class PathMap {
    class PathNode {
    public:
        ERepairCrewDirection arrive_direction;
        bool right, down;
        PathNode() : arrive_direction(RC_None), right(false), down(false) { };
    };
private:
    std::vector<PathNode> pathMap;
public:
    int width;
    int height;

    PathMap(const glm::ivec2 &size) : pathMap(size.x*size.y), width(size.x), height(size.y) {
        // init all interior down/right doors (not right or bottom edge)
        for (int x = 0; x < width-1; x++) {
            for (int y = 0; y < height-1; y++) {
                Node(x, y).down = true;
                Node(x, y).right = true;
            }
        }
    }

    inline PathNode& Node(int x, int y) {
        SDL_assert(x >= 0 && x < width && y >= 0 && y < height);
        return pathMap[y*width + x];
    }
};

ERepairCrewDirection pathFind(glm::ivec2 start_pos, glm::ivec2 target_pos, P<ShipTemplate> t)
{
    PathMap map(t->interiorSize());

    for(unsigned int n=0; n<t->rooms.size(); n++)
    {
        const ShipRoomTemplate &room = t->rooms[n];
        for(int x=0; x<room.size.x; x++)
        {
            map.Node(room.position.x + x,room.position.y - 1).down = false;
            map.Node(room.position.x + x,room.position.y + room.size.y - 1).down = false;
        }
        for(int y=0; y<room.size.y; y++)
        {
            map.Node(room.position.x - 1,room.position.y + y).right = false;
            map.Node(room.position.x + room.size.x - 1,room.position.y + y).right = false;
        }
    }
    for(unsigned int n=0; n<t->doors.size(); n++)
    {
        const ShipDoorTemplate &door = t->doors[n];
        if (door.horizontal)
        {
            map.Node(door.position.x,door.position.y - 1).down = true;
        }else{
            map.Node(door.position.x - 1,door.position.y).right = true;
        }
    }

    std::vector<glm::ivec2> search_points;
    search_points.push_back(start_pos);
    while(search_points.size() > 0)
    {
        glm::ivec2 pos = search_points[0];
        if (pos == target_pos)
            return map.Node(pos.x,pos.y).arrive_direction;
        search_points.erase(search_points.begin());

        if (map.Node(pos.x,pos.y).right && map.Node(pos.x + 1,pos.y).arrive_direction == RC_None)
        {
            map.Node(pos.x + 1,pos.y).arrive_direction = map.Node(pos.x,pos.y).arrive_direction;
            if (map.Node(pos.x + 1,pos.y).arrive_direction == RC_None) map.Node(pos.x + 1,pos.y).arrive_direction = RC_Right;
            search_points.push_back(glm::ivec2(pos.x + 1, pos.y));
        }
        if (pos.x > 0 && map.Node(pos.x - 1,pos.y).right && map.Node(pos.x - 1,pos.y).arrive_direction == RC_None)
        {
            map.Node(pos.x - 1,pos.y).arrive_direction = map.Node(pos.x,pos.y).arrive_direction;
            if (map.Node(pos.x - 1,pos.y).arrive_direction == RC_None) map.Node(pos.x - 1,pos.y).arrive_direction = RC_Left;
            search_points.push_back(glm::ivec2(pos.x - 1, pos.y));
        }
        if (map.Node(pos.x,pos.y).down && map.Node(pos.x,pos.y + 1).arrive_direction == RC_None)
        {
            map.Node(pos.x,pos.y + 1).arrive_direction = map.Node(pos.x,pos.y).arrive_direction;
            if (map.Node(pos.x,pos.y + 1).arrive_direction == RC_None) map.Node(pos.x,pos.y + 1).arrive_direction = RC_Down;
            search_points.push_back(glm::ivec2(pos.x, pos.y + 1));
        }
        if (pos.y > 0 && map.Node(pos.x,pos.y - 1).down && map.Node(pos.x,pos.y - 1).arrive_direction == RC_None)
        {
            map.Node(pos.x,pos.y - 1).arrive_direction = map.Node(pos.x,pos.y).arrive_direction;
            if (map.Node(pos.x,pos.y - 1).arrive_direction == RC_None) map.Node(pos.x,pos.y - 1).arrive_direction = RC_Up;
            search_points.push_back(glm::ivec2(pos.x, pos.y - 1));
        }
    }

    return RC_None;
}

void RepairCrew::update(float delta)
{
    if (gameGlobalInfo->use_nano_repair_crew)
    {
        destroy();
        return;
    }    

    P<PlayerSpaceship> ship;
    if (game_server)
        ship = game_server->getObjectById(ship_id);
    else if (game_client)
        ship = game_client->getObjectById(ship_id);
    else
    {
        destroy();
        return;
    }


    if (game_server && !ship)
    {
        destroy();
        return;
    }
    if (!ship || !ship->ship_template)
        return;

    if (ship->ship_template->rooms.size() == 0)
    {
        destroy();
        return;
    }

    if (position.x < -0.5f)
    {
        ship->ship_template->interiorSize();
        if(ship->ship_template->rooms.size() != 0)
        {
            int n=irandom(0, ship->ship_template->rooms.size() - 1);
            position.x = ship->ship_template->rooms[n].position.x + irandom(0, ship->ship_template->rooms[n].size.x - 1);
            position.y = ship->ship_template->rooms[n].position.y + irandom(0, ship->ship_template->rooms[n].size.y - 1);
            target_position = glm::ivec2(position.x, position.y);
        }
    }

    action_delay -= delta;
    glm::ivec2 pos = glm::ivec2(position.x + 0.5f, position.y + 0.5f);
    switch(action)
    {
    case RC_Idle:
        {
            action_delay = 1.0f / move_speed;
            if (pos != target_position)
            {
                ERepairCrewDirection new_direction = pathFind(pos, target_position, ship->ship_template);
                if (new_direction != RC_None)
                {
                    action = RC_Move;
                    direction = new_direction;
                }
            }
            position = glm::vec2{pos.x, pos.y};

            ESystem system = ship->ship_template->getSystemAtRoom(pos);
            if (system != SYS_None)
            {
                ship->systems[system].health += repair_per_second * delta;
                if (ship->systems[system].health > 1.0f)
                    ship->systems[system].health = 1.0;
                // ship->systems[system].hacked_level -= repair_per_second * delta; //Tsht : desactivation pour le moment, a mettre sous option
                // if (ship->systems[system].hacked_level < 0.0f)
                //     ship->systems[system].hacked_level = 0.0f;
            }
            else if (ship->docking_state == DS_Docked)
            {
                float hull_old = ship->hull_strength / ship->hull_max;
                ship->hull_strength += repair_per_second * delta * 20;
                float hull_new = ship->hull_strength / ship->hull_max;

                if (hull_old <= 0.2f && hull_new > 0.2f)
                    ship->hull_strength = ship->hull_max * 0.2f;
                else if (hull_old <= 0.4f && hull_new > 0.4f)
                    ship->hull_strength = ship->hull_max * 0.4f;
                else if (hull_old <= 0.6f && hull_new > 0.6f)
                    ship->hull_strength = ship->hull_max * 0.6f;
                else if (hull_old <= 0.8f && hull_new > 0.8f)
                    ship->hull_strength = ship->hull_max * 0.8f;
                else if (hull_new > 1.0f)
                    ship->hull_strength = ship->hull_max;
            }
            if (ship->auto_repair_enabled && pos == target_position && (system == SYS_None || !ship->hasSystem(system) || ship->systems[system].health >= ship->systems[system].health_max))
            {
                int n=irandom(0, SYS_COUNT - 1);

                if (ship->hasSystem(ESystem(n)) && ship->systems[n].health < ship->systems[system].health_max)
                {
                    for(unsigned int idx=0; idx<ship->ship_template->rooms.size(); idx++)
                    {
                        if (ship->ship_template->rooms[idx].system == ESystem(n))
                        {
                            target_position = ship->ship_template->rooms[idx].position + glm::ivec2(irandom(0, ship->ship_template->rooms[idx].size.x - 1), irandom(0, ship->ship_template->rooms[idx].size.y - 1));
                        }
                    }
                }
            }
        }
        break;
    case RC_Move:
        switch(direction)
        {
        case RC_None: break;
        case RC_Left: position.x -= delta * move_speed; break;
        case RC_Right: position.x += delta * move_speed; break;
        case RC_Up: position.y -= delta * move_speed; break;
        case RC_Down: position.y += delta * move_speed; break;
        }
        if (action_delay < 0.0f)
            action = RC_Idle;
        break;
    }
}

void RepairCrew::onReceiveClientCommand(int32_t client_id, sp::io::DataBuffer& packet)
{
    int16_t command;
    packet >> command;
    switch(command)
    {
    case CMD_SET_TARGET_POSITION:
        {
            glm::ivec2 pos;
            packet >> pos;
            if (!isTargetPositionTaken(pos))
            {
                target_position = pos;
            }
        }
        break;
    }
}

void RepairCrew::commandSetTargetPosition(glm::ivec2 position)
{
    sp::io::DataBuffer packet;
    packet << CMD_SET_TARGET_POSITION << position;
    sendClientCommand(packet);
}

bool RepairCrew::isTargetPositionTaken(glm::ivec2 pos)
{
    foreach(RepairCrew, c, repairCrewList)
    {
        if (c->ship_id == ship_id && c->target_position == pos)
            return true;
    }
    return false;
}

PVector<RepairCrew> getRepairCrewFor(P<PlayerSpaceship> ship)
{
    PVector<RepairCrew> ret;
    if (!ship)
        return ret;

    foreach(RepairCrew, c, repairCrewList)
        if (c->ship_id == ship->getMultiplayerId())
            ret.push_back(c);
    return ret;
}
