#include "humanoid.h"

void humanoid_entity_update(HumanoidEntity* humanoid_entity)
{
    if (humanoid_entity->in_dash)
    {
        if (humanoid_entity->base.physics->grounded || vec3_length(humanoid_entity->base.physics->vel) < 10)
        {
            humanoid_entity->in_dash = false;
            LOG_DEBUG("DASH FINISH");
        }
    }
}