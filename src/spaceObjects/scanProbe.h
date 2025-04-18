#ifndef SCAN_PROBE_H
#define SCAN_PROBE_H

#include "spaceObject.h"
#include "multiplayer_server.h"

class ScanProbe : public SpaceObject, public Updatable
{
private:
    bool moving;
    // Probe flight speed; 1U/sec.
    float probe_speed;
    // Remaining lifetime in seconds.
    float lifetime;
    // Probe target coordinates.
    glm::vec2 target_position{0, 0};
    // Whether the probe has arrived to the target_position.
    bool has_arrived;
public:
    int owner_id;
    P<SpaceObject> owner;

    ScriptSimpleCallback on_expiration;
    ScriptSimpleCallback on_destruction;

    ScriptSimpleCallback on_arrival;

    ScanProbe();
    virtual ~ScanProbe();

    void setSpeed(float probe_speed);
    float getSpeed();
    void setLifetime(float lifetime);
    float getLifetime();

    virtual void update(float delta) override;
    virtual bool canBeTargetedBy(P<SpaceObject> other) override;
    virtual void takeDamage(float damage_amount, DamageInfo info) override;
    virtual void drawOnRadar(sp::RenderTarget& renderer, glm::vec2 position, float scale, float rotation, bool long_range) override;
    virtual void collide(Collisionable* target, float force) override;
    virtual void drawOnGMRadar(sp::RenderTarget& window, glm::vec2 position, float scale, float rotation, bool long_range) override;
    bool hasArrived() { return has_arrived; }
    void setTarget(glm::vec2 target) { target_position = target; }
    glm::vec2 getTarget() { return target_position; }
    P<SpaceObject> getOwner() { return game_server ? game_server->getObjectById(owner_id) : nullptr; }
    void setOwner(P<SpaceObject> owner);

    void onArrival(ScriptSimpleCallback callback);
    void onExpiration(ScriptSimpleCallback callback);
    void onDestruction(ScriptSimpleCallback callback);
};

#endif//SCAN_PROBE_H
