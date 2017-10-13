#ifndef NET_CODERODDE_CIRCUITS_INPUT_GATE_HPP
#define NET_CODERODDE_CIRCUITS_INPUT_GATE_HPP

#include "../AbstractSingleInputPinCircuitComponent.hpp"
#include <string>

namespace net {
namespace coderodde {
namespace circuits {
            
    class InputGate : public AbstractSingleInputPinCircuitComponent {
    public:
        
        InputGate(const std::string& name, bool bit) :
        AbstractSingleInputPinCircuitComponent{name},
        m_bit{bit}
        {}
        
        InputGate(const std::string& name) : InputGate{name, DEFAULT_BIT} {}
                
        bool doCycle() const {
            if (m_input != nullptr) {
                return m_input->doCycle();
            }
            
            return m_bit;
        }
        
        bool getBit() const {
            return m_bit;
        }
        
        void setBit(bool bit) {
            m_bit = bit;
        }

        std::vector<AbstractCircuitComponent*> getInputComponents() const {
            std::vector<AbstractCircuitComponent*> input_components;
            
            if (m_input != nullptr) {
                input_components.push_back(m_input);
            }
            
            return input_components;
        }
        
    private:
        
        static const bool DEFAULT_BIT = false;
        bool m_bit;
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_INPUT_GATE_HPP
