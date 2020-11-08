#include <SFML/OpenGL.hpp>
#include "scanProbe.h"
#include "explosionEffect.h"
#include "main.h"
#include "playerSpaceship.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_SUBCLASS_NO_CREATE(ScanProbe, SpaceObject)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(ScanProbe, setTarget);
    // Callback when the probe's lifetime expires.
    // Returns the probe.
    // Example: probe:onExpiration(probeExpired)
    REGISTER_SCRIPT_CLASS_FUNCTION(ScanProbe, onExpiration);
    // Callback when the probe is destroyed by damage.
    // Returns the probe and instigator.
    // Example: probe:onDestruction(probeDestroyed)
    REGISTER_SCRIPT_CLASS_FUNCTION(ScanProbe, onDestruction);
}

REGISTER_MULTIPLAYER_CLASS(ScanProbe, "ScanProbe");
ScanProbe::ScanProbe()
: SpaceObject(100, "ScanProbe")
{
    lifetime = 60 * 60;
    hull = 2;
    moving = true;

    registerMemberReplication(&owner_id);
    registerMemberReplication(&target_position);
    registerMemberReplication(&lifetime, 60.0);
    setRadarSignatureInfo(0.0, 0.2, 0);

    switch(irandom(1, 3))
    {
    case 1:
        model_info.setData("SensorBuoyMKI");
        break;
    case 2:
        model_info.setData("SensorBuoyMKII");
        break;
    default:
        model_info.setData("SensorBuoyMKIII");
        break;
    }

    setCallSign(string(getMultiplayerId()) + "P");
}

ScanProbe::~ScanProbe()
{
}

void ScanProbe::update(float delta)
{
    lifetime -= delta;
    if (lifetime <= 0.0)
    {
        if (on_expiration.isSet())
            on_expiration.call(P<ScanProbe>(this));

        destroy();
    }
    if ((target_position - getPosition()) > getRadius() && moving)
    {
        sf::Vector2f v = normalize(target_position - getPosition());
        setPosition(getPosition() + v * delta * probe_speed);
    }
    else
        moving = false;
}

void ScanProbe::collide(Collisionable* target, float force)
{
    if (!game_server)
        return;
    P<PlayerSpaceship> player = P<Collisionable>(target);
    if (!player)
        return;
    if (player -> getMultiplayerId() != owner_id)
        return;
    if ((getTarget() - getPosition()) > getRadius())
        return;
    player -> scan_probe_stock = player -> scan_probe_stock + 1;
    destroy();
}

bool ScanProbe::canBeTargetedBy(P<SpaceObject> other)
{
    return (getTarget() - getPosition()) < getRadius();
}

void ScanProbe::takeDamage(float damage_amount, DamageInfo info)
{
    if (on_destruction.isSet())
    {
        if (info.instigator)
        {
            on_destruction.call(P<ScanProbe>(this), P<SpaceObject>(info.instigator));
        } else {
            on_destruction.call(P<ScanProbe>(this));
        }
    }
    destroy();
}

void ScanProbe::drawOnRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range)
{
    sf::Sprite object_sprite;
    textureManager.setTexture(object_sprite, "ProbeBlip.png");
    object_sprite.setPosition(position);
    object_sprite.setColor(sf::Color(96, 192, 128));
    float size = 0.3;
    object_sprite.setScale(size, size);
    window.draw(object_sprite);

    if (long_range && moving)
    {
        sf::VertexArray a(sf::Lines, 2);
        a[0].position = position;
        a[1].position = position + (target_position - getPosition()) * scale;
        a[0].color = sf::Color(255, 255, 255, 32);
        float distance = sf::length(position - target_position);
        if (distance > 1000.0)
            window.draw(a);
    }
}

void ScanProbe::setOwner(P<SpaceObject> owner)
{
    if (!owner) return;

    setFactionId(owner->getFactionId());
    owner_id = owner->getMultiplayerId();
}

void ScanProbe::onDestruction(ScriptSimpleCallback callback)
{
    this->on_destruction = callback;
}

void ScanProbe::onExpiration(ScriptSimpleCallback callback)
{
    this->on_expiration = callback;
}

