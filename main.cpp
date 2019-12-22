//Extrasklep's language interpreter made by extrasklep copyright license bla bla
//version 1.0
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include <signal.h>

unsigned char debug=0;

unsigned long gLine=0; //for l and handling errors

//here variable limit
long variables[256];
unsigned char cvariables[32768];

void catchFault(int sig) {
    std::cout << "\nsegmentation fault at line " << gLine << "\n\0";
    exit(255);
}

bool isWhiteSpace(char character) {
    if(character==' ') {return true;}
    return false;
}

bool isAStringNumber(char character) {
    if(character>='0' && character<='9') {return true;}
    return false;
}

long getNumberFrom(char* em) {
    long currentNumber=0;
    unsigned int len=strlen(em);
    if(len<1) {std::cout << '\n' << gLine << ": invalid read\n"; exit(255);}
    else {
        if(isAStringNumber(em[0]) || em[0]=='-') {
            currentNumber=atoi(em);
            return currentNumber;
        } else if(em[0]=='o'||em[0]=='r') {
            std::cout << '\n' << gLine << ": illegal operation: cannot read from output\n";
            exit(255);
        } else if(em[0]=='i') {
            //get cin
            std::cout << "\033[30;47m" << "< " << "\033[0m";
            std::cin >> currentNumber;
            return currentNumber;
        } else if(em[0]=='l') {
            return gLine;
        } else if(em[0]=='n') {
            return '\n';
        } else if(len>1 && em[0]=='v') {
            char vnumS[len];
            for(unsigned int i=1;i<len;i++) {vnumS[i-1]=em[i];}
            unsigned long vnum;
            if(vnumS[0]=='v'||vnumS[0]=='c') {
                vnum=getNumberFrom(vnumS);
            } else {
                vnum=atoi(vnumS);
            }
            if(vnum>=sizeof(variables)) {std::cout << '\n' << gLine << ": memory read error: variable is outside of variable limit\n"; exit(255);}
            currentNumber=variables[vnum];
            return currentNumber;
        } else if(len>1 && em[0]=='c') {
            char vnumS[len];
            for(unsigned int i=1;i<len;i++) {vnumS[i-1]=em[i];}
            unsigned long vnum;
            if(vnumS[0]=='v'||vnumS[0]=='c') {
                vnum=getNumberFrom(vnumS);
            } else {
                vnum=atoi(vnumS);
            }
            if(vnum>=sizeof(cvariables)) {std::cout << '\n' << gLine << ": memory read error: variable is outside of variable limit\n"; exit(255);}
            currentNumber=cvariables[vnum];
            return currentNumber;
        } else if(len>1 && em[0]=='\\') {
            currentNumber=em[1];
            return currentNumber;
        }
    }
    return 0;
}

void putNumberTo(char* em, long num) {
    unsigned int len=strlen(em);
    if(len<1) {std::cout << '\n' << gLine << ": invalid write\n"; exit(255);}
    else {
        if(isAStringNumber(em[0])) {return;}
        else if(em[0]=='o') {
            if(debug) {std::cout << '\n';}
            std::cout << "\033[30;47m" << "> " << "\033[0m" << num << " \n";
            return;
        } else if(em[0]=='r') {
            char rchar=num;
            std::cout << rchar;
            return;
        } else if(em[0]=='i') {
            std::cout << '\n' << gLine << ": illegal operation: cannot write to input\n";
            exit(255);
        } else if(len>1 && em[0]=='v') {
            char vnumS[len];
            for(unsigned int i=1;i<len;i++) {vnumS[i-1]=em[i];}
            unsigned long vnum;
            if(vnumS[0]=='v'||vnumS[0]=='c') {
                vnum=getNumberFrom(vnumS);
            } else {
                vnum=atoi(vnumS);
            }
            if(vnum>=sizeof(variables)) {std::cout << '\n' << gLine << ": memory write error: variable is outside of variable limit\n"; exit(255);}
            variables[vnum]=num;
            return;
        } else if(len>1 && em[0]=='c') {
            char vnumS[len];
            for(unsigned int i=1;i<len;i++) {vnumS[i-1]=em[i];}
            unsigned long vnum;
            if(vnumS[0]=='v'||vnumS[0]=='c') {
                vnum=getNumberFrom(vnumS);
            } else {
                vnum=atoi(vnumS);
            }
            if(vnum>=sizeof(cvariables)) {std::cout << '\n' << gLine << ": memory write error: variable is outside of variable limit\n"; exit(255);}
            cvariables[vnum]=num;
            return;
        }
    }
}

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

bool isInputType(unsigned char type) {
    if(type==1||type==3||type==7||type==9||type==10||type==11) {return true;}
    return false;
}

bool isOutputType(unsigned char type) {
    if(type==3||type==6||type==8) {return true;}
    return false;
}

int main(int argc,char* args[]) {
    if(argc<2) {
        std::cout << "usage: " << args[0] << " [script] [debug level]\n";
        return 0;
    }
    if(argc>2) {
        debug=atoi(args[2]);
    }
    //segfault handling
    signal(SIGSEGV,catchFault);
    
    std::ifstream file(args[1]);
    if(file.is_open()) {
        std::string sline;
        char* line;
        unsigned long currentLine=0;
        unsigned long jumpTo=1;
        bool jumpRequested=0;
        while (std::getline(file,sline) && ++currentLine) {
            //jumps suck
            if(jumpRequested) {
                if(currentLine==jumpTo) {
                    if(debug>=200) {std::cout << currentLine << ": jump done\n";}
                    jumpRequested=0;
                } else if(jumpTo<currentLine) {
                    if(debug>=200) {std::cout << "jump: seeking back: " << "cline=" << currentLine << " jt=" << jumpTo << " jr=" << jumpRequested << "\n";}
                    currentLine=1;
                    file.seekg(std::ios::beg);
                } else {
                    continue;
                }
            }
            line = new char[sline.length()+1];
            strcpy(line,sline.c_str());
            gLine=currentLine;
            unsigned long linecpos=0;
            unsigned long linelen=strlen(line);
            if(debug) {std::cout << '\n' << currentLine << ": " << line;}
            
            //start parsing
            if(debug>=255) {std::cout << '\n';}
            //set offset to first not space character
            while(isWhiteSpace(line[linecpos]) && linecpos<linelen) {
                if(debug>=255) {std::cout << currentLine << ":" << linecpos << " whitespace character\n";}
                linecpos++;
            }
            if(linecpos>=linelen) {
                continue;
            }
            //check if line is a comment
            if(line[linecpos]=='/' || line[linecpos]=='#') {
                if(debug) {std::cout << "   (commented line)";}
                continue;
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
            unsigned char types=11; //number of different types
            unsigned char elementTypes[6]={0,0,0,0,0,0};
            char* elementStrings[6];
            unsigned char currentElement=0;
            unsigned long currentElementStrPos=0;
            bool newElement=1;
            bool keepReading=1;
            for(;linecpos<linelen && keepReading;linecpos++) {
                if(currentElement>sizeof(elementTypes)) {std::cout << '\n' << currentLine << ":" << linecpos+1 << " syntax error: too many elements\n"; exit(255);}
                if(newElement) {
                    //try to find type of element
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
                            elementStrings[currentElement] = new char[elementStringSize];
                            elementStrings[currentElement][currentElementStrPos++] = line[linecpos];
                            if(i==4) {
                                if(debug>=255) {std::cout << currentLine << ":" << linecpos+1 << " terminator\n";}
                                currentElement++; currentElementStrPos=0;
                                keepReading=0;
                            }
                            break;
                        }
                    }
                } else {
                    //read more of current element
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
            
            //parse and execute
            
            //no terminator
            if(elementStrings[currentElement-1][0]!=';') {std::cout << '\n' << "syntax error: expected ; near " << currentLine << ":" << linecpos+1 << '\n'; exit(255);}
            //not enough elements
            if(currentElement-1<3) {std::cout << "\nsyntax error: incomplete operation at line " << currentLine << '\n'; exit(255);}
            
            //>> write input to output
            if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==1 && isOutputType(elementTypes[2])) {
                long op1=getNumberFrom(elementStrings[0]);
                putNumberTo(elementStrings[2],op1);
                if(debug>=200) {std::cout << currentLine << ": action: write " << op1 << " to " << elementStrings[2] << '\n';}
                
                //+>add input to output
            } else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==2 && isOutputType(elementTypes[2])) {
                long op1=getNumberFrom(elementStrings[0]);
                long op2=getNumberFrom(elementStrings[2]);
                putNumberTo(elementStrings[2],op2+op1);
                if(debug>=200) {std::cout << currentLine << ": action: add " << op1 << " to " << op2 << " at " << elementStrings[2] << '\n';}
                
                //->subtract input from output
            } else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==3 && isOutputType(elementTypes[2])) {
                long op1=getNumberFrom(elementStrings[0]);
                long op2=getNumberFrom(elementStrings[2]);
                putNumberTo(elementStrings[2],op2-op1);
                if(debug>=200) {std::cout << currentLine << ": action: subtract " << op1 << " from " << op2 << " at " << elementStrings[2] << '\n';}
                
                //*>multiply output by input
            } else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==4 && isOutputType(elementTypes[2])) {
                long op1=getNumberFrom(elementStrings[0]);
                long op2=getNumberFrom(elementStrings[2]);
                putNumberTo(elementStrings[2],op2*op1);
                if(debug>=200) {std::cout << currentLine << ": action: multiply " << op2 << " by " << op1 << " at " << elementStrings[2] << '\n';}
                
                // /> divide output by input
            } else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==5 && isOutputType(elementTypes[2])) {
                long op1=getNumberFrom(elementStrings[0]);
                long op2=getNumberFrom(elementStrings[2]);
                putNumberTo(elementStrings[2],op2/op1);
                if(debug>=200) {std::cout << currentLine << ": action: divide " << op2 << " by " << op1 << " at " << elementStrings[2] << '\n';}
            } else if(isInputType(elementTypes[0]) && getSignType(elementStrings[1])==6 && isInputType(elementTypes[2])) {
                long op1=getNumberFrom(elementStrings[0]);
                if(op1>0) {
                    long op2=getNumberFrom(elementStrings[2]);
                    //do jump
                    jumpTo=op2;
                    if(jumpTo<1) {
                        std::cout << '\n' << currentLine << ": illegal jump\n";
                        exit(255);
                    }
                    jumpRequested=1;
                    currentLine=0;
                    file.seekg(std::ios::beg);
                    if(debug>=200) {std::cout << gLine << ": action: jump to " << op2 << ": " << op1 << " is bigger than 0 at " << elementStrings[0] << '\n';}
                } else {
                    if(debug>=200) {std::cout << currentLine << ": no jump: " << op1 << " is not more than 0 at " << elementStrings[0] << '\n';}
                }
            } 
            else {
                std::cout << '\n' << currentLine << ": warning: invalid operation - skipping line\n";
            }
            
        }
    file.close();
    if(debug) {std::cout << '\n';}
    } else {
        std::cout << "unable to open file " << args[2] << '\n';
        return 0;
    }
}
