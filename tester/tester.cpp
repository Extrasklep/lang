#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <memory>
#include <array>

char* system_goutput_data;
char* system_goutput(const char* cmd) {
    delete[] system_goutput_data;
    system_goutput_data = new char[65536];
    std::string output;
    std::array<char, 65536> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        std::cout << "popen() error.\n";
        exit(255);
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        output += buffer.data();
    }
    strcpy(system_goutput_data,output.c_str());
    return system_goutput_data;
}

char* cmd;
const char* join_str_with_space(const char* p1,const char* p2) {
    delete[] cmd;
    cmd = new char[strlen(p1)+strlen(p2)+2];
    unsigned int strpos;
    for(unsigned int i=0;i<strlen(p1);i++) {
        cmd[strpos++] = p1[i];
    }
    cmd[strpos++] = ' ';
    for(unsigned int i=0;i<strlen(p2);i++) {
        cmd[strpos++] = p2[i];
    }
    cmd[strpos++] = '\0';
    return cmd;
}

int main(int argc, char** args) {
    if(argc<2) {
        std::cout << "usage: " << args[0] << " [path to interpreter executable]\n";
        return 0;
    }
    bool passing = true;
    
    std::cout << "Comments... ";
    char* t1o = system_goutput(join_str_with_space(args[1],"test.script"));
    if(strlen(t1o) == 1 && t1o[0] == 'a') {
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED\n";
        passing = false;
    }
    
    std::cout << "Negative numbers... ";
    char* t2o = system_goutput(join_str_with_space(args[1],"negativeint.script"));
    if(strlen(t2o) == 3 && t2o[0] == (char)-1 && t2o[1] == (char)-10 && t2o[2] == (char)206) {
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED\n";
        passing = false;
    }
    
    std::cout << "Pointers... ";
    char* t3o = system_goutput(join_str_with_space(args[1],"pointer.script"));
    if(strlen(t3o) == 3 && t3o[0] == (char)13 && t3o[1] == (char)7 && t3o[2] == (char)4) {
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED\n";
        passing = false;
    }
    
    std::cout << "Loop... ";
    char* t4o = system_goutput(join_str_with_space(args[1],"example.script"));
    if(strlen(t4o) == 5 && t4o[0] == (char)5 && t4o[1] == (char)4 && t4o[2] == (char)3 && t4o[3] == (char)2 && t4o[4] == (char)1) {
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED\n";
        passing = false;
    }
    
    std::cout << "HelloWorld... ";
    const char* expectedT5string = "Hello world!\nHello world!\nHello world!\nHello world!\nHello world!\n";
    char* t5o = system_goutput(join_str_with_space(args[1],"helloworld.script"));
    if(strlen(t5o) == strlen(expectedT5string) && strcmp(t5o,expectedT5string) == 0) {
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED\n";
        passing = false;
    }
    
    std::cout << "Bitmap... ";
    if(system(join_str_with_space(args[1],"genbitmap.script > ./.test.bmp"))) {
        std::cout << "FAILED\n";
        passing = false;
    } else {
        char* t6o = system_goutput("sha256sum -z ./.test.bmp");
        if(strcmp(t6o,"760964e1c05e1f12b2986bea976f8d258161d4d7231ab20acc850b82b4b9d32e  ./.test.bmp") == 0) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED\n";
            passing = false;
        }
    }
    
    std::cout << "Bitmap255... ";
    if(system(join_str_with_space(args[1],"genbitmap255.script > ./.test.bmp"))) {
        std::cout << "FAILED\n";
        passing = false;
    } else {
        char* t7o = system_goutput("sha256sum -z ./.test.bmp");
        if(strcmp(t7o,"ae870073f22bba9a89c09021401493b345c19b0034d7137e9f2c079d96f4aa58  ./.test.bmp") == 0) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED\n";
            passing = false;
        }
    }
    
    std::cout << "Bitmap511... ";
    if(system(join_str_with_space(args[1],"genbitmap511.script > ./.test.bmp"))) {
        std::cout << "FAILED\n";
        passing = false;
    } else {
        char* t7o = system_goutput("sha256sum -z ./.test.bmp");
        if(strcmp(t7o,"563cd15a15d6f307be5d7fc889ca10c88bf713ab71415503ccfa7819f1fc220b  ./.test.bmp") == 0) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED\n";
            passing = false;
        }
    }
    
    std::cout << "Raw input (v1.1)... ";
    const char* jsres1 = join_str_with_space(args[1],"11.script");
    char* jsres1c = new char[strlen(jsres1)+1];
    strcpy(jsres1c,jsres1);
    char* t8o = system_goutput(join_str_with_space("cat /dev/null |",jsres1c));
    if(strlen(t8o) == 1 && t8o[0] == 'a') {
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED\n";
        passing = false;
    }
    
    std::cout << "Brainfuck interpreter (v1.1)... ";
    const char* jsres2 = join_str_with_space(args[1],"../brainfuck.script");
    char* jsres2c = new char[strlen(jsres2)+1];
    strcpy(jsres2c,jsres2);
    char* t9o = system_goutput(join_str_with_space("cat test.bf |",jsres2c));
    if(strlen(t9o) == 6 && t9o[0] == 'a' && t9o[1] == 'b' && t9o[2] == 'c' && t9o[3] == 'd' && t9o[4] == 'e' && t9o[5] == 'f') {
        std::cout << "PASSED\n";
    } else {
        std::cout << "FAILED\n";
        passing = false;
    }
    
    system("rm .test.bmp");
    
    std::cout << "Automatic test " << (passing ? "passed" : "failed") << ".\n\nStarting manual test...\n";
    
    std::cout << "Please input something. You should see the same number printed after it.\n";
    system(join_str_with_space(args[1],"cin.script"));
    
    std::cout << "You should now see 5, 4, 3, 2, 1 in order.\n";
    system(join_str_with_space(args[1],"example_o.script"));
    
    std::cout << "Test done.\n";
    return 0;
}
