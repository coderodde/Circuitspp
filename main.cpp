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
    
    try {
        c.lock();
        ASSERT(false); // Should not get here.
    } catch (ForwardCycleException& ex) {
        
    } catch (...) {
        ASSERT(false); // Should not get here.
    }
}

void test() {
    testFindsForwardCycle();
}

int main() {
    test();
    REPORT
}
