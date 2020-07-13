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
#include <unordered_map>

#define EPSILON '#'
#define OR '|'
#define ASTERISK '*'
#define PLUS '+'
enum Operation {Plus, Asterisk};

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

struct PairOfIndices{
    int start;
    int end;
    PairOfIndices(int start, int end) : start(start), end(end) {};
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
    State* initialState = nullptr;
    State* initialEntryState = nullptr;
    State* finalEntryState = nullptr;
    State* finalState = nullptr;
    State* currentState = nullptr;
    void constructInitialStates(bool setInitialStates, bool setFinalStates);
    StatesPair baseStone(string symbol);
    State* conjunction(State* currentState, string first, string second);
    State* disjunction(State* currentState, string first, string second);
    State* asterisk(State* currentState, string word);
    State* plus(State* currentState, string word);
    void connectStates(State* fromState, State* toState);
    vector<PairOfIndices> topLevelBrackets(string input);
    int closingBracketIndex(int index, string input);
    bool isInBracketRange(int index, vector<PairOfIndices> pairs);
    vector<string> topLevelDisjunctions(string input);
    void repetition(State* currentState, string subregex, FiniteAutomation* nestedAutomation, Operation operation);
    void constructRegex(string subregex);
    State* constructFlatRegex(State* currentState, string subregex);
    void printFromState(State* currentState, unordered_map<State*, int>& map);
    
    //Operations
    
    State* conjunction(State* currentState, FiniteAutomation* automation1, FiniteAutomation* automation2);
    State* disjunction(State* currentState, FiniteAutomation* automation1, FiniteAutomation* automation2);
    State* asterisk(State* currentState, FiniteAutomation* automation);
    State* plus(State* currentState, FiniteAutomation* automation);
public:
    FiniteAutomation(string regex, bool setInitialStates = false, bool setFinalStates = false);
    bool existsPathway(State* fromState, string word, vector<State*>& visitedStates);
    bool existsPathwayToFinalState(State* fromState, vector<Edge*>& visitedTransitions) const;
    bool isAccepted(string word);
    State* getCurrentState() const;
    void printFromInitialState();
    

};
    
    
    
#endif /* FiniteAutomation_h */
