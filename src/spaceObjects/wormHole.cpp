#include <graphics/opengl.h>
#include <glm/gtc/type_ptr.hpp>

#include "main.h"
#include "random.h"
#include "wormHole.h"
#include "spaceship.h"
#include "scriptInterface.h"
#include "textureManager.h"

#include "glObjects.h"
#include "shaderRegistry.h"

#include <glm/ext/matrix_transform.hpp>

#define FORCE_MULTIPLIER          50.0f
#define FORCE_MAX                 10000.0f
#define ALPHA_MULTIPLIER          10.0f
#define DEFAULT_COLLISION_RADIUS  2500.0f
#define AVOIDANCE_MULTIPLIER      1.2f
#define TARGET_SPREAD             500.0f

struct VertexAndTexCoords
{
    glm::vec3 vertex;
    glm::vec2 texcoords;
};

/// A WormHole is a piece of space terrain that pulls all nearby SpaceObjects within a 5U radius, including otherwise immobile objects like SpaceStations, toward its center.
/// Any SpaceObject that reaches its center is teleported to another point in space.
/// AI behaviors avoid WormHoles by a 2U margin.
/// Example: wormhole = WormHole():setPosition(1000,1000):setTargetPosition(10000,10000)
REGISTER_SCRIPT_SUBCLASS(WormHole, SpaceObject)
{
    /// Sets the target teleportation coordinates for SpaceObjects that pass through the center of this WormHole.
    /// Example: wormhole:setTargetPosition(10000,10000)
    REGISTER_SCRIPT_CLASS_FUNCTION(WormHole, setTargetPosition);
    /// Returns the target teleportation coordinates for SpaceObjects that pass through the center of this WormHole.
    /// Example: wormhole:getTargetPosition()
    REGISTER_SCRIPT_CLASS_FUNCTION(WormHole, getTargetPosition);
    /// Defines a function to call when this WormHole teleports a SpaceObject.
    /// Passes the WormHole object and the teleported SpaceObject.
    /// Example:
    /// -- Outputs teleportation details to the console window and logging file
    /// wormhole:onTeleportation(function(this_wormhole,teleported_object) print(teleported_object:getCallSign() .. " teleported to " .. this_wormhole:getTargetPosition()) end)
    REGISTER_SCRIPT_CLASS_FUNCTION(WormHole, onTeleportation);
}

REGISTER_MULTIPLAYER_CLASS(WormHole, "WormHole");
WormHole::WormHole()
: SpaceObject(DEFAULT_COLLISION_RADIUS, "WormHole")
{
    pathPlanner = PathPlannerManager::getInstance();
    pathPlanner->addAvoidObject(this, (DEFAULT_COLLISION_RADIUS * AVOIDANCE_MULTIPLIER) );

    setRadarSignatureInfo(0.9, 0.0, 0.0);
}

void WormHole::draw3DTransparent()
{
   static std::array<VertexAndTexCoords, 4> quad{
        glm::vec3{}, {0.f, 1.f},
        glm::vec3{}, {1.f, 1.f},
        glm::vec3{}, {1.f, 0.f},
        glm::vec3{}, {0.f, 0.f}
    };

    textureManager.getTexture("wormHole3d.png")->bind();
    ShaderRegistry::ScopedShader shader(ShaderRegistry::Shaders::Billboard);

    auto model_matrix = getModelMatrix();
    auto modeldata_matrix = glm::rotate(model_matrix, glm::radians(120.f), {1.f, 0.f, 0.f});

    glUniformMatrix4fv(shader.get().uniform(ShaderRegistry::Uniforms::Model), 1, GL_FALSE, glm::value_ptr(modeldata_matrix));
    glUniform4f(shader.get().uniform(ShaderRegistry::Uniforms::Color), 1.f, 1.f, 1.f, 5000.f);
    gl::ScopedVertexAttribArray positions(shader.get().attribute(ShaderRegistry::Attributes::Position));
    gl::ScopedVertexAttribArray texcoords(shader.get().attribute(ShaderRegistry::Attributes::Texcoords));



    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glVertexAttribPointer(positions.get(), 3, GL_FLOAT, GL_FALSE, sizeof(VertexAndTexCoords), (GLvoid*)quad.data());
    glVertexAttribPointer(texcoords.get(), 2, GL_FLOAT, GL_FALSE, sizeof(VertexAndTexCoords), (GLvoid*)((char*)quad.data() + sizeof(glm::vec3)));

    std::initializer_list<uint16_t> indices = { 0, 2, 1, 0, 3, 2 };
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, std::begin(indices));
    glBlendFunc(GL_ONE, GL_ONE);
}

void WormHole::drawOnRadar(sp::RenderTarget& renderer, glm::vec2 position, float scale, float rotation, bool long_range)
{
    renderer.drawRotatedSpriteBlendAdd("radar/wormHole.png", position, getRadius() * scale * 3.0f, getRotation() - rotation);
}

// Draw a line toward the target position
void WormHole::drawOnGMRadar(sp::RenderTarget& renderer, glm::vec2 position, float scale, float rotation, bool long_range)
{
    auto offset = target_position - getPosition();
    renderer.drawLine(position, position + glm::vec2(offset.x, offset.y) * scale, glm::u8vec4(255, 255, 255, 32));

    renderer.drawCircleOutline(position, getRadius() * scale, 2.0, glm::u8vec4(255, 255, 255, 32));
}


void WormHole::update(float delta)
{
    update_delta = delta;
}

void WormHole::collide(Collisionable* target, float collision_force)
{
    if (update_delta == 0.0f)
        return;

    if (getRadarSignatureGravity() < 0.15f)
        return;

    P<SpaceObject> obj = P<Collisionable>(target);
    if (!obj) return;
    if (!obj->hasWeight()) { return; } // the object is not affected by gravitation
    if (P<Nebula>(obj))
        return;

    auto diff = getPosition() - target->getPosition();
    float distance = glm::length(diff);
    float force = (getRadius() * getRadius() * FORCE_MULTIPLIER) / (distance * distance);
    P<SpaceShip> spaceship = P<Collisionable>(target);

    // Warp postprocessor-alpha is calculated using alpha = (1 - (delay/10))
    if (spaceship)
        spaceship->wormhole_alpha = ((distance / getRadius()) * ALPHA_MULTIPLIER);

    if (force > FORCE_MAX)
    {
        force = FORCE_MAX;
        if (isServer())
            target->setPosition( (target_position +
                                  glm::vec2(random(-TARGET_SPREAD, TARGET_SPREAD), random(-TARGET_SPREAD, TARGET_SPREAD))));
        if (on_teleportation.isSet())
        {
            on_teleportation.call<void>(P<WormHole>(this), obj);
        }
        if (spaceship)
        {
            spaceship->wormhole_alpha = 0.0;
        }
    }

    // TODO: Escaping is impossible. Change setPosition to something Newtonianish.
    target->setPosition(target->getPosition() + diff / distance * update_delta * force);
}

void WormHole::setTargetPosition(glm::vec2 v)
{
    target_position = v;
}

glm::vec2 WormHole::getTargetPosition()
{
    return target_position;
}

void WormHole::onTeleportation(ScriptSimpleCallback callback)
{
    this->on_teleportation = callback;
}
