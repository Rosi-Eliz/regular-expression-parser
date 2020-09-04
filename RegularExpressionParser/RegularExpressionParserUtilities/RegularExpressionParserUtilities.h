//
//  RegularExpressionParserUtilities.h
//  RegularExpressionParser
//
//  Created by Rosi-Eliz Dzhurkova on 03.09.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#ifndef RegularExpressionParserUtilities_h
#define RegularExpressionParserUtilities_h
#include "FiniteAutomationAttributes.h"
class FiniteAutomation;

using namespace std;

class RegularExpressionParserUtilities {
public:
    static void connectStates(State* fromState, State* toState);
    static StatesPair baseStone(string symbol, FiniteAutomation* automation);
    static State* conjunction(State* currentState, string first, string second, FiniteAutomation* automation);
    static State* disjunction(State* currentState, string first, string second, FiniteAutomation* automation);
    static State* asterisk(State* currentState, string word, FiniteAutomation* automation);
    static State* plus(State* currentState, string word, FiniteAutomation* automation);
    
    //FA Operations
    
    static State* faConjunction(State* currentState, FiniteAutomation* automation1, FiniteAutomation* automation2);
    static State* faDisjunction(State* currentState, FiniteAutomation* automation1, FiniteAutomation* automation2);
    static State* faAsterisk(State* currentState, FiniteAutomation* automation);
    static State* faPlus(State* currentState, FiniteAutomation* automation);
};



#endif /* RegularExpressionParserUtilities_h */
