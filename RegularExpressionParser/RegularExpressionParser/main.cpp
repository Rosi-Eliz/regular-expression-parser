//
//  main.cpp
//  RegularExpressionParser
//
//  Created by Rosi-Eliz Dzhurkova on 24.06.20.
//  Copyright © 2020 Rosi-Eliz Dzhurkova. All rights reserved.
//

#include <iostream>
#include "FiniteAutomation.h"
using namespace std;

int main(int argc, const char * argv[]) {
    
    //Example
    string regex = "((c|b)+b)*gf";
    FiniteAutomation automation(regex, true, true);
    string word = "ccbbcbcbcbbcbgf";
    
    bool isAccepted = automation.isAccepted(word);
    if(isAccepted)
        cout<<"Accepted!"<<endl;
    else
        cout<<"Rejected!"<<endl;

    return 0;
}
