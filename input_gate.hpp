#ifndef NET_CODERODDE_CIRCUITS_INPUT_GATE_HPP
#define NET_CODERODDE_CIRCUITS_INPUT_GATE_HPP

#include "abstract_circuit_component.hpp"
#include "abstract_single_input_pin_circuit_component.hpp"
#include <string>
#include <utility>
#include <vector>

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
        
    private:
        
        static const bool DEFAULT_BIT = false;
        bool m_bit;
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_INPUT_GATE_HPP
