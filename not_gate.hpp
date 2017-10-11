#ifndef NET_CODERODDE_CIRCUITS_NOT_GATE_HPP
#define NET_CODERODDE_CIRCUITS_NOT_GATE_HPP

#include "abstract_circuit_component.hpp"
#include "abstract_single_input_pin_circuit_component.hpp"
#include <string>
#include <utility>
#include <vector>

namespace net {
namespace coderodde {
namespace circuits {
            
    class NotGate : public AbstractSingleInputPinCircuitComponent {
    public:
                
        NotGate(const std::string& name) :
        AbstractSingleInputPinCircuitComponent{name} {}
                
        bool doCycle() const {
            return !m_input;
        }
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_NOT_GATE_HPP
