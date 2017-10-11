#include "net/coderodde/circuits/Circuit.hpp"
#include "net/coderodde/circuits/components/support/AndGate.hpp"
#include "net/coderodde/circuits/components/support/NotGate.hpp"
#include "net/coderodde/circuits/components/support/OrGate.hpp"
#include "net/coderodde/circuits/components/support/InputGate.hpp"
#include <iostream>

using net::coderodde::circuits::NotGate;
using net::coderodde::circuits::AndGate;
using net::coderodde::circuits::OrGate;
using net::coderodde::circuits::InputGate;
using net::coderodde::circuits::Circuit;

int main(int argc, const char * argv[]) {
    NotGate notGate{"not"};
    AndGate andGate{"and"};
    OrGate orGate{"or"};
    InputGate inputGate{"input", true};
    Circuit circuit{"yeah", 1, 2};
}
