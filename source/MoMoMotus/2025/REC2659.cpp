#include "REC2659.h"
#include <Helpers.h>
#include <stdexcept>

namespace REC2659
{

void GlobalInit()
{
}

void GlobalTeardown()
{
}

UniqueRandomGenerator::UniqueRandomGenerator(uint64_t N, uint64_t K)
{
    UNUSED_PARAM(N);
    UNUSED_PARAM(K);
    throw std::runtime_error("Not implemented");
}

uint64_t UniqueRandomGenerator::GenerateOneUniqueRand()
{
    throw std::runtime_error("Not implemented");
}

} // namespace REC2659
