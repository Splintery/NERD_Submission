#ifndef REC2659_H
#define REC2659_H
#include <stdint.h>

namespace REC2659
{


void GlobalInit();
void GlobalTeardown();

class UniqueRandomGenerator
{
public:
    UniqueRandomGenerator(uint64_t N, uint64_t K);
    uint64_t GenerateOneUniqueRand();

private:
    // Some state ?
};

} // namespace REC2659

#endif // REC2659_H
