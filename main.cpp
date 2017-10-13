#include "assert.hpp"
#include "net/coderodde/circuits/BackwardCycleException.hpp"
#include "net/coderodde/circuits/Circuit.hpp"
#include "net/coderodde/circuits/ForwardCycleException.hpp"

using net::coderodde::circuits::BackwardCycleException;
using net::coderodde::circuits::Circuit;
using net::coderodde::circuits::ForwardCycleException;

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

void test() {
    testFindsForwardCycle();
    testFindsBackwardCycle();
}

int main() {
    test();
    REPORT
}
