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
    initialState = new State(setInitialStates,false);
    initialEntryState = new State(false, false);
    finalState = new State(false,setFinalStates);
    finalEntryState = new State(false, false);
    
    Edge* initialEpsilonTransition = new Edge(EPSILON, initialEntryState);
    Edge* finalEpsilonTransition = new Edge(EPSILON, finalState);
    initialState->setOutboundTransition(initialEpsilonTransition);
    finalEntryState->setOutboundTransition(finalEpsilonTransition);
}




void FiniteAutomation::baseStone(string symbol, State* initialState, State* finalState)
{
    initialState = new State(false, false);
    finalState = new State(false, false);
    State* symbolStartState = new State(false, false);
    State* symbolEndState = new State(false, false);
    Edge* inboundEpsilonTransition = new Edge(EPSILON, symbolStartState);
    Edge* outboundEpsilonTransition = new Edge(EPSILON, finalState);
    Edge* symbolEdge = new Edge(symbol, symbolEndState);
    initialState->setOutboundTransition(inboundEpsilonTransition);
    symbolStartState->setOutboundTransition(symbolEdge);
    symbolEndState->setOutboundTransition(outboundEpsilonTransition);
    
}



State* FiniteAutomation::constructSubRegex(string subregex)
{
    State *currentState = initialEntryState;
    if(subregex.size() > 1)
    {
        for(int i{0}; i < subregex.size() - 1; i++)
        {
            if(subregex[i+1] == PLUS[0])
            {
                string copy = string(1, subregex[i]);
                currentState  = plus(currentState, copy);
            }
            
            else if(subregex[i+1] == STAR[0])
            {
                string copy = string(1, subregex[i]);
                currentState  = star(currentState, copy);
            }
            else{
                string copy1 = string(1, subregex[i]);
                string copy2 = string(1, subregex[i+1]);
                currentState = conjunction(currentState, copy1, copy2);
            }
            i++;
            
        }
    }
    else
    {
        State *initialState = nullptr;
        State *finalState = nullptr;
        baseStone(subregex, initialState, finalState);
        Edge* connectingInitialEdge = new Edge(EPSILON, initialState);
        currentState->setOutboundTransition(connectingInitialEdge);
        currentState = finalState;
        
    }
    Edge* connectingFinalEdge = new Edge(EPSILON, finalEntryState);
    currentState->setOutboundTransition(connectingFinalEdge);
    
    return currentState;
}

State* FiniteAutomation::conjunction(State* currentState, string first, string second)
{
    State* firstStoneInitialState = nullptr;
    State* firstStoneFinalState = nullptr;
    baseStone(first, firstStoneInitialState, firstStoneFinalState);
    Edge* initialConnectingEdge = new Edge(EPSILON, firstStoneInitialState);
    currentState->setOutboundTransition(initialConnectingEdge);
    
    State* secondStoneInitialState = nullptr;
    State* secondStoneFinalState = nullptr;
    baseStone(second, secondStoneInitialState, secondStoneFinalState);
    State* finalState = new State(false, false);
    
    Edge* finalConnectingEdge = new Edge(EPSILON, finalState);
    secondStoneFinalState->setOutboundTransition(finalConnectingEdge);
    
    return finalState;
}

State* FiniteAutomation::disjunction(State* currentState, string first, string second){
    State* firstStoneInitialState = nullptr;
    State* firstStoneFinalState = nullptr;
    baseStone(first, firstStoneInitialState, firstStoneFinalState);
    
    State* secondStoneInitialState = nullptr;
    State* secondStoneFinalState = nullptr;
    baseStone(second, secondStoneInitialState, secondStoneFinalState);
    
    State* connectingState = new State(false, false);
    Edge* connectingEdge = new Edge(EPSILON, connectingState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* firstEpsilonTransition = new Edge (EPSILON, firstStoneInitialState);
    Edge* secondEpsilonTransition = new Edge(EPSILON, secondStoneInitialState);
    connectingState->setOutboundTransition(firstEpsilonTransition);
    connectingState->setOutboundTransition(secondEpsilonTransition);
    
    State* connectingFinalState = new State(false, false);
    Edge* firstFinalTransition = new Edge(EPSILON, connectingFinalState);
    Edge* secondFinalTransition = new Edge(EPSILON, connectingFinalState);
    firstStoneFinalState->setOutboundTransition(firstFinalTransition);
    secondStoneFinalState->setOutboundTransition(secondFinalTransition);
    
    State* finalState = new State(false, false);
    Edge* finalTransition = new Edge(EPSILON, finalState);
    connectingFinalState->setOutboundTransition(finalTransition);
    
    return finalState;
}

State* FiniteAutomation::star(State* currentState, string word){
    
    State* symbolInitialState = nullptr;
    State* symbolFinalState = nullptr;
    baseStone(word, symbolInitialState, symbolFinalState);
    
    Edge* connectingEdge = new Edge(EPSILON, symbolInitialState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* returnEdge = new Edge(EPSILON, symbolInitialState);
    symbolFinalState->setOutboundTransition(returnEdge);
    Edge* skipEdge = new Edge(EPSILON, symbolFinalState);
    symbolInitialState->setOutboundTransition(skipEdge);
    
    
    State* finalState = new State(false, false);
    Edge* finalEdge = new Edge(EPSILON, finalState);
    symbolFinalState->setOutboundTransition(finalEdge);
    
    return finalState;
}

State* FiniteAutomation::plus(State* currentState, string word){
    
    State* symbolInitialState = nullptr;
    State* symbolFinalState = nullptr;
    baseStone(word, symbolInitialState, symbolFinalState);
    
    Edge* connectingEdge = new Edge(EPSILON, symbolInitialState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* returnEdge = new Edge(EPSILON, symbolInitialState);
    symbolFinalState->setOutboundTransition(returnEdge);
    
    State* finalState = new State(false, false);
    Edge* finalEdge = new Edge(EPSILON, finalState);
    symbolFinalState->setOutboundTransition(finalEdge);
    
    return finalState;
}

State* FiniteAutomation::getCurrentState() const
{
    return currentState;
}
