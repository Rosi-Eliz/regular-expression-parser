//
//  RegularExpressionParserutilitiesTests.cpp
//  RegularExpressionParserTests
//
//  Created by Rosi-Eliz Dzhurkova on 04.09.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#include <string>
#include "catch.hh"
#include "FiniteAutomation.h"

SCENARIO("Test for base stone functionality")
{
    GIVEN("A substring from a regular expression")
    {
        string symbol = "a";
        WHEN("Creating a partial automation for substring")
        {
            FiniteAutomation* fa = new FiniteAutomation("");
            StatesPair pair = RegularExpressionParserUtilities::baseStone(symbol, fa);
            THEN("Sub - automation should be created")
            {
                REQUIRE(pair.initialState != nullptr);
                REQUIRE(pair.finalState != nullptr);
                REQUIRE(!pair.initialState->outboundTransitions.empty());
                REQUIRE(pair.finalState->outboundTransitions.empty());
                REQUIRE(pair.initialState->outboundTransitions[0]->symbol == string(1,EPSILON));
                REQUIRE(pair.initialState->outboundTransitions[0]->toState != nullptr);
                State* newState = pair.initialState->outboundTransitions[0]->toState;
                REQUIRE(newState->outboundTransitions[0]->symbol == symbol);
            }
        }
    }
}

SCENARIO("Test for regex conjunction functionality")
{
    GIVEN("Two substrings")
    {
        string substring1 = "a";
        string substring2 = "c";
        WHEN("Creating a partial automation for substring")
        {
            FiniteAutomation* fa = new FiniteAutomation("");
            State* oldCurrentState = fa->getFinalState(); 
            State* newEndState = RegularExpressionParserUtilities::conjunction(fa->getFinalState(), substring1, substring2, fa);
            THEN("The two strings should be concatenated")
            {
                //->a->->->b->
                //initially entering the base stone
                vector<Edge*> edgesToExamine = oldCurrentState->outboundTransitions;
                REQUIRE(edgesToExamine.size() == 1);
                Edge* edgeToExamine = edgesToExamine[0];
                REQUIRE(edgeToExamine->symbol[0] == EPSILON);

                //base stone begins here
                oldCurrentState = edgeToExamine->toState;
                edgesToExamine = oldCurrentState->outboundTransitions;
                REQUIRE(edgesToExamine.size() == 1);
                edgeToExamine = edgesToExamine[0];
                REQUIRE(edgeToExamine->symbol[0] == EPSILON);

                oldCurrentState = edgeToExamine->toState;
                edgesToExamine = oldCurrentState->outboundTransitions;
                REQUIRE(edgesToExamine.size() == 1);
                edgeToExamine = edgesToExamine[0];
                REQUIRE(edgeToExamine->symbol == substring1);

                oldCurrentState = edgeToExamine->toState;
                edgesToExamine = oldCurrentState->outboundTransitions;
                REQUIRE(edgesToExamine.size() == 1);
                edgeToExamine = edgesToExamine[0];
                REQUIRE(edgeToExamine->symbol[0] == EPSILON);
                //end of base stone 1
                //connecting edge between the first and second base stone
                oldCurrentState = edgeToExamine->toState;
                edgesToExamine = oldCurrentState->outboundTransitions;
                REQUIRE(edgesToExamine.size() == 1);
                edgeToExamine = edgesToExamine[0];
                REQUIRE(edgeToExamine->symbol[0] == EPSILON);
                //base stone 2
                oldCurrentState = edgeToExamine->toState;
                edgesToExamine = oldCurrentState->outboundTransitions;
                REQUIRE(edgesToExamine.size() == 1);
                edgeToExamine = edgesToExamine[0];
                REQUIRE(edgeToExamine->symbol[0] == EPSILON);

                oldCurrentState = edgeToExamine->toState;
                edgesToExamine = oldCurrentState->outboundTransitions;
                REQUIRE(edgesToExamine.size() == 1);
                edgeToExamine = edgesToExamine[0];
                REQUIRE(edgeToExamine->symbol == substring2);

                oldCurrentState = edgeToExamine->toState;
                edgesToExamine = oldCurrentState->outboundTransitions;
                REQUIRE(edgesToExamine.size() == 1);
                edgeToExamine = edgesToExamine[0];
                REQUIRE(edgeToExamine->symbol[0] == EPSILON);
                // end of base stone 2
                oldCurrentState = edgeToExamine->toState;
                REQUIRE(oldCurrentState == newEndState);
                REQUIRE(newEndState != nullptr);
                REQUIRE(newEndState-> outboundTransitions.empty());
            }
        }
    }
}

SCENARIO("Test for regex disjunction functionality")
{
    GIVEN("Two substrings")
    {
        string substring1 = "a";
        string substring2 = "b";
        WHEN("Creating a partial automation for substring")
        {
            FiniteAutomation* fa = new FiniteAutomation("");
            State* oldCurrentState = fa->getFinalState();
            State* newEndState = RegularExpressionParserUtilities::conjunction(oldCurrentState, substring1, substring2, fa);
            THEN("The two strings should be connected by a '|' ")
            {
                string result = "";
                unordered_map<State*, int> map;
                map[fa->getInitialState()] = 1;
                result = fa->assignPrintToString(fa->getInitialState(), result, map);
                string expectedResult = "(1)-#->(2)] [(2)-#->(3)] [(3)-#->(4)] [(4)-#->(5)] [(5)-#->(6)] [(6)-a->(7)] [(7)-#->(8)] [(8)-#->(9)] [(9)-#->(10)] [(10)-b->(11)] [(11)-#->(12)] [] [";
                REQUIRE(expectedResult == result);
                REQUIRE(newEndState != nullptr);
                REQUIRE(newEndState-> outboundTransitions.empty());
            }
        }
    }
}

SCENARIO("Test for regex asterisk functionality")
{
    GIVEN("A substring")
    {
        string substring = "a";
        WHEN("Creating a partial automation for substring")
        {
            FiniteAutomation* fa = new FiniteAutomation("");
            State* newEndState = RegularExpressionParserUtilities::asterisk(fa->getCurrentState(), substring, fa);
            THEN("The automation should accept an arbitrary positive number ofsubstring symbols as well as none at all")
            {
                string result = "";
                unordered_map<State*, int> map;
                map[fa->getInitialState()] = 1;
                result = fa->assignPrintToString(fa->getInitialState(), result, map);
                string expectedResult = "(1)-#->(2)] [(2)-#->(3)(2)-#->(4)] [(3)-#->(5)] [] [(4)-#->(6)(4)-#->(7)] [(6)-a->(8)] [(8)-#->(7)] [(7)-#->(4)(7)-#->(9)] [] [";
                REQUIRE(expectedResult == result);
                REQUIRE(newEndState != nullptr);
                REQUIRE(newEndState-> outboundTransitions.empty());
            }
        }
    }
}

SCENARIO("Test for regex plus functionality")
{
    GIVEN("A substring")
    {
        string substring = "a";
        WHEN("Creating a partial automation for substring")
        {
            FiniteAutomation* fa = new FiniteAutomation("");
            State* newEndState = RegularExpressionParserUtilities::plus(fa->getFinalState(), substring, fa);
            THEN("The automation should accept an arbitrary positive number of substring symbols ")
            {
                string result = "";
                unordered_map<State*, int> map;
                map[fa->getInitialState()] = 1;
                result = fa->assignPrintToString(fa->getInitialState(), result, map);
                string expectedResult = "(1)-#->(2)] [(2)-#->(3)] [(3)-#->(4)] [(4)-#->(5)] [(5)-#->(6)] [(6)-a->(7)] [(7)-#->(8)] [(8)-#->(5)(8)-#->(9)] [] [";
                REQUIRE(expectedResult == result);
                REQUIRE(newEndState != nullptr);
                REQUIRE(newEndState-> outboundTransitions.empty());
            }
        }
    }
}

