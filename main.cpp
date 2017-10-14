#include "assert.hpp"
#include "net/coderodde/circuits/BackwardCycleException.hpp"
#include "net/coderodde/circuits/Circuit.hpp"
#include "net/coderodde/circuits/ForwardCycleException.hpp"
#include "net/coderodde/circuits/InputPinOccupiedException.hpp"

using net::coderodde::circuits::BackwardCycleException;
using net::coderodde::circuits::Circuit;
using net::coderodde::circuits::ForwardCycleException;
using net::coderodde::circuits::InputPinOccupiedException;

void testFindsForwardCycle() {
    Circuit c{"c", 1, 1, };
    
    c.addAndGate("and");
    c.connectToFirstInputPin("inputPin0", "and");
    c.connectTo("and", "outputPin0");
    c.connectToSecondInputPin("and", "and");
    
    bool catched = false;
    
    try {
        c.lock();
    } catch (ForwardCycleException& ex) {
        catched = true;
    } catch (...) {
        catched = false;
    }
    
    ASSERT(catched);
}

void testFindsBackwardCycle() {
    Circuit c{"circuit", 1, 1};
    
    c.addOrGate("or");
    c.addAndGate("and");

    c.connectToFirstInputPin("inputPin0", "or");
    c.connectToSecondInputPin("and", "or");
    c.connectTo("or", "outputPin0");
    c.connectToFirstInputPin("and", "and");
    c.connectToSecondInputPin("and", "and");
    
    bool catched = false;
    
    try {
        c.lock();
    } catch (BackwardCycleException& ex) {
        catched = true;
    } catch (...) {
        catched = false;
    }
    
    ASSERT(catched);
}

void testCannotConnectToOccupiedInput() {
    Circuit c{"myCircuit", 1, 1};
    
    c.addNotGate("not");
    c.connectTo("inputPin0", "outputPin0");
    c.connectTo("inputPin0", "not");
    
    bool catched = false;
    
    try {
        c.connectTo("not", "outputPin0");
    } catch (InputPinOccupiedException& ex) {
        catched = true;
    } catch (...) {
        catched = false;
    }
    
    ASSERT(catched);
}

void test1() {
    Circuit c{"c", 4, 1};
    
    c.addAndGate("and1");
    c.addAndGate("and2");
    c.addOrGate("or");
    
    c.connectToFirstInputPin("inputPin0", "and1");
    c.connectToSecondInputPin("inputPin1", "and1");
    c.connectToFirstInputPin("inputPin2", "and2");
    c.connectToSecondInputPin("inputPin3", "and2");
    c.connectToFirstInputPin("and1", "or");
    c.connectToSecondInputPin("and2", "or");
    c.connectTo("or", "outputPin0");
    
    for (bool bit0 : { false, true }) {
        for (bool bit1 : { false, true }) {
            for (bool bit2 : { false, true }) {
                for (bool bit3 : { false, true }) {
                    bool expected = (bit0 && bit1) || (bit2 && bit3);
                    std::vector<bool> input = { bit0, bit1, bit2, bit3 };
                    std::vector<bool> output = c.doCycle(input);
                    ASSERT(output.size() == 1);
                    ASSERT(expected == output[0]);
                }
            }
        }
    }
}

void test2() {
    Circuit c{"c", 2, 1};
    
    c.addAndGate("and1");
    c.addAndGate("and2");
    c.addNotGate("not1");
    c.addNotGate("not2");
    c.addOrGate("or");
    
    c.connectToFirstInputPin("inputPin0", "and1");
    c.connectTo("inputPin1", "not1");
    c.connectToSecondInputPin("not1", "and1");
    
    c.connectTo("inputPin0", "not2");
    c.connectToFirstInputPin("not2", "and2");
    c.connectToSecondInputPin("inputPin1", "and2");
    
    c.connectToFirstInputPin("and1", "or");
    c.connectToSecondInputPin("and2", "or");
    c.connectTo("or", "outputPin0");
    
    for (bool bit0 : { false, true }) {
        for (bool bit1 : { false, true }) {
            bool expected = (bit0 && !bit1) || (!bit0 && bit1);
            std::vector<bool> input = { bit0, bit1 };
            std::vector<bool> output = c.doCycle(input);
            ASSERT(output.size() == 1);
            ASSERT(expected == output[0]);
        }
    }
}

void testSubcircuit() {
    Circuit subcircuit {"mySubcircuit", 2, 1};
    
    subcircuit.addAndGate("and");
    subcircuit.addNotGate("not1");
    subcircuit.addNotGate("not2");
    
    subcircuit.connectTo("inputPin0", "not1");
    subcircuit.connectTo("inputPin1", "not2");
    subcircuit.connectToFirstInputPin("not1", "and");
    subcircuit.connectToSecondInputPin("not2", "and");
    subcircuit.connectTo("and", "outputPin0");
    
    Circuit circuit {"myCircuit", 2, 1};
    circuit.addCircuit(&subcircuit);
    circuit.addNotGate("not");
    
    circuit.connectTo("inputPin0", "mySubcircuit.inputPin0");
    circuit.connectTo("inputPin1", "mySubcircuit.inputPin1");
    circuit.connectTo("mySubcircuit.outputPin0", "not");
    circuit.connectTo("not", "outputPin0");
    
    for (bool bit0 : { false, true }) {
        for (bool bit1 : { false, true }) {
            std::vector<bool> input = { bit0, bit1 };
            std::vector<bool> output = circuit.doCycle(input);
            bool expected = !(!bit0 && !bit1);
            ASSERT(output.size() == 1);
            ASSERT(output[0] == expected);
        }
    }
}

void testCopyConstructor() {
    Circuit c1{"c", 2, 2};
    
    c1.addAndGate("and");
    c1.addOrGate("or");
    
    c1.connectToFirstInputPin("inputPin0", "and");
    c1.connectToSecondInputPin("inputPin1", "and");
    c1.connectToFirstInputPin("inputPin0", "or");
    c1.connectToSecondInputPin("inputPin1", "or");
    c1.connectTo("and", "outputPin0");
    c1.connectTo("or", "outputPin1");
    
    Circuit c2{c1, "c2"};
    
    for (bool bit0 : { false, true }) {
        for (bool bit1 : { false, true }) {
            std::vector<bool> input = { bit0, bit1 };
            std::vector<bool> output1 = c1.doCycle(input);
            std::vector<bool> output2 = c2.doCycle(input);
            ASSERT(output1.size() == 2);
            ASSERT(output2.size() == 2);
            
            for (int i = 0; i < 2; ++i) {
                ASSERT(output1[i] == output2[i]);
            }
        }
    }
}

void test() {
    testFindsForwardCycle();
    testFindsBackwardCycle();
    testCannotConnectToOccupiedInput();
    test1();
    test2();
    testSubcircuit();
    testCopyConstructor();
}

int main() {
    test();
    REPORT
}
