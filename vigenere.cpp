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
unique_ptr<string> expand_key(const unique_ptr<string>, const int);

int main(int argc, char *argv[]){

    if (argc != 3){
        cerr << "usage: " << argv[0] << "  <key>  <msg>" << endl;
        exit(EXIT_FAILURE);
    } // TODO: more failsafe argument checking

    const string raw_key = string(argv[1]);
    const string raw_msg = string(argv[2]);

    if (raw_key.length() > raw_msg.length()){
        cout << "[WARNING] usage: " << argv[0] << "  <key>  <msg>" << endl;
    } 

    const unique_ptr<string> msg = prepare_str(&raw_msg);
    const unique_ptr<string> expanded_key = expand_key(prepare_str(&raw_key), msg->length());

    cout << "key: " << *expanded_key << endl;
    cout << "msg: " << *msg << endl;
    
    // build substitution-alphabets (ROT-n, 1<=n<=25)
    subst_table[0] = alph;
    for (auto it = alph.begin()+1; it < alph.end(); it++){
        char c = *it;
        int pos = alph.find(c);
        string subst_alph = alph.substr(pos, alph.length());
        subst_alph += alph.substr(0, pos);
        subst_table[pos] = subst_alph;
    }

    // reserve memory for output string for mor efficient concatenation
    string cipher_message = "";
    cipher_message.reserve(msg->size());

    // build cipher message
    char row, col, i, key_char;
    for (char msg_char: *msg){
        key_char = expanded_key->at(i);
        row = alph.find(key_char);
        col = alph.find(msg_char);
        cipher_message += subst_table[row][col];
        i++;
    }

    cout << cipher_message << endl;

    return 0;
}

unique_ptr<string> expand_key(const unique_ptr<string> key, const int msg_len){
    // need key with length == msg-length
    // repeat key as often as needed

    int i;
    unique_ptr<string> expanded_key = std::make_unique<string>("");
    expanded_key->reserve(msg_len);
    for (i=0; i<msg_len; i++){
        *expanded_key += (*key)[i%key->length()];
    }
    return expanded_key;
}

unique_ptr<string> prepare_str(const string* str){
    // convert string to only ascii-lowercase letters

    unique_ptr<string> str_p = std::make_unique<string>(*str);

    if (str_p->empty()){
        cerr << "empty argument. exiting" << endl;
        exit(EXIT_FAILURE);
    }

    // make lowercase
    std::transform(str_p->begin(), str_p->end(), str_p->begin(), tolower);

    // remove non-(lowercase)letters (ASCII)
    auto is_not_ascii_lower = [](char c){ return !((c>=97)&&(c<=122)); };
    str_p->erase(std::remove_if(str_p->begin(), str_p->end(), is_not_ascii_lower), str_p->end());

    if (!str_p) {
        cerr << "error during key/message preparation" << endl;
        exit(EXIT_FAILURE);
    }
    if (str_p->empty()){
        cerr << "empty string after removing non-letters. exiting" << endl;
        exit(EXIT_FAILURE);
    }

    return str_p;
}