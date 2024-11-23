#pragma once

#include "core.h"
#include "engine/physics/physics.h"

typedef struct Entity {
    f32 health;
    bool damagable;
    PhysicsComponent* physics;
} Entity;