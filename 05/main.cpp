#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <vector>

using std::string, std::regex, std::smatch, std::array, std::vector,
    std::regex_search, std::format, std::regex_replace;

size_t replace_regex(string &s, const regex &reg) {
    string s2 = regex_replace(s, reg, "");
    size_t diff = s.length() - s2.length();
    s = s2;
    return diff;
}

string cdecl_translate(string s) {
    string s_copy = s;
    regex whitespace("^\\s*");
    array types = {"int", "char", "float", "double"};
    size_t pos = 0;
    pos += replace_regex(s, whitespace);

    smatch matches;
    regex type_rg("^\\w+");
    bool found = regex_search(s, matches, type_rg);
    if (!found) {
        return format("Syntax error in '{}' at position {}", s_copy, pos);
    }
    string type = *matches.begin();
    if (std::find(types.begin(), types.end(), type) == types.end())
        return "Invalid type: '" + type + "'";
    pos += replace_regex(s, type_rg);
    pos += replace_regex(s, whitespace);

    regex brackets_regx("^\\(.*\\)\\s*(;|\\()");
    bool brackets_open = regex_search(s, matches, brackets_regx);
    if (brackets_open) {
        pos++;
        s = s.substr(1);
    }

    regex pointer_rg("^\\**");
    found = regex_search(s, matches, pointer_rg);
    size_t pointer_cnt = matches.begin()->length();
    if (found)
        pos += replace_regex(s, pointer_rg);
    pos += replace_regex(s, whitespace);

    regex name_rg("^[a-zA-Z_]\\w*");
    found = regex_search(s, matches, name_rg);
    if (!found) {
        return format("Syntax error in '{}' at position {}", s_copy, pos);
    }
    string name = *matches.begin();
    pos += replace_regex(s, name_rg);
    pos += replace_regex(s, whitespace);

    vector<size_t> array_sizes;
    while (true) {
        regex array_rg("^\\[[0-9]+\\]");
        found = regex_search(s, matches, array_rg);
        if (!found)
            break;

        size_t array_size = 0;
        string number = *matches.begin();
        number = number.substr(1, number.length() - 2);
        array_size = std::stoull(number);
        if (array_size == 0)
            return "Error: array size cannot be equal to zero";
        array_sizes.push_back(array_size);
        pos += replace_regex(s, array_rg);
        pos += replace_regex(s, whitespace);
    }

    if (brackets_open) {
        pos++;
        s = s.substr(1);
    }
    pos += replace_regex(s, whitespace);

    regex func_ptr_rg("^\\(.*\\)");
    bool is_function = regex_search(s, matches, func_ptr_rg);
    if (is_function) {
        type = "function returning " + type;
        pos += replace_regex(s, func_ptr_rg);
        pos += replace_regex(s, whitespace);
    }

    if (s.empty() || s.front() != ';') {
        return format("Syntax error in '{}' at position {}", s_copy, pos);
    }
    pos++;
    s = s.substr(1);
    if (!s.empty())
        return format("Syntax error in '{}' at position {}", s_copy, pos);

    string res = format("declare {} as ", name);
    for (auto size : array_sizes)
        res += format("array of {} elements of ", size);
    for (size_t i = 0; i < pointer_cnt; i++)
        res += "pointer to ";
    res += type;
    return res;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "ERROR: input file not provided" << std::endl;
        return 1;
    }
    std::ifstream inp(argv[1]);
    if (inp.fail()) {
        std::cerr << "ERROR: failed to open file [" << argv[1] << "]"
                  << std::endl;
        return 1;
    }
    std::string line;
    while (getline(inp, line)) {
        if (line.empty())
            continue;
        std::cout << cdecl_translate(line) << std::endl;
    }
    inp.close();

    return 0;
}
