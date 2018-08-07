# include <iostream>
# include <fstream> // input file
# include <string>
# include <array> // substitution table
# include <algorithm> // transform() and remove_if() in prepare_key()
# include <memory> // unique_ptr

using std::cout; using std::cerr; 
using std::endl; using std::unique_ptr;

const std::string alph = "abcdefghijklmnopqrstuvwxyz";
std::array<std::string, 26> subst_table;

unique_ptr<std::string> prepare_key(const std::string*);
auto is_not_ascii_lower = [](char c){ return !((c>=97)&&(c<=122));};
auto is_ascii_lower = [](char c){ return (c>=97)&&(c<=122);};
auto is_ascii_upper = [](char c){ return (c>=65)&&(c<=90);};


int main(int argc, char *argv[]){

    std::istream* msg_stream;
    std::string filename;
    std::ifstream file;

    // decide if using stdin or file as input
    switch (argc){
        case 2:
            msg_stream = &std::cin;
            break;
        case 3:
            filename = argv[2];
            file.open(argv[2]);
            if (!file) {
                cerr << "[ERROR] could not open file: " << argv[2] << endl;
                exit(EXIT_FAILURE);
            }
            if (file.is_open()){
                msg_stream = static_cast<std::istream *>(&file);
            } else {
                cerr << "[ERROR] could not open file: " << argv[2] << endl;
                exit(EXIT_FAILURE);
            }
            break;
        default:
            cerr << "usage: " << argv[0] << "  <key>  [file]" 
                << "if no file provided as argument, msg is expected from stdin" << endl;
            exit(EXIT_FAILURE);
            break;
    } // TODO: more failsafe argument checking

    const std::string raw_key = std::string(argv[1]); // TODO: also use smart pointers
    const unique_ptr<std::string> key = prepare_key(&raw_key);
    
    // build substitution-alphabets (ROT-n, 1<=n<=25)
    char pos;
    subst_table[0] = alph;
    for (auto it = alph.begin()+1; it < alph.end(); it++){
        pos = *it - 'a';
        std::string subst_alph = alph.substr(pos, alph.length());
        subst_alph += alph.substr(0, pos);
        subst_table[pos] = subst_alph;
    }

    std::string cipher_message = "";

    // translate char for char in in msg to cipher_msg
    // ignore non-alphabetical chars and convert upper- to loercase
    char row, col, key_pos=0, key_char, msg_char;
    while (msg_stream->get(msg_char)){
        if (is_ascii_upper(msg_char)){
            msg_char -= 32; // convert to lowercase
        }
        if (is_ascii_lower(msg_char)){
            key_char = key->at(key_pos);
            row = key_char - 'a'; // ASCII(lower) char -'a' -> 0-25
            col = msg_char - 'a';
            cipher_message += subst_table[row][col];
            key_pos = (++key_pos)%key->length(); // iterate over key for as long as needed
        }
    }

    cout << cipher_message << endl;

    return EXIT_SUCCESS;
}

unique_ptr<std::string> prepare_key(const std::string* str){
    // convert key to only ascii-lowercase letters

    unique_ptr<std::string> ret_str = std::make_unique<std::string>(*str);

    if (ret_str->empty()){
        cerr << "[ERROR] key empty. exiting" << endl;
        exit(EXIT_FAILURE);
    }

    // make lowercase
    std::transform(ret_str->begin(), ret_str->end(), ret_str->begin(), tolower);

    // remove non-(lowercase)letters (ASCII)
    ret_str->erase(std::remove_if(ret_str->begin(), ret_str->end(), is_not_ascii_lower), ret_str->end());

    if (!ret_str) {
        cerr << "[ERROR] key preparation" << endl;
        exit(EXIT_FAILURE);
    }
    if (ret_str->empty()){
        cerr << "[ERROR] empty key after removing non-letters. exiting" << endl;
        exit(EXIT_FAILURE);
    }

    return ret_str;
}