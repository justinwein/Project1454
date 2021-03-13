//Created by Justin Weiner, Max Lankford, Marco Barragan
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cassert>
#include <queue>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <vector>
#include "BigUnsignedInABase.h" //unlimited precision library

using namespace std;

class DFA
{
public:

    void set_q0(unsigned int q0)
    {
        if(!(is_in_Q(q0))){
            return;
        };
        this->q0 = q0;
    }

    unsigned int get_q0() const { return q0; }

    void set_Sigma(const std::string& Sigma)
    {
        this->Sigma = Sigma;
        sort(this->Sigma.begin(), this->Sigma.end());
    }
    string get_sigma() const { return Sigma; } //access sigma letters

    //transition function to get to next state. returns the next state.
    virtual unsigned int delta(unsigned int j, char a) const = 0;

    //checks to see if state is in Q. returns true if q is in Q.
    virtual bool is_in_Q(unsigned int q) const = 0;

    //checks to see if state is in accepting state. returns true if state is accepted.
    virtual bool is_in_F(unsigned int q) const = 0;

    virtual unsigned int Q_cardinality() const = 0;

protected:

    std::string Sigma;	// Alphabet
    unsigned int q0;	// Initial state
};

//For problem #1
class Substring : public DFA
{
public:
    Substring() {set_Sigma("abcd"); set_q0(0);} //sigma will always be a,b,c,d for this DFA
private:

    bool is_in_Q(unsigned int q) const { return 0 <= q && q <= 1365; } //have 1366 states

    unsigned int Q_cardinality() const { return 1366; };//number of states

    unsigned int delta(unsigned int j, char a) const {
        if (j == 1365){//check for fail state
            return 1365;
        }
        string word = stateToString(j); //take state and convert to letter representation
        word += a;
        if (word.length() == 6){ //if less than 6, just return state.
            for (auto c : get_sigma()){//loop through letters in Sigma
                //check to see if string has all 4 letters of sigma
                if(word.find(c) == string::npos){//if we dont find an occurrence of a letter in sigma, it is a failstate
                    return 1365;//if string does not have 1 of the letters in Sigma, return failstate
                }
            }
            word.erase(word.begin());//erase first letter of word so we dont keep adding letters to string. only
            //encodes 5 letter strings. always remove first letter of string,so we can check for the next letter.
        }
        return strToState(word);
    }

    bool is_in_F(unsigned int q) const { return 0 <= q && q < 1365; }//make sure state is a valid state

    unsigned int strToState(const string & str) const
    {
        if (str.empty()) {
            return 0;
        }
        //encoding the length into the bits 10, 11, 12 of q
        unsigned int q = str.length() * 1024;

        // Loop through the letters of the string in reverse order
        unsigned int r = unsigned(str.back() - 'a'); //get binary value of last letter(00, 01, 10, 11)
        for (auto it = str.crbegin(); ++it != str.crend(); ) {//loop through string in reverse
            r *= 4; //left shift for each letter two bits
            r += unsigned(*it - 'a');//calculate binary representation of letter
        }
        q += r; //add r to q
        q = wideToNarrow(q);//map value of q to corresponding narrow value

        return q;
    }

    string stateToString(unsigned int q) const {
        if (!is_in_Q(q)){
            cout << "not a state in q" << endl;
        }
        string result;
        q = narrowToWide(q);//convert from condensed form and then converted to string
        for (unsigned int i = 0, length = q / 1024; i < length; i++){ //loop through q LSB's from right to left
            result.push_back('a' + char(q % 4)); //convert bits to chars
            q /= 4; //discard the least two significant bit we just converted.
        }
        return result;
    }

    static unsigned int wideToNarrow(unsigned int q) {//map intervals to continuos range
        if (q == 0) { return 0; }
        if (q <= 1027) { return q - 1023; }
        if (q <= 2063) { return q - 2043; }
        if (q <= 3135) { return q - 3051; }
        if (q <= 4351) { return q - 4011; }
        if (q <= 6143) { return q - 4779; }
        return 1365;
    }

    static unsigned int narrowToWide(unsigned int q) {//map intervals from continous range
        if (q == 0) { return 0; }
        if (q <= 4) { return q + 1023; }
        if (q <= 20) { return q + 2043; }
        if (q <= 84) { return q + 3051; }
        if (q <= 340) { return q + 4011; }
        if (q <= 1364) { return q + 4779; }
        return 6144;
    }
};

BigUnsigned count(const DFA& M, unsigned int n){
    unsigned int Qsize = M.Q_cardinality(); //get size of Q = amount of states
    //initialize two vectors of type Bigunsigned
    vector<BigUnsigned> counts[2] = { vector<BigUnsigned>(Qsize), vector<BigUnsigned>(Qsize) };
    for (unsigned int i = 0; i < Qsize; i++){
        counts[0][i] = 1; //set all of currents counts to 1
        counts[1][i] = 0;//set all nexts counts to 0
    }
    counts[0][Qsize - 1] = 0;//set last of curr to 0 because it is fail state.
    unsigned int curr = 0;
    for (unsigned int c = 0; c < n; c++){
        for (unsigned int j = 0; j < Qsize; j++){
            BigUnsigned temp = 0;
            for (auto s : M.get_sigma()){
                temp += counts[curr][M.delta(j, s)];
            }
            counts[1-curr][j] = temp;//updating next count
        }
        curr = 1 - curr; //switch curr vector with next
        for (unsigned int j = 0; j < Qsize; j++){
            counts[1-curr][j] = 0;//next vector will always be set back to 0 after every iteration
        }
    }
    return counts[curr][0];//number of states reachable from starting state
}

// For problem #2
class SmallestMultiple : public DFA
{
public:

    unsigned int k;
    bool is_in_Q(unsigned int q) const { return 0 <= q && q <= k; }
    unsigned int Q_cardinality() const { return k + 1; };
    unsigned int delta(unsigned int j, char a) const { return (10 * j + (unsigned int)(a - '0')) % k; }
    bool is_in_F(unsigned int q) const { return q == 0; }
};

string findString(unsigned int k, const DFA& M, char exclude_as_start_letter){
    unordered_map < unsigned int, pair < unsigned int, char > > visited;
    queue <tuple < unsigned int, unsigned int, char> > Queue;
    string sigmaLetters = M.get_sigma();
    for (auto s : sigmaLetters){
        //dont start with a 0, will just output 0 because it thinks we have hit our accepting state
        if (s != exclude_as_start_letter) {
            unsigned int curr = M.delta(M.get_q0(), s);//get to next state from start state
            Queue.push(make_tuple(curr, M.get_q0(), s));//state we transitioned to, parent, symbol
        }
    }
    bool found = false;
    unsigned int lastState;
    while (!Queue.empty()){
        tuple<unsigned int, unsigned int, char> curr = Queue.front();//state we transitioned to, parent, symbol
        Queue.pop();
        auto result = visited.insert({get<0>(curr), make_pair(get<1>(curr), get<2>(curr))});
        if (result.second == false){ //return pair, first is an iterator that tells us where it was inserted if performed
            //second is a bool if the insertion was actually performed, if not, then skip.
            //checking to see if curr has been visited, if not, continue
            continue;
        }
        if (M.is_in_F(get<0>(curr))){//if we find an accepting state, we have a solution
            found = true;
            lastState = get<0>(curr);
            break;
        }
        for (auto s : sigmaLetters){
            unsigned int next = M.delta(get<0>(curr), s);
            Queue.push(make_tuple(next, get<0>(curr), s));//current state, parent, symbol
        }
    }
    string solution;
    if (found){
        //reconstruct a path
        auto it = visited.find(lastState);
        while (it->second.first != M.get_q0()){  //while parent is not start state
            solution.push_back(it->second.second);//push back into solution the transition labels
            it = visited.find(it -> second.first);//go to predecessor
        }
        solution.push_back(it->second.second);//push back first transition letter from the path
        reverse(solution.begin(), solution.end());//solution is now backwards, need to reverse
    }

    return solution;
}

int main()
{


    cout << "Enter 1 for Problem 1" << endl;
    cout << "Enter 2 for Problem 2" << endl;
    cout << "Enter 3 to quit" << endl;
    cout << "Enter Choice: ";
    unsigned int problemChoice;
    cin >> problemChoice;
    while(problemChoice != 3){
        if (problemChoice == 1){
            Substring substr;
            unsigned int n;
            cout << "Enter a value for n: ";
            cin >> n;
            cout << ((string)BigUnsignedInABase(count(substr, n), 10)) << endl;
            cout << endl << "Enter 1 for Problem 1" << endl;
            cout << "Enter 2 for Problem 2" << endl;
            cout << "Enter 3 to quit" << endl;
            cout << "Enter Choice: ";
            cin >> problemChoice;
        }
        else if (problemChoice == 2){
            unsigned int k;
            string sigma, w;

            // Get input from user
            cout << "Enter k: ";
            cin >> k;
            cout << "Enter digits permitted: ";
            cin.ignore();
            getline(cin, sigma);

            // Clean up the input (by removing non-digit characters)
            sigma.resize(remove_if(sigma.begin(), sigma.end(), [](char c) {return c < '0' || '9' < c; }) - sigma.begin());

            // Create the DFA based on the user input
            SmallestMultiple Prob2_M;
            Prob2_M.k = k;
            Prob2_M.set_q0(0);
            Prob2_M.set_Sigma(sigma);

            string BFSPath = findString(k, Prob2_M, '0');

            if (BFSPath.empty()) {
                cout << "No Solution" << endl;
            }
            else{
                cout << BFSPath << endl;
            }
            cout << endl << "Enter 1 for Problem 1" << endl;
            cout << "Enter 2 for Problem 2" << endl;
            cout << "Enter 3 to quit" << endl;
            cout << "Enter Choice: ";
            cin >> problemChoice;
        }
    }


    return 0;
}
