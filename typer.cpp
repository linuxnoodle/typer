#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;
vector<string> words;

mt19937 generator(time(nullptr));
uniform_int_distribution<> distribution;

string generateString(int wordCount){
    string poggerString;

    for (int i = 0; i < wordCount; ++i){
        poggerString += words[distribution(generator)];
        if (i != wordCount - 1){
            poggerString += " ";
        }
    }
    return poggerString;
}

void findAllOccurances(std::vector<size_t> & vec, std::string data, std::string toSearch){
    size_t pos = data.find(toSearch);

    while (pos != string::npos){
        vec.push_back(pos);
        pos = data.find(toSearch, pos + toSearch.size());
    }
}

int main(){
	
    #ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
	
    cout << "Type in the amount of words you want to type: (type \"quotes\" for quotes)" << endl;
    string s;
    getline(cin, s);

    while (s != "quotes" && (!(s.find_first_not_of("0123456789") == string::npos) || s == "0")){
        cout << "Please enter a positive non-zero integer." << endl;
        getline(cin, s);
    }

    ifstream wordsFile;

    if (s == "quotes"){
        wordsFile = ifstream("quotes.txt");
    } else {
        wordsFile = ifstream("words.txt");
    }

    if (wordsFile.is_open()){
        string line;
        while (getline(wordsFile, line)){
            words.push_back(line);
        }
    }
    wordsFile.close();

    distribution = uniform_int_distribution<>(0, words.size() - 1);

    while (true){
        string phrase;
        if (s == "quotes"){
            phrase = words[distribution(generator)];
        } else {
            phrase = generateString(stoi(s));
        }
		
        #ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif
		
        cout << "\033[0;32m" << phrase << "\033[0m" << endl;

        vector<size_t> occurrences;
        findAllOccurances(occurrences, phrase, " ");
        int wordCount = occurrences.size() + 1;
        
        auto startTime = chrono::system_clock::now();
        
        string userInput;
        getline(cin, userInput);

        float timeTaken = (float)(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTime).count())/1000;

        cout << "Time taken: " << timeTaken << "s" << endl;
        cout << "Raw WPM: " << wordCount/(timeTaken/60) << endl;

        int errors = 0;
        if (userInput.size() != phrase.size()){
            errors += abs((int)userInput.size() - (int)phrase.size());
            for (int i = 0; i <= abs((int)userInput.size() - (int)phrase.size()); ++i){
                if (phrase[i] != userInput[i]){
                    ++errors;
                }
            }
        } else {
            for (int i = 0; i < phrase.size(); ++i){
                if (phrase[i] != userInput[i]){
                    ++errors;
                }
            }
        }
        cout << "Mistyped characters: " << errors << endl;
        errors = 0;
		this_thread::sleep_for(chrono::seconds(1));
    }
}