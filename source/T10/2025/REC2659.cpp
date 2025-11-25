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
 * @brief : oLayout, the outputed layout.
   @brief : corpus, the input corpus
 */
void ComputeT10Layout(Layout& oLayout, std::string const& corpus)
{
    UNUSED_PARAM(oLayout);
    UNUSED_PARAM(corpus);
    throw std::runtime_error("Not implemented");
}

} // namespace REC2659
