//
//  helper.cpp
//  px
//
//  Created by Jonathan Cichon on 03.01.16.
//
//

#include "helper.hpp"
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <cstdarg>
#include <cassert>
#include <random>

#include <dirent.h>

namespace px {
    namespace String {
        const string& Empty() {
            static string e;
            static std::once_flag f;
            std::call_once(f, []{
                e = string("");
            });
            return e;
        }
    }
}

/*************************
 
 String Helper
 
 *************************/

std::vector<string> &px::split(const string &s, char delim, std::vector<string> &elems) {
    std::stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}

std::vector<string> px::split(const string &s, char delim) {
    std::vector<string> elems;
    split(s, delim, elems);
    return elems;
}

std::vector<string> &px::split(const string &s, const string &delim, std::vector<string> &elems) {
    string item;
    size_t found;
    size_t current_pos = 0;
    while ((found = s.find(delim, current_pos)) != string::npos) {
        auto tmp = s.substr(current_pos, found-current_pos);
        if (!tmp.empty()) {
            elems.push_back(tmp);
        }
        current_pos = found + delim.length();
    }
    if (current_pos < s.length()) {
        elems.push_back(s.substr(current_pos, s.length()-current_pos));
    }
    return elems;
}

std::vector<string> px::split(const string &s, const string &delim) {
    std::vector<string> elems;
    split(s, delim, elems);
    return elems;
}

string px::strip(const string& s, const string& c) {
    string str(s);
    if (c.length() > 0) {
        bool check = true;
        while (check && str.length() > 0) {
            bool found = false;
            for (int i = 0; i<c.length(); ++i) {
                if (str.front() == c[i]) {
                    str.erase(0, 1);
                    found = true;
                    break;
                }
            }
            check = found;
        }
        check = true;
        while (check && str.length() > 0) {
            bool found = false;
            for (int i = 0; i<c.length(); ++i) {
                if (str.back() == c[i]) {
                    str.pop_back();
                    found = true;
                    break;
                }
            }
            check = found;
        }
    }
    return str;
}

string px::camelcase(const string& s, bool uppercase_first) {
    string str;
    auto comps = split(s, '_');
    int i=0;
    for (auto& comp : comps) {
        if ((i==0 && uppercase_first) || i>0) {
            str += upcase(comp, 1);
        } else {
            str += comp;
        }
        ++i;
    }
    return str;
}

string px::upcase(const string& s) {
    return upcase(s, (unsigned int)s.size());
}

string px::upcase(const string& s, unsigned int length) {
    string ret = string(s);
    for(unsigned int i = 0; i < s.size() && i < length; ++i){
        ret[i] = toupper(ret[i]);
    }
    return ret;
}

string px::downcase(const string& s) {
    return downcase(s, (unsigned int)s.size());
}

string px::downcase(const string& s, unsigned int length) {
    string ret = string(s);
    for(unsigned int i = 0; i < s.size() && i < length; ++i){
        ret[i] = tolower(ret[i]);
    }
    return ret;
}

/*************************
 
 File Helper
 
 *************************/

string px::first_component(const string& path) {
    return path.substr(0, path.find_first_of("/\\", 1));
}

string px::base_name(const string& path) {
    return path.substr(path.find_last_of("/\\") + 1);
}

string px::file_stem(const string& path) {
    return path.substr(path.find_last_of("/\\") + 1, path.find_last_of("."));
}

string px::file_extension(const string& path) {
    auto index = path.find_last_of(".");
    if (index != string::npos && index < path.length()) {
        return path.substr(index+1);
    }
    return px::String::Empty();
}

bool px::file_exists(const char *path, bool* is_directory) {
    *is_directory = false;
    struct stat buffer;
    if (stat (path, &buffer) == 0) {
        *is_directory = S_ISDIR(buffer.st_mode);
        return true;
    }
    return false;
}

vector<string> px::dir_content(const string& path, FileType type) {
    return dir_content(path.c_str(), type);
}

vector<string> px::dir_content(const char* path, FileType type) {
    vector<string> content;
    DIR           *d;
    struct dirent *dir;
    d = opendir(path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if ((dir->d_type == DT_DIR && type & FileTypeDir) || (dir->d_type == DT_REG && type & FileTypeFile)) {
                if (dir->d_name[0] != '.') {
                    content.push_back(dir->d_name);
                }
            }
        }
        closedir(d);
    } else {
        printf("%s is not a directory\n", path);
    }
    std::sort(content.begin(), content.end());
    return content;
}

time_t px::file_modification_time(const string& path) {
    return file_modification_time(path.c_str());
}

time_t px::file_modification_time(const char* path) {
    struct stat attrib;
    stat(path, &attrib);
    char date[10];
    return attrib.st_mtime;
}



/*************************
 
 XML Escape Sequences
 
 *************************/

#define ADD_ESCAPE_SEQUENCE(__BUFFER__, __SEQ__) __BUFFER__[b++] = '&';for (int j=0; j<sizeof(__SEQ__)/sizeof(__SEQ__[0]); j++) {__BUFFER__[b++] = __SEQ__[j];}; __BUFFER__[b++] = ';';
#define COMP_ESCAPE_SEQUENCE(__BUFFER__, __SEQ__) compareEscapeSequence(__BUFFER__, __SEQ__, sizeof(__SEQ__)/sizeof(__SEQ__[0]))
#define MEM_PAGE_SIZE 1024
#define MAX_SEQ_LENGTH 5
#define UNICODE_MAX 0x10FFFFul

char __newLine[3] = {'#', 'x', 'A'};
char __quot[4] = {'q', 'u', 'o', 't'};
char __amp[3] = {'a', 'm', 'p'};
char __apos[4] = {'a', 'p', 'o', 's'};
char __lt[2] = {'l', 't'};
char __gt[2] = {'g', 't'};
char __Auml[4] = {'A', 'u', 'm', 'l'};
char __Ouml[4] = {'O', 'u', 'm', 'l'};
char __Uuml[4] = {'U', 'u', 'm', 'l'};
char __sz[5] = {'s', 'z', 'l', 'i', 'g'};
char __auml[4] = {'a', 'u', 'm', 'l'};
char __ouml[4] = {'o', 'u', 'm', 'l'};
char __uuml[4] = {'u', 'u', 'm', 'l'};

static inline bool compareEscapeSequence(char *buffer, char *seq, int seqLen) {
    bool isSeq = true; for (int x=0; x<seqLen; x++) {if(seq[x] != buffer[x]) {isSeq = false; break;}}
    return isSeq;
}

string px::xmlescape(const string& s) {
    string out;
    xmlescape(s, out);
    return out;
}

string px::xmlunescape(const string& s) {
    string out;
    xmlunescape(s, out);
    return out;
}

void px::xmlescape(const string &s, string& out) {
    auto length = s.length();
    
    char buffer[MEM_PAGE_SIZE*(MAX_SEQ_LENGTH+2)];
    for (int page = 0; page<length; page += MEM_PAGE_SIZE) {
        int b = 0;
        int i;
        
        for (i=page; i<length && i<MEM_PAGE_SIZE+page; i++) {
            u_char c = s[i];
            switch (c) {
                case 10: // \n
                    ADD_ESCAPE_SEQUENCE(buffer, __newLine);
                    break;
                case 34: // "
                    ADD_ESCAPE_SEQUENCE(buffer, __quot);
                    break;
                case 38: // &
                    ADD_ESCAPE_SEQUENCE(buffer, __amp);
                    break;
                case 39: // '
                    ADD_ESCAPE_SEQUENCE(buffer, __apos);
                    break;
                case 60: // <
                    ADD_ESCAPE_SEQUENCE(buffer, __lt);
                    break;
                case 62: // >
                    ADD_ESCAPE_SEQUENCE(buffer, __gt);
                    break;
//                case 196: // Ä
//                    ADD_ESCAPE_SEQUENCE(buffer, __Auml);
//                    break;
//                case 214: // Ö
//                    ADD_ESCAPE_SEQUENCE(buffer, __Ouml);
//                    break;
//                case 220: // Ü
//                    ADD_ESCAPE_SEQUENCE(buffer, __Uuml);
//                    break;
//                case 223: // ß
//                    ADD_ESCAPE_SEQUENCE(buffer, __sz);
//                    break;
//                case 228: // ä
//                    ADD_ESCAPE_SEQUENCE(buffer, __auml);
//                    break;
//                case 246: // ö
//                    ADD_ESCAPE_SEQUENCE(buffer, __ouml);
//                    break;
//                case 252: // ü
//                    ADD_ESCAPE_SEQUENCE(buffer, __uuml);
//                    break;
                default:
                    buffer[b++] = c;
                    break;
            }
        }
        out.append(buffer, b);
    }
}

void px::xmlunescape(const string &s, string& out) {
    auto length = s.length();
    char *retBuffer = (char*)malloc(length*sizeof(char));
    char seqBuffer[MAX_SEQ_LENGTH];
    memset(seqBuffer, 0, MAX_SEQ_LENGTH);
    
    int b=0;
    for (int i = 0; i<length; i++) {
        char c = s[i];
        if (c == '&') {
            i++;
            int j = 0;
            while (i<length && j<MAX_SEQ_LENGTH && (seqBuffer[j] = s[i]) != ';') {j++;i++;}
            
            if (COMP_ESCAPE_SEQUENCE(seqBuffer, __newLine)) {
                retBuffer[b++] = '\n';
            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __quot)) {
                retBuffer[b++] = '"';
            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __amp)) {
                retBuffer[b++] = '&';
            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __apos)) {
                retBuffer[b++] = '\'';
            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __lt)) {
                retBuffer[b++] = '<';
            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __gt)) {
                retBuffer[b++] = '>';
//            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __Auml)) {
//                retBuffer[b++] = (unsigned char)196;
//            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __Ouml)) {
//                retBuffer[b++] = (unsigned char)214;
//            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __Uuml)) {
//                retBuffer[b++] = (unsigned char)220;
//            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __sz)) {
//                retBuffer[b++] = (unsigned char)223;
//            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __auml)) {
//                retBuffer[b++] = (unsigned char)228;
//            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __ouml)) {
//                retBuffer[b++] = (unsigned char)246;
//            }else if(COMP_ESCAPE_SEQUENCE(seqBuffer, __uuml)) {
//                retBuffer[b++] = (unsigned char)252;
            } else if(seqBuffer[0] == '#') {
                int errno_save = errno;
                bool hex = seqBuffer[1] == 'x' || seqBuffer[1] == 'X';
                
                int l = j-1;
                auto substring = s.substr(i-l, l);
                size_t x;
                unsigned long cp = std::stoul(substring, &x, hex ? 16 : 10);
                
                errno = 0;
                bool fail = errno ||  cp > UNICODE_MAX;
                errno = errno_save;
                if (fail) {free(retBuffer);return;};
                
                if (cp <= 0x007Ful) {
                    retBuffer[b++] = (unsigned char)cp;
                } else if(cp <= 0x07FFul) {
                    retBuffer[b++] = (unsigned char)((6 << 5) | (cp >> 6));
                    retBuffer[b++] = (unsigned char)((2 << 6) | (cp & 0x3F));
                } else if(cp <= 0xFFFFul) {
                    retBuffer[b++] = (unsigned char)((14 << 4) |  (cp >> 12));
                    retBuffer[b++] = (unsigned char)(( 2 << 6) | ((cp >> 6) & 0x3F));
                    retBuffer[b++] = (unsigned char)(( 2 << 6) | ( cp       & 0x3F));
                } else if(cp <= 0x10FFFFul) {
                    retBuffer[b++] = (unsigned char)((30 << 3) |  (cp >> 18));
                    retBuffer[b++] = (unsigned char)(( 2 << 6) | ( cp        & 0x3F));
                    retBuffer[b++] = (unsigned char)(( 2 << 6) | ((cp >>  6) & 0x3F));
                    retBuffer[b++] = (unsigned char)(( 2 << 6) | ((cp >> 12) & 0x3F));
                }
            }
        }else {
            retBuffer[b++] = c;
        }
    }
    out.append(retBuffer, b);
    free(retBuffer);
}

/*************************
 
 String <> Number
 
 *************************/

unsigned long long px::random(unsigned long long max) {
    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned long long> distribution(0, max);
    return distribution(generator);
}


const string digits = "0123456789abcdefghijklmnopqrstuvwxyz";
template<class T> string to_string(T src, int radix, int length, char padding) {
    assert(radix >= 2 && radix <= 36);
    string str;
    for ( ; src > 0; src /= radix) {
        str += digits[src % radix];
    }
    
    if (length > str.length() && padding) {
        while (str.length() < length) {
            str += padding;
        }
    }
    
    std::reverse(str.begin(), str.end());
    return str;
}

string px::to_string(unsigned long src, int radix, int length, char padding) {
    return ::to_string(src, radix, length, padding);
}

string px::to_string(unsigned long long src, int radix, int length, char padding) {
    return ::to_string(src, radix, length, padding);
}

long px::hex_to_long(const string& s) {
    long l;
    std::stringstream ss;
    ss << std::hex << s;
    ss >> l;
    return l;
}

time_t px::timestamp() {
    return std::time(nullptr);
}

long px::timestampms() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
}

double px::system_timestamp() {
    struct timeval tp;
    static long secbase;
    gettimeofday(&tp, NULL);
    
    if (!secbase) {
        secbase = tp.tv_sec;
        return tp.tv_usec/1000000.0;
    }
    return (tp.tv_sec - secbase) + tp.tv_usec/1000000.0;
}

string px::create_uniq_id(long user_seed) {
    static std::atomic<u_int32_t> counter_random_seed;
    static std::once_flag flag;
    std::call_once(flag, [](){
        counter_random_seed.store(px::random(UINT_MAX));
    });
    
    long milliseconds = px::timestampms();
    return create_id(milliseconds, user_seed, counter_random_seed++);
}

string px::create_id(unsigned long milliseconds, unsigned long user_seed, unsigned long random_counter) {
    char buffer[200+1];
    snprintf(buffer, 200, "%s%s%s",
             px::to_string(milliseconds, 36, 10, '0').c_str(),                    // timestamp
             px::to_string(user_seed, 36, 8, '0').c_str(),                        // user
             px::to_string(random_counter++ % 0x19a0ff, 36, 4, '0').c_str());     // local counter and random seed with 'zzzz' as upper bounds
    
    return string(buffer);
}

string px::vstringfmt(const char *fmt, va_list argList) {
    std::ostringstream stm;
    va_list dupArgs;
    
#ifdef __GNUC__
    va_copy(dupArgs, argList); // Clone arguments for reuse by different call to vsprintf.
#else
    dupArgs = argList;        // Simply ptr copy for GCC compatibility
#endif
    int bufferSize = vsnprintf(NULL, 0, fmt, dupArgs);
#ifdef __GNUC__
    va_end(dupArgs); // cleanup
#endif
    
    if (bufferSize) {
        char *buffer;
        
        bufferSize++;
        buffer = (char *)malloc(sizeof(char)*bufferSize);
        
#ifdef __GNUC__
        va_copy(dupArgs, argList); // Clone arguments for reused by different call to vsprintf.
#endif
        vsnprintf(buffer, bufferSize, fmt, dupArgs);
#ifdef __GNUC__
        va_end(dupArgs); // cleanup
#endif
        stm << buffer;
        
        free(buffer);
    }
    
    return stm.str();
}

string px::stringfmt(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    auto s = vstringfmt(fmt, ap);
    va_end(ap);
    return s;
}

string px::join(const std::vector<string>& elements, const string& delim) {
    return px::join<string>(elements, delim, [](const string& element) {
        return element;
    });
}








