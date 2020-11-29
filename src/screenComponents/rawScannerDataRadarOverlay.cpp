#include "rawScannerDataRadarOverlay.h"
#include "radarView.h"
#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"

RawScannerDataRadarOverlay::RawScannerDataRadarOverlay(GuiRadarView* owner, string id, float distance)
: GuiElement(owner, id), radar(owner), distance(distance)
{
    setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
}

void RawScannerDataRadarOverlay::onDraw(sf::RenderTarget& window)
{
    if (!my_spaceship)
        return;

    // If probe
    P<ScanProbe> probe;
    if (game_server)
        probe = game_server->getObjectById(my_spaceship->linked_science_probe_id);
    else
        probe = game_client->getObjectById(my_spaceship->linked_science_probe_id);

    if (my_spaceship->getSystemEffectiveness(SYS_Drones) < 0.1)
        return;

    sf::Vector2f view_position = radar->getViewPosition();
    float view_rotation = radar->getViewRotation();

    // Cap the number of signature points, which determines the raw data's
    // resolution.
    const int point_count = 512;
    float radius = std::min(rect.width, rect.height) / 2.0f;

    RawRadarSignatureInfo signatures[point_count];

    // For each SpaceObject ...
    foreach(SpaceObject, obj, space_object_list)
    {
        // Don't measure our own ship.
        if (my_spaceship && obj == my_spaceship)
            continue;

        if (probe && obj == probe)
            continue;

        if (my_spaceship && my_spaceship->id_galaxy != obj->id_galaxy)
            continue;

        // Initialize angle, distance, and scale variables.
        float a_0, a_1;
        float dist = sf::length(obj->getPosition() - view_position);
        float scale = 1.0;
        
        // If the object is more than twice as far away as the maximum radar
        // range, disregard it.
        //float distance_modif = distance * std::pow(my_spaceship->getSystemEffectiveness(SYS_Drones),2)/2.0;
        float distance_modif = distance;
        if(!probe && my_spaceship && my_spaceship->hasSystem(SYS_Drones))
        {
            distance_modif = distance * std::sqrt(my_spaceship->getSystemEffectiveness(SYS_Drones));
        }

        if (dist > distance_modif * 2)
            continue;

        // The further away the object is, the less its effect on radar data.
        if (dist > distance_modif)
            scale = 1.0f - ((dist - distance_modif) / distance_modif);

        // If we're adjacent to the object ...
        if (dist <= obj->getRadius())
        {
            // ... affect all angles of the radar.
            a_0 = 0.0f;
            a_1 = 360.0f;
        }else{
            // Otherwise, measure the affected range of angles by the object's
            // distance and radius.
            float a_diff = asinf(obj->getRadius() / dist) / M_PI * 180.0f;
            float a_center = sf::vector2ToAngle(obj->getPosition() - view_position);
            a_0 = a_center - a_diff;
            a_1 = a_center + a_diff;
        }

        // Get the object's radar signature.
        // If the object is a SpaceShip, adjust the signature dynamically based
        // on its current state and activity.
        RawRadarSignatureInfo info;
        P<SpaceShip> ship = obj;

        if (ship)
        {
            // Use dynamic signatures for ships.
            info = ship->getDynamicRadarSignatureInfo();
        } else 
        {
            // Otherwise, use the baseline only.
            info = obj->getRadarSignatureInfo();
        }

        // Anti-object have more signature range
        float material = info.gravity + info.biological + info.electrical;
        if (material < 0.0)
        {
            a_0 -= - material * 10;
            a_1 += - material * 10;
        }
        // For each interval determined by the level of raw data resolution,
        // initialize the signatures array.
        for(float a = a_0; a <= a_1; a += 360.f / float(point_count))
        {
            int idx = (int(a / 360.0f * point_count) + point_count * 2) % point_count;
            signatures[idx] += info * scale;
        }
    }

    // Initialize the data's amplitude along each of the three color bands.
    float amp_r[point_count];
    float amp_g[point_count];
    float amp_b[point_count];

    // For each data point ...
    for(int n = 0; n < point_count; n++)
    {
        // ... initialize its values in the array ...
        signatures[n].gravity = std::max(0.0f, std::min(1.0f, signatures[n].gravity));
        signatures[n].electrical = std::max(0.0f, std::min(1.0f, signatures[n].electrical));
        signatures[n].biological = std::max(0.0f, std::min(1.0f, signatures[n].biological));

        // ... make some noise ...
        float r = random(-1, 1);
        float g = random(-1, 1);
        float b = random(-1, 1);

         if (gameGlobalInfo->use_complex_radar_signatures)
        {
            // ... and then modify the bands' values based on the object's signature.
            // Biological signatures amplify the red and green bands.
            r += signatures[n].biological * 30;
            g += signatures[n].biological * 30;

            // Electrical signatures amplify the red and blue bands.
            r += random(-20, 20) * signatures[n].electrical;
            b += random(-20, 20) * signatures[n].electrical;

            // Gravitational signatures amplify all bands, but especially modify
            // the green and blue bands.
            r = r * (1.0f - signatures[n].gravity);
            g = g * (1.0f - signatures[n].gravity) + 40 * signatures[n].gravity;
            b = b * (1.0f - signatures[n].gravity) + 40 * signatures[n].gravity;
        }
        else
        {
            r += signatures[n].gravity * 40;
            g += signatures[n].biological * 30;
            b += random(-20, 20) * signatures[n].electrical;
        }

        // Apply the values to the radar bands.
        if(!probe)
        {
            r *= my_spaceship->getSystemEffectiveness(SYS_Drones);
            g *= my_spaceship->getSystemEffectiveness(SYS_Drones);
            b *= my_spaceship->getSystemEffectiveness(SYS_Drones);
        }
        amp_r[n] = r;
        amp_g[n] = g;
        amp_b[n] = b;
        
    }

    // Create a vertex array containing each data point.
    sf::VertexArray a_r(sf::LinesStrip, point_count+1);
    sf::VertexArray a_g(sf::LinesStrip, point_count+1);
    sf::VertexArray a_b(sf::LinesStrip, point_count+1);

    // For each data point ...
    for(int n = 0; n < point_count; n++)
    {
        // ... set a baseline of 0 ...
        float r = 0.0;
        float g = 0.0;
        float b = 0.0;

        // ... then sum the amplitude values ...
        for(int m = n - 2 + point_count; m <= n + 2 + point_count; m++)
        {
            r += amp_r[m % point_count];
            g += amp_g[m % point_count];
            b += amp_b[m % point_count];
        }

        // ... divide them by 5 ...
        r /= 5;
        g /= 5;
        b /= 5;

        // ... and add vectors for each point.
        a_r[n].position.x = rect.left + rect.width / 2.0f;
        a_r[n].position.y = rect.top + rect.height / 2.0f;
        a_r[n].position += sf::vector2FromAngle(float(n) / float(point_count) * 360.0f - view_rotation) * (radius * (0.95f - r / 500));
        a_r[n].color = sf::Color(255, 0, 0);

        a_g[n].position.x = rect.left + rect.width / 2.0f;
        a_g[n].position.y = rect.top + rect.height / 2.0f;
        a_g[n].position += sf::vector2FromAngle(float(n) / float(point_count) * 360.0f - view_rotation) * (radius * (0.92f - g / 500));
        a_g[n].color = sf::Color(0, 255, 0);

        a_b[n].position.x = rect.left + rect.width / 2.0f;
        a_b[n].position.y = rect.top + rect.height / 2.0f;
        a_b[n].position += sf::vector2FromAngle(float(n) / float(point_count) * 360.0f - view_rotation) * (radius * (0.89f - b / 500));
        a_b[n].color = sf::Color(0, 0, 255);
    }

    // Set a zero value at the "end" of the data point array.
    a_r[point_count] = a_r[0];
    a_g[point_count] = a_g[0];
    a_b[point_count] = a_b[0];

    // Draw each band as a line.
    if (my_spaceship->has_electrical_sensor) window.draw(a_r, sf::BlendAdd);
    if (my_spaceship->has_biological_sensor) window.draw(a_g, sf::BlendAdd);
    if (my_spaceship->has_gravity_sensor) window.draw(a_b, sf::BlendAdd);
}
