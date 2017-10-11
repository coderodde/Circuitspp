#ifndef NET_CODERODDE_CIRCUITS_AND_GATE_HPP
#define NET_CODERODDE_CIRCUITS_AND_GATE_HPP

#include "../AbstractDoubleInputPinCircuitComponent.hpp"
#include <string>
#include <utility>
#include <vector>

namespace net {
namespace coderodde {
namespace circuits {
    
    class AndGate : public AbstractDoubleInputPinCircuitComponent {
    public:
        
        AndGate(const std::string& name) :
        AbstractDoubleInputPinCircuitComponent{name} {}
        
        bool doCycle() const {
            return m_input1->doCycle() && m_input2->doCycle();
        }
    };
    
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_AND_GATE_HPP
