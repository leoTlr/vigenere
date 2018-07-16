# include <iostream>
# include <string>
# include <array>
# include <algorithm> // transform() and remove_if() in prepare_str()
# include <memory> // unique_ptr

using std::string; using std::cout; using std::cerr; 
using std:: endl; using std::unique_ptr;

const string alph = "abcdefghijklmnopqrstuvwxyz";
std::array<string, 26> subst_table;

unique_ptr<string> prepare_str(const string*);

int main(int argc, char *argv[]){

    if (argc != 3){
        cerr << "usage: " << argv[0] << "  <key>  <msg>" << endl;
        exit(EXIT_FAILURE);
    } // TODO: more failsafe argument checking

    const string raw_key = string(argv[1]); // TODO: also use smart pointers
    const string raw_msg = string(argv[2]);

    if (raw_key.length() > raw_msg.length()){
        cout << "[WARNING] usage: " << argv[0] << "  <key>  <msg>" << endl;
    } 

    const unique_ptr<string> msg = prepare_str(&raw_msg);
    const unique_ptr<string> key = prepare_str(&raw_key);

    cout << "key: " << *key << endl;
    cout << "msg: " << *msg << endl;
    
    // build substitution-alphabets (ROT-n, 1<=n<=25)
    char pos;
    subst_table[0] = alph;
    for (auto it = alph.begin()+1; it < alph.end(); it++){
        pos = *it - 'a';
        string subst_alph = alph.substr(pos, alph.length());
        subst_alph += alph.substr(0, pos);
        subst_table[pos] = subst_alph;
    }

    // reserve memory for output string for mor efficient concatenation
    string cipher_message = "";
    cipher_message.reserve(msg->size());

    // translate char for char in in msg to cipher_msg
    char row, col, key_pos, key_char;
    for (char msg_char: *msg){
        key_char = key->at(key_pos);
        row = key_char - 'a'; // ASCII(lower) char -'a' -> 0-25
        col = msg_char - 'a';
        cipher_message += subst_table[row][col];
        key_pos = (++key_pos)%key->length(); // iterate over key for as long as needed
    }

    cout << cipher_message << endl;

    return 0;
}

unique_ptr<string> prepare_str(const string* str){
    // convert string to only ascii-lowercase letters

    unique_ptr<string> ret_str = std::make_unique<string>(*str);

    if (ret_str->empty()){
        cerr << "empty argument. exiting" << endl;
        exit(EXIT_FAILURE);
    }

    // make lowercase
    std::transform(ret_str->begin(), ret_str->end(), ret_str->begin(), tolower);

    // remove non-(lowercase)letters (ASCII)
    auto is_not_ascii_lower = [](char c){ return !((c>=97)&&(c<=122)); };
    ret_str->erase(std::remove_if(ret_str->begin(), ret_str->end(), is_not_ascii_lower), ret_str->end());

    if (!ret_str) {
        cerr << "error during key/message preparation" << endl;
        exit(EXIT_FAILURE);
    }
    if (ret_str->empty()){
        cerr << "empty string after removing non-letters. exiting" << endl;
        exit(EXIT_FAILURE);
    }

    return ret_str;
}