//Extrasklep's language interpreter made by extrasklep copyright license bla bla
//version 1.0
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include <signal.h>

//debug level
unsigned char debug=0;

unsigned long gLine=0; //global line number variable, for 'l' letter in the language and handling errors

unsigned char parseExecErr = 0; //allow handling errors instead of always exiting

unsigned char peErr(unsigned char code) {
    parseExecErr = code;
    return parseExecErr;
}

unsigned char peErr() {
    return parseExecErr;
}

//here variable limit
long variables[256];
unsigned char cvariables[32768];

//catch system segmentation faults and print what line they happened on
void catchFault(int sig) {
    std::cout << "\nsegmentation fault at line " << gLine << "\n\0";
    exit(255);
}

//checks if character is whitespace
bool isWhiteSpace(char character) {
    if(character==' ') {return true;}
    return false;
}

//checks if the character is a number character
bool isAStringNumber(char character) {
    if(character>='0' && character<='9') {return true;}
    return false;
}

//gets the number from an input element
long getNumberFrom(char* em) {
    //result number
    long currentNumber=0;
    unsigned int len=strlen(em);
    if(len<1) {std::cout << '\n' << gLine << ": invalid read\n"; peErr(255); return 0;}
    else {
        //number
        if(isAStringNumber(em[0]) || em[0]=='-') {
            currentNumber=atoi(em);
            return currentNumber;
        }
        //output (illegal)
        else if(em[0]=='o'||em[0]=='r') {
            std::cout << '\n' << gLine << ": illegal operation: cannot read from output\n";
            peErr(255); return 0;
        }
        //number input
        else if(em[0]=='i') {
            //get cin
            std::cout << "\033[30;47m" << "< " << "\033[0m";
            std::cin >> currentNumber;
            return currentNumber;
        }
        //current line number
        else if(em[0]=='l') {
            return gLine;
        }
        //newline character
        else if(em[0]=='n') {
            return '\n';
        }
        //long variable
        else if(len>1 && em[0]=='v') {
            //variable number string (characters after 'v')
            char vnumS[len];
            //copy element string into variable number string starting from character 1
            for(unsigned int i=1;i<len;i++) {vnumS[i-1]=em[i];}
            //variable number
            unsigned long vnum;
            //pointer (vv0/vc0)
            if(vnumS[0]=='v'||vnumS[0]=='c') {
                //get number from variable number string (read the pointer variable)
                vnum=getNumberFrom(vnumS);
                if(peErr()) return 0;
            } else {
                //convert the string number after 'v' into a number normally
                vnum=atoi(vnumS);
            }
            //don't try to read out of bounds
            if(vnum>=sizeof(variables)) {std::cout << '\n' << gLine << ": memory read error: variable is outside of variable limit\n"; peErr(255); return 0;}
            //read the number stored in the variable
            currentNumber=variables[vnum];
            return currentNumber;
        }
        //char variable
        else if(len>1 && em[0]=='c') {
            //variable number string (characters after 'c')
            char vnumS[len];
            //copy element string into variable number string starting from character 1
            for(unsigned int i=1;i<len;i++) {vnumS[i-1]=em[i];}
            //variable number
            unsigned long vnum;
            //pointer (cv0/cc0)
            if(vnumS[0]=='v'||vnumS[0]=='c') {
                //get number from variable number string (read the pointer variable)
                vnum=getNumberFrom(vnumS);
                if(peErr()) return 0;
            } else {
                //convert the string number after 'c' into a number normally
                vnum=atoi(vnumS);
            }
            //don't try to read out of bounds
            if(vnum>=sizeof(cvariables)) {std::cout << '\n' << gLine << ": memory read error: variable is outside of variable limit\n"; peErr(255); return 0;}
            //read the number stored in the variable
            currentNumber=cvariables[vnum];
            return currentNumber;
        }
        //character escape
        else if(len>1 && em[0]=='\\') {
            //return the character after the escape character
            currentNumber=em[1];
            return currentNumber;
        }
    }
    return 0;
}

void putNumberTo(char* em, long num) {
    unsigned int len=strlen(em);
    if(len<1) {std::cout << '\n' << gLine << ": invalid write\n"; peErr(255); return;}
    else {
        //can't write to a number, the interpreter should never attempt to do this though
        if(isAStringNumber(em[0])) {return;}
        //write to number output
        else if(em[0]=='o') {
            if(debug) {std::cout << '\n';}
            std::cout << "\033[30;47m" << "> " << "\033[0m" << num << " \n";
            return;
        }
        //write to raw character output
        else if(em[0]=='r') {
            char rchar=num;
            std::cout << rchar;
            return;
        }
        //input (illegal)
        else if(em[0]=='i') {
            std::cout << '\n' << gLine << ": illegal operation: cannot write to input\n";
            peErr(255); return;
        }
        //write to long variable
        else if(len>1 && em[0]=='v') {
            //variable number string (characters after 'v')
            char vnumS[len];
            //copy element string into variable number string starting from character 1
            for(unsigned int i=1;i<len;i++) {vnumS[i-1]=em[i];}
            //variable number
            unsigned long vnum;
            //pointer (vv0/vc0)
            if(vnumS[0]=='v'||vnumS[0]=='c') {
                //get number from variable number string (read the pointer variable)
                vnum=getNumberFrom(vnumS);
                if(peErr()) return;
            } else {
                //convert the string number after 'v' into a number normally
                vnum=atoi(vnumS);
            }
            //don't try to write out of bounds
            if(vnum>=sizeof(variables)) {std::cout << '\n' << gLine << ": memory write error: variable is outside of variable limit\n"; peErr(255); return;}
            //write the input number to the variable
            variables[vnum]=num;
            return;
        }
        //write to char variable
        else if(len>1 && em[0]=='c') {
            //variable number string (characters after 'c')
            char vnumS[len];
            //copy element string into variable number string starting from character 1
            for(unsigned int i=1;i<len;i++) {vnumS[i-1]=em[i];}
            //variable number
            unsigned long vnum;
            //pointer (cv0/cc0)
            if(vnumS[0]=='v'||vnumS[0]=='c') {
                //get number from variable number string (read the pointer variable)
                vnum=getNumberFrom(vnumS);
                if(peErr()) return;
            } else {
                //convert the string number after 'c' into a number normally
                vnum=atoi(vnumS);
            }
            //don't try to write out of bounds
            if(vnum>=sizeof(cvariables)) {std::cout << '\n' << gLine << ": memory write error: variable is outside of variable limit\n"; peErr(255); return;}
            //write the input number to the variable
            cvariables[vnum]=num;
            return;
        }
    }
}

//get sign type (middle element)
unsigned char getSignType(char* em) {
    unsigned int len=strlen(em);
    if(len!=2) {return 0;}
    // (1:>> 2:>+ 3:>- 4:>* 5:>/ 6:>?)
    if(em[0]!='>') {return 0;}
    else {
        if(em[1]=='>') {return 1;}
        if(em[1]=='+') {return 2;}
        if(em[1]=='-') {return 3;}
        if(em[1]=='*') {return 4;}
        if(em[1]=='/') {return 5;}
        if(em[1]=='?') {return 6;}
    }
    return 0;
}

//check type of a character
//returns true if the character and type argument match
//"first" argument will be true if the character is the first character in an element
//this is stupid but I don't feel like changing anything about it
bool isType(char character,unsigned char type,bool first) {
    switch(type) {
        case 0: return false;
        case 1: if(isAStringNumber(character)) {return true;} else if(first==1) {if(character=='-') {return true;}} else {return false;} break;
        case 2: if(character=='>') {return true;} else if(first==0) {if(character=='+'||character=='-'||character=='*'||character=='/'||character=='?') {return true;}} else {return false;} break;
        case 3: if(character=='v'||character=='c') {return true;} else if(first==0) {if(isAStringNumber(character)) {return true;}} else {return false;} break;
        case 4: if(character==';') {return true;} else {return false;} break;
        case 5: if(character=='/') {return true;} else {return false;} break;
        case 6: if(character=='o') {return true;} else {return false;} break;
        case 7: if(character=='i') {return true;} else {return false;} break;
        case 8: if(character=='r') {return true;} else {return false;} break;
        case 9: if(character=='l') {return true;} else {return false;} break;
        case 10: if(character=='\\') {return true;} else if(first==0) {return true;} break;
        case 11: if(character=='n') {return true;} else {return false;} break;
    }
    return false;
}

//returns true if element type is an input type
bool isInputType(unsigned char type) {
    if(type==1||type==3||type==7||type==9||type==10||type==11) {return true;}
    return false;
}

//returns true if element type is an output type
bool isOutputType(unsigned char type) {
    if(type==3||type==6||type==8) {return true;}
    return false;
}

void splitLine(char* line,unsigned char* elementTypes,char** elementStrings,unsigned char& currentElement,bool& isEmpty) {
    unsigned long currentLine = gLine;
    //character position in current line
    unsigned long linecpos=0;
    //line length
    unsigned long linelen=strlen(line);
    if(debug) {std::cout << '\n' << currentLine << ": " << line;}
    
    //start parsing
    if(debug>=255) {std::cout << '\n';}
    //set offset to first not space character
    while(isWhiteSpace(line[linecpos]) && linecpos<linelen) {
        if(debug>=255) {std::cout << currentLine << ":" << linecpos << " whitespace character\n";}
        linecpos++;
    }
    //end of line reached
    if(linecpos>=linelen) {
        //continue;
        isEmpty = 1;
        return;
    }
    //check if line is a comment
    if(line[linecpos]=='/' || line[linecpos]=='#') {
        if(debug) {std::cout << "   (commented line)";}
        //continue;
        isEmpty = 1;
        return;
    }
    //split elements
    /* element types
     * 0 = none
     * 1 = number
     * 2 = signs (1:>> 2:>+ 3:>- 4:>* 5:>/ 6:>? )
     * 3 = variable
     * 4 = terminator
     * 5 = comment
     * 6 = output
     * 7 = input
     * 8 = raw output
     * 9 = line number
     * 10 = escape character
     * 11 = newline
     */
    unsigned char types=11; //amount of different types
    //unsigned char elementTypes[6]={0,0,0,0,0,0};
    //char elementStrings[6][32];
    //unsigned char currentElement=0;
    unsigned long currentElementStrPos=0;
    bool newElement=1;
    bool keepReading=1;
    //this part is also a mess i guess
    for(;linecpos<linelen && keepReading;linecpos++) {
        if(currentElement>sizeof(elementTypes)) {std::cout << '\n' << currentLine << ":" << linecpos+1 << " syntax error: too many elements\n"; peErr(255); return;}
        //starting new element, find out the type of it
        if(newElement) {
            if(debug>=255) {std::cout << currentLine << ":" << linecpos+1 << " trying to start new element\n";}
            if(isWhiteSpace(line[linecpos])) {
                if(debug>=255) {std::cout << currentLine << ":" << linecpos+1 << " character is whitespace, trying next\n";}
                continue;
            }
            bool found=0;
            for(unsigned char i=1;i<=types && found==0;i++) {
                if(isType(line[linecpos],i,1)) {
                    if(debug>=255) {unsigned int dei=i; std::cout << currentLine << ":" << linecpos+1 << " element type " << dei << '\n';}
                    found=1;
                    if(i==5) {
                        if(debug>=255) {std::cout << currentLine << ":" << linecpos+1 << " comment\n";}
                        keepReading=0;
                        break;
                    }
                    newElement=0;
                    elementTypes[currentElement]=i;
                    unsigned long elementStringSize=1;
                    for(unsigned long p=linecpos;p<linelen && isType(line[p],i,0);p++) {elementStringSize++;}
                    if(elementStringSize>32) {
                        std::cout << '\n' << currentLine << ":" << linecpos+1 << " syntax error: element too long\n";
                        peErr(255); return;
                    }
                    elementStrings[currentElement][currentElementStrPos++] = line[linecpos];
                    if(i==4) {
                        if(debug>=255) {std::cout << currentLine << ":" << linecpos+1 << " terminator\n";}
                        elementStrings[currentElement][currentElementStrPos++] = '\0';
                        currentElement++; currentElementStrPos=0;
                        keepReading=0;
                    }
                    break;
                }
            }
            if(!found) {
                std::cout << '\n' << currentLine << ":" << linecpos+1 << " syntax error: no valid element found\n";
                peErr(255); return;
            }
        }
        //read more of current element
        else {
            //if current character is no longer of current type
            if(isType(line[linecpos],elementTypes[currentElement],0)==false || (elementTypes[currentElement]==2 && currentElementStrPos>1) || (elementTypes[currentElement]==10 && currentElementStrPos>1)) {
                if(debug>=255) {std::cout << currentLine << ":" << linecpos+1 << " element is over\n";} 
                linecpos--; 
                elementStrings[currentElement][currentElementStrPos] = '\0';
                currentElement++; 
                currentElementStrPos=0; 
                newElement=1; 
                continue;
            }
            elementStrings[currentElement][currentElementStrPos++] = line[linecpos];
        }
    }
    
    //print split elements
    if(debug) {
        std::cout << "   (";
        bool firstTime=1;
        for(unsigned int i=0;i<currentElement;i++) {
            if(firstTime==false) {std::cout << ", ";} else {firstTime=0;}
            std::cout << elementStrings[i];
        }
        std::cout << ")";
        if(debug>=200) {std::cout << '\n';}
    }
    return;
}

//returns 0 or jump destination
unsigned long executeLine(unsigned char* elementTypes,char** elementStrings,unsigned char& currentElement) {
    unsigned long currentLine = gLine;
    //no terminator
    if(elementStrings[currentElement-1][0]!=';') {std::cout << '\n' << "syntax error: expected ; on line " << currentLine << '\n'; peErr(255); return 0;}
    //not enough elements
    if(currentElement-1<3) {std::cout << "\nsyntax error: incomplete operation at line " << currentLine << '\n'; peErr(255); return 0;}
    
    // >> write input to output
    if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==1 && isOutputType(elementTypes[2])) {
        long op1=getNumberFrom(elementStrings[0]);
        if(peErr()) return 0;
        putNumberTo(elementStrings[2],op1);
        if(peErr()) return 0;
        if(debug>=200) {std::cout << currentLine << ": action: write " << op1 << " to " << elementStrings[2] << '\n';}
    }
    // >+ add input to output
    else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==2 && isOutputType(elementTypes[2])) {
        long op1=getNumberFrom(elementStrings[0]);
        if(peErr()) return 0;
        long op2=getNumberFrom(elementStrings[2]);
        if(peErr()) return 0;
        putNumberTo(elementStrings[2],op2+op1);
        if(peErr()) return 0;
        if(debug>=200) {std::cout << currentLine << ": action: add " << op1 << " to " << op2 << " at " << elementStrings[2] << '\n';}
    }
    // >- subtract input from output
    else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==3 && isOutputType(elementTypes[2])) {
        long op1=getNumberFrom(elementStrings[0]);
        if(peErr()) return 0;
        long op2=getNumberFrom(elementStrings[2]);
        if(peErr()) return 0;
        putNumberTo(elementStrings[2],op2-op1);
        if(peErr()) return 0;
        if(debug>=200) {std::cout << currentLine << ": action: subtract " << op1 << " from " << op2 << " at " << elementStrings[2] << '\n';}
    }
    // >* multiply output by input
    else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==4 && isOutputType(elementTypes[2])) {
        long op1=getNumberFrom(elementStrings[0]);
        if(peErr()) return 0;
        long op2=getNumberFrom(elementStrings[2]);
        if(peErr()) return 0;
        putNumberTo(elementStrings[2],op2*op1);
        if(peErr()) return 0;
        if(debug>=200) {std::cout << currentLine << ": action: multiply " << op2 << " by " << op1 << " at " << elementStrings[2] << '\n';}
    }
    // >/ divide output by input
    else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==5 && isOutputType(elementTypes[2])) {
        long op1=getNumberFrom(elementStrings[0]);
        if(peErr()) return 0;
        long op2=getNumberFrom(elementStrings[2]);
        if(peErr()) return 0;
        putNumberTo(elementStrings[2],op2/op1);
        if(peErr()) return 0;
        if(debug>=200) {std::cout << currentLine << ": action: divide " << op2 << " by " << op1 << " at " << elementStrings[2] << '\n';}
    }
    // >? jump to output(actually an input) if input is bigger than 0
    else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==6 && isInputType(elementTypes[2])) {
        long op1=getNumberFrom(elementStrings[0]);
        if(peErr()) return 0;
        if(op1>0) {
            long op2=getNumberFrom(elementStrings[2]);
            if(peErr()) return 0;
            if(op2<1) {
                std::cout << '\n' << currentLine << ": illegal jump\n";
                peErr(255); return 0;
            }
            if(debug>=200) {std::cout << gLine << ": action: jump to " << op2 << ": " << op1 << " is bigger than 0 at " << elementStrings[0] << '\n';}
            return op2;
        } else {
            if(debug>=200) {std::cout << currentLine << ": no jump: " << op1 << " is not more than 0 at " << elementStrings[0] << '\n';}
        }
    }
    //not a valid operation
    else {
        std::cout << '\n' << currentLine << ": error: invalid operation\n";
        peErr(255); return 0;
    }
    return 0;
}

//returns 0 or jump destination
unsigned long parseLine(char* line) {
    /* element types
     * 0 = none
     * 1 = number
     * 2 = signs (1:>> 2:>+ 3:>- 4:>* 5:>/ 6:>? )
     * 3 = variable
     * 4 = terminator
     * 5 = comment
     * 6 = output
     * 7 = input
     * 8 = raw output
     * 9 = line number
     * 10 = escape character
     * 11 = newline
     */
    unsigned char elementTypes[6]={0,0,0,0,0,0};
    char* elementStrings[6];
    //allocate memory for element strings
    for(unsigned char i=0;i<6;i++) {elementStrings[i] = new char[32];}
    unsigned char currentElement=0;
    bool isEmpty = 0;
    splitLine(line,elementTypes,elementStrings,currentElement,isEmpty);
    //return if the line is a comment/whitespace/... or an error occurred
    if(isEmpty || peErr()) {for(unsigned char i=0;i<6;i++) {delete[] elementStrings[i];} return 0;}
    
    unsigned long res = executeLine(elementTypes,elementStrings,currentElement);
    if(peErr()) {for(unsigned char i=0;i<6;i++) {delete[] elementStrings[i];} return 0;}
    //free strings
    for(unsigned char i=0;i<6;i++) {delete[] elementStrings[i];}
    return res;
}

int main(int argc,char* args[]) {
    //segfault handling
    signal(SIGSEGV,catchFault);
    
    if(argc<2) {
        //too lazy to do arg proper arg parsing yet
        std::cout << "Extrasklep's language interpreter version 1.0\nusage: " << args[0] << " [script] [debug level] [mode]\nmode: 0 = split lines during execution (slow but you might want to use this when debugging)\n      1 = split all lines before execution (fast, default)\n";
        //begin interactive shell
        std::cout << "interactive shell\n";
        char* line;
        while(1) {
            line = new char[256];
            std::cout << "$ ";
            std::cin >> line;
            if(parseLine(line)) {
                std::cout << "cannot jump in interactive shell\n";
            }
            if(peErr()) {
                peErr(0);
            }
            delete[] line;
        }
    }
    if(argc>2) {
        debug = atoi(args[2]);
    }
    bool splitMode = 1;
    if(argc>3) {
        splitMode = atoi(args[3]);
    }
    
    char** lines;
    
    //read lines into memory
    std::ifstream file(args[1],std::ios::in|std::ios::ate);
    if(file.is_open()) {
        std::streampos fsize = file.tellg();
        file.seekg(std::ios::beg);
        char* memblock = new char[fsize];
        file.read(memblock,fsize);
        file.close();
        
        //count total lines
        unsigned long totalLines = 1;
        for(unsigned long i=0;i<fsize;i++) {
            if(memblock[i] == '\n') totalLines++;
        }
        if(debug) {std::cout << "total lines: " << totalLines << '\n';}
        
        //split file into lines
        lines = new char* [totalLines];
        unsigned long filePos = 0;
        unsigned long previousFilePos = 0;
        //count current line length
        unsigned long cLineLen = 0;
        for(unsigned long l=1;l<totalLines;l++) {
            cLineLen = 0;
            previousFilePos = filePos;
            while(memblock[filePos++] != '\n') {cLineLen++;}
            lines[l] = new char[cLineLen+1];
            for(unsigned long i=previousFilePos;i<filePos;i++) {
                lines[l][i-previousFilePos] = memblock[i];
            }
            lines[l][cLineLen] = '\0';
        }
        delete[] memblock;
        
        
        unsigned char elementTypes[totalLines][6];
        char* elementStrings[totalLines][6];
        unsigned char currentElement[totalLines];
        bool isEmpty[totalLines];
        if(splitMode) {
            //split lines into elements
            for(unsigned long l=1;l<totalLines;l++) {
                gLine = l;
                
                currentElement[l] = 0;
                isEmpty[l] = 0;
                for(unsigned char i=0;i<6;i++) {
                    elementStrings[l][i] = new char[32];
                    elementTypes[l][i] = 0;
                }
                
                splitLine(lines[l],elementTypes[l],elementStrings[l],currentElement[l],isEmpty[l]);
                if(peErr()) {exit(255);}
            }
        }
        
        unsigned long currentLine=0;
        while (++currentLine < totalLines) {
            gLine=currentLine;
            
            //parse and execute
            unsigned long jumpTo;
            if(jumpTo = splitMode ?
                ( !isEmpty[currentLine] ? executeLine(elementTypes[currentLine],elementStrings[currentLine],currentElement[currentLine]) : 0 ) :
                parseLine(lines[currentLine])) {
                //-1 because the loop is going to add to it
                currentLine=jumpTo-1;
            }
            if(peErr()) {exit(peErr());}
        }
        if(debug) {std::cout << '\n';}
    } else {
        perror("unable to open file");
        return 0;
    }
}
