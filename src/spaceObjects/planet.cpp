#include <graphics/opengl.h>
#include "planet.h"
#include "main.h"
#include "pathPlanner.h"
#include "random.h"

#include "scriptInterface.h"
#include "glObjects.h"
#include "shaderRegistry.h"
#include "textureManager.h"
#include "multiplayer_server.h"
#include "multiplayer_client.h"

#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <i18n.h>

struct VertexAndTexCoords
{
    glm::vec3 vertex;
    glm::vec2 texcoords;
};

static Mesh* planet_mesh[16];

class PlanetMeshGenerator
{
public:
    std::vector<MeshVertex> vertices;
    int max_iterations;

    PlanetMeshGenerator(int iterations)
    {
        max_iterations = iterations;

        createFace(0, glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec2(0, 0.5), glm::vec2(0.25, 0.5));
        createFace(0, glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0,-1, 0), glm::vec2(0.25, 1), glm::vec2(0.25, 0.5), glm::vec2(0.5, 0.5));
        createFace(0, glm::vec3(0, 0, 1), glm::vec3(0,-1, 0), glm::vec3(-1, 0, 0), glm::vec2(0.5, 1), glm::vec2(0.5, 0.5), glm::vec2(0.75, 0.5));
        createFace(0, glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0), glm::vec2(0.75, 1), glm::vec2(0.75, 0.5), glm::vec2(1.0, 0.5));

        createFace(0, glm::vec3(0, 0,-1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec2(0.25, 0.5), glm::vec2(0.0, 0.5));
        createFace(0, glm::vec3(0, 0,-1), glm::vec3(0,-1, 0), glm::vec3(1, 0, 0), glm::vec2(0.25, 0), glm::vec2(0.5, 0.5), glm::vec2(0.25, 0.5));
        createFace(0, glm::vec3(0, 0,-1), glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0), glm::vec2(0.5, 0), glm::vec2(0.75, 0.5), glm::vec2(0.5, 0.5));
        createFace(0, glm::vec3(0, 0,-1), glm::vec3(0,1, 0), glm::vec3(-1, 0, 0), glm::vec2(0.75, 0), glm::vec2(1.0, 0.5), glm::vec2(0.75, 0.5));

        for(unsigned int n=0; n<vertices.size(); n++)
        {
            float u = vec2ToAngle(glm::vec2(vertices[n].position[1], vertices[n].position[0])) / 360.0f;
            if (u < 0.0f)
                u = 1.0f + u;
            if (std::abs(u - vertices[n].uv[0]) > 0.5f)
                u += 1.0f;
            vertices[n].uv[0] = u;
            vertices[n].uv[1] = 0.5f + vec2ToAngle(glm::vec2(glm::length(glm::vec2(vertices[n].position[0], vertices[n].position[1])), -vertices[n].position[2])) / 180.0f;
        }
    }

    void createFace(int iteration, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2)
    {
        if (iteration < max_iterations)
        {
            glm::vec3 v01 = v0 + v1;
            glm::vec3 v12 = v1 + v2;
            glm::vec3 v02 = v0 + v2;
            glm::vec2 uv01 = (uv0 + uv1) / 2.0f;
            glm::vec2 uv12 = (uv1 + uv2) / 2.0f;
            glm::vec2 uv02 = (uv0 + uv2) / 2.0f;
            v01 /= glm::length(v01);
            v12 /= glm::length(v12);
            v02 /= glm::length(v02);
            createFace(iteration + 1, v0, v01, v02, uv0, uv01, uv02);
            createFace(iteration + 1, v01, v1, v12, uv01, uv1, uv12);
            createFace(iteration + 1, v01, v12, v02, uv01, uv12, uv02);
            createFace(iteration + 1, v2, v02, v12, uv2, uv02, uv12);
        }else{
            vertices.emplace_back();
            vertices.back().position[0] = v0.x;
            vertices.back().position[1] = v0.y;
            vertices.back().position[2] = v0.z;
            vertices.back().normal[0] = v0.x;
            vertices.back().normal[1] = v0.y;
            vertices.back().normal[2] = v0.z;
            vertices.back().uv[0] = uv0.x;
            vertices.back().uv[1] = uv0.y;

            vertices.emplace_back();
            vertices.back().position[0] = v1.x;
            vertices.back().position[1] = v1.y;
            vertices.back().position[2] = v1.z;
            vertices.back().normal[0] = v1.x;
            vertices.back().normal[1] = v1.y;
            vertices.back().normal[2] = v1.z;
            vertices.back().uv[0] = uv1.x;
            vertices.back().uv[1] = uv1.y;

            vertices.emplace_back();
            vertices.back().position[0] = v2.x;
            vertices.back().position[1] = v2.y;
            vertices.back().position[2] = v2.z;
            vertices.back().normal[0] = v2.x;
            vertices.back().normal[1] = v2.y;
            vertices.back().normal[2] = v2.z;
            vertices.back().uv[0] = uv2.x;
            vertices.back().uv[1] = uv2.y;
        }
    }
};

/// A Planet is a spherical piece of space terrain that can orbit other SpaceObjects.
/// Each Planet has separate textures for its surface, atmosphere, and cloud layers.
/// Several planetary textures are included in the resources/planets/ directory.
/// Planets can collide with objects and run callback functions upon collisions.
/// Examples:
/// -- Creates a small planetary system with a sun, a planet orbiting the sun, and a moon orbiting the planet.
/// sun = Planet():setPosition(5000, 15000):setPlanetRadius(1000):setPlanetAtmosphereTexture("planets/star-1.png"):setPlanetAtmosphereColor(1.0, 1.0, 1.0)
/// planet = Planet():setPosition(5000, 5000):setPlanetRadius(3000):setPlanetSurfaceTexture("planets/planet-1.png")
/// planet:setPlanetCloudTexture("planets/clouds-1.png"):setPlanetAtmosphereTexture("planets/atmosphere.png"):setPlanetAtmosphereColor(0.2, 0.2, 1.0):setOrbit(sun,40)
/// moon = Planet():setPosition(5000, 0):setPlanetRadius(1000):setPlanetSurfaceTexture("planets/moon-1.png"):setAxialRotationTime(20.0):setOrbit(planet,20)
REGISTER_SCRIPT_SUBCLASS(Planet, SpaceObject)
{
    /// Sets this Planet's atmospheric effect color.
    /// Example: planet:setPlanetAtmosphereColor(0.2,0.2,1.0) -- sets a blue atmosphere
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setPlanetAtmosphereColor);
    /// Sets this Planet's atmospheric effect texture.
    /// Valid values are filenames of PNG files relative to the resources/ directory.
    /// Also sets icon : value of string = "default_icon.png"
    /// Optional; if defined, atmosphere textures should be transparent or translucent.
    /// For stars, you can set an atmosphere texture such as planets/star-1.png with no surface texture.
    /// Example: planet:setPlanetSurfaceTexture("planets/atmosphere.png")
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setPlanetAtmosphereTexture);
    /// Sets this Planet's surface texture.
    /// Valid values are filenames of PNG files relative to the resources/ directory.
    /// Also sets icon : value of string = "default_icon.png"
    /// Optional; if defined, surface textures should be opaque and use a 2:1-ratio equirectangular projection.
    /// Example: planet:setPlanetSurfaceTexture("planets/planet-1.png")
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setPlanetSurfaceTexture);
    /// Set this planet's icon on radar
    /// This feature is not that well done, it's here to override default icon.
    /// Example : planet:setPlanetIcon("planets/icons/17.png")
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setPlanetIcon);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, getPlanetSurfaceTexture);
    /// Sets this Planet's cloud layer effect texture, which rotates independently of the planet.
    /// Valid values are filenames of PNG files relative to the resources/ directory.
    /// Optional; if defined, cloud layer textures should be transparent or translucent.
    /// Example: planet:setPlanetCloudTexture("planets/cloud-1.png")
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setPlanetCloudTexture);
    /// Returns this Planet's radius.
    /// Example: planet:getPlanetRadius()
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, getPlanetRadius);
    /// Sets this Planet's radius, which also sets:
    /// - its cloud radius to 1.05x this value
    /// - its atmosphere radius to 1.2x this value
    /// - its collision size to a function of this value and the planet's z-position
    /// Defaults to 5000 (5U).
    /// Example: planet:setPlanetRadius(2000)
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setPlanetRadius);
    /// Sets this Planet's collision radius.
    /// Defaults to a function of the Planet's radius and its z-position.
    /// AI behaviors use this size to plot routes that try to avoid colliding with this Planet.
    /// Example: planet:getCollisionSize()
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, getCollisionSize);
    /// Sets this Planet's cloud radius, overriding Planet:setPlanetRadius().
    /// Defaults to 1.05x this Planet's radius.
    /// If this value isn't larger than the Planet's radius, the cloud layer won't be visible.
    /// Example: planet:setPlanetCloudRadius(2500) -- sets this Planet's cloud radius to 2.5U
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setPlanetCloudRadius);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setPlanetAtmosphereRadius);
    /// Sets the z-position of this Planet, the distance by which it's offset above (positive) or below (negative) the movement plane.
    /// This value also modifies the Planet's collision radius.
    /// Defaults to 0.
    /// Example: planet:setDistanceFromMovementPlane(-500) -- sets the planet 0.5U below the movement plane
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setDistanceFromMovementPlane);
    /// Sets this Planet's axial rotation time, in degrees per tick.
    /// Defaults to 0.
    /// Example: planet:setAxialRotationTime(20)
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setAxialRotationTime);
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setAxialRotation);
    /// Sets a SpaceObject around which this Planet orbits, as well as its orbital period in orbital degrees per tick.
    /// Example: moon:setOrbit(planet,20)
    REGISTER_SCRIPT_CLASS_FUNCTION(Planet, setOrbit);
}

REGISTER_MULTIPLAYER_CLASS(Planet, "Planet");
Planet::Planet()
: SpaceObject(5000, "Planet")
{
    planet_size = 5000;
    cloud_size = 5200;
    planet_texture = "planets/2k_mercury.jpg";
    planet_icon = "planets/icons/" + planet_texture.substr(planet_texture.find("/") + 1, planet_texture.rfind(".")) + "_default_icon.png";
    
    cloud_texture = "";
    atmosphere_texture = "";
    atmosphere_size = 0;
    atmosphere_size = 0;
    distance_from_movement_plane = 0;
    //axial_rotation_time = random(100.0,400.0);
    //rotation_axis = random(0.0,360.0);
    axial_rotation_time = 0;
    rotation_axis = 0;
    orbit_target_id = -1;
    orbit_time = 0.0f;
    orbit_distance = 0.0f;

    callsign = "PL" + string(getMultiplayerId());
    setScanningParameters(3,1);

    collision_size = -2.0f;

    setRadarSignatureInfo(0.5f, 0.f, 0.3f);

    // addInfos(0,tr("Rotation"),string(irandom(5,45))+ " H.LO");
    // addInfos(1,tr("Revolution"),string(irandom(50,5000))+ " J.LO");
    // addInfos(2,tr("Rotation axis"),string(irandom(1,360))+"deg" + string(irandom(1,60))+"'" + string(irandom(1,60))+"''");
    // addInfos(3,tr("Size"),string(irandom(50,500) * 100) + " km");
    // if (random(0.f,1.f) < 0.1f)
    // {
    //     addInfos(4,tr("Type"),tr("Gazeous"));
    // }
    // else
    // {
    //     addInfos(4,tr("Type"),tr("Terestrial"));
    // }
    // addInfos(5,tr("Age"),string(irandom(5,100)*100)+ " M3 A.LO");

    // if (infos_value[4] == tr("Terestrial") && random(0.f,1.f) < 0.3f)
    // { 
    //     addInfos(6,tr("Atmosphere"),tr("Yes"));
    // }
    // else
    // {
    //     addInfos(6,tr("Atmosphere"),tr("No"));
    // }
    // addInfos(7,tr("Pressure"),string(random(0.1,4),1)+" Pa");
    // addInfos(8,tr("Gravity"),string(random(2.0,20.0),3)+" m/s2");
    // addInfos(9,tr("Main resource"),"");

    registerMemberReplication(&planet_size);
    registerMemberReplication(&cloud_size);
    registerMemberReplication(&atmosphere_size);
    registerMemberReplication(&planet_texture);
    registerMemberReplication(&planet_icon);
    registerMemberReplication(&cloud_texture);
    registerMemberReplication(&atmosphere_texture);
    registerMemberReplication(&atmosphere_color);
    registerMemberReplication(&atmosphere_size);
    registerMemberReplication(&distance_from_movement_plane);
    registerMemberReplication(&axial_rotation_time);
    registerMemberReplication(&rotation_axis);
    registerMemberReplication(&orbit_target_id);
    registerMemberReplication(&orbit_time);
    registerMemberReplication(&orbit_distance);
}

void Planet::setPlanetAtmosphereColor(float r, float g, float b)
{
    atmosphere_color = glm::vec3{ r, g, b };
}

void Planet::setPlanetAtmosphereTexture(std::string_view texture_name)
{
    atmosphere_texture = texture_name;
    planet_icon = "planets/icons/" + atmosphere_texture.substr(atmosphere_texture.find("/") + 1, atmosphere_texture.rfind(".")) + "_default_icon.png"; //By default, set with setPlanetIcon after otherwise
}

string Planet::getPlanetAtmosphereTexture()
{
    return atmosphere_texture;
}

void Planet::setPlanetSurfaceTexture(std::string_view texture_name)
{
    planet_texture = texture_name;
    planet_icon = "planets/icons/" + planet_texture.substr(planet_texture.find("/") + 1, planet_texture.rfind(".")) + "_default_icon.png"; //By default, set with setPlanetIcon after otherwise
}

string Planet::getPlanetSurfaceTexture()
{
    return planet_texture;
}

void Planet::setPlanetIcon(string texture_name)
{
    planet_icon = texture_name;
}

string Planet::getPlanetIcon()
{
    return planet_icon;
}

void Planet::setPlanetCloudTexture(std::string_view texture_name)
{
    cloud_texture = texture_name;
}

string Planet::getPlanetCloudTexture()
{
    return cloud_texture;
}

float Planet::getPlanetRadius()
{
    return planet_size;
}

float Planet::getCollisionSize()
{
    return collision_size;
}

void Planet::setPlanetRadius(float size)
{
    setRadius(size);
    this->planet_size = size;
    this->cloud_size = size * 1.05f;
    this->atmosphere_size = size * 1.2f;
}

void Planet::setPlanetCloudRadius(float size)
{
    cloud_size = size;
}

void Planet::setPlanetAtmosphereRadius(float size)
{
    atmosphere_size = size;
}

void Planet::setDistanceFromMovementPlane(float distance_from_movement_plane)
{
    this->distance_from_movement_plane = distance_from_movement_plane;
}

void Planet::setAxialRotationTime(float time)
{
    axial_rotation_time = time;
}

void Planet::setAxialRotation(float axis)
{
    rotation_axis = axis;
}

void Planet::setOrbit(P<SpaceObject> target, float orbit_time)
{
    if (!target)
        return;
    this->orbit_target_id = target->getMultiplayerId();
    this->orbit_distance = glm::length(getPosition() - target->getPosition());
    this->orbit_time = orbit_time;
}

void Planet::update(float delta)
{
    if (collision_size == -2.0f)
    {
        updateCollisionSize();
        if (collision_size > 0.0f)
            PathPlannerManager::getInstance()->addAvoidObject(this, collision_size);
    }

    if (orbit_distance > 0.0f)
    {
        P<SpaceObject> orbit_target;
        if (game_server)
            orbit_target = game_server->getObjectById(orbit_target_id);
        else
            orbit_target = game_client->getObjectById(orbit_target_id);
        if (orbit_target)
        {
            float angle = vec2ToAngle(getPosition() - orbit_target->getPosition());
            angle += delta / orbit_time * 360.0f;
            setPosition(orbit_target->getPosition() + vec2FromAngle(angle) * orbit_distance);
        }
    }

    if (axial_rotation_time != 0.0f)
        setRotation(getRotation() + delta / axial_rotation_time * 360.0f);
}

void Planet::draw3D()
{
    float distance = glm::length(camera_position - glm::vec3(getPosition().x, getPosition().y, distance_from_movement_plane));

    //view_scale ~= about the size the planet is on the screen.
//    float view_scale = planet_size / distance;
    float view_scale = getRadius() / distance;
    int level_of_detail = 4;
    if (view_scale < 0.01f)
        level_of_detail = 2;
    if (view_scale < 0.1f)
        level_of_detail = 3;
//    level_of_detail = 4;

//    if (planet_texture != "" && planet_size > 0)
    if (planet_texture != "" && getRadius() > 0)
    {
        
        if (!planet_mesh[level_of_detail])
        {
            PlanetMeshGenerator planet_mesh_generator(level_of_detail);
            planet_mesh[level_of_detail] = new Mesh(std::move(planet_mesh_generator.vertices));
        }

        ShaderRegistry::ScopedShader shader(ShaderRegistry::Shaders::Planet);
        auto planet_matrix = glm::scale(getModelMatrix(), glm::vec3(getRadius())); //tdelc planet_size -> getRadius
        glUniformMatrix4fv(shader.get().uniform(ShaderRegistry::Uniforms::Model), 1, GL_FALSE, glm::value_ptr(planet_matrix));
        glUniform4f(shader.get().uniform(ShaderRegistry::Uniforms::Color), 1.f, 1.f, 1.f, 1.f);
        glUniform4fv(shader.get().uniform(ShaderRegistry::Uniforms::AtmosphereColor), 1, glm::value_ptr(glm::vec4(atmosphere_color, 1.f)));

        ShaderRegistry::setupLights(shader.get(), planet_matrix);

        textureManager.getTexture(planet_texture)->bind();
        {
            gl::ScopedVertexAttribArray positions(shader.get().attribute(ShaderRegistry::Attributes::Position));
            gl::ScopedVertexAttribArray texcoords(shader.get().attribute(ShaderRegistry::Attributes::Texcoords));
            gl::ScopedVertexAttribArray normals(shader.get().attribute(ShaderRegistry::Attributes::Normal));

            planet_mesh[level_of_detail]->render(positions.get(), texcoords.get(), normals.get());
        }
    }
}

void Planet::draw3DTransparent()
{
    float distance = glm::length(camera_position - glm::vec3(getPosition().x, getPosition().y, distance_from_movement_plane));

    //view_scale ~= about the size the planet is on the screen.
//    float view_scale = planet_size / distance;
    float view_scale = getRadius() / distance;
    int level_of_detail = 4;
    if (view_scale < 0.01f)
        level_of_detail = 2;
    if (view_scale < 0.1f)
        level_of_detail = 3;
    level_of_detail = 4;

    auto planet_matrix = getModelMatrix();
    if (cloud_texture != "" && cloud_size > 0)
    {
       
        if (!planet_mesh[level_of_detail])
        {
            PlanetMeshGenerator planet_mesh_generator(level_of_detail);
            planet_mesh[level_of_detail] = new Mesh(std::move(planet_mesh_generator.vertices));
        }

        ShaderRegistry::ScopedShader shader(ShaderRegistry::Shaders::Planet);
        auto cloud_matrix = glm::scale(planet_matrix, glm::vec3(cloud_size));
        cloud_matrix = glm::rotate(cloud_matrix, glm::radians(engine->getElapsedTime() * 1.0f), glm::vec3(0.f, 0.f, 1.f));

        glUniformMatrix4fv(shader.get().uniform(ShaderRegistry::Uniforms::Model), 1, GL_FALSE, glm::value_ptr(cloud_matrix));
        glUniform4f(shader.get().uniform(ShaderRegistry::Uniforms::Color), 1.f, 1.f, 1.f, 1.f);
        glUniform4fv(shader.get().uniform(ShaderRegistry::Uniforms::AtmosphereColor), 1, glm::value_ptr(glm::vec4(0.f)));

        ShaderRegistry::setupLights(shader.get(), cloud_matrix);

        textureManager.getTexture(cloud_texture)->bind();
        {
            gl::ScopedVertexAttribArray positions(shader.get().attribute(ShaderRegistry::Attributes::Position));
            gl::ScopedVertexAttribArray texcoords(shader.get().attribute(ShaderRegistry::Attributes::Texcoords));
            gl::ScopedVertexAttribArray normals(shader.get().attribute(ShaderRegistry::Attributes::Normal));

            planet_mesh[level_of_detail]->render(positions.get(), texcoords.get(), normals.get());
        }
    }
    if (atmosphere_texture != "" && atmosphere_size > 0)
    {
        static std::array<VertexAndTexCoords, 4> quad{
        glm::vec3(), {0.f, 1.f},
        glm::vec3(), {1.f, 1.f},
        glm::vec3(), {1.f, 0.f},
        glm::vec3(), {0.f, 0.f}
        };

        ShaderRegistry::ScopedShader shader(ShaderRegistry::Shaders::Billboard);

        textureManager.getTexture(atmosphere_texture)->bind();
        glm::vec4 color(atmosphere_color, atmosphere_size * 2.0f);
        glUniform4fv(shader.get().uniform(ShaderRegistry::Uniforms::Color), 1, glm::value_ptr(color));
        glUniformMatrix4fv(shader.get().uniform(ShaderRegistry::Uniforms::Model), 1, GL_FALSE, glm::value_ptr(planet_matrix));
        
        gl::ScopedVertexAttribArray positions(shader.get().attribute(ShaderRegistry::Attributes::Position));
        gl::ScopedVertexAttribArray texcoords(shader.get().attribute(ShaderRegistry::Attributes::Texcoords));
        
        glVertexAttribPointer(positions.get(), 3, GL_FLOAT, GL_FALSE, sizeof(VertexAndTexCoords), (GLvoid*)quad.data());
        glVertexAttribPointer(texcoords.get(), 2, GL_FLOAT, GL_FALSE, sizeof(VertexAndTexCoords), (GLvoid*)((char*)quad.data() + sizeof(glm::vec3)));

        std::initializer_list<uint16_t> indices = { 0, 2, 1, 0, 3, 2 };
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, std::begin(indices));
    }
}

void Planet::drawOnRadar(sp::RenderTarget& renderer, glm::vec2 position, float scale, float rotation, bool long_range)
{
    if (long_range){
        
        float size = 1;
        if (collision_size < 0)
        {
            size = getRadius() * scale * 0.5f * 3.f;
            
        }
        else
        {
            size = getRadius() * scale * 3.f;
        }
        renderer.drawRotatedSprite(planet_icon, position, size, getRotation(), glm::u8vec4(255, 255, 255, 255));
    }
    else if (collision_size > 0)
    {
//        float size = getRadius() * collision_size * scale / object_sprite.getTextureRect().width * 3.0;
        float size = getRadius() * scale * 3.f;
        renderer.drawRotatedSprite(planet_icon, position, size, getRotation());
    }
}

void Planet::drawOnGMRadar(sp::RenderTarget& renderer, glm::vec2 position, float scale, float rotation, bool long_range)
{
    renderer.drawCircleOutline(position, planet_size * scale, 3, glm::u8vec4(255, 255, 255, 128));
}

void Planet::collide(Collisionable* target, float collision_force)
{
    if (collision_size > 0)
    {
        //Something hit this planet...
    }
}

void Planet::updateCollisionSize()
{
    setRadius(planet_size);
    if (std::abs(distance_from_movement_plane) >= planet_size)
    {
        collision_size = -1.0;
    }else{
        collision_size = sqrt((planet_size * planet_size) - (distance_from_movement_plane * distance_from_movement_plane)) * 1.1f;
        setCollisionRadius(collision_size);
        setCollisionPhysics(true, true);
    }
}

string Planet::getExportLine()
{
    string ret="Planet():setPosition(" + string(getPosition().x, 0) + ", " + string(getPosition().y, 0) + "):setPlanetRadius(" + string(getPlanetRadius(), 0) + ")";
    
    if (atmosphere_color != glm::vec3{})
    {
        ret += ":setPlanetAtmosphereColor(" + string(atmosphere_color.r) + "," + string(atmosphere_color.g) + "," + string(atmosphere_color.b) + ")";
    }

    if (distance_from_movement_plane != 0.f)
    {
        ret += ":setDistanceFromMovementPlane("  + string(distance_from_movement_plane) + ")";
    }

    if (!atmosphere_texture.empty())
    {
        ret += ":setPlanetAtmosphereTexture(\"" + atmosphere_texture + "\")";
    }

    if (!planet_texture.empty())
    {
        ret += ":setPlanetSurfaceTexture(\"" + planet_texture + "\")";
    }

    if (!cloud_texture.empty())
    {
        ret += ":setPlanetCloudTexture(\"" + cloud_texture + "\")";
    }

    if (cloud_size > 0.f)
    {
        ret += ":setPlanetCloudRadius(" + string(cloud_size) + ")";
    }

    if (axial_rotation_time != 0.f)
    {
        ret += ":setAxialRotationTime(" + string(axial_rotation_time) + ")";
    }

    if (orbit_distance > 0.f)
    {
        ret += ":setOrbit(?, " + string(orbit_time) + ")";
    }

    return ret;
}

glm::mat4 Planet::getModelMatrix() const
{
    return glm::translate(SpaceObject::getModelMatrix(), glm::vec3(0.f, 0.f, distance_from_movement_plane));
}
