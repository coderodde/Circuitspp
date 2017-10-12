#ifndef NET_CODERODDE_CIRCUITS_FORWARD_CYCLE_EXCEPTION_HPP
#define NET_CODERODDE_CIRCUITS_FORWARD_CYCLE_EXCEPTION_HPP

#include <stdexcept>

namespace net {
namespace coderodde {
namespace circuits {
            
    class ForwardCycleException : public std::logic_error {
    public:
        ForwardCycleException(std::string const& message)
        : logic_error{message} {}
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_FORWARD_CYCLE_EXCEPTION_HPP
