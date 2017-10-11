#ifndef NET_CODERODDE_CIRCUITS_ABSTRACT_CIRCUIT_COMPONENT_HPP
#define NET_CODERODDE_CIRCUITS_ABSTRACT_CIRCUIT_COMPONENT_HPP

#include <string>
#include <vector>

namespace net {
namespace coderodde {
namespace circuits {
    
    class AbstractCircuitComponent {
        std::string m_name;
        
    protected:
        AbstractCircuitComponent* m_output;
        
    public:
        
        AbstractCircuitComponent(const std::string& name) : m_name{name} {}
        
        std::string& getName() { return m_name; }
        
        virtual bool doCycle() const = 0;
        
        AbstractCircuitComponent* getOutputComponent() {
            return m_output;
        }
        
        void setOutputComponent(AbstractCircuitComponent* output) {
            this->m_output = output;
        }
        
        virtual std::vector<AbstractCircuitComponent*>&&
            getInputComponents() const = 0;
        
        virtual std::vector<AbstractCircuitComponent*>&&
            getOutputComponents() const = 0;
    };
    
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_ABSTRACT_CIRCUIT_COMPONENT_HPP
