#ifndef NET_CODERODDE_CIRCUITS_CIRCUIT_HPP
#define NET_CODERODDE_CIRCUITS_CIRCUIT_HPP

#include "BackwardCycleException.hpp"
#include "ForwardCycleException.hpp"
#include "IncompleteCircuitException.hpp"
#include "InputPinOccupiedException.hpp"
#include "components/AbstractCircuitComponent.hpp"
#include "components/AbstractDoubleInputPinCircuitComponent.hpp"
#include "components/AbstractSingleInputPinCircuitComponent.hpp"
#include "components/support/AndGate.hpp"
#include "components/support/BranchWire.hpp"
#include "components/support/InputGate.hpp"
#include "components/support/NotGate.hpp"
#include "components/support/OrGate.hpp"
#include "components/support/OutputGate.hpp"

#include <algorithm>
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
    
    enum class Color {
        WHITE,
        GRAY,
        BLACK
    };
    
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
                std::stringstream ss;
                ss << INPUT_PIN_NAME_PREFIX << input_pin;
                std::string input_component_name;
                input_component_name = ss.str();
                
                InputGate* input_component =
                    new InputGate{input_component_name};
                
                m_component_map[input_component_name] = input_component;
                m_component_set.insert(input_component);
                m_input_gates.push_back(input_component);
            }
            
            for (size_t output_pin = 0;
                 output_pin < m_number_of_output_pins;
                 output_pin++) {
                std::stringstream ss;
                ss << OUTPUT_PIN_NAME_PREFIX << output_pin;
                std::string output_component_name;
                output_component_name = ss.str();
                
                OutputGate* output_component =
                    new OutputGate(output_component_name);
                
                m_component_map[output_component_name] = output_component;
                m_component_set.insert(output_component);
                m_output_gates.push_back(output_component);
            }
        }
        
        Circuit(Circuit& circuit, std::string const& name)
        :
        Circuit{checkName(name),
                circuit.m_number_of_input_pins,
                circuit.m_number_of_output_pins} {
            std::unordered_map<AbstractCircuitComponent*,
                               AbstractCircuitComponent*> component_map;
                    
            for (InputGate* mapped_input_gate : m_input_gates) {
                AbstractCircuitComponent* input_gate =
                circuit.m_component_map[mapped_input_gate->getName()];
                
                component_map[input_gate] = mapped_input_gate;
            }
                    
            for (OutputGate* mapped_output_gate : m_output_gates) {
                AbstractCircuitComponent* output_gate =
                circuit.m_component_map[mapped_output_gate->getName()];
                
                component_map[output_gate] = mapped_output_gate;
            }
                    
            for (AbstractCircuitComponent* component :
                 circuit.m_component_set) {
                if (!isInputGate(component) && !isOutputGate(component)) {
                    component_map[component] = copyComponent(component);
                }
            }
                    
            for (AbstractCircuitComponent* component :
                 circuit.m_component_set) {
                AbstractCircuitComponent* mapped_component =
                component_map[component];
                
                for (AbstractCircuitComponent* input_component :
                     component->getInputComponents()) {
                    AbstractCircuitComponent* mapped_input_component =
                    component_map[input_component];
                    
                    connectInput(component,
                                 input_component,
                                 mapped_component,
                                 mapped_input_component);
                }
                
                for (AbstractCircuitComponent* output_component :
                     component->getOutputComponents()) {
                    AbstractCircuitComponent* mapped_output_component =
                    component_map[output_component];
                    
                    connectOutput(component,
                                  mapped_component,
                                  mapped_output_component);
                }
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
        
        std::vector<AbstractCircuitComponent*> getInputComponents() const {
            std::vector<AbstractCircuitComponent*> input_components;
            input_components.assign(m_input_gates.cbegin(),
                                    m_input_gates.cend());
            
            return std::move(input_components);
        }
        
        std::vector<AbstractCircuitComponent*> getOutputComponents() const {
            std::vector<AbstractCircuitComponent*> output_components;
            output_components.assign(m_output_gates.cbegin(),
                                     m_output_gates.cend());
            
            return std::move(output_components);
        }
        
        void connectToFirstInputPin(std::string const& source_component_name,
                                    std::string const& target_component_name) {
            AbstractCircuitComponent* source_component;
            AbstractCircuitComponent* target_component;
            
            source_component = getComponentByName(source_component_name);
            target_component = getComponentByName(target_component_name);
            
            checkIsDoubleInputGate(target_component);
            
            if (((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->getInputComponent1() != nullptr) {
                throw InputPinOccupiedException{
                    "The first input pin of ... is occupied"
                };
            }
            
            if (source_component->getOutputComponent() == nullptr) {
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent1(source_component);
                
                source_component->setOutputComponent(target_component);
            } else if (isBranchWire(source_component->getOutputComponent())) {
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent1(source_component->getOutputComponent());
                
                ((BranchWire*) source_component->getOutputComponent())->
                connectTo(target_component);
            } else {
                // Replace an existing wire with BranchWire.
                BranchWire* branch_wire = new BranchWire();
                
                // Introduce the new BranchWire to the circuit.
                m_component_set.insert(branch_wire);
                
                // Load the BranchWire outputs:
                branch_wire->connectTo(
                                       source_component->getOutputComponent());
                
                branch_wire->connectTo(target_component);
                
                if (isDoubleInputPinComponent(
                                    source_component->getOutputComponent())) {
                    AbstractDoubleInputPinCircuitComponent* tmp_component =
                    (AbstractDoubleInputPinCircuitComponent*)
                    source_component->getOutputComponent();
                    
                    if (tmp_component->getInputComponent2() ==
                        source_component) {
                        tmp_component->setInputComponent1(branch_wire);
                    } else {
                        tmp_component->setInputComponent2(branch_wire);
                    }
                } else {
                    AbstractSingleInputPinCircuitComponent* tmp_component =
                    (AbstractSingleInputPinCircuitComponent*)
                    source_component->getOutputComponent();
                    
                    tmp_component->setInputComponent(branch_wire);
                }
                
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent1(branch_wire);
                
                source_component->setOutputComponent(branch_wire);
                branch_wire->setInputComponent(source_component);
            }
        }
        
        void connectToSecondInputPin(std::string const& source_component_name,
                                     std::string const& target_component_name) {
            AbstractCircuitComponent* source_component;
            AbstractCircuitComponent* target_component;
            
            source_component = getComponentByName(source_component_name);
            target_component = getComponentByName(target_component_name);
            
            checkIsDoubleInputGate(target_component);
            
            if (((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->getInputComponent2() != nullptr) {
                throw InputPinOccupiedException{
                    "The second input pin of ... is occupied."
                };
            }
            
            if (source_component->getOutputComponent() == nullptr) {
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent2(source_component);
                
                source_component->setOutputComponent(target_component);
            } else if (isBranchWire(source_component->getOutputComponent())) {
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent2(source_component->getOutputComponent());
                
                ((BranchWire*) source_component->getOutputComponent())
                ->connectTo(target_component);
            } else {
                // Replace an existing wire with BranchWire.
                BranchWire* branch_wire = new BranchWire();
                
                // Introduce the new BranchWire to the circuit.
                m_component_set.insert(branch_wire);
                
                // Load the BranchWire outputs:
                branch_wire->connectTo(source_component->getOutputComponent());
                branch_wire->connectTo(target_component);
                
                if (isDoubleInputPinComponent(
                                    source_component->getOutputComponent())) {
                    AbstractDoubleInputPinCircuitComponent* tmp_component =
                    (AbstractDoubleInputPinCircuitComponent*)
                    source_component->getOutputComponent();
                    
                    if (tmp_component->getInputComponent1() ==
                        source_component) {
                        tmp_component->setInputComponent1(branch_wire);
                    } else {
                        tmp_component->setInputComponent2(branch_wire);
                    }
                } else {
                    AbstractSingleInputPinCircuitComponent* tmp_component =
                    (AbstractSingleInputPinCircuitComponent*)
                    source_component->getOutputComponent();
                    
                    tmp_component->setInputComponent(branch_wire);
                }
                
                ((AbstractDoubleInputPinCircuitComponent*) target_component)
                ->setInputComponent2(branch_wire);
                
                source_component->setOutputComponent(branch_wire);
                branch_wire->setInputComponent(source_component);
            }
        }
        
        void connectTo(std::string const& source_component_name,
                       std::string const& target_component_name) {
            AbstractCircuitComponent* source_component;
            AbstractCircuitComponent* target_component;
            
            source_component = getComponentByName(source_component_name);
            target_component = getComponentByName(target_component_name);
            
            checkIsSingleInputGate(target_component);
            
            if (((AbstractSingleInputPinCircuitComponent*) target_component)
                ->getInputComponent() != nullptr) {
                throw InputPinOccupiedException{
                    "The only input pin is occupied."
                };
            }
            
            if (source_component->getOutputComponent() == nullptr) {
                ((AbstractSingleInputPinCircuitComponent*) target_component)
                ->setInputComponent(source_component);
                
                source_component->setOutputComponent(target_component);
            } else if (
                       isBranchWire(source_component->getOutputComponent())) {
                ((AbstractSingleInputPinCircuitComponent*) target_component)
                ->setInputComponent(source_component->getOutputComponent());
                
                ((BranchWire*) source_component->getOutputComponent())
                ->connectTo(target_component);
            } else {
                // Replace an existing wire with BranchWire.
                BranchWire* branch_wire = new BranchWire();
                
                // Introduce the BranchWire to the circuit.
                m_component_set.insert(branch_wire);
                
                // Load the BranchWire outputs.
                branch_wire->connectTo(source_component->getOutputComponent());
                branch_wire->connectTo(target_component);
                
                source_component->setOutputComponent(branch_wire);
                ((AbstractSingleInputPinCircuitComponent*) target_component)
                ->setInputComponent(branch_wire);
                
                branch_wire->setInputComponent(source_component);
            }
        }
        
        void lock() {
            if (m_locked) {
                return;
            }
            
            m_locked = true;
            lockSubcircuits();
            checkAllPinsAreConnected();
            checkIsDagInForwardDirection();
            checkIsDagInBackwardDirection();
        }
        
    private:
        
        bool isInputGate(AbstractCircuitComponent* input_gate_candidate) {
            return dynamic_cast<InputGate*>(input_gate_candidate) != nullptr;
        }
        
        bool isOutputGate(AbstractCircuitComponent* output_gate_candidate) {
            return dynamic_cast<OutputGate*>(output_gate_candidate) != nullptr;
        }
        
        bool isAndGate(AbstractCircuitComponent* and_gate_candidiate) {
            return dynamic_cast<AndGate*>(and_gate_candidiate) != nullptr;
        }
        
        bool isOrGate(AbstractCircuitComponent* or_gate_candidate) {
            return dynamic_cast<OrGate*>(or_gate_candidate) != nullptr;
        }
        
        bool isNotGate(AbstractCircuitComponent* not_gate_candidate) {
            return dynamic_cast<NotGate*>(not_gate_candidate) != nullptr;
        }
        
        bool isBranchWire(AbstractCircuitComponent* branch_wire_candidate) {
            return dynamic_cast<BranchWire*>(branch_wire_candidate) != nullptr;
        }
        
        AbstractCircuitComponent* copyComponent(
                                                AbstractCircuitComponent* component) {
            if (isNotGate(component)) {
                return new NotGate(component->getName());
            }
            
            if (isAndGate(component)) {
                return new AndGate(component->getName());
            }
            
            if (isOrGate(component)) {
                return new OrGate(component->getName());
            }
            
            if (isBranchWire(component)) {
                return new BranchWire();
            }
            
            throw std::logic_error{"Should not get here."};
        }
        
        void connectInput(AbstractCircuitComponent* component,
                          AbstractCircuitComponent* input_component,
                          AbstractCircuitComponent* mapped_component,
                          AbstractCircuitComponent* mapped_input_component) {
            if (isSingleInputPinComponent(mapped_component)) {
                ((AbstractSingleInputPinCircuitComponent*) mapped_component)
                ->setInputComponent(mapped_input_component);
            } else {
                AbstractDoubleInputPinCircuitComponent* component1 =
                (AbstractDoubleInputPinCircuitComponent*) mapped_component;
                
                AbstractDoubleInputPinCircuitComponent* component2 =
                (AbstractDoubleInputPinCircuitComponent*) component;
                
                if (input_component == component2->getInputComponent1()) {
                    component1->setInputComponent1(mapped_input_component);
                } else {
                    component1->setInputComponent2(mapped_input_component);
                }
            }
        }
        
        void connectOutput(AbstractCircuitComponent* component,
                           AbstractCircuitComponent* mapped_component,
                           AbstractCircuitComponent* mapped_output_component) {
            if (isBranchWire(component)) {
                BranchWire* branch_wire = (BranchWire*) mapped_component;
                branch_wire->connectTo(mapped_output_component);
            } else {
                mapped_component->setOutputComponent(mapped_output_component);
            }
        }
        
        bool isCircuit(AbstractCircuitComponent* circuit_candidate) {
            return dynamic_cast<Circuit*>(circuit_candidate) != nullptr;
        }
        
        void lockSubcircuits() {
            for (AbstractCircuitComponent* component : m_component_set) {
                if (isCircuit(component)) {
                    ((Circuit*) component)->lock();
                }
            }
        }
                
        void checkInputGateConnected(AbstractCircuitComponent* component) {
            InputGate* gate = (InputGate*) component;
            
            if (gate->getOutputComponent() == nullptr) {
                throw IncompleteCircuitException{"A"};
            }
        }
        
        void checkOutputGateConnected(AbstractCircuitComponent* component) {
            OutputGate* gate = (OutputGate*) component;
            
            if (gate->getInputComponent() == nullptr) {
                throw IncompleteCircuitException{"FD"};
            }
        }
                
        void checkAllPinsAreConnected() {
            for (const std::pair<std::string, AbstractCircuitComponent*> p :
                 m_component_map) {
                std::string name = p.first;
                AbstractCircuitComponent* component = p.second;
                
                if (isInputGate(component)) {
                    checkInputGateConnected(component);
                } else if (isOutputGate(component)) {
                    checkOutputGateConnected(component);
                } else if (isSingleInputPinComponent(component)) {
                    checkSingleInputComponentConnected(
                            (AbstractSingleInputPinCircuitComponent*) component,
                            name);
                } else if (isDoubleInputPinComponent(component)) {
                    checkDoubleInputComponentConnected(
                            (AbstractDoubleInputPinCircuitComponent*) component,
                            name);
                } else {
                    throw std::logic_error{"Unknown circuit type."};
                }
            }
        }
        
        void checkOutputIsConnected(AbstractCircuitComponent* component,
                                    std::string const& component_name) {
            if (component->getOutputComponent() == nullptr) {
                std::stringstream ss;
                ss << "The output pin of \""
                << component_name
                << "\" is null.";
                
                throw IncompleteCircuitException{ss.str()};
            }
        }
        
        void checkSingleInputComponentConnected(
                            AbstractSingleInputPinCircuitComponent* component,
                            std::string const& component_name) {
            if (component->getInputComponent() == nullptr) {
                std::stringstream ss;
                ss << "The only input pin of \""
                   << component_name
                   << "\" is null.";
                
                throw IncompleteCircuitException{ss.str()};
            }
            
            checkOutputIsConnected(component, component_name);
        }
        
        void checkDoubleInputComponentConnected(
                            AbstractDoubleInputPinCircuitComponent* component,
                            std::string const& component_name) {
            if (component->getInputComponent1() == nullptr) {
                std::stringstream ss;
                ss << "The first input pin of \""
                   << component_name
                   << "\" is null.";
                
                throw IncompleteCircuitException{ss.str()};
            }
            
            if (component->getInputComponent2() == nullptr) {
                std::stringstream ss;
                ss << "The second input pin of \""
                   << component_name
                   << "\" is null.";
                
                throw IncompleteCircuitException{ss.str()};
            }
            
            checkOutputIsConnected(component, component_name);
        }
        
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
        
        bool hasPrefix(std::string const& str, std::string const& prefix) {
            return std::mismatch(prefix.begin(),
                                 prefix.end(),
                                 str.begin()).first == prefix.end();
        }
        
        const std::string checkNewGateName(const std::string& gate_name) {
            if (gate_name.empty()) {
                throw std::invalid_argument{"The name of a new gate is empty."};
            }
            
            if (hasPrefix(gate_name, INPUT_PIN_NAME_PREFIX) ||
                hasPrefix(gate_name, OUTPUT_PIN_NAME_PREFIX)) {
                throw std::invalid_argument{
                    "The new gate name has invalid prefix."
                };
            }
            
            if (m_component_map.find(gate_name) != m_component_map.cend()) {
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
        
        AbstractCircuitComponent*
        getComponentByName(std::string const& component_name) {
            AbstractCircuitComponent* component;
            
            std::vector<std::string> component_name_components =
            split(component_name, '.');
            
            if (component_name_components.size() > 1) {
                if (component_name_components.size() != 2) {
                    throw std::invalid_argument{"More than one dot operators."};
                }
                
                Circuit* subcircuit =
                (Circuit*) m_component_map[component_name_components[0]];
                
                if (subcircuit == nullptr) {
                    throw std::invalid_argument{
                        "Subcircuit is not present in this circuit."
                    };
                }
                
                component =
                subcircuit->m_component_map[component_name_components[1]];
            } else {
                component = m_component_map[component_name];
            }
            
            if (component == nullptr) {
                throw std::invalid_argument{
                    "The target component is not present in the circuit."
                };
            }
            
            return component;
        }
        
        bool isDoubleInputPinComponent(AbstractCircuitComponent* component) {
            return dynamic_cast<AbstractDoubleInputPinCircuitComponent*>
            (component) != nullptr;
        }
        
        bool isSingleInputPinComponent(AbstractCircuitComponent* component) {
            return dynamic_cast<AbstractSingleInputPinCircuitComponent*>
            (component) != nullptr;
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
        
        void checkIsDagInForwardDirection() {
            std::unordered_map<AbstractCircuitComponent*, Color> colors;
            
            for (AbstractCircuitComponent* component : m_component_set) {
                colors[component] = Color::WHITE;
            }
            
            for (AbstractCircuitComponent* component : m_input_gates) {
                if (colors[component] == Color::WHITE) {
                    dfsForwardVisit(component, colors);
                }
            }
        }
        
        void checkIsDagInBackwardDirection() {
            std::unordered_map<AbstractCircuitComponent*, Color> colors;
            
            for (AbstractCircuitComponent* component : m_component_set) {
                colors[component] = Color::WHITE;
            }
            
            for (AbstractCircuitComponent* component : m_output_gates) {
                if (colors[component] == Color::WHITE) {
                    dfsBackwardVisit(component, colors);
                }
            }
        }
        
        void dfsForwardVisit(
                AbstractCircuitComponent* component,
                std::unordered_map<AbstractCircuitComponent*, Color>& colors) {
            colors[component] = Color::GRAY;
            
            for (AbstractCircuitComponent* child :
                 component->getOutputComponents()) {
                if (colors[child] == Color::GRAY) {
                    std::stringstream ss;
                    ss << "Found a cycle in circuit\""
                       << getName()
                       << "\" in forward direction.";
                    
                    throw ForwardCycleException(ss.str());
                }
                
                if (colors[child] == Color::WHITE) {
                    dfsForwardVisit(child, colors);
                }
            }
            
            colors[component] = Color::BLACK;
        }
        
        void dfsBackwardVisit(
                AbstractCircuitComponent* component,
                std::unordered_map<AbstractCircuitComponent*, Color>& colors) {
            colors[component] = Color::GRAY;
            
            for (AbstractCircuitComponent* parent :
                 component->getInputComponents()) {
                if (colors[parent] == Color::GRAY) {
                    std::stringstream ss;
                    ss << "Found a cycle in circuit\""
                       << getName()
                       << "\" in backward direction.";
                    
                    throw BackwardCycleException(ss.str());
                }
                
                if (colors[parent] == Color::WHITE) {
                    dfsBackwardVisit(parent, colors);
                }
            }
            
            colors[component] = Color::BLACK;
        }
    };
    
    const std::string Circuit::INPUT_PIN_NAME_PREFIX  = "inputPin";
    const std::string Circuit::OUTPUT_PIN_NAME_PREFIX = "outputPin";
    
} // End of namespace net::coderodde::circuits.
} // End of namespace net::coderodde.
} // End of namespace net.

#endif // NET_CODERODDE_CIRCUITS_CIRCUIT_HPP
