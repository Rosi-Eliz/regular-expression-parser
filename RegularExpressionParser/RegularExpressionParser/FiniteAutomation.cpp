//
//  FiniteAutomation.cpp
//  RegularExpressionParser
//
//  Created by Rosi-Eliz Dzhurkova on 24.06.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#include <stdio.h>
#include "FiniteAutomation.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

FiniteAutomation::FiniteAutomation(string regex, bool setInitialStates, bool setFinalStates) {    
    vector<string> result = topLevelDisjunctions(regex);
    constructInitialStates(setInitialStates, setFinalStates);
    this->regex = regex;
    constructRegex(regex);
}

void FiniteAutomation::constructInitialStates(bool setInitialStates, bool setFinalStates)
{
    initialState = new State(setInitialStates,false, this);
    initialEntryState = new State(false, false, this);
    finalState = new State(false,setFinalStates, this);
    finalEntryState = new State(false, false, this);
    
    Edge* initialEpsilonTransition = new Edge(string(1, EPSILON), initialEntryState);
    Edge* finalEpsilonTransition = new Edge(string(1, EPSILON), finalState);
    initialState->setOutboundTransition(initialEpsilonTransition);
    finalEntryState->setOutboundTransition(finalEpsilonTransition);
}

void FiniteAutomation::repetition(State* currentState, string subregex, FiniteAutomation* nestedAutomation,Operation operation)
{
    State* oldCurrentState = currentState;
    switch(operation)
    {
        case Plus: currentState = RegularExpressionParserUtilities::faPlus(currentState, nestedAutomation); break;
        case Asterisk:
            currentState = RegularExpressionParserUtilities::faAsterisk(currentState, nestedAutomation); break;
    }
    subregex = subregex.substr(1);
    if(subregex.size() > 0 && subregex[1] == OR)
    {
        FiniteAutomation* rightAutomation = new FiniteAutomation(subregex);
        RegularExpressionParserUtilities::connectStates(oldCurrentState, rightAutomation->initialState);
        State* finalConnectingState = new State(false, false);
        RegularExpressionParserUtilities::connectStates(currentState, finalConnectingState);
        RegularExpressionParserUtilities::connectStates(rightAutomation->finalState, finalConnectingState);
        currentState = finalConnectingState;
    }
    else if(subregex.size() > 0)
    {
        FiniteAutomation* rightAutomation = new FiniteAutomation(subregex);
        RegularExpressionParserUtilities::connectStates(currentState, rightAutomation->initialState);
        currentState = rightAutomation->finalState;
    }
}

vector<PairOfIndices> FiniteAutomation::topLevelBrackets(string input)
{
    vector<PairOfIndices> result;
    for(int i{0}; i < input.size(); i++)
    {
        if(input[i] == '(')
        {
            int closingIndex = closingBracketIndex(i, input);
            PairOfIndices pair = PairOfIndices(i, closingIndex);
            result.push_back(pair);
            i = closingIndex;
        }
    }
    return result;
}

int FiniteAutomation::closingBracketIndex(int index, string input)
{
    int nestedBrackets{0};
    for(int i{index + 1}; i < input.size(); i++)
    {
        if(input[i] == '(')
            nestedBrackets++;
        else if(input[i] == ')' && nestedBrackets > 0)
            nestedBrackets--;
        else if(input[i] == ')')
            return i;
    }
    throw runtime_error("Invalid regex!");
}

bool FiniteAutomation::isInBracketRange(int index, vector<PairOfIndices> pairs)
{
    for(int i{0}; i < pairs.size(); i++)
    {
        if(index >= pairs[i].start && index <= pairs[i].end)
            return true;
    }
    return false;
}

vector<string> FiniteAutomation::topLevelDisjunctions(string input)
{
    vector<string> result;
    vector<PairOfIndices> brackets = topLevelBrackets(input);
    vector<int> disjunctionIndices;
    for(int i{0}; i < input.size(); i++)
    {
        if(input[i] == OR && !isInBracketRange(i, brackets))
            disjunctionIndices.push_back(i);
    }
    int previousSubStringEndIndex = 0;
    for(int i{0}; i < disjunctionIndices.size(); i++)
    {
        string substring = input.substr(previousSubStringEndIndex, disjunctionIndices[i]);
        previousSubStringEndIndex = disjunctionIndices[i] + 1;
        result.push_back(substring);
        if(i == disjunctionIndices.size() - 1)
        {
            string lastPortion = input.substr(previousSubStringEndIndex);
            result.push_back(lastPortion);
        }
    }
    return result;
}


void FiniteAutomation::constructRegex(string subregex)
{
    State *currentState = initialEntryState;
    vector<string> disjunctions = topLevelDisjunctions(subregex);
    if(!disjunctions.empty())
    {
        State* commonFinalState = new State(false, false);
        for(int i{0}; i < disjunctions.size(); i++)
        {
            FiniteAutomation* automation = new FiniteAutomation(disjunctions[i]);
            RegularExpressionParserUtilities::connectStates(initialEntryState, automation->initialState);
            RegularExpressionParserUtilities::connectStates(automation->finalState, commonFinalState);
        }
        RegularExpressionParserUtilities::connectStates(commonFinalState, finalEntryState);
        currentState = commonFinalState;
    }
    else
    {
        for(int i{0}; i < subregex.size(); i++)
        {
            switch(subregex[i])
            {
                case '(' :
                {
                    int closingBracketPosition = closingBracketIndex(i, subregex);
                    string nestedSubstring = subregex.substr(i+1, closingBracketPosition - (i+1));
                    FiniteAutomation* automation = new FiniteAutomation(nestedSubstring);
                    i = closingBracketPosition;
                    
                    if(i < subregex.size() - 1 && subregex[i + 1] == PLUS)
                    {
                        currentState = RegularExpressionParserUtilities::faPlus(currentState, automation);
                        i++;
                    }
                    else if(i < subregex.size() - 1 && subregex[i + 1] == ASTERISK)
                    {
                        currentState = RegularExpressionParserUtilities::faAsterisk(currentState, automation);
                        i++;
                    }
                    else
                    {
                        RegularExpressionParserUtilities::connectStates(currentState, automation->initialState);
                        currentState = automation->finalState;
                    }
                    break;
                }
                default :
                {
                    if(i < subregex.size() - 1)
                    {
                        if(subregex[i + 1] == PLUS)
                        {
                            currentState =  RegularExpressionParserUtilities::plus(currentState, string(1,subregex[i]), this);
                            i++;
                            break;
                        }
                        if(subregex[i + 1] == ASTERISK)
                        {
                            currentState =  RegularExpressionParserUtilities::asterisk(currentState, string(1,subregex[i]), this);
                            i++;
                            break;
                        }
                    }
                    StatesPair pair =  RegularExpressionParserUtilities::
                    
                    RegularExpressionParserUtilities::baseStone(string(1,subregex[i]), this);
                    RegularExpressionParserUtilities::connectStates(currentState, pair.initialState);
                    currentState = pair.finalState;
                }
            }
        }
        RegularExpressionParserUtilities::connectStates(currentState, finalEntryState);
    }
}

State* FiniteAutomation::constructFlatRegex(State* currentState, string subregex)
{
    if(subregex.size() > 1)
    {
        for(int i{0}; i < subregex.size(); i++)
        {
            if(i == subregex.size() - 1) {
                string copy = string(1, subregex[i]);
                StatesPair pair =  RegularExpressionParserUtilities::baseStone(copy, this);
                RegularExpressionParserUtilities::connectStates(currentState, pair.initialState);
                currentState = pair.finalState;
                break;
            }
            
            if(subregex[i+1] == PLUS)
            {
                string copy = string(1, subregex[i]);
                currentState  = RegularExpressionParserUtilities::plus(currentState, copy, this);
            }
            
            else if(subregex[i+1] == ASTERISK)
            {
                string copy = string(1, subregex[i]);
                currentState  = RegularExpressionParserUtilities::asterisk(currentState, copy, this);
            }
            else
            {
                string copy1 = string(1, subregex[i]);
                string copy2 = string(1, subregex[i+1]);
                if(regex.size() > i+2) {
                    switch (subregex[i+2]) {
                        case PLUS: {
                            StatesPair pair =  RegularExpressionParserUtilities::baseStone(copy1, this);
                            RegularExpressionParserUtilities::connectStates(currentState, pair.initialState);
                            currentState = RegularExpressionParserUtilities::plus(pair.finalState, copy2, this);
                            i++;
                            break;
                        }
                        case ASTERISK: {
                            StatesPair pair =  RegularExpressionParserUtilities::baseStone(copy1, this);
                            RegularExpressionParserUtilities::connectStates(currentState, pair.initialState);
                            currentState = RegularExpressionParserUtilities::asterisk(pair.finalState, copy2, this);
                            i++;
                            break;
                        }
                        default:
                            currentState =  RegularExpressionParserUtilities::conjunction(currentState, copy1, copy2, this);
                            break;
                    }
                }
                else
                {
                    currentState =  RegularExpressionParserUtilities::conjunction(currentState, copy1, copy2, this);
                }
            }
            i++;
        }
    }
    else
    {
        StatesPair pair =  RegularExpressionParserUtilities::baseStone(subregex, this);
        RegularExpressionParserUtilities::connectStates(currentState, pair.initialState);
        currentState = pair.finalState;
        
    }
    return currentState;
}

State* FiniteAutomation::getCurrentState() const
{
    return currentState;
}

void FiniteAutomation::printFromState(State* currentState, unordered_map<State*, int>& map)
{
    if(currentState->isInitial)
    {
        cout<< "[start->";
    }
    else if(currentState->isFinal)
    {
        cout<<"("<<map[currentState]<<")->end]"<<endl;
        return;
    }
    
    vector<State*> recursiveBranchingStates;
    for(Edge* edge : currentState->outboundTransitions)
    {
        State* toState = edge->toState;
        int edgeNumber = 0;
        if (map.find(toState)==map.end())
        {
            edgeNumber = static_cast<int>(map.size()) + 1;
            map[toState] = edgeNumber;
            recursiveBranchingStates.push_back(toState);
        }
        else
        {
            edgeNumber = map[toState];
        }
        
        cout<<"("<<map[currentState]<<")"<<"-"<<edge->symbol<<"->("<<edgeNumber<<")";
    }
    cout<<"] [";
    
    for(State* state : recursiveBranchingStates)
    {
        printFromState(state, map);
    }
}

void FiniteAutomation::printFromInitialState() {
    unordered_map<State*, int> map;
    map[initialState] = 1;
    printFromState(initialState,  map);
}


bool FiniteAutomation::existsPathway(State* fromState, string word, vector<State*>& visitedStates)
{
    if(word.empty())
    {
        vector<Edge*> visitedTransitions;
        return existsPathwayToFinalState(fromState, visitedTransitions);
    }
    
    bool result = false;
    string initialSymbol = word.substr(0,1);
    string restString = word.substr(1);
    for(Edge* currentStateEdge : fromState->outboundTransitions)
    {
        if(currentStateEdge->symbol == initialSymbol)
        {
            vector<State*> resetVisitedStates;
            result = result || existsPathway(currentStateEdge->getToState(), restString, resetVisitedStates);
        }
        else if(currentStateEdge->symbol == string(1,EPSILON))
        {
            if(find(visitedStates.begin(), visitedStates.end(), currentStateEdge->getToState()) == visitedStates.end())
            {
                visitedStates.push_back(currentStateEdge->getToState());
                result = result || existsPathway(currentStateEdge->getToState(), word, visitedStates);
            }
        }
    }
    return result;
}

bool FiniteAutomation::existsPathwayToFinalState(State* fromState, vector<Edge*>& visitedTransitions) const
{
    bool foundFinalStatePathway = false;
    for(Edge* e: fromState->outboundTransitions)
    {
        if(e->toState->isFinal)
            return true;
       else if(e->symbol == string(1,EPSILON) && find(visitedTransitions.begin(), visitedTransitions.end(), e) == visitedTransitions.end())
        {
            visitedTransitions.push_back(e);
            bool flag = existsPathwayToFinalState(e->getToState(), visitedTransitions);
            foundFinalStatePathway = flag == true ? flag : foundFinalStatePathway;
        }
    }
    return foundFinalStatePathway;
}

bool FiniteAutomation::isAccepted(string word)
{
    State* initialState = nullptr;
    initialState = this->initialState;
    
    vector<State*> visitedStates;
    return existsPathway(initialState, word, visitedStates);
}

State* FiniteAutomation::getInitialState() const
{
    return initialState;
}
   State* FiniteAutomation::getInitialEntryState() const
{
    return initialEntryState;
}
   State* FiniteAutomation::getFinalState() const
{
    return finalState;
}
   State* FiniteAutomation::getFinalEntryState() const
{
    return finalEntryState;
}
