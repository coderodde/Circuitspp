#ifndef NET_CODERODDE_CIRCUITS_OR_GATE_HPP
#define NET_CODERODDE_CIRCUITS_OR_GATE_HPP

#include "abstract_circuit_component.hpp"
#include "abstract_double_input_pin_circuit_component.hpp"
#include <string>
#include <utility>
#include <vector>

namespace net {
namespace coderodde {
namespace circuits {
            
    class OrGate : public AbstractDoubleInputPinCircuitComponent {
    public:
                
        OrGate(const std::string& name) :
        AbstractDoubleInputPinCircuitComponent{name} {}
                
        bool doCycle() const {
            return m_input1->doCycle() || m_input2->doCycle();
        }
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_OR_GATE_HPP
