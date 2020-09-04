//
//  RegularExpressionParserUtilities.cpp
//  RegularExpressionParser
//
//  Created by Rosi-Eliz Dzhurkova on 04.09.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#include "RegularExpressionParserUtilities.h"
#include <string>
#include "FiniteAutomation.h"

void RegularExpressionParserUtilities::connectStates(State* fromState, State* toState)
{
    Edge* connectingEdge = new Edge(string(1, EPSILON), toState);
    fromState->setOutboundTransition(connectingEdge);
}

StatesPair RegularExpressionParserUtilities::baseStone(string symbol, FiniteAutomation* automation)
{
    State* initialState = new State(false, false, automation);
    State* finalState = new State(false, false, automation);
    State* symbolStartState = new State(false, false, automation);
    State* symbolEndState = new State(false, false, automation);
    Edge* inboundEpsilonTransition = new Edge(string(1, EPSILON), symbolStartState);
    Edge* outboundEpsilonTransition = new Edge(string(1, EPSILON), finalState);
    Edge* symbolEdge = new Edge(symbol, symbolEndState);
    initialState->setOutboundTransition(inboundEpsilonTransition);
    symbolStartState->setOutboundTransition(symbolEdge);
    symbolEndState->setOutboundTransition(outboundEpsilonTransition);
    
    return StatesPair(initialState, finalState);
}


State* RegularExpressionParserUtilities::conjunction(State* currentState, string first, string second, FiniteAutomation* automation)
{
    StatesPair firstStonePair = baseStone(first, automation);
    Edge* initialConnectingEdge = new Edge(string(1, EPSILON), firstStonePair.initialState);
    currentState->setOutboundTransition(initialConnectingEdge);
    
    StatesPair secondStonePair = baseStone(second, automation);
    connectStates(firstStonePair.finalState, secondStonePair.initialState);
    
    return secondStonePair.finalState;
}

State* RegularExpressionParserUtilities::disjunction(State* currentState, string first, string second, FiniteAutomation* automation){
    StatesPair firstStonePair = baseStone(first, automation);
    State* firstStoneInitialState = firstStonePair.initialState;
    State* firstStoneFinalState = firstStonePair.finalState;
    
    StatesPair secondStonePair = baseStone(second, automation);
    State* secondStoneInitialState = secondStonePair.initialState;
    State* secondStoneFinalState = secondStonePair.finalState;
    
    State* connectingState = new State(false, false, automation);
    Edge* connectingEdge = new Edge(string(1, EPSILON), connectingState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* firstEpsilonTransition = new Edge (string(1, EPSILON), firstStoneInitialState);
    Edge* secondEpsilonTransition = new Edge(string(1, EPSILON), secondStoneInitialState);
    connectingState->setOutboundTransition(firstEpsilonTransition);
    connectingState->setOutboundTransition(secondEpsilonTransition);
    
    State* connectingFinalState = new State(false, false, automation);
    Edge* firstFinalTransition = new Edge(string(1, EPSILON), connectingFinalState);
    Edge* secondFinalTransition = new Edge(string(1, EPSILON), connectingFinalState);
    firstStoneFinalState->setOutboundTransition(firstFinalTransition);
    secondStoneFinalState->setOutboundTransition(secondFinalTransition);
    
    State* finalState = new State(false, false, automation);
    Edge* finalTransition = new Edge(string(1, EPSILON), finalState);
    connectingFinalState->setOutboundTransition(finalTransition);
    
    return finalState;
}

State* RegularExpressionParserUtilities::asterisk(State* currentState, string word, FiniteAutomation* automation){
    
    StatesPair pair = baseStone(word, automation);
    State* symbolInitialState = pair.initialState;
    State* symbolFinalState = pair.finalState;
    
    Edge* connectingEdge = new Edge(string(1, EPSILON), symbolInitialState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* returnEdge = new Edge(string(1, EPSILON), symbolInitialState);
    symbolFinalState->setOutboundTransition(returnEdge);
    Edge* skipEdge = new Edge(string(1, EPSILON), symbolFinalState);
    symbolInitialState->setOutboundTransition(skipEdge);

    State* finalState = new State(false, false, automation);
    Edge* finalEdge = new Edge(string(1, EPSILON), finalState);
    symbolFinalState->setOutboundTransition(finalEdge);
    
    return finalState;
}

State* RegularExpressionParserUtilities::plus(State* currentState, string word, FiniteAutomation* automation){
    
    StatesPair pair = baseStone(word, automation);
    State* symbolInitialState = pair.initialState;
    State* symbolFinalState = pair.finalState;
    
    Edge* connectingEdge = new Edge(string(1, EPSILON), symbolInitialState);
    currentState->setOutboundTransition(connectingEdge);
    
    Edge* returnEdge = new Edge(string(1, EPSILON), symbolInitialState);
    symbolFinalState->setOutboundTransition(returnEdge);
    
    State* finalState = new State(false, false, automation);
    Edge* finalEdge = new Edge(string(1, EPSILON), finalState);
    symbolFinalState->setOutboundTransition(finalEdge);
    
    return finalState;
}

// MARK: - FA Operations
 
State* RegularExpressionParserUtilities::faConjunction(State* currentState, FiniteAutomation* automation1, FiniteAutomation* automation2)
{
    RegularExpressionParserUtilities::connectStates(currentState, automation1->getInitialState());
    RegularExpressionParserUtilities::connectStates(automation1->getFinalState(), automation2->getInitialState());
    return automation2->getFinalState();
}
 
State* RegularExpressionParserUtilities::faDisjunction(State* currentState, FiniteAutomation* automation1, FiniteAutomation* automation2)
{
    RegularExpressionParserUtilities::connectStates(currentState, automation1->getInitialState());
    RegularExpressionParserUtilities::connectStates(currentState, automation2->getInitialState());
    State* finalConnectingState = new State(false, false);
    RegularExpressionParserUtilities::connectStates(automation1->getFinalState(), finalConnectingState);
    RegularExpressionParserUtilities::connectStates(automation2->getFinalState(), finalConnectingState);
    return finalConnectingState;
}

State* RegularExpressionParserUtilities::faAsterisk(State* currentState, FiniteAutomation* automation)
{
    RegularExpressionParserUtilities::connectStates(currentState, automation->getInitialState());
    RegularExpressionParserUtilities::connectStates(automation->getInitialEntryState(), automation->getFinalEntryState());
    RegularExpressionParserUtilities::connectStates(automation->getFinalEntryState(), automation->getInitialEntryState());
    return automation->getFinalState();
}

State* RegularExpressionParserUtilities::faPlus(State* currentState, FiniteAutomation* automation)
{
    RegularExpressionParserUtilities::connectStates(currentState, automation->getInitialState());
    RegularExpressionParserUtilities::connectStates(automation->getFinalEntryState(), automation->getInitialEntryState());
    return automation->getFinalState();
}

