#ifndef REC2659_H
#define REC2659_H

#include <string>
#include <vector>

namespace REC2659
{
void GlobalInit();
void GlobalTeardown();
std::string LeCompteEstBon(unsigned target, const std::vector<unsigned> &operands);

} // namespace REC2659

#endif // REC2659_H
