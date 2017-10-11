#ifndef NET_CODERODDE_CIRCUITS_BRANCH_WIRE_HPP
#define NET_CODERODDE_CIRCUITS_BRANCH_WIRE_HPP

#include "abstract_circuit_component.hpp"
#include "abstract_single_input_pin_circuit_component.hpp"
#include <string>
#include <unordered_set>

namespace net {
namespace coderodde {
namespace circuits {
            
    class BranchWire : public AbstractSingleInputPinCircuitComponent {
    public:
        
        BranchWire() : AbstractSingleInputPinCircuit{nullptr} {}
        
        bool doCycle() const {
            return m_input->doCycle();
        }
        
        void connectTo(AbstractCircuitComponent* component) {
            m_outputs.insert(component);
        }
        
        void removeFrom(AbstractCircuitComponent* component) {
            m_outputs.erase(component);
        }
        
        std::vector<AbstractCircuitComponent*>&& getOutputComponents() {
            std::vector<AbstractCircuitComponent*> output_components;
            output_components.assign(m_outputs.cbegin(), m_outputs.cend()); 
            return output_components;
        }
        
    private:
        std::unordered_set<AbstractCircuitComponent*> m_outputs;
    };
            
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_BRANCH_WIRE_HPP
