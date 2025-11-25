#ifndef REC2659_H
#define REC2659_H

#include <string>
#include <vector>

namespace REC2659
{

// The layout have to respect this interface. 
// If you want to use a different structure, it should have the proper iterators that vector and string define.
typedef std::vector<std::string> Layout;

void GlobalInit();
void GlobalTeardown();
void ComputeT10Layout(Layout& oLayout, std::string const& corpus);


} // namespace REC2659

#endif // REC2659_H
