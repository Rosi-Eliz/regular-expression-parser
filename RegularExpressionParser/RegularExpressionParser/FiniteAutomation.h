//
//  FiniteAutomation.h
//  RegularExpressionParser
//
//  Created by Rosi-Eliz Dzhurkova on 24.06.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#ifndef FiniteAutomation_h
#define FiniteAutomation_h
#include <vector>
#include <string>
#define EPSILON "#"
#define OR "|"
#define STAR "*"
#define PLUS "+"

using namespace std;

struct State;
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

class FiniteAutomation;
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

class FiniteAutomation{
    string regex;
    State* initialState;
    State* initialEntryState;
    State* finalEntryState;
    State* finalState;
    State* currentState;
    
    void constructInitialStates(bool setInitialStates, bool setFinalStates);
    StatesPair baseStone(string symbol);
    State* conjunction(State* currentState, string first, string second);
    State* disjunction(State* currentState, string first, string second);
    State* star(State* currentState, string word);
    State* plus(State* currentState, string word);
    
    State* constructSubRegex(string subregex);
    
public:
FiniteAutomation(string regex, bool setInitialStates = false, bool setFinalStates = false);
    bool wasAccepted(string regex);
   State* getCurrentState() const;
};










#endif /* FiniteAutomation_h */
