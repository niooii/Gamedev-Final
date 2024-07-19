#include "generator.h"

struct Generator_T {
    u32 testfield;
};

GDF_Generator GDF_WORLDGEN_CreateGenerator(GDF_WorldGenParameters* parameters)
{
    GDF_Generator gen;

    return gen;
}

bool GDF_WORLDGEN_GenChunk(GDF_Generator generator, GDF_Chunk* chunk)
{
    Generator_T* gen = (Generator_T*)generator;
}