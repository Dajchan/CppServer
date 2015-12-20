#include "view_template.hpp"
#include <sys/stats>

using namespace px;


#define CLEAN_FILE_BEFORE_RETURN funlockfile(file);fclose(file);

#define GETC_UNLOCKED_SAVE(__var__, __file__) if ((__var__ = getc_unlocked(__file__)) == (unsigned char)EOF){ LogError("TEMPLATE Syntax Error: Unexpected End Of File!");CLEAN_FILE_BEFORE_RETURN; return 0;}

int parse_template_file(const char *file_path) {
    if (!file_path) {
        return 0;
    }
    
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        return 0;
    }
    
    flockfile(file);
    
    if (file == NULL) {
        return 0;
    }
	
	vector<ViewTemplatePart_p> stack;
	ViewTemplatePart_p current;

	string next_content;
	string next_cmd;
	string next_param;
	
	unsigned char character;
	
	while ( (character = getc_unlocked(file)) != (unsigned char)EOF ) {
		if (character == '{'){
			GETC_UNLOCKED_SAVE(character,file);
			if (character == '-') {
                if (checkEndTag(stack, current)) {
                    CLEAN_FILE_BEFORE_RETURN
                    return 1;
                }
                while ((character = getc_unlocked(file)) !='}' && character != (unsigned char)EOF) {} // file/buffer overflow!
			} else {  // TODO: check for valid variable // start an Command, get the cmd-name first
				string cmd = "";
                while (character != '}' && character != '-') {
					cmd += character;
                    GETC_UNLOCKED_SAVE(character, file);
                }
				next_cmd = cmd;
				
				GETC_UNLOCKED_SAVE(character, file);
				
				string param = "";
                while (character != '}' && character != '-') {
					param += character;
                    GETC_UNLOCKED_SAVE(character, file);
                }
				next_param = param;
				
				if (character == '-') {
					GETC_UNLOCKED_SAVE(character,file);
					if (character == '}') {
		                if (checkEndTag(stack, current)) {
		                    CLEAN_FILE_BEFORE_RETURN
		                    return 1;
		                }
					} else {
						LogError("TEMPLATE Syntax Error: End Tag Missing!");CLEAN_FILE_BEFORE_RETURN; return 0;
					}
				}
			}
		} else if (character == '-') {
			endTag(stack, next_content, next_cmd, next_param, current);
            if (checkEndTag(stack, current)) {
                CLEAN_FILE_BEFORE_RETURN
                return 1;
            }
		} else if (character == '}') {
			endTag(stack, next_content, next_cmd, next_param, current);
		} else {
			next_content += character;
		}
	}
}


ViewTemplate_p ViewTemplate::New(const string& filePath) {
	// open file
	string file_content;
	if (true) {
		// parse the file
		// file_content
		
		
		if (true) {	
			return ViewTemplate_p(new ViewTemplate(nullptr));
		}
	}
	return nullptr;
}

ViewTemplate::ViewTemplate(vector<ViewTemplatePart> *parts) : m_parts(parts) {}

ViewTemplate::~ViewTemplate() {
	if (m_parts) {
		delete m_parts;
	}
}

void ViewTemplate::each_part(const function<void(const ViewTemplatePart& part)>& lambda) {
	if (lambda && m_parts) {
		for (auto& t : *m_parts) {
			lambda(t);
		}
	}
}