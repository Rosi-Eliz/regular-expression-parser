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
    
    vector<string> result = topLevelDisjunctions("adsad|sad(asd|(ds|d*))|sd");
    
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

StatesPair FiniteAutomation::baseStone(string symbol)
{
    State* initialState = new State(false, false, this);
    State* finalState = new State(false, false, this);
    State* symbolasterisktState = new State(false, false, this);
    State* symbolEndState = new State(false, false, this);
    Edge* inboundEpsilonTransition = new Edge(string(1, EPSILON), symbolasterisktState);
    Edge* outboundEpsilonTransition = new Edge(string(1, EPSILON), finalState);
    Edge* symbolEdge = new Edge(symbol, symbolEndState);
    initialState->setOutboundTransition(inboundEpsilonTransition);
    symbolasterisktState->setOutboundTransition(symbolEdge);
    symbolEndState->setOutboundTransition(outboundEpsilonTransition);
    
    return StatesPair(initialState, finalState);
}

void FiniteAutomation::connectStates(State* fromState, State* toState)
{
    Edge* connectingEdge = new Edge(string(1, EPSILON), toState);
    fromState->setOutboundTransition(connectingEdge);
}

void FiniteAutomation::repetition(State* currentState, string subregex, FiniteAutomation* nestedAutomation,Operation operation)
{
    State* oldCurrentState = currentState;
    switch(operation)
    {
        case Plus:  currentState = plus(currentState, nestedAutomation); break;
        case Asterisk:  currentState = asterisk(currentState, nestedAutomation); break;
    }
    subregex = subregex.substr(1);
    if(subregex.size() > 0 && subregex[1] == OR)
    {
        FiniteAutomation* rightAutomation = new FiniteAutomation(subregex);
        connectStates(oldCurrentState, rightAutomation->initialState);
        State* finalConnectingState = new State(false, false);
        connectStates(currentState, finalConnectingState);
        connectStates(rightAutomation->finalState, finalConnectingState);
        currentState = finalConnectingState;
    }
    else if(subregex.size() > 0)
    {
        FiniteAutomation* rightAutomation = new FiniteAutomation(subregex);
        connectStates(currentState, rightAutomation->initialState);
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
    
    if(subregex.size() == 0)
        throw runtime_error("Empty regex!");
    
    //asd(a|b)hg
    FiniteAutomation *nestedAutomation = nullptr;
    if(subregex.find("(") != string::npos)
    {
        size_t firstIndex = subregex.find("(");
        string leftPart = subregex.substr(0, firstIndex);
        
        if(subregex.find(")") == string::npos)
            throw runtime_error("Missing bracket!");
        
        size_t lastIndex = subregex.find_last_of(")");
        string nestedSubRegex = subregex.substr(firstIndex+1, lastIndex-1);
        nestedAutomation = new FiniteAutomation(nestedSubRegex);
        subregex = subregex.substr(lastIndex+1);
    }
    if(nestedAutomation != nullptr)
    {
        if(subregex[0] == PLUS)
        {
            repetition(currentState, subregex, nestedAutomation, Operation::Plus);
            //as|((c|d)bbdf)*bf(af)d
        }
        else if(subregex[0] == ASTERISK)
        {
             repetition(currentState, subregex, nestedAutomation, Operation::Asterisk);
        }
        else if(subregex[0] == OR)
        {
            subregex = subregex.substr(1);
            FiniteAutomation* rightAutomation = new FiniteAutomation(subregex);
            currentState = disjunction(currentState, nestedAutomation, rightAutomation);
            
        }
        else
        {
            subregex = subregex.substr(1);
            FiniteAutomation* rightAutomation = new FiniteAutomation(subregex);
            currentState = conjunction(currentState, nestedAutomation, rightAutomation);
        }
    }
    else
    {
        if(regex.find(OR) != string::npos)
        {
            //abbc|d*t|c
            vector<FiniteAutomation*> processedNestedAutomations;
            size_t searchasterisktIndex = 0;
            while(regex.find(OR, searchasterisktIndex) != string::npos){
                size_t index = regex.find(OR, searchasterisktIndex);
                string subRegex = regex.substr(searchasterisktIndex, index - searchasterisktIndex );
                searchasterisktIndex = index + 1;
                FiniteAutomation* rightAutomation = new FiniteAutomation(subRegex);
                processedNestedAutomations.push_back(rightAutomation);
            }
            string finalPortion = regex.substr(searchasterisktIndex);
            FiniteAutomation* rightAutomation = new FiniteAutomation(finalPortion);
            processedNestedAutomations.push_back(rightAutomation);
            
            State* finalDisjunctionState = new State(false, false);
            for(int i{0}; i < processedNestedAutomations.size(); i++)
            {
                connectStates(currentState, processedNestedAutomations[i]->initialState);
                connectStates(processedNestedAutomations[i]->finalState, finalDisjunctionState);
            }
            currentState = finalDisjunctionState;
        }
        else
        {
            currentState = constructFlatRegex(currentState, regex);
        }
    }
    connectStates(currentState, finalEntryState);
}

State* FiniteAutomation::constructFlatRegex(State* currentState, string subregex)
{
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
            else if(subregex[i+1] == ASTERISK)
            {
                string copy = string(1, subregex[i]);
                currentState  = asterisk(currentState, copy);
            }
            else
            {
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
                        case ASTERISK: {
                            StatesPair pair = baseStone(copy1);
                            connectStates(currentState, pair.initialState);
                            currentState = asterisk(pair.finalState, copy2);
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
        connectStates(currentState, pair.initialState);
        currentState = pair.finalState;
        
    }
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

State* FiniteAutomation::asterisk(State* currentState, string word){
    
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
        cout<< "[asteriskt->";
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

//FA Operations

State* FiniteAutomation::conjunction(State* currentState, FiniteAutomation* automation1, FiniteAutomation* automation2)
{
    connectStates(currentState, automation1->initialState);
    connectStates(automation1->finalState, automation2->initialState);
    return automation2->finalState;
}


State* FiniteAutomation::disjunction(State* currentState, FiniteAutomation* automation1, FiniteAutomation* automation2)
{
    connectStates(currentState, automation1->initialState);
    connectStates(currentState, automation2->initialState);
    State* finalConnectingState = new State(false, false);
    connectStates(automation1->finalState, finalConnectingState);
    connectStates(automation2->finalState, finalConnectingState);
    return finalConnectingState;
}

State* FiniteAutomation::asterisk(State* currentState, FiniteAutomation* automation)
{
    connectStates(currentState, automation->initialState);
    connectStates(automation->initialEntryState, automation->finalEntryState);
    connectStates(automation->finalEntryState, automation->initialEntryState);
    return automation->finalState;
}

State* FiniteAutomation::plus(State* currentState, FiniteAutomation* automation)
{
    connectStates(currentState, automation->initialState);
    connectStates(automation->initialEntryState, automation->finalEntryState);
    return automation->finalState;
}
