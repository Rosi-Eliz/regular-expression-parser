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
#include <iostream>
using namespace std;

FiniteAutomation::FiniteAutomation(string regex, bool setInitialStates, bool setFinalStates) {
constructInitialStates(setInitialStates, setFinalStates);
    this->regex = regex;
    
    
    if(regex.find(OR) != string::npos)
    {
        size_t searchStartIndex = 0;
        while(regex.find(OR, searchStartIndex) != string::npos){
            size_t index = regex.find(OR, searchStartIndex);
            string subRegex = regex.substr(searchStartIndex, index - searchStartIndex );
            searchStartIndex = index + 1;
            cout<<subRegex<<endl;
            
        }
        string finalPortion = regex.substr(searchStartIndex);
        cout<<finalPortion<<endl;
    }
    else
    {
        constructSubRegex(regex);
    }
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

StatesPair FiniteAutomation::baseStone(string symbol)
{
    State* initialState = new State(false, false, this);
    State* finalState = new State(false, false, this);
    State* symbolStartState = new State(false, false, this);
    State* symbolEndState = new State(false, false, this);
    Edge* inboundEpsilonTransition = new Edge(string(1, EPSILON), symbolStartState);
    Edge* outboundEpsilonTransition = new Edge(string(1, EPSILON), finalState);
    Edge* symbolEdge = new Edge(symbol, symbolEndState);
    initialState->setOutboundTransition(inboundEpsilonTransition);
    symbolStartState->setOutboundTransition(symbolEdge);
    symbolEndState->setOutboundTransition(outboundEpsilonTransition);
    
    return StatesPair(initialState, finalState);
}

void FiniteAutomation::connectStates(State* fromState, State* toState)
{
    Edge* connectingEdge = new Edge(string(1, EPSILON), toState);
    fromState->setOutboundTransition(connectingEdge);
}

State* FiniteAutomation::constructSubRegex(string subregex)
{
    State *currentState = initialEntryState;
    if(subregex.size() > 1)
    {
        for(int i{0}; i < subregex.size(); i++)
        {
            if(i == subregex.size() - 1) {
                string copy = string(1, subregex[i]);
                StatesPair pair = baseStone(copy);
                connectStates(currentState, pair.initialState);
                currentState = pair.finalState;
                break;
            }
            if(subregex[i+1] == PLUS)
            {
                string copy = string(1, subregex[i]);
                currentState  = plus(currentState, copy);
            }
            
            else if(subregex[i+1] == STAR)
            {
                string copy = string(1, subregex[i]);
                currentState  = star(currentState, copy);
            }
            else{
                string copy1 = string(1, subregex[i]);
                string copy2 = string(1, subregex[i+1]);
                if(regex.size() > i+2) {
                    switch (subregex[i+2]) {
                        case PLUS: {
                            StatesPair pair = baseStone(copy1);
                            connectStates(currentState, pair.initialState);
                            currentState = plus(pair.finalState, copy2);
                            i++;
                            break;
                        }
                        case STAR: {
                            StatesPair pair = baseStone(copy1);
                            connectStates(currentState, pair.initialState);
                            currentState = star(pair.finalState, copy2);
                            i++;
                            break;
                        }
                        default:
                            currentState = conjunction(currentState, copy1, copy2);
                            break;
                    }
                }
                else
                {
                    currentState = conjunction(currentState, copy1, copy2);
                }
            }
            i++;
        }
    }
    else
    {
        StatesPair pair = baseStone(subregex);
        Edge* connectingInitialEdge = new Edge(string(1, EPSILON), pair.initialState);
        currentState->setOutboundTransition(connectingInitialEdge);
        currentState = pair.finalState;
        
    }
    Edge* connectingFinalEdge = new Edge(string(1, EPSILON), finalEntryState);
    currentState->setOutboundTransition(connectingFinalEdge);
    
    return currentState;
}

State* FiniteAutomation::conjunction(State* currentState, string first, string second)
{
    StatesPair firstStonePair = baseStone(first);
    Edge* initialConnectingEdge = new Edge(string(1, EPSILON), firstStonePair.initialState);
    currentState->setOutboundTransition(initialConnectingEdge);
    
    StatesPair secondStonePair = baseStone(second);
    connectStates(firstStonePair.finalState, secondStonePair.initialState);
    
    return secondStonePair.finalState;
}

State* FiniteAutomation::disjunction(State* currentState, string first, string second){
    StatesPair firstStonePair = baseStone(first);
    State* firstStoneInitialState = firstStonePair.initialState;
    State* firstStoneFinalState = firstStonePair.finalState;
    
    StatesPair secondStonePair = baseStone(second);
    State* secondStoneInitialState = secondStonePair.initialState;
    State* secondStoneFinalState = secondStonePair.finalState;
    
    State* connectingState = new State(false, false, this);
    Edge* connectingEdge = new Edge(string(1, EPSILON), connectingState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* firstEpsilonTransition = new Edge (string(1, EPSILON), firstStoneInitialState);
    Edge* secondEpsilonTransition = new Edge(string(1, EPSILON), secondStoneInitialState);
    connectingState->setOutboundTransition(firstEpsilonTransition);
    connectingState->setOutboundTransition(secondEpsilonTransition);
    
    State* connectingFinalState = new State(false, false, this);
    Edge* firstFinalTransition = new Edge(string(1, EPSILON), connectingFinalState);
    Edge* secondFinalTransition = new Edge(string(1, EPSILON), connectingFinalState);
    firstStoneFinalState->setOutboundTransition(firstFinalTransition);
    secondStoneFinalState->setOutboundTransition(secondFinalTransition);
    
    State* finalState = new State(false, false, this);
    Edge* finalTransition = new Edge(string(1, EPSILON), finalState);
    connectingFinalState->setOutboundTransition(finalTransition);
    
    return finalState;
}

State* FiniteAutomation::star(State* currentState, string word){
    
    StatesPair pair = baseStone(word);
    State* symbolInitialState = pair.initialState;
    State* symbolFinalState = pair.finalState;
    
    Edge* connectingEdge = new Edge(string(1, EPSILON), symbolInitialState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* returnEdge = new Edge(string(1, EPSILON), symbolInitialState);
    symbolFinalState->setOutboundTransition(returnEdge);
    Edge* skipEdge = new Edge(string(1, EPSILON), symbolFinalState);
    symbolInitialState->setOutboundTransition(skipEdge);
    
    
    State* finalState = new State(false, false, this);
    Edge* finalEdge = new Edge(string(1, EPSILON), finalState);
    symbolFinalState->setOutboundTransition(finalEdge);
    
    return finalState;
}

State* FiniteAutomation::plus(State* currentState, string word){
    
    StatesPair pair = baseStone(word);
    State* symbolInitialState = pair.initialState;
    State* symbolFinalState = pair.finalState;
    
    Edge* connectingEdge = new Edge(string(1, EPSILON), symbolInitialState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* returnEdge = new Edge(string(1, EPSILON), symbolInitialState);
    symbolFinalState->setOutboundTransition(returnEdge);
    
    State* finalState = new State(false, false, this);
    Edge* finalEdge = new Edge(string(1, EPSILON), finalState);
    symbolFinalState->setOutboundTransition(finalEdge);
    
    return finalState;
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
