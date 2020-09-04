//
//  RegularExpressionParserUtilitiesTests.cpp
//  RegularExpressionParserTests
//
//  Created by Rosi-Eliz Dzhurkova on 04.09.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#include "catch.hh"
#include "FiniteAutomation.h"

SCENARIO("Test for base stone functionality")
{
    GIVEN("A substring from a regular expression")
    {
        string regex = "a*";
        WHEN("Creating a partial automation for substring")
        {
            FiniteAutomation* fa = new FiniteAutomation(regex);
            StatesPair pair = RegularExpressionParserUtilities::baseStone(regex, fa);
            THEN("Sub-automation should be created")
            
            REQUIRE(pair.initialState != nullptr);
            REQUIRE(pair.finalState != nullptr);
            REQUIRE(!pair.intialState->outboundTransitions().empty());
            REQUIRE(pair.finalState->outboundTransitions().empty());
        }
        
    }
}
