//
//  FiniteAutomationAttributes.h
//  RegularExpressionParser
//
//  Created by Rosi-Eliz Dzhurkova on 03.09.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#ifndef FiniteAutomationAttributes_h
#define FiniteAutomationAttributes_h
#include <vector>
#include <string>

#define EPSILON '#'
#define OR '|'
#define ASTERISK '*'
#define PLUS '+'
using namespace std;

enum Operation {Plus, Asterisk};
class FiniteAutomation;
struct Edge;

struct State{
    bool isInitial;
    bool isFinal;
    FiniteAutomation* fromAutomation;
    vector<Edge*> outboundTransitions;
    State(bool isInitial, bool isFinal,FiniteAutomation* fromAutomation = nullptr) : isInitial(isInitial), isFinal(isFinal), fromAutomation(fromAutomation) {};
    
    void setOutboundTransition(Edge* edge) {outboundTransitions.push_back(edge);};
    vector<Edge*> getOutboundTransitions() const {return outboundTransitions;};
    bool getIsInitial() const {return isInitial;};
    bool getIsFinal() const {return isFinal;};
};

struct StatesPair {
    State* initialState;
    State* finalState;
    StatesPair(State* initialState, State* finalState) : initialState(initialState), finalState(finalState) {}
};

struct Edge{
    string symbol;
    State* toState;
    Edge(string symbol, State* toState) : symbol(symbol), toState(toState) {};
    State* getToState() const{return toState;};
};

struct PairOfIndices{
    int start;
    int end;
    PairOfIndices(int start, int end) : start(start), end(end) {};
};

#endif /* FiniteAutomationAttributes_h */
