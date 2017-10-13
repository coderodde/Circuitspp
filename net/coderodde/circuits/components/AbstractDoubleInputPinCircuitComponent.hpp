#ifndef NET_CODERODDE_CIRCUITS_ABSTRACT_DOUBLE_INPUT_PIN_CIRCUIT_COMPONENT_HPP
#define NET_CODERODDE_CIRCUITS_ABSTRACT_DOUBLE_INPUT_PIN_CIRCUIT_COMPONENT_HPP

#include "AbstractCircuitComponent.hpp"
#include <string>
#include <utility>

namespace net {
namespace coderodde {
namespace circuits {
    
    class AbstractDoubleInputPinCircuitComponent :
    public AbstractCircuitComponent {
        
    protected:
        AbstractCircuitComponent* m_input1;
        AbstractCircuitComponent* m_input2;
        
    public:
        AbstractDoubleInputPinCircuitComponent(const std::string& name) :
        AbstractCircuitComponent{name},
        m_input1{nullptr},
        m_input2{nullptr} {}
        
        AbstractCircuitComponent* getInputComponent1() { return m_input1; }
        AbstractCircuitComponent* getInputComponent2() { return m_input2; }
        
        void setInputComponent1(AbstractCircuitComponent* input) {
            m_input1 = input;
        }
        
        void setInputComponent2(AbstractCircuitComponent* input) {
            m_input2 = input;
        }
        
        std::vector<AbstractCircuitComponent*> getInputComponents() const {
            std::vector<AbstractCircuitComponent*> input_components = {
                m_input1, m_input2
            };
            
            return std::move(input_components);
        }
        
        std::vector<AbstractCircuitComponent*> getOutputComponents() const {
            std::vector<AbstractCircuitComponent*> output_components = {
                m_output
            };
            
            return std::move(output_components);
        }
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_ABSTRACT_DOUBLE_INPUT_PIN_CIRCUIT_COMPONENT_HPP
