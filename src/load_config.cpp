#include <app.h>
#include <stdio.h>

namespace t4editor {
    bool application::load_config() {
        FILE* fp = fopen("config.txt","r");
        if(!fp) {
            printf("Unable to open config.txt\n");
            return false;
        }

        vector<string> lines;
        while(!feof(fp)) {
            char c = fgetc(fp);
            string line;
            while(c != '\n' && c != '\r' && !feof(fp)) {
                line += c;
                c = fgetc(fp);
            }
            if(line.length() != 0) lines.push_back(line);
        }

        for(auto i = lines.begin();i != lines.end();i++) {
            string line = *i;
            size_t first_space = 0;
            while(first_space < line.length()) {
                if(line[first_space] == ' ') break;
                first_space++;
            }

            string variable_name = line.substr(0, first_space);
            string variable_value;
            size_t q0 = line.find_first_of('\'');
            size_t q1 = line.find_last_of('\'');

            if(q0 != q1) {
                variable_value = line.substr(q0 + 1,(q1 - q0) - 1);
            } else {
                size_t last_space = first_space;
                while(last_space < line.length()) {
                    if(line[last_space] != ' ') break;
                    last_space++;
                }
                variable_value = line.substr(last_space + 2, line.length() - last_space);
            }
            //printf("%s: %s\n", variable_name.c_str(), variable_value.c_str());
            handle_config_var(variable_name,variable_value);
        }

        fclose(fp);
        return true;
    }
}
