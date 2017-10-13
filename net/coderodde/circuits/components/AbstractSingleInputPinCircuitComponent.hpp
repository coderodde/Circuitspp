#ifndef NET_CODERODDE_CIRCUITS_ABSTRACT_SINGLE_INPUT_PIN_CIRCUIT_COMPONENT_HPP
#define NET_CODERODDE_CIRCUITS_ABSTRACT_SINGLE_INPUT_PIN_CIRCUIT_COMPONENT_HPP

#include "AbstractCircuitComponent.hpp"
#include <string>

namespace net {
namespace coderodde {
namespace circuits {
    
    class AbstractSingleInputPinCircuitComponent :
    public AbstractCircuitComponent {
        
    protected:
        
        AbstractCircuitComponent* m_input;
        
    public:
        
        AbstractSingleInputPinCircuitComponent(const std::string& name) :
        AbstractCircuitComponent{name},
        m_input{nullptr} {}
        
        AbstractCircuitComponent* getInputComponent() {
            return m_input;
        }
        
        void setInputComponent(AbstractCircuitComponent* input) {
            m_input = input;
        }
        
        std::vector<AbstractCircuitComponent*> getInputComponents() const {
            std::vector<AbstractCircuitComponent*> input_components = {
                m_input
            };
            
            return input_components;
        }
        
        std::vector<AbstractCircuitComponent*> getOutputComponents() const {
            std::vector<AbstractCircuitComponent*> output_components = {
                m_output
            };
            
            return output_components;
        }
    };
    
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_ABSTRACT_SINGLE_INPUT_PIN_CIRCUIT_COMPONENT_HPP
