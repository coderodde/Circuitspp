#ifndef NET_CODERODDE_CIRCUITS_OUTPUT_GATE_HPP
#define NET_CODERODDE_CIRCUITS_OUTPUT_GATE_HPP

#include "../AbstractSingleInputPinCircuitComponent.hpp"
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
        
        std::vector<AbstractCircuitComponent*> getOutputComponents() const {
            std::vector<AbstractCircuitComponent*> output_components;
            
            if (m_output != nullptr) {
                output_components.push_back(m_output);
            }
            
            return output_components;
        }
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_OUTPUT_GATE_HPP
