#ifndef NET_CODERODDE_CIRCUITS_OUTPUT_GATE_HPP
#define NET_CODERODDE_CIRCUITS_OUTPUT_GATE_HPP

#include "abstract_circuit_component.hpp"
#include "abstract_single_input_pin_circuit_component.hpp"
#include <string>

namespace net {
namespace coderodde {
namespace circuits {
            
    class OutputGate : public AbstractSingleInputPinCircuitComponent {
    public:
                
        OutputGate(const std::string& name) :
        AbstractSingleInputPinCircuitComponent{name} {}
        
        bool doCycle() const {
            return m_input->doCycle();
        }
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_OUTPUT_GATE_HPP
