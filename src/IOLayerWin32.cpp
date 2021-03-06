#define WIN32_LEAN_AND_MEAN

#include "windows.h"

#include <stdlib.h>
#include <stdio.h>

namespace TEdit {
    int tedit_main(int,char **);
}

int main(int argc,char ** argv) {
    return TEdit::tedit_main(argc,argv);
}

namespace TEdit::IOLayer {
    
    HANDLE hStdin;
    HANDLE hStdout;
    
    uint16_t fgcolors[]{
        0,//BLACK
        FOREGROUND_BLUE,//BLUE
        FOREGROUND_GREEN,//GREEN
        FOREGROUND_GREEN | FOREGROUND_BLUE,//CYAN
        FOREGROUND_RED,//RED
        FOREGROUND_RED | FOREGROUND_BLUE,//MAGENTA
        FOREGROUND_RED | FOREGROUND_GREEN,//BROWN
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,//LIGHT_GREY
        FOREGROUND_INTENSITY,//DARK_GREY
        FOREGROUND_INTENSITY | FOREGROUND_BLUE,//LIGHT_BLUE
        FOREGROUND_INTENSITY | FOREGROUND_GREEN,//LIGHT_GREEN
        FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,//LIGHT_CYAN
        FOREGROUND_INTENSITY | FOREGROUND_RED,//LIGHT_RED
        FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,//LIGHT_MAGENTA
        FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,//LIGHT_BROWN
        FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,//WHITE
    };
    
    uint16_t bgcolors[]{
        0,//BLACK
        BACKGROUND_BLUE,//BLUE
        BACKGROUND_GREEN,//GREEN
        BACKGROUND_GREEN | BACKGROUND_BLUE,//CYAN
        BACKGROUND_RED,//RED
        BACKGROUND_RED | BACKGROUND_BLUE,//MAGENTA
        BACKGROUND_RED | BACKGROUND_GREEN,//BROWN
        BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,//LIGHT_GREY
        BACKGROUND_INTENSITY,//DARK_GREY
        BACKGROUND_INTENSITY | BACKGROUND_BLUE,//LIGHT_BLUE
        BACKGROUND_INTENSITY | BACKGROUND_GREEN,//LIGHT_GREEN
        BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,//LIGHT_CYAN
        BACKGROUND_INTENSITY | BACKGROUND_RED,//LIGHT_RED
        BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,//LIGHT_MAGENTA
        BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,//LIGHT_BROWN
        BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,//WHITE
    };
    
    void init(){
        hStdin=GetStdHandle(STD_INPUT_HANDLE);
        hStdout=GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleScreenBufferSize(hStdout,{80,25});
        SMALL_RECT window={0,0,79,24};
        SetConsoleWindowInfo(hStdout,true,&window);
    }
    
    keypress get_key(){
        INPUT_RECORD input;
        DWORD n;
        for(;;){
            ReadConsoleInput(hStdin,&input,1,&n);
            if(n>0&&input.EventType==KEY_EVENT&&input.Event.KeyEvent.bKeyDown){
                auto & k=input.Event.KeyEvent;
                if(k.uChar.AsciiChar!=0){
                    return {k.uChar.AsciiChar,KEY,k.wRepeatCount};
                }else{
                    switch(k.wVirtualKeyCode){
                    case VK_LEFT:
                        return {0,ARROW_LEFT,k.wRepeatCount};
                    case VK_UP:
                        return {0,ARROW_UP,k.wRepeatCount};
                    case VK_RIGHT:
                        return {0,ARROW_RIGHT,k.wRepeatCount};
                    case VK_DOWN:
                        return {0,ARROW_DOWN,k.wRepeatCount};
                    case VK_MENU:
                        return {0,ALT,k.wRepeatCount};
                    }
                }
            }
        }
    }
    
    void moveCursor(int16_t x,int16_t y){
        SetConsoleCursorPosition(hStdout,{x,y});
    }
    
    void writeChar(char c){
        WriteConsole(hStdout,&c,1,NULL,NULL);
    }
    
    void writeChars(const char * s,size_t n){
        WriteConsole(hStdout,s,n,NULL,NULL);
    }
    
    void writeStr(const char * s){
        WriteConsole(hStdout,s,strlen(s),NULL,NULL);
    }
    
    void setColor(color fg,color bg){
        SetConsoleTextAttribute(hStdout,fgcolors[fg]|bgcolors[bg]);
    }
    
    void fillLine(int16_t line,int16_t rows,char c,color fg,color bg){
        DWORD temp;
        FillConsoleOutputCharacter(hStdout,c,rows*80,{0,line},&temp);
        FillConsoleOutputAttribute(hStdout,fgcolors[fg]|bgcolors[bg],rows*80,{0,line},&temp);
    }
    
    [[noreturn]] void exit_error(const char * fmt,...){
        va_list args1;
        va_start(args1,fmt);
        va_list args2;
        va_copy(args2,args1);
        size_t len=vsnprintf(NULL,0,fmt,args2);
        va_end(args2);
        char * buf=(char*)calloc(len+1,sizeof(char));
        vsnprintf(buf,len+1,fmt,args1);
        va_end(args1);
        fillLine(0,25,' ',WHITE,BLACK);
        moveCursor(0,0);
        MessageBox(NULL,buf,"ERROR",MB_OK);
        free(buf);
        exit(1);
    }
}
