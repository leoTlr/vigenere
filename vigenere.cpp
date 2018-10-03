# include <iostream>
# include <fstream> // input file
# include <string>
# include <array> // substitution table
# include <algorithm> // transform() and remove_if() in prepare_key()
# include <memory> // shared_ptr

using std::cout; using std::cerr; using std::endl;
using std::shared_ptr; using std::string;

const string alph = "abcdefghijklmnopqrstuvwxyz";
std::array<string, 26> subst_table;

// functions for input validation
auto is_not_ascii_lower = [](char c){ return !((c>=97)&&(c<=122));}; //used in prepare_key()
auto is_ascii_lower = [](char c){ return (c>=97)&&(c<=122);}; // used in encryption process
auto is_ascii_upper = [](char c){ return (c>=65)&&(c<=90);}; // ''
const shared_ptr<string> prepare_key(const shared_ptr<string>);

int main(int argc, char *argv[]){

    std::istream* msg_stream;
    string filename;
    std::ifstream file;
    size_t filesize=0;

    // decide if using stdin or file as input based on # arguments
    // msg_stream is either set to stdin or file
    switch (argc){
        case 2:
            msg_stream = &std::cin; // set msg_stream to stdin
            break;
        case 3:
            filename = argv[2];
            file.open(argv[2]); // TODO maybe put a semaphore on it
            if (!file) {
                cerr << "[ERROR] could not open file: " << argv[2] << endl;
                exit(EXIT_FAILURE);
            }
            if (file.is_open()){
                // find out filesize 
                file.seekg(0, file.end);
                filesize = file.tellg();
                file.seekg(0, file.beg);

                msg_stream = static_cast<std::istream *>(&file); // set nsg_stream to file
            } else {
                cerr << "[ERROR] could not open file: " << argv[2] << endl;
                exit(EXIT_FAILURE);
            }
            break;
        default:
            cerr << "usage: " << argv[0] << "  <key>  [file]" 
                 << "if no file provided as argument, msg is expected from stdin" << endl;
            exit(EXIT_FAILURE);
    } // TODO: more failsafe argument checking

    // prepare key for usage (convert to only ascii-lowercase)
    const shared_ptr<string> raw_key = std::make_shared<string>(argv[1]);
    const shared_ptr<string> key = prepare_key(raw_key);
    
    // build substitution-alphabets (ROT-n, 1<=n<=25)
    char pos;
    subst_table[0] = alph;
    for (auto it = alph.begin()+1; it < alph.end(); it++){
        pos = *it - 'a';
        string subst_alph = alph.substr(pos, alph.length());
        subst_alph += alph.substr(0, pos);
        subst_table[pos] = subst_alph;
    }

    // reserve memory for output string based on inmput msg length to prevent reallocation
    // only useful with file input, not stream from stdin
    string cipher_message;
    cipher_message.reserve(filesize);

    // translate char for char in in msg to cipher_msg
    // ignore non-alphabetical chars and convert upper- to loercase
    char row, col, key_pos=0, key_char, msg_char;
    while (msg_stream->get(msg_char)){

        // convert to lowercase first if needed
        if (is_ascii_upper(msg_char)){
            msg_char -= 32;
        }

        // actual char encryption
        if (is_ascii_lower(msg_char)){
            key_char = key->at(key_pos);
            row = key_char - 'a'; // ASCII(lower) char -'a' -> 0-25
            col = msg_char - 'a';
            cipher_message += subst_table[row][col];
            key_pos = (++key_pos)%key->length(); // iterate over key for as long as needed
        }

        // continue with next char if not ascii lowercase
    }

    cout << cipher_message << endl;

    return EXIT_SUCCESS;
}

const shared_ptr<string> prepare_key(const shared_ptr<string> str){
    // convert key to only ascii-lowercase letters

    if (str->empty()){
        cerr << "[ERROR] key empty. exiting" << endl;
        exit(EXIT_FAILURE);
    }

    // make lowercase
    std::transform(str->begin(), str->end(), str->begin(), tolower);

    // remove non-(lowercase)letters (ASCII)
    str->erase(std::remove_if(str->begin(), str->end(), is_not_ascii_lower), str->end());

    if (!str) {
        cerr << "[ERROR] key preparation" << endl;
        exit(EXIT_FAILURE);
    }
    if (str->empty()){
        cerr << "[ERROR] empty key after removing non-letters. exiting" << endl;
        exit(EXIT_FAILURE);
    }

    return str;
}