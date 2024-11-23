#pragma once

#include "core.h"
#include "entity.h"

typedef struct HumanoidEntity {
    Entity base;

    bool in_dash;
} HumanoidEntity;

void humanoid_entity_update(HumanoidEntity* humanoid_entity);