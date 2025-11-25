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

/**
 * @brief Determine the mathematical integer expression resulting in the target number
 * 
 * @param target the target result to obtain with the given operands
 * @param operands : the set of numbers
 * 
 * @return the function returns a VALID mathematical expression if a solution is found, otherwise an EMPTY string if there isn't any
 * 
 * @note if you want to compute the value of your mathematical expression, you can use the MathsParser funtion in the Helpers file
 * a mathematical expression is, for example : 2 * (4+(3*2)-8) + 4. You can write it the way you want (with spaces, with or without
 * parentheseses, in whatever order(just pay attention to operators precedence if you don't put parentheses)
 */
std::string LeCompteEstBon(unsigned target, const std::vector<unsigned> &operands)
{
    UNUSED_PARAM(target);
    UNUSED_PARAM(operands);
    throw std::runtime_error("Not implemented");
}

} // namespace REC2659
