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
    
    while(!cin.eof())
    {
        string input;
        cout<<"Please enter regex: ";
        getline(cin,input);
        string word;
        cout<<"Enter a word you would like to verify: ";
        getline(cin, word);
        //a+(c|b)*fg
        FiniteAutomation automation(input, true, true);
        cout<<"Epsilon - NFA regex Representation: "<<endl;
        automation.printFromInitialState();
        
        bool isAccepted = automation.isAccepted(word);
        if(isAccepted)
            cout<<"Accepted!"<<endl;
        else
            cout<<"Rejected!"<<endl;
    }
    return 0;
}
