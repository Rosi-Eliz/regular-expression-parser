//
//  FiniteAutomation.h
//  RegularExpressionParser
//
//  Created by Rosi-Eliz Dzhurkova on 24.06.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#ifndef FiniteAutomation_h
#define FiniteAutomation_h
#include <string>
#include "FiniteAutomationAttributes.h"
#include "RegularExpressionParserUtilities.h"
#include <unordered_map>

using namespace std;

class FiniteAutomation{
    string regex;
    State* initialState = nullptr;
    State* initialEntryState = nullptr;
    State* finalEntryState = nullptr;
    State* finalState = nullptr;
    State* currentState = nullptr;
    void constructInitialStates(bool setInitialStates, bool setFinalStates);
    vector<PairOfIndices> topLevelBrackets(string input);
    int closingBracketIndex(int index, string input);
    bool isInBracketRange(int index, vector<PairOfIndices> pairs);
    vector<string> topLevelDisjunctions(string input);
    void repetition(State* currentState, string subregex, FiniteAutomation* nestedAutomation, Operation operation);
    void constructRegex(string subregex);
    State* constructFlatRegex(State* currentState, string subregex);
    void printFromState(State* currentState, unordered_map<State*, int>& map);
    
public:
    FiniteAutomation(string regex, bool setInitialStates = false, bool setFinalStates = false);
    bool existsPathway(State* fromState, string word, vector<State*>& visitedStates);
    bool existsPathwayToFinalState(State* fromState, vector<Edge*>& visitedTransitions) const;
    bool isAccepted(string word);
    State* getCurrentState() const;
    string assignPrintToString(State* currentState, string& result, unordered_map<State*, int>& map);
    void printFromInitialState();
    
    State* getInitialState() const;
    State* getInitialEntryState() const;
    State* getFinalState() const;
    State* getFinalEntryState() const;
};
    
    
    
#endif /* FiniteAutomation_h */
