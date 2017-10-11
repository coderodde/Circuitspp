#ifndef NET_CODERODDE_CIRCUITS_CIRCUIT_HPP
#define NET_CODERODDE_CIRCUITS_CIRCUIT_HPP

#include "abstract_circuit_component.hpp"
#include "abstract_single_input_pin_circuit_component.hpp"
#include "branch_wire.hpp"
#include "input_gate.hpp"

#include "InputPinOccupiedException"

#include "output_gate.hpp"
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace net {
namespace coderodde {
namespace circuits {
    
    class Circuit : public AbstractCircuitComponent {
    public:
        
        friend class TargetComponentSelector;
        
        Circuit(const std::string& name, size_t inputPins, size_t outputPins) :
        AbstractCircuitComponent{checkName(name)},
        m_number_of_input_pins {checkInputPinCount(inputPins)},
        m_number_of_output_pins{checkOutputPinCount(outputPins)}
        {
            for (size_t input_pin = 0;
                 input_pin < m_number_of_input_pins;
                 input_pin++) {
                std::string input_component_name = INPUT_PIN_NAME_PREFIX;
                input_component_name += input_pin;
                InputGate* input_component =
                    new InputGate{input_component_name};
                
                m_component_map[input_component_name] = input_component;
                m_component_set.insert(input_component);
                m_input_gates.push_back(input_component);
            }
            
            for (size_t output_pin = 0;
                 output_pin < m_number_of_output_pins;
                 output_pin++) {
                std::string output_component_name = OUTPUT_PIN_NAME_PREFIX;
                output_component_name += output_pin;
                OutputGate* output_component =
                    new OutputGate(output_component_name);
                
                m_component_map[output_component_name] = output_component;
                m_component_set.insert(output_component);
                m_output_gates.push_back(output_component);
            }
        }
        
        size_t size() {
            return m_component_set.size();
        }
        
        void addNotGate(std::string not_gate_name) {
            checkIsNotLocked();
            checkNewGateName(not_gate_name);
            NotGate* not_gate = new NotGate{not_gate_name};
            m_component_map[not_gate_name] = not_gate;
            m_component_set.insert(not_gate);
        }
        
        void addAndGate(std::string and_gate_name) {
            checkIsNotLocked();
            checkNewGateName(and_gate_name);
            AndGate* and_gate = new AndGate{and_gate_name};
            m_component_map[and_gate_name] = and_gate;
            m_component_set.insert(and_gate);
        }
        
        void addOrGate(std::string or_gate_name) {
            checkIsNotLocked();
            checkNewGateName(or_gate_name);
            OrGate* or_gate = new OrGate{or_gate_name};
            m_component_map[or_gate_name] = or_gate;
            m_component_set.insert(or_gate);
        }
        
        void addCircuit(Circuit* circuit) {
            checkIsNotLocked();
            checkNewGateName(circuit->getName());
            m_component_map[circuit->getName()] = circuit;
            m_component_set.insert(circuit);
        }
        
        size_t getNumberOfInputPins() {
            return m_number_of_input_pins;
        }
        
        size_t getNumberOfOutputPins() {
            return m_number_of_output_pins;
        }
        
        bool doCycle() const {
            for (OutputGate* output_gate : m_output_gates) {
                output_gate->doCycle();
            }
            
            return false;
        }
        
        void setInputBits(std::vector<bool>& bits) {
            unsetAllInputPins();
            
            for (size_t i = 0; i < bits.size(); ++i) {
                m_input_gates[i]->setBit(bits[i]);
            }
        }
        
        std::vector<bool>&& doCycle(std::vector<bool>& bits) {
            setInputBits(bits);
            doCycle();
            return getOutputBits();
        }
        
        std::vector<bool>&& getOutputBits() {
            std::vector<bool> output_bits{};
            output_bits.resize(m_number_of_output_pins);
            
            for (size_t i = 0; i < m_output_gates.size(); ++i) {
                output_bits[i] = m_output_gates[i]->doCycle();
            }
            
            return std::move(output_bits);
        }
        
        std::vector<AbstractCircuitComponent*>&& getInputComponents() const {
            std::vector<AbstractCircuitComponent*> input_components;
            input_components.assign(m_input_gates.cbegin(),
                                    m_input_gates.cend());
            
            return std::move(input_components);
        }
        
        std::vector<AbstractCircuitComponent*>&& getOutputComponents() const {
            std::vector<AbstractCircuitComponent*> output_components;
            output_components.assign(m_output_gates.cbegin(),
                                     m_output_gates.cend());
            
            return std::move(output_components);
        }
        
    private:
        
        static const size_t MINIMUM_INPUT_PINS  = 1;
        static const size_t MINIMUM_OUTPUT_PINS = 1;
        
        static const std::string INPUT_PIN_NAME_PREFIX;
        static const std::string OUTPUT_PIN_NAME_PREFIX;
        
        std::unordered_map<std::string,
                           AbstractCircuitComponent*> m_component_map;
        std::unordered_set<AbstractCircuitComponent*> m_component_set;
        
        const size_t m_number_of_input_pins;
        const size_t m_number_of_output_pins;
        
        std::vector<InputGate*> m_input_gates;
        std::vector<OutputGate*> m_output_gates;
        
        bool m_locked{false};
        
        const std::string& checkName(const std::string& name) {
            if (name.empty()) {
                throw std::invalid_argument{"The circuit name is empty."};
            }
            
            if (name.find(INPUT_PIN_NAME_PREFIX) == 0) {
                throw std::invalid_argument{
                    "The circuit name has illegal prefix."
                };
            }
            
            if (name.find(OUTPUT_PIN_NAME_PREFIX) == 0) {
                throw std::invalid_argument{
                    "The circuit name has illegal prefix."
                };
            }
            
            return name;
        }
        
        size_t checkInputPinCount(size_t inputPins) {
            if (inputPins == 0) {
                throw std::invalid_argument{"Number of input pins is zero."};
            }
            
            return inputPins;
        }
        
        size_t checkOutputPinCount(size_t outputPins) {
            if (outputPins == 0) {
                throw std::invalid_argument{"Number of output pins is zero."};
            }
            
            return outputPins;
        }
        
        void checkIsNotLocked() {
            if (m_locked) {
                throw std::logic_error{"The circuit is locked."};
            }
        }
        
        const std::string checkNewGateName(const std::string& gate_name) {
            if (gate_name.empty()) {
                throw std::invalid_argument{"The name of a new gate is empty."};
            }
            
            if (gate_name.find(INPUT_PIN_NAME_PREFIX) == 0
                || gate_name.find(OUTPUT_PIN_NAME_PREFIX)) {
                throw std::invalid_argument{
                    "The new gate name has invalid prefix."
                };
            }
            
            if (m_component_map.find(gate_name) != m_component_map.end()) {
                throw std::invalid_argument{
                    "The new gate name is already occupied."
                };
            }
            
            return gate_name;
        }
        
        void unsetAllInputPins() {
            for (InputGate* input_gate : m_input_gates) {
                input_gate->setBit(false);
            }
        }
    };
    
    const std::string Circuit::INPUT_PIN_NAME_PREFIX  = "inputPin";
    const std::string Circuit::OUTPUT_PIN_NAME_PREFIX = "outputPin";
    
    template<typename Out>
    void split(std::string const& s, char delimeter, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        
        while (std::getline(ss, item, delimeter)) {
            *(result++) = item;
        }
    }
    
    std::vector<std::string> split(std::string const& s, char delimeter) {
        std::vector<std::string> elements;
        split(s, delimeter, std::back_inserter(elements));
        return elements;
    }
    
    class TargetComponentSelector {
    public:
        
        TargetComponentSelector(Circuit* owner_circuit,
                                const std::string& source_component_name) :
        m_owner_circuit{owner_circuit} {
            AbstractCircuitComponent* source_component;
            std::vector<std::string> name_components =
                split(source_component_name, '.');
            
            if (name_components.size() > 1) {
                
                if (name_components.size() != 2) {
                    throw std::invalid_argument{
                        "More than one dot operators."
                    };
                }
                
                Circuit* subcircuit =
                (Circuit*) owner_circuit->m_component_map[name_components[0]];
                
                if (subcircuit == nullptr) {
                    throw std::invalid_argument{
                        "Subcircuit is not present in this circuit."
                    };
                }
                
                source_component =
                subcircuit->m_component_map[name_components[1]];
            } else {
                source_component =
                owner_circuit->m_component_map[source_component_name];
            }
            
            if (source_component == nullptr) {
                
            }
            
            this->m_source_component = source_component;
        }
        
        void toFirstPinOf(std::string const& target_component_name) {
            AbstractCircuitComponent* target_component =
            getTargetComponent(target_component_name);
            
            checkIsDoubleInputGate(target_component);
            
            if (((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->getInputComponent1() != nullptr) {
                throw InputPinOccupiedException{
                    "The first input pin of ... is occupied"
                };
            }
            
            if (m_source_component->getOutputComponent() == nullptr) {
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent1(m_source_component);
                
                m_source_component->setOutputComponent(target_component);
            } else if (isBranchWire(m_source_component->getOutputComponent())) {
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent1(m_source_component->getOutputComponent());
                
                ((BranchWire*) m_source_component->getOutputComponent())->
                    connectTo(target_component);
            } else {
                // Replace an existing wire with BranchWire.
                BranchWire* branch_wire = new BranchWire();
                
                // Introduce the new BranchWire to the circuit.
                m_owner_circuit->m_component_set.insert(branch_wire);
                
                // Load the BranchWire outputs:
                branch_wire->connectTo(
                                    m_source_component->getOutputComponent());
                
                branch_wire->connectTo(target_component);
                
                if (isDoubleInputPinComponent(
                        m_source_component->getOutputComponent())) {
                    AbstractDoubleInputPinCircuitComponent* tmp_component =
                    (AbstractDoubleInputPinCircuitComponent*)
                    m_source_component->getOutputComponent();
                    
                    if (tmp_component->getInputComponent2() ==
                        m_source_component) {
                        tmp_component->setInputComponent1(branch_wire);
                    } else {
                        tmp_component->setInputComponent2(branch_wire);
                    }
                } else {
                    AbstractSingleInputPinCircuitComponent* tmp_component =
                    (AbstractSingleInputPinCircuitComponent*)
                    m_source_component->getOutputComponent();
                    
                    tmp_component->setInputComponent(branch_wire);
                }
                
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent1(branch_wire);
                
                m_source_component->setOutputComponent(branch_wire);
                branch_wire->setInputComponent(m_source_component);
            }
        }
        
        void toSecondPinOf(std::string const& target_component_name) {
            AbstractCircuitComponent* target_component =
            getTargetComponent(target_component_name);
            
            checkIsDoubleInputGate(target_component);
            
            if (((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->getInputComponent2() != nullptr) {
                throw InputPinOccupiedException{
                    "The second input pin of ... is occupied."
                };
            }
            
            if (m_source_component->getOutputComponent() == nullptr) {
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent2(m_source_component);
                
                m_source_component->setOutputComponent(target_component);
            } else if (isBranchWire(m_source_component->getOutputComponent())) {
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent2(m_source_component->getOutputComponent());
                
                ((BranchWire*) m_source_component->getOutputComponent())
                ->connectTo(target_component);
            } else {
                // Replace an existing wire with BranchWire.
                BranchWire* branch_wire = new BranchWire();
                
                // Introduce the new BranchWire to the circuit.
                m_owner_circuit->m_component_set.insert(branch_wire);
                
                // Load the BranchWire outputs:
                branch_wire->connectTo(
                    m_source_component->getOutputComponent());
            
                branch_wire->connectTo(target_component);
                
                if (isDoubleInputPinComponent(
                        m_source_component->getOutputComponent())) {
                    AbstractDoubleInputPinCircuitComponent* tmp_component =
                    (AbstractDoubleInputPinCircuitComponent*)
                    m_source_component->getOutputComponent();
                    
                    if (tmp_component->getInputComponent1() ==
                        m_source_component) {
                        tmp_component->setInputComponent1(branch_wire);
                    } else {
                        tmp_component->setInputComponent2(branch_wire);
                    }
                } else {
                    AbstractSingleInputPinCircuitComponent* tmp_component =
                    (AbstractSingleInputPinCircuitComponent*)
                    m_source_component->getOutputComponent();
                    
                    tmp_component->setInputComponent(branch_wire);
                }
                
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent2(branch_wire);
                
                m_source_component->setOutputComponent(branch_wire);
                branch_wire->setInputComponent(m_source_component);
            }
        }
        
    private:
        
        Circuit* m_owner_circuit;
        AbstractCircuitComponent* m_source_component;
        
        bool isBranchWire(AbstractCircuitComponent* component) {
            return dynamic_cast<BranchWire*>(component) != nullptr;
        }
        
        bool isDoubleInputPinComponent(AbstractCircuitComponent* component) {
            return dynamic_cast<AbstractDoubleInputPinCircuitComponent*>
            (component) != nullptr;
        }
        
        bool isSingleInputPinComponent(AbstractCircuitComponent* component) {
            return dynamic_cast<AbstractSingleInputPinCircuitComponent*>
            (component) != nullptr;
        }
        
        AbstractCircuitComponent*
        getTargetComponent(std::string const& target_component_name) {
            AbstractCircuitComponent* target_component;
            
            std::vector<std::string> target_component_name_components =
                split(target_component_name, '.');
            
            if (target_component_name_components.size() > 1) {
                if (target_component_name_components.size() != 2) {
                    throw std::invalid_argument{"More than one dot operators."};
                }
                
                Circuit* subcircuit =
                (Circuit*) m_owner_circuit->
                    m_component_map[target_component_name_components[0]];
                
                if (subcircuit == nullptr) {
                    throw std::invalid_argument{
                        "Subcircuit is not present in this circuit."
                    };
                }
                
                target_component =
                subcircuit->m_component_map
                [target_component_name_components[1]];
            } else {
                target_component = m_owner_circuit->
                                   m_component_map[target_component_name];
            }
            
            if (target_component == nullptr) {
                throw std::invalid_argument{
                    "The target component is not present in the circuit."
                };
            }
            
            return target_component;
        }
        
        void checkIsSingleInputGate(AbstractCircuitComponent* gate) {
            if (dynamic_cast<AbstractSingleInputPinCircuitComponent*>(gate)
                == nullptr) {
                throw std::logic_error{
                    "A single input pin is expected here."
                };
            }
        }
        
        void checkIsDoubleInputGate(AbstractCircuitComponent* gate) {
            if (dynamic_cast<AbstractDoubleInputPinCircuitComponent*>(gate)
                == nullptr) {
                throw std::logic_error{
                    "A double input pin is expected here."
                };
            }
        }
    };
    
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_CIRCUIT_HPP
