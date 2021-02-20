#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>

#ifdef _WIN32
  #include <windows.h>
#endif

#ifdef __unix__
  #include <pwd.h>
  #include <filesystem>
#endif

using namespace std;

vector<string> words;

// For some reason, when using a random device as a seed for a generator on windows, it always gives the same value.
// So, I just give the time as a seed.
mt19937 generator(time(0));
uniform_int_distribution<mt19937::result_type> distribution;

// Grabs random words from the vector "words", and concatenates them together; delimited with a space;
string generateString(int wordCount){
    string generated;
    for (int i = 0; i < wordCount; ++i){
        generated += words[distribution(generator)];
        if (i != wordCount - 1){
            generated += " ";
        }
    }
    return generated;
}

int main(){
    // Clears screen when first ran.
    cout << "\x1B[2J"; 
    cout << "\x1B[0;0H";

    cout << "Enter the amount of words you want to type: (type \"quotes\" for quotes)\n";
    string s = "bruh";
    
    // Makes sure user input is valid.
    while (s != "quotes" && (!(s.find_first_not_of("0123456789") == string::npos) || s == "0")){
        #ifdef _WIN32
            cout << "Please enter a positive non-zero integer.\n";
        #else
            cout << "\033[0;32mPlease enter a positive non-zero integer.\033[0m\n";
        #endif
        getline(cin, s, '\n');
    }

    ifstream wordsFile;
    
    #ifdef _WIN32
        char *path = getenv("appdata");
        if (s == "quotes"){
            strcat(path, "\\quotes.txt"); 
        } else {
            strcat(getenv("appdata"), "\\words.txt"); 
        }   
    #else
        struct passwd *pw = getpwuid(getuid());
        char *path = pw->pw_dir;
        if (s == "quotes"){
            strcat(path, "/.local/share/quotes.txt");
        } else {
            strcat(path, "/.local/share/words.txt");
        } 
    #endif

    // Reads file into the vector "words".
    wordsFile = ifstream(path);

    if (!wordsFile){
        if (s == "quotes"){
            wordsFile = ifstream("quotes.txt");
        } else {
            wordsFile = ifstream("words.txt");
        }
        if (!wordsFile){
            #ifdef _WIN32
                cerr << "Error: quotes and/or words file is missing. Put \"words.txt\" and \"quotes.txt\" in \%appdata\%\\Roaming\\\n";
            #else
                cerr << "Error: quotes and/or words file is missing. Put \"words.txt\" and \"quotes.txt\" in ~/.local/share/\n";
            #endif
            return -1;
        } else {
            #ifdef _WIN32
                // Moving the files in windows doesn't appear to work, but at the very least, 
                // there is no reason to move them in windows
                if (s == "quotes"){
                    MoveFile("quotes.txt", path);
                } else {
                    MoveFile("words.txt", path);
                }
            #else
                if (s == "quotes"){
                    filesystem::rename("quotes.txt", path);
                } else {
                    filesystem::rename("words.txt", path);
                }
            #endif
        }
    }
    if (wordsFile.is_open()){
        string line;
        while (getline(wordsFile, line)){
            words.push_back(line);
        }
    }
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
        
        cout << "\x1B[2J";
        cout << "\x1B[0;0H";

        // Displays phrase in green. 
        // (\033 is the escape character, 0;32 means green.)
        #ifdef _WIN32
            cout << phrase << "\n";
        #else
            cout << "\033[0;32m" << phrase << "\033[0m\n";
        #endif
        // Splits phrase delimited by space, and finds wordcount. (Will be used to rework new typing system)
        vector<string> splitPhrase;
        istringstream iss(phrase);
        
        string word;
        while (getline(iss, word, ' ')){
            splitPhrase.push_back(word);
        }
        
        int wordCount = splitPhrase.size();
        
        // Starts timer to calculate WPM.
        auto startTime = chrono::system_clock::now();
        
        string userInput;
        getline(cin, userInput, '\n');
        
        float timeTaken = (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTime).count());
        
        cout << "Time taken: " << timeTaken/1000 << "s\n";
        cout << "Raw WPM: " << wordCount/(timeTaken/60000) << "wpm\n";
        
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
            for (int i = 0; i < (int)phrase.size(); ++i){
                if (phrase[i] != userInput[i]){
                    ++errors;
                }
            }
        }
        
        cout << "Mistyped characters: " << errors << "\n";
        errors = 0;
        
        // Waits for one second so that the user can actually read the output.
        #ifdef _WIN32
            Sleep(1000);
        #else
            usleep(1000000);
        #endif
    }
}
