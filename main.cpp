#include "and_gate.hpp"
#include "or_gate.hpp"
#include "not_gate.hpp"
#include <iostream>

using net::coderodde::circuits::NotGate;
using net::coderodde::circuits::AndGate;
using net::coderodde::circuits::OrGate;

int main(int argc, const char * argv[]) {
    NotGate notGate{"not"};
    AndGate andGate{"and"};
    OrGate orGate{"or"};
}
