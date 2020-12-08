#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;

vector<string> words;

// Define the random number generators globally, or Windows uses the same quotes in the same orders every time.
// Also seeded on time instead of 
mt19937 generator(time(nullptr));
uniform_int_distribution<mt19937::result_type> distribution;

// Grabs random words from the vector "words", and concatenates them together; delimited with a space;
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

// Generic "findAllOccurances" function. Was made to not split the individiual phrase apart, and will most likely be removed in the next rework.
void findAllOccurances(std::vector<size_t> & vec, std::string data, std::string toSearch){
    size_t pos = data.find(toSearch);

    while (pos != string::npos){
        vec.push_back(pos);
        pos = data.find(toSearch, pos + toSearch.size());
    }
}

int main(){
	
    // Clears screen when first ran.
    #ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
	
    cout << "\033[0;32m" << "Type in the amount of words you want to type: (type \"quotes\" for quotes)" << "\033[0m" << endl;
    string s;
    getline(cin, s);

    // Makes sure user input is valid.
    // (Yes, getline() needs to be used. For some reason cin just doesnt pause for input, and it has to be used.)
    while (s != "quotes" && (!(s.find_first_not_of("0123456789") == string::npos) || s == "0")){
        cout << "\033[0;32m" << "Please enter a positive non-zero integer." << "\033[0m" << endl;
        getline(cin, s);
    }

    ifstream wordsFile;

    // Reads file into the vector "words". Will need to be changed to be appdata compliant.
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

    // Initializes random distribution, as before this the bounds are not decided.
    distribution = uniform_int_distribution<mt19937::result_type>(0, words.size() - 1);

    while (true){
        // Grabs the next phrase, and loads it.
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

        // Displays phrase in green. 
        // (\033 is the escape character, 0;32 means green.)
        cout << "\033[0;32m" << phrase << "\033[0m" << endl;

        // Calculates wordcount based off of the amount of spaces + 1. Was used due to only needing a single scan,
        // but will be changed when individual words need to be grabbed.
        vector<size_t> occurrences;
        findAllOccurances(occurrences, phrase, " ");
        int wordCount = occurrences.size() + 1;
        
        // Starts timer to calculate WPM. Defined as auto as the type is honestly just way too long.
        auto startTime = chrono::system_clock::now();
        
        string userInput;
        getline(cin, userInput);

        float timeTaken = (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTime).count())/1000;

        cout << "\033[0;32m" << "Time taken: " << timeTaken << "s" << "\033[0m" << endl;
        cout << "\033[0;32m" << "Raw WPM: " << wordCount/(timeTaken/60) << "\033[0m" << endl;

        // Calculates the amount of mistyped characters.
        
        // First, it checks if the size of the phrase typed and the actual phrase are the same, as if they aren't,
        // checking each character would segfault due to an out of bounds exception.

        // If it is, it adds the difference to the error count, and only check for differences up to the shorter length.
        // If it isn't, it just checks each character with the same indicies. 
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
        
        // Waits for a second so the user can actually read the output.
        // (Note that this is POSIX threading, so you have to use a version of MinGW with POSIX threading to compile it on windows.)
		this_thread::sleep_for(chrono::seconds(1));
    }
}
