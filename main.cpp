#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include <cstring>
#include <vector>
#include <windows.h>
#include <map>
#include <stdio.h>
#include <bits/stdc++.h>
#include <fstream>
#include "tinyexpr.h"
#define ever ;;
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600
#define BLOCK_HEIGHT 40

using namespace std;

enum blockType{
    start, stop, input, output, calculation, decision, null
};

struct Color{
    Color(){}
    Color(short red_, short green_, short blue_){
        red = red_;
        green = green_;
        blue = blue_;
    }
    short red, green, blue;
};

int principalWindow  = initwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "InterSchem", 100, 10);
Color backgroundColor(25, 10, 44);
Color lineColor(200, 233, 166);
Color reliseButtonColor(23, 124, 221);
Color reliseTextButtonColor(12, 33, 24);
Color mouseOverButtonColor(94, 190, 241);
Color mouseOverTextButtonColor(121, 133, 24);
Color blockBordreColor(0,80,163);
Color blockColor(215,229,0);
Color textBoxColor(250,250,250);
Color textBoxBorderColor(0,10,27);
Color circleArrowBlock(155,122,0);
Color arrowColor(24, 230, 40);
Color executeBlockColor(200, 40, 20);
short buttonHeight = 40;
bool isAChange = true;
bool listenKeys = false;
map<string, double> variables;
blockType currentBlock;

struct Button{
    Button(){}
    Button(char text_[100]){
        width = textwidth(text_);
        strcpy(text, text_);
    }
    char text[100];
    short width;
    Color color = reliseButtonColor;
    Color textColor = reliseTextButtonColor;
    int x1, x2, y1, y2;
    bool isMouseOver = false;
};

vector<Button> buttons;
Button generateBlockButton;

struct Block{
    blockType type;
    Block *leftNext;
    Block *rightNext;
    char text1[100];
    char text2[100];
    char text3[100];
    int xPoz, yPoz;
    int width, height = 35;
    Color color = blockColor;
    Color textColor = Color(12, 22, 33);
    bool isMouseOver(int xMouse, int yMouse){
        if(xMouse > maxLeft && xMouse < maxRight && yMouse > top && yMouse < bottom){
            if(getpixel(xMouse, yMouse) == 53652685 || getpixel(xMouse, yMouse) == 50390487){
                return true;
            }
            if(getpixel(xMouse, yMouse) == 53011992 && (getpixel(xMouse+2, yMouse+2) == 53652685 || getpixel(xMouse+2, yMouse+2) == 50390487)){
                return true;
            }
        }
        if(xMouse > xPoz - width/2 && xMouse < xPoz + width/2 && yMouse > yPoz - height/2 && yMouse < yPoz + height/2){
            return true;
        }
        return false;
    }
    int maxLeft, maxRight, top, bottom;
    int level_tree = -1;
    int code_line_index = 0;
    int index_leftNext = -1;
    int index_rightNext = -1;
};

Block *startBlock = new Block();
Block *stopBlock = new Block();
vector<Block*> blocks;

struct InputTextBox{
    InputTextBox(){}
    InputTextBox(int xPoz, int yPoz, int width_, int height_){
        x = xPoz;
        y = yPoz;
        width = width_;
        height = height_;
        strcpy(text, "");
    }
    int x, y;
    char text[100];
    int width, height;
    bool isSelected = false;
    bool isActive = false;
    bool isMouseOver(int xMouse, int yMouse){
        if(xMouse > x - width/2 && xMouse < x + width/2 && yMouse > y - height/2 && yMouse < y + height/2){
            return true;
        }
        return false;
    }
};
vector <InputTextBox> inputTextBoxes;

void drawArrow(int x1, int y1, int x2, int y2){
    setlinestyle(0, 0, 2);
    setcolor(COLOR(arrowColor.red, arrowColor.green, arrowColor.blue));
    line(x1,y1,(x1+x2)/2,y1);
    line((x1+x2)/2,y1,(x1+x2)/2,y2);
    line((x1+x2)/2,y2,x2,y2);
    setlinestyle(0, 0, 1);
}

double distanceBetween2Points(double x1, double y1, double x2, double y2){
     return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

void drawArrowhead(int x, int y){
    int size_a = 10;
    setlinestyle(0, 0, 2);
    setcolor(COLOR(arrowColor.red, arrowColor.green, arrowColor.blue));
    line(x, y, x - size_a, y - size_a);
    line(x, y, x + size_a, y - size_a);
}

void drawArrow2(){
    int dist = 20;
    int minDistForDetour = 100;
    setlinestyle(0, 0, 2);
    setcolor(COLOR(arrowColor.red, arrowColor.green, arrowColor.blue));
    for(int index = 0; index < blocks.size(); index++){
        if(blocks.at(index)->type == decision){
            if(blocks.at(index)->leftNext != NULL){ /// Trasarea sagetii din stanga pentru blocul de tip decizie
                if(blocks.at(index)->yPoz > blocks.at(index)->leftNext->yPoz){
                    bool allBlocksBetweenAreLeft = true;
                    int maxLeftPozXBlock = min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->maxLeft);
                    for(int index2 = 0; index2 < blocks.size(); index2++){
                        if(blocks.at(index2) != blocks.at(index) && blocks.at(index2) != blocks.at(index)->leftNext && abs(blocks.at(index)->leftNext->yPoz - blocks.at(index)->yPoz) > minDistForDetour){
                            if(blocks.at(index2)->bottom + 15 >= blocks.at(index)->leftNext->top && blocks.at(index2)->top <= blocks.at(index)->bottom){
                                maxLeftPozXBlock = min(maxLeftPozXBlock, blocks.at(index2)->maxLeft);
                                if(blocks.at(index2)->maxRight > min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->maxLeft)
                                   &&!(blocks.at(index2)->maxLeft > min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->maxLeft))
                                   ){
                                    allBlocksBetweenAreLeft = false;
                                }
                            }
                        }
                    }
                    if(allBlocksBetweenAreLeft){
                        maxLeftPozXBlock = min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->maxLeft);
                    }
                    line(maxLeftPozXBlock-dist, blocks.at(index)->bottom, blocks.at(index)->maxLeft, blocks.at(index)->bottom);
                    line(maxLeftPozXBlock-dist, blocks.at(index)->bottom, maxLeftPozXBlock-dist, blocks.at(index)->leftNext->top - 15);
                    line(maxLeftPozXBlock-dist, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15);
                    line(blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top);
                    drawArrowhead(blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top + 3);
                }
                else if(blocks.at(index)->yPoz <= blocks.at(index)->leftNext->yPoz){
                    bool allBlocksBetweenAreLeft = true;
                    int maxLeftPozXBlock = min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->xPoz+dist);
                    for(int index2 = 0; index2 < blocks.size(); index2++){
                        if(blocks.at(index2) != blocks.at(index) && blocks.at(index2) != blocks.at(index)->leftNext  && abs(blocks.at(index)->leftNext->yPoz - blocks.at(index)->yPoz) > minDistForDetour){
                            if(blocks.at(index2)->bottom + 15 <= blocks.at(index)->leftNext->bottom && blocks.at(index2)->bottom >= blocks.at(index)->bottom){
                                maxLeftPozXBlock = min(maxLeftPozXBlock, blocks.at(index2)->maxLeft);
                                if(blocks.at(index2)->maxRight > min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->maxLeft)
                                   &&!(blocks.at(index2)->maxLeft > min(blocks.at(index)->maxRight, blocks.at(index)->leftNext->maxRight))
                                   ){
                                    allBlocksBetweenAreLeft = false;
                                }
                            }
                        }
                    }
                    if(allBlocksBetweenAreLeft){
                        maxLeftPozXBlock = min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->xPoz+dist);
                    }
                    line(maxLeftPozXBlock-dist, blocks.at(index)->bottom, blocks.at(index)->maxLeft, blocks.at(index)->bottom);
                    line(maxLeftPozXBlock-dist, blocks.at(index)->bottom, maxLeftPozXBlock-dist, blocks.at(index)->leftNext->top - 15);
                    line(maxLeftPozXBlock-dist, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15);
                    line(blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top);
                    drawArrowhead(blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top + 3);
                }

            }
        }
        if(blocks.at(index)->type == decision){/// Trasarea sagetii din dreapta pentru blocul de tip decizie
            if(blocks.at(index)->rightNext != NULL){
                if(blocks.at(index)->yPoz > blocks.at(index)->rightNext->yPoz){
                    bool allBlocksBetweenAreRight = true;
                    int maxRightPozXBlock = max(blocks.at(index)->maxRight, blocks.at(index)->rightNext->maxRight);
                    for(int index2 = 0; index2 < blocks.size(); index2++){
                        if(blocks.at(index2) != blocks.at(index) && blocks.at(index2) != blocks.at(index)->rightNext  && abs(blocks.at(index)->rightNext->yPoz - blocks.at(index)->yPoz) > minDistForDetour){
                            if(blocks.at(index2)->bottom + 15 >= blocks.at(index)->rightNext->top && blocks.at(index2)->top <= blocks.at(index)->bottom){
                                maxRightPozXBlock = max(maxRightPozXBlock, blocks.at(index2)->maxRight);
                                if(blocks.at(index2)->maxLeft < max(blocks.at(index)->maxRight, blocks.at(index)->rightNext->maxRight)
                                   &&!(blocks.at(index2)->maxRight < max(blocks.at(index)->maxLeft, blocks.at(index)->rightNext->maxLeft))
                                   ){
                                    allBlocksBetweenAreRight = false;
                                }
                            }
                        }
                    }
                    if(allBlocksBetweenAreRight){
                        maxRightPozXBlock = max(blocks.at(index)->maxRight, blocks.at(index)->rightNext->maxRight);
                    }
                    line(maxRightPozXBlock+dist, blocks.at(index)->bottom, blocks.at(index)->maxRight, blocks.at(index)->bottom);
                    line(maxRightPozXBlock+dist, blocks.at(index)->bottom, maxRightPozXBlock+dist, blocks.at(index)->rightNext->top - 15);
                    line(maxRightPozXBlock+dist, blocks.at(index)->rightNext->top - 15, blocks.at(index)->rightNext->xPoz, blocks.at(index)->rightNext->top - 15);
                    line(blocks.at(index)->rightNext->xPoz, blocks.at(index)->rightNext->top - 15, blocks.at(index)->rightNext->xPoz, blocks.at(index)->rightNext->top);
                    drawArrowhead(blocks.at(index)->rightNext->xPoz, blocks.at(index)->rightNext->top + 3);
                }
                else if(blocks.at(index)->yPoz <= blocks.at(index)->rightNext->yPoz){
                    bool allBlocksBetweenAreRight = true;
                    int maxRightPozXBlock = max(blocks.at(index)->maxRight, blocks.at(index)->rightNext->xPoz-dist);
                    for(int index2 = 0; index2 < blocks.size(); index2++){
                        if(blocks.at(index2) != blocks.at(index) && blocks.at(index2) != blocks.at(index)->rightNext && abs(blocks.at(index)->rightNext->yPoz - blocks.at(index)->yPoz) > minDistForDetour){
                            if(blocks.at(index2)->top + 15 <= blocks.at(index)->rightNext->top && blocks.at(index2)->bottom >= blocks.at(index)->bottom){
                                maxRightPozXBlock = max(maxRightPozXBlock, blocks.at(index2)->maxRight);
                                if(blocks.at(index2)->maxLeft < max(blocks.at(index)->maxRight, blocks.at(index)->rightNext->maxRight)
                                   &&!(blocks.at(index2)->maxRight < max(blocks.at(index)->maxLeft, blocks.at(index)->rightNext->maxLeft))
                                   ){
                                    allBlocksBetweenAreRight = false;
                                }
                            }
                        }
                    }
                    if(allBlocksBetweenAreRight){
                        maxRightPozXBlock = max(blocks.at(index)->maxRight, blocks.at(index)->rightNext->xPoz-dist);
                    }
                    line(maxRightPozXBlock+dist, blocks.at(index)->bottom, blocks.at(index)->maxRight, blocks.at(index)->bottom);
                    line(maxRightPozXBlock+dist, blocks.at(index)->bottom, maxRightPozXBlock+dist, blocks.at(index)->rightNext->top - 15);
                    line(maxRightPozXBlock+dist, blocks.at(index)->rightNext->top - 15, blocks.at(index)->rightNext->xPoz, blocks.at(index)->rightNext->top - 15);
                    line(blocks.at(index)->rightNext->xPoz, blocks.at(index)->rightNext->top - 15, blocks.at(index)->rightNext->xPoz, blocks.at(index)->rightNext->top);
                    drawArrowhead(blocks.at(index)->rightNext->xPoz, blocks.at(index)->rightNext->top + 3);
                }
            }
        }
        if(blocks.at(index)->type != decision){
            int distanceLeft;
            int distanceRight;
            int maxLeftPozXBlock;
            int maxRightPozXBlock;
            if(blocks.at(index)->leftNext != NULL){
                maxLeftPozXBlock = min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->maxLeft);
                if(blocks.at(index)->bottom > blocks.at(index)->leftNext->top){
                    maxLeftPozXBlock = min(blocks.at(index)->maxLeft, blocks.at(index)->leftNext->maxLeft);
                    for(int index2 = 0; index2 < blocks.size(); index2++){
                        if(blocks.at(index2)->bottom + 15 >= blocks.at(index)->leftNext->top && blocks.at(index2)->top <= blocks.at(index)->bottom  && abs(blocks.at(index)->leftNext->yPoz - blocks.at(index)->yPoz) > minDistForDetour){
                            if(blocks.at(index) != blocks.at(index2)){
                                maxLeftPozXBlock = min(maxLeftPozXBlock, blocks.at(index2)->maxLeft);
                            }
                        }
                    }
                }
                else if(blocks.at(index)->bottom <= blocks.at(index)->leftNext->top){
                    maxLeftPozXBlock = min(blocks.at(index)->xPoz+dist, blocks.at(index)->leftNext->xPoz+dist);
                    for(int index2 = 0; index2 < blocks.size(); index2++){
                        if(blocks.at(index2)->bottom + 15 <= blocks.at(index)->leftNext->bottom && blocks.at(index2)->bottom >= blocks.at(index)->bottom && abs(blocks.at(index)->leftNext->yPoz - blocks.at(index)->yPoz) > minDistForDetour){
                            if(blocks.at(index) != blocks.at(index2)){
                                maxLeftPozXBlock = min(maxLeftPozXBlock, blocks.at(index2)->maxLeft);
                            }
                        }
                    }
                }
                distanceLeft = distanceBetween2Points(maxLeftPozXBlock-dist, blocks.at(index)->bottom, blocks.at(index)->xPoz, blocks.at(index)->bottom)+
                               distanceBetween2Points(maxLeftPozXBlock-dist, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15);

                if(blocks.at(index)->bottom > blocks.at(index)->leftNext->top){
                    maxRightPozXBlock = max(blocks.at(index)->maxLeft - dist, blocks.at(index)->leftNext->maxLeft);
                    for(int index2 = 0; index2 < blocks.size(); index2++){
                        if(blocks.at(index2)->bottom + 15 >= blocks.at(index)->leftNext->top && blocks.at(index2)->top <= blocks.at(index)->bottom && abs(blocks.at(index)->leftNext->yPoz - blocks.at(index)->yPoz) > minDistForDetour){
                            if(blocks.at(index) != blocks.at(index2)){
                                maxRightPozXBlock = max(maxRightPozXBlock, blocks.at(index2)->maxRight);
                            }
                        }
                    }
                }
                else if(blocks.at(index)->bottom <= blocks.at(index)->leftNext->top){
                    maxRightPozXBlock = max(blocks.at(index)->xPoz - dist, blocks.at(index)->leftNext->xPoz-dist);
                    for(int index2 = 0; index2 < blocks.size(); index2++){
                        if(blocks.at(index2)->top + 15 <= blocks.at(index)->leftNext->top && blocks.at(index2)->bottom >= blocks.at(index)->bottom && abs(blocks.at(index)->leftNext->yPoz - blocks.at(index)->yPoz) > minDistForDetour){
                            if(blocks.at(index) != blocks.at(index2)){
                                maxRightPozXBlock = max(maxRightPozXBlock, blocks.at(index2)->maxRight);
                            }
                        }
                    }
                }
                distanceRight = distanceBetween2Points(maxRightPozXBlock+dist, blocks.at(index)->bottom, blocks.at(index)->xPoz, blocks.at(index)->bottom)+
                                distanceBetween2Points(maxRightPozXBlock+dist, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15);

                if(distanceLeft < distanceRight){
                    line(blocks.at(index)->xPoz, blocks.at(index)->bottom, blocks.at(index)->xPoz, blocks.at(index)->bottom-10);
                    line(maxLeftPozXBlock-dist, blocks.at(index)->bottom, blocks.at(index)->xPoz, blocks.at(index)->bottom);
                    line(maxLeftPozXBlock-dist, blocks.at(index)->bottom, maxLeftPozXBlock-dist, blocks.at(index)->leftNext->top - 15);
                    line(maxLeftPozXBlock-dist, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15);
                    line(blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top);
                    drawArrowhead(blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top + 3);
                }
                else{
                    line(blocks.at(index)->xPoz, blocks.at(index)->bottom, blocks.at(index)->xPoz, blocks.at(index)->bottom-10);
                    line(maxRightPozXBlock+dist, blocks.at(index)->bottom, blocks.at(index)->xPoz, blocks.at(index)->bottom);
                    line(maxRightPozXBlock+dist, blocks.at(index)->bottom, maxRightPozXBlock+dist, blocks.at(index)->leftNext->top - 15);
                    line(maxRightPozXBlock+dist, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15);
                    line(blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top - 15, blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top);
                    drawArrowhead(blocks.at(index)->leftNext->xPoz, blocks.at(index)->leftNext->top + 3);
                }
            }
        }
    }
    setlinestyle(0, 0, 1);
}

void drawButton(Button button){
    setcolor(COLOR(button.textColor.red, button.textColor.green, button.textColor.blue));
    setbkcolor(COLOR(button.color.red, button.color.green, button.color.blue));
    setfillstyle(SOLID_FILL, COLOR(button.color.red, button.color.green, button.color.blue));
    if(button.isMouseOver){
        bar(button.x1 - 2, button.y1 - 2, button.x2 + 2 , button.y2 );
        outtextxy(button.x1 + 10, button.y1 , button.text);
    }
    else{
        bar(button.x1, button.y1, button.x2, button.y2);
        outtextxy(button.x1 + 10, button.y1 + 2, button.text);
    }
}

void drawButtons(){
    short xPozition = 10;
    short top = 10;
    for(short index = 0; index < 4; index++){ /// butoanele de pe prima linie
        buttons.at(index).width = textwidth(buttons.at(index).text);
        buttons.at(index).x1 = xPozition;
        buttons.at(index).y1 = top;
        buttons.at(index).x2 = xPozition + buttons.at(index).width + 20;
        buttons.at(index).y2 = top + buttonHeight;
        drawButton(buttons.at(index));
        xPozition += (buttons.at(index).width + 40);
    }
    drawButton(buttons.at(4)); /// butonul de generare

    xPozition = 10;
    top = WINDOW_HEIGHT - 50;
    for(int index = 5; index < buttons.size(); index++){
        buttons.at(index).width = textwidth(buttons.at(index).text);
        buttons.at(index).x1 = xPozition;
        buttons.at(index).y1 = top;
        buttons.at(index).x2 = xPozition + buttons.at(index).width + 20;
        buttons.at(index).y2 = top + buttonHeight;
        drawButton(buttons.at(index));
        xPozition += (buttons.at(index).width + 40);
    }
    setcolor(COLOR(lineColor.red, lineColor.green, lineColor.blue));
    line(0, 60, WINDOW_WIDTH, 60);
    line(0, 120, WINDOW_WIDTH, 120);
    setfillstyle(SOLID_FILL,  COLOR(backgroundColor.red + 30, backgroundColor.green + 30, backgroundColor.blue + 30));
    bar(20, 140, WINDOW_WIDTH - 20, WINDOW_HEIGHT - 80);
    line(0, WINDOW_HEIGHT - 60, WINDOW_WIDTH, WINDOW_HEIGHT - 60);
}

void initialization(){
    Beep(1500, 1);
    buttons.push_back(Button("Intrare"));
    buttons.push_back(Button("Iesire"));
    buttons.push_back(Button("Calcul"));
    buttons.push_back(Button("Decizie"));
    inputTextBoxes.push_back(InputTextBox(250, 90, 100, 35));
    inputTextBoxes.push_back(InputTextBox(370, 90, 100, 35));
    inputTextBoxes.push_back(InputTextBox(490, 90, 100, 35));
    settextstyle(8, 0, 4);
    strcpy(generateBlockButton.text, "Genereaza");
    generateBlockButton.width = textwidth("Genereaza") + 20;
    generateBlockButton.x1 = WINDOW_WIDTH - generateBlockButton.width - 80;
    generateBlockButton.x2 = generateBlockButton.x1 + generateBlockButton.width;
    generateBlockButton.y1 = 70;
    generateBlockButton.y2 = generateBlockButton.y1 + buttonHeight;
    buttons.push_back(generateBlockButton);

    buttons.push_back(Button("Salveaza"));
    buttons.push_back(Button("Deschide"));
    buttons.push_back(Button("Vezi codul C++"));
    buttons.push_back(Button("Executare"));

    strcpy(startBlock->text1, "START");
    startBlock->type = start;
    startBlock->xPoz = WINDOW_WIDTH/2;
    startBlock->yPoz = 170;
    startBlock->width = textwidth(startBlock->text1);
    blocks.push_back(startBlock);

    strcpy(stopBlock->text1, "STOP");
    stopBlock->type = stop;
    stopBlock->xPoz = WINDOW_WIDTH/2;
    stopBlock->yPoz = WINDOW_HEIGHT - 110;
    stopBlock->width = textwidth(stopBlock->text1);
    blocks.push_back(stopBlock);
}

void drawInputBox(InputTextBox textBox){
    if(textBox.isSelected){
        setfillstyle(SOLID_FILL, COLOR(textBoxColor.red, textBoxColor.green, textBoxColor.blue));
        setbkcolor(COLOR(textBoxColor.red, textBoxColor.green, textBoxColor.blue));
    }
    else{
        setfillstyle(SOLID_FILL, COLOR(textBoxColor.red-50, textBoxColor.green-50, textBoxColor.blue-50));
        setbkcolor(COLOR(textBoxColor.red-50, textBoxColor.green-50, textBoxColor.blue-50));
    }
    bar(textBox.x - textBox.width/2-2, textBox.y - textBox.height/2, textBox.x + textBox.width/2, textBox.y + textBox.height/2);
    outtextxy(textBox.x - textBox.width/2, textBox.y - textheight(textBox.text)/2, textBox.text + max(0, (int)strlen(textBox.text) - 5));
}

double getValueForVariable(char *variableName){
    int inputVariableWindow = initwindow(320, 110, "Valoare variabilei", 300, 300);
    setcurrentwindow(inputVariableWindow);
    setbkcolor(COLOR(backgroundColor.red, backgroundColor.green, backgroundColor.blue));
    cleardevice();
    settextstyle(8, 0, 2);
    InputTextBox textBox(textwidth(variableName) + 68, 52, 62, 30);
    textBox.isSelected = true;
    drawInputBox(textBox);

    Button submitButton("OK");
    submitButton.x1 = textwidth(variableName) + 130;
    submitButton.y1 = 37;
    submitButton.x2 = textwidth(variableName) + 180;
    submitButton.y2 = 67;
    drawButton(submitButton);
    clearmouseclick(WM_LBUTTONDOWN);
    for(ever){
        if(mousex() > submitButton.x1 && mousex() < submitButton.x2 && mousey() > submitButton.y1 && mousey() < submitButton.y2){
            submitButton.isMouseOver = true;
            submitButton.color = mouseOverButtonColor;
            submitButton.textColor = mouseOverTextButtonColor;
            if(ismouseclick(WM_LBUTTONDOWN)){
                clearmouseclick(WM_LBUTTONDOWN);
                closegraph(inputVariableWindow);
                setcurrentwindow(principalWindow);
                return atof(textBox.text);
            }
        }else{
            clearmouseclick(WM_LBUTTONDOWN);
            submitButton.isMouseOver = false;
            submitButton.color = reliseButtonColor;
            submitButton.textColor = reliseTextButtonColor;
        }
        if(kbhit()){
            char character = (char) getch();
            if(character >= '0' && character <= '9' || character == '-' || character == '.'){
                textBox.text[strlen(textBox.text) + 1] = '\0';
                textBox.text[strlen(textBox.text)] = character;
            }else if(character == 8){
                textBox.text[strlen(textBox.text)-1] = '\0';
            }
        }
        setbkcolor(COLOR(backgroundColor.red, backgroundColor.green, backgroundColor.blue));
        cleardevice();
        setcolor(WHITE);
        outtextxy(10, 10, "Dati valoarea variabilei");
        outtextxy(10, 40, variableName);
        setcolor(BLACK);
        drawInputBox(textBox);
        drawButton(submitButton);
        swapbuffers();

    }
}

void drawConstructionBlock(blockType type){
    inputTextBoxes.at(0).isActive = true;
    inputTextBoxes.at(1).isActive = false;
    inputTextBoxes.at(2).isActive = false;
    switch(type)
    {
    case input:
        setcolor(COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
        setlinestyle(0, 0, 3);
        line(25, 70, 175, 70);
        line(50, 110, 150, 110);
        line(25, 70, 50, 110);
        line(175, 70, 150, 110);
        setfillstyle(SOLID_FILL, COLOR(blockColor.red, blockColor.green, blockColor.blue));
        floodfill(53, 73, COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
        drawInputBox(inputTextBoxes.at(0));
        break;
    case output:
        setcolor(COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
        setlinestyle(0, 0, 3);
        line(50, 70, 150, 70);
        line(25, 110, 175, 110);
        line(50, 70, 25, 110);
        line(150, 70, 175, 110);
        setfillstyle(SOLID_FILL, COLOR(blockColor.red, blockColor.green, blockColor.blue));
        floodfill(53, 73, COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
        drawInputBox(inputTextBoxes.at(0));
        break;
    case calculation:
        setcolor(COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
        setlinestyle(0, 0, 3);
        line(25, 70, 175, 70);
        line(25, 110, 175, 110);
        line(25, 70, 25, 110);
        line(175, 70, 175, 110);
        setfillstyle(SOLID_FILL, COLOR(blockColor.red, blockColor.green, blockColor.blue));
        floodfill(30, 75, COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
        inputTextBoxes.at(1).isActive = true;
        drawInputBox(inputTextBoxes.at(0));
        drawInputBox(inputTextBoxes.at(1));
        break;
    case decision:
        setcolor(COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
        setlinestyle(0, 0, 3);
        line(25, 110, 100, 70);
        line(100, 70, 175, 110);
        line(25, 110, 175, 110);
        setfillstyle(SOLID_FILL, COLOR(blockColor.red, blockColor.green, blockColor.blue));
        floodfill(50, 100, COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
        inputTextBoxes.at(1).isActive = true;
        inputTextBoxes.at(2).isActive = true;
        drawInputBox(inputTextBoxes.at(0));
        drawInputBox(inputTextBoxes.at(1));
        drawInputBox(inputTextBoxes.at(2));
        break;
    }
    setlinestyle(0, 0, 1);
}

void uncheckAllTextBoxes(){
    for(int indexTextBox = 0; indexTextBox < inputTextBoxes.size(); indexTextBox++){
        inputTextBoxes.at(indexTextBox).isSelected = false;
    }
}

void clearTextTextBoxes(){
    for(int indexTextBox = 0; indexTextBox < inputTextBoxes.size(); indexTextBox++){
        strcpy(inputTextBoxes.at(indexTextBox).text, "");
    }
}

void drawBlocks(){
    for(int index = 0; index < blocks.size(); index++){
        switch(blocks.at(index)->type){
            case start:
                setcolor(COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                circle(blocks.at(index)->xPoz - blocks.at(index)->width/2, blocks.at(index)->yPoz,  blocks.at(index)->height/2);
                circle(blocks.at(index)->xPoz + blocks.at(index)->width/2, blocks.at(index)->yPoz, blocks.at(index)->height/2);
                floodfill(blocks.at(index)->xPoz - blocks.at(index)->width/2 + 1, blocks.at(index)->yPoz, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                floodfill(blocks.at(index)->xPoz + blocks.at(index)->width/2 - 1, blocks.at(index)->yPoz, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));

                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                setlinestyle(0,1,2);
                circle(blocks.at(index)->xPoz - blocks.at(index)->width/2, blocks.at(index)->yPoz,  blocks.at(index)->height/2);
                circle(blocks.at(index)->xPoz + blocks.at(index)->width/2, blocks.at(index)->yPoz, blocks.at(index)->height/2);
                setlinestyle(0,1,4);
                line(blocks.at(index)->xPoz - blocks.at(index)->width/2,blocks.at(index)->yPoz - blocks.at(index)->height/2,blocks.at(index)->xPoz + blocks.at(index)->width/2,blocks.at(index)->yPoz - blocks.at(index)->height/2);
                line(blocks.at(index)->xPoz - blocks.at(index)->width/2,blocks.at(index)->yPoz + blocks.at(index)->height/2,blocks.at(index)->xPoz + blocks.at(index)->width/2,blocks.at(index)->yPoz + blocks.at(index)->height/2);
                setlinestyle(0,1,1);

                bar(blocks.at(index)->xPoz - blocks.at(index)->width/2, blocks.at(index)->yPoz - blocks.at(index)->height/2, blocks.at(index)->xPoz + blocks.at(index)->width/2, blocks.at(index)->yPoz + blocks.at(index)->height/2);
                setbkcolor(COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blocks.at(index)->textColor.red, blocks.at(index)->textColor.green, blocks.at(index)->textColor.blue));
                outtextxy(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2, blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2, blocks.at(index)->text1);

                setcolor(COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                setfillstyle(SOLID_FILL, COLOR(circleArrowBlock.red + 50, circleArrowBlock.green + 50, circleArrowBlock.blue + 50));
                circle(blocks.at(index)->xPoz, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, 5);
                floodfill(blocks.at(index)->xPoz, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                break;

            case stop:
                setcolor(COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                circle(blocks.at(index)->xPoz - blocks.at(index)->width/2, blocks.at(index)->yPoz,  blocks.at(index)->height/2);
                circle(blocks.at(index)->xPoz + blocks.at(index)->width/2, blocks.at(index)->yPoz, blocks.at(index)->height/2);
                floodfill(blocks.at(index)->xPoz - blocks.at(index)->width/2 + 1, blocks.at(index)->yPoz, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                floodfill(blocks.at(index)->xPoz + blocks.at(index)->width/2 - 1, blocks.at(index)->yPoz, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));

                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                setlinestyle(0,1,2);
                circle(blocks.at(index)->xPoz - blocks.at(index)->width/2 -1, blocks.at(index)->yPoz,  blocks.at(index)->height/2+1);
                circle(blocks.at(index)->xPoz + blocks.at(index)->width/2 +1, blocks.at(index)->yPoz, blocks.at(index)->height/2+1);
                setlinestyle(0,1,4);
                line(blocks.at(index)->xPoz - blocks.at(index)->width/2,blocks.at(index)->yPoz - blocks.at(index)->height/2,blocks.at(index)->xPoz + blocks.at(index)->width/2,blocks.at(index)->yPoz - blocks.at(index)->height/2);
                line(blocks.at(index)->xPoz - blocks.at(index)->width/2,blocks.at(index)->yPoz + blocks.at(index)->height/2,blocks.at(index)->xPoz + blocks.at(index)->width/2,blocks.at(index)->yPoz + blocks.at(index)->height/2);
                setlinestyle(0,1,1);

                bar(blocks.at(index)->xPoz - blocks.at(index)->width/2, blocks.at(index)->yPoz - blocks.at(index)->height/2, blocks.at(index)->xPoz + blocks.at(index)->width/2, blocks.at(index)->yPoz + blocks.at(index)->height/2);
                setbkcolor(COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blocks.at(index)->textColor.red, blocks.at(index)->textColor.green, blocks.at(index)->textColor.blue));
                outtextxy(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2, blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2, blocks.at(index)->text1);
                break;

            case input:
                setlinestyle(0,1,3);
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                line(blocks.at(index)->xPoz - 10 - textwidth(blocks.at(index)->text1)/2 - textheight(blocks.at(index)->text1),blocks.at(index)->yPoz - 1 -textheight(blocks.at(index)->text1)/2,blocks.at(index)->xPoz + 10 + textwidth(blocks.at(index)->text1)/2 + textheight(blocks.at(index)->text1),blocks.at(index)->yPoz - 1 -textheight(blocks.at(index)->text1)/2);
                line(blocks.at(index)->xPoz - 10 - textwidth(blocks.at(index)->text1)/2 - textheight(blocks.at(index)->text1), blocks.at(index)->yPoz - 1 -textheight(blocks.at(index)->text1)/2, blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 -10,blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 -1);
                line(blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 +10,blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 -1, blocks.at(index)->xPoz +10+ textwidth(blocks.at(index)->text1)/2 + textheight(blocks.at(index)->text1),blocks.at(index)->yPoz - 1-textheight(blocks.at(index)->text1)/2);
                line(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 -10,blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 -1, blocks.at(index)->xPoz +10+ textwidth(blocks.at(index)->text1)/2,blocks.at(index)->yPoz +textheight(blocks.at(index)->text1)/2);
                floodfill(blocks.at(index)->xPoz, blocks.at(index)->yPoz, COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
                setlinestyle(0,1,1);

                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                bar(blocks.at(index)->xPoz - blocks.at(index)->width/2 - 10, blocks.at(index)->yPoz - blocks.at(index)->height/2, blocks.at(index)->xPoz + blocks.at(index)->width/2+10, blocks.at(index)->yPoz + blocks.at(index)->height/2);
                setbkcolor(COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blocks.at(index)->textColor.red, blocks.at(index)->textColor.green, blocks.at(index)->textColor.blue));
                outtextxy(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2, blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2, blocks.at(index)->text1);

                setlinestyle(0,1,3);
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                line(blocks.at(index)->xPoz - 10 - textwidth(blocks.at(index)->text1)/2 - textheight(blocks.at(index)->text1),blocks.at(index)->yPoz - 1 -textheight(blocks.at(index)->text1)/2,blocks.at(index)->xPoz + 10 + textwidth(blocks.at(index)->text1)/2 + textheight(blocks.at(index)->text1),blocks.at(index)->yPoz - 1 -textheight(blocks.at(index)->text1)/2);
                line(blocks.at(index)->xPoz - 10 - textwidth(blocks.at(index)->text1)/2 - textheight(blocks.at(index)->text1), blocks.at(index)->yPoz - 1 -textheight(blocks.at(index)->text1)/2, blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 -10,blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 -1);
                line(blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 +10,blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 -1, blocks.at(index)->xPoz +10+ textwidth(blocks.at(index)->text1)/2 + textheight(blocks.at(index)->text1),blocks.at(index)->yPoz - 1-textheight(blocks.at(index)->text1)/2);
                line(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 -10,blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 -1, blocks.at(index)->xPoz +10+ textwidth(blocks.at(index)->text1)/2,blocks.at(index)->yPoz +textheight(blocks.at(index)->text1)/2);
                setlinestyle(0,1,1);

                setcolor(COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                setfillstyle(SOLID_FILL, COLOR(circleArrowBlock.red + 50, circleArrowBlock.green + 50, circleArrowBlock.blue + 50));
                circle(blocks.at(index)->xPoz, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, 5);
                floodfill(blocks.at(index)->xPoz, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                break;

            case output:
                setlinestyle(0,1,3);
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                line(blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 + 10 + textheight(blocks.at(index)->text1),blocks.at(index)->yPoz +textheight(blocks.at(index)->text1)/2,blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 + 10,blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2 - 1);
                line(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 - 10,blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2,blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 - 10 - textheight(blocks.at(index)->text1) - 1,blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 );
                line(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 - 10, blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2 -1, blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 + 10, blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2 -1);
                line(blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 + 10 + textheight(blocks.at(index)->text1), blocks.at(index)->yPoz +textheight(blocks.at(index)->text1)/2, blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 - 10 - textheight(blocks.at(index)->text1), blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 +1);
                setlinestyle(0, 1, 1);
                floodfill(blocks.at(index)->xPoz,  blocks.at(index)->yPoz,  COLOR(blockBordreColor.red,  blockBordreColor.green,  blockBordreColor.blue));
                setbkcolor(COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blocks.at(index)->textColor.red, blocks.at(index)->textColor.green, blocks.at(index)->textColor.blue));
                outtextxy(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2, blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2, blocks.at(index)->text1);

                setlinestyle(0,1,3);
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                line(blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 + 10 + textheight(blocks.at(index)->text1),blocks.at(index)->yPoz +textheight(blocks.at(index)->text1)/2,blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 + 10,blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2 - 1);
                line(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 - 10,blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2,blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 - 10 - textheight(blocks.at(index)->text1) - 1,blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 );
                line(blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 - 10, blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2 -1, blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 + 10, blocks.at(index)->yPoz - textheight(blocks.at(index)->text1)/2 -1);
                line(blocks.at(index)->xPoz + textwidth(blocks.at(index)->text1)/2 + 10 + textheight(blocks.at(index)->text1), blocks.at(index)->yPoz +textheight(blocks.at(index)->text1)/2, blocks.at(index)->xPoz - textwidth(blocks.at(index)->text1)/2 - 10 - textheight(blocks.at(index)->text1), blocks.at(index)->yPoz + textheight(blocks.at(index)->text1)/2 +1);
                setlinestyle(0, 1, 1);

                setcolor(COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                setfillstyle(SOLID_FILL, COLOR(circleArrowBlock.red + 50, circleArrowBlock.green + 50, circleArrowBlock.blue + 50));
                circle(blocks.at(index)->xPoz, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, 5);
                floodfill(blocks.at(index)->xPoz, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                break;

            case calculation:
                char finalText[100];
                strcpy(finalText, blocks.at(index)->text1);
                strcat(finalText, "<-");
                strcat(finalText, blocks.at(index)->text2);
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                bar(blocks.at(index)->xPoz - blocks.at(index)->width/2 - 10, blocks.at(index)->yPoz - blocks.at(index)->height/2, blocks.at(index)->xPoz + blocks.at(index)->width/2+10, blocks.at(index)->yPoz + blocks.at(index)->height/2);
                setbkcolor(COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blocks.at(index)->textColor.red, blocks.at(index)->textColor.green, blocks.at(index)->textColor.blue));
                outtextxy(blocks.at(index)->xPoz - textwidth(finalText)/2, blocks.at(index)->yPoz - textheight(finalText)/2, finalText);

                setlinestyle(0,1,3);
                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                line(blocks.at(index)->xPoz - textwidth(finalText)/2, blocks.at(index)->yPoz - textheight(finalText)/2,blocks.at(index)->xPoz + textwidth(finalText)/2, blocks.at(index)->yPoz - textheight(finalText)/2);
                line(blocks.at(index)->xPoz - textwidth(finalText)/2, blocks.at(index)->yPoz + textheight(finalText)/2,blocks.at(index)->xPoz + textwidth(finalText)/2, blocks.at(index)->yPoz + textheight(finalText)/2);
                line(blocks.at(index)->xPoz + textwidth(finalText)/2, blocks.at(index)->yPoz - textheight(finalText)/2,blocks.at(index)->xPoz + textwidth(finalText)/2, blocks.at(index)->yPoz + textheight(finalText)/2);
                line(blocks.at(index)->xPoz - textwidth(finalText)/2, blocks.at(index)->yPoz - textheight(finalText)/2,blocks.at(index)->xPoz - textwidth(finalText)/2, blocks.at(index)->yPoz + textheight(finalText)/2);
                setlinestyle(0,1,1);

                setcolor(COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                setfillstyle(SOLID_FILL, COLOR(circleArrowBlock.red + 50, circleArrowBlock.green + 50, circleArrowBlock.blue + 50));
                circle(blocks.at(index)->xPoz, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, 5);
                floodfill(blocks.at(index)->xPoz, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                break;

            case decision:
                strcpy(finalText, blocks.at(index)->text1);
                strcat(finalText, blocks.at(index)->text2);
                strcat(finalText, blocks.at(index)->text3);

                setlinestyle(0,1,3);
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                line(blocks.at(index)->xPoz, blocks.at(index)->yPoz - textheight(finalText)/2 - textheight(finalText), blocks.at(index)->xPoz + textwidth(finalText)/2 +textwidth(finalText) +10, blocks.at(index)->yPoz + textheight(finalText)/2);
                line(blocks.at(index)->xPoz, blocks.at(index)->yPoz - textheight(finalText)/2 - textheight(finalText), blocks.at(index)->xPoz - textwidth(finalText)/2 -textwidth(finalText) -10, blocks.at(index)->yPoz + textheight(finalText)/2);
                line( blocks.at(index)->xPoz + textwidth(finalText)/2 +textwidth(finalText) +10, blocks.at(index)->yPoz + textheight(finalText)/2,blocks.at(index)->xPoz - textwidth(finalText)/2 -textwidth(finalText) -10, blocks.at(index)->yPoz + textheight(finalText)/2);
                floodfill(blocks.at(index)->xPoz, blocks.at(index)->yPoz, COLOR(blockBordreColor.red, blockBordreColor.green, blockBordreColor.blue));
                setlinestyle(0,1,1);

                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                bar(blocks.at(index)->xPoz - blocks.at(index)->width/2 - 10, blocks.at(index)->yPoz - blocks.at(index)->height/2, blocks.at(index)->xPoz + blocks.at(index)->width/2+10, blocks.at(index)->yPoz + blocks.at(index)->height/2);
                setbkcolor(COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blocks.at(index)->textColor.red, blocks.at(index)->textColor.green, blocks.at(index)->textColor.blue));
                outtextxy(blocks.at(index)->xPoz - textwidth(finalText)/2, blocks.at(index)->yPoz - textheight(finalText)/2, finalText);

                setlinestyle(0,1,3);
                setfillstyle(SOLID_FILL, COLOR(blocks.at(index)->color.red, blocks.at(index)->color.green, blocks.at(index)->color.blue));
                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                line(blocks.at(index)->xPoz, blocks.at(index)->yPoz - textheight(finalText)/2 - textheight(finalText), blocks.at(index)->xPoz + textwidth(finalText)/2 +textwidth(finalText) +10, blocks.at(index)->yPoz + textheight(finalText)/2);
                line(blocks.at(index)->xPoz, blocks.at(index)->yPoz - textheight(finalText)/2 - textheight(finalText), blocks.at(index)->xPoz - textwidth(finalText)/2 -textwidth(finalText) -10, blocks.at(index)->yPoz + textheight(finalText)/2);
                line( blocks.at(index)->xPoz + textwidth(finalText)/2 +textwidth(finalText) +10, blocks.at(index)->yPoz + textheight(finalText)/2,blocks.at(index)->xPoz - textwidth(finalText)/2 -textwidth(finalText) -10, blocks.at(index)->yPoz + textheight(finalText)/2);
                setlinestyle(0,1,1);

                setcolor(COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                setfillstyle(SOLID_FILL, COLOR(circleArrowBlock.red + 50, circleArrowBlock.green + 50, circleArrowBlock.blue + 50));
                circle(blocks.at(index)->xPoz - 3*blocks.at(index)->width/2 + 15, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, 5);
                floodfill(blocks.at(index)->xPoz - 3*blocks.at(index)->width/2 + 15, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));
                circle(blocks.at(index)->xPoz + 3*blocks.at(index)->width/2 - 15, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, 5);
                floodfill(blocks.at(index)->xPoz + 3*blocks.at(index)->width/2 - 15, blocks.at(index)->yPoz + blocks.at(index)->height/2 - 5, COLOR(circleArrowBlock.red, circleArrowBlock.green, circleArrowBlock.blue));

                setcolor(COLOR(blockBordreColor.red,blockBordreColor.green,blockBordreColor.blue));
                setbkcolor(COLOR(backgroundColor.red + 30, backgroundColor.green + 30, backgroundColor.blue + 30));
                outtextxy(blocks.at(index)->maxLeft-10, blocks.at(index)->bottom - 40, "A");
                outtextxy(blocks.at(index)->maxRight-5, blocks.at(index)->bottom - 40, "F");
                break;
        }
    }
    drawArrow2();
}

void generateBlock(blockType generateBlockType, char textBox1[100], char textBox2[100], char textBox3[100], int x, int y, int index_leftNext, int index_rightNext){
    Block *block = new Block();
    strcpy(block->text1, textBox1);
    strcpy(block->text2, textBox2);
    strcpy(block->text3, textBox3);
    block->type = generateBlockType;
    block->xPoz = x;
    block->yPoz = y;
    block->index_leftNext = index_leftNext;
    block->index_rightNext = index_rightNext;
    block->width = textwidth(textBox1) + textwidth(textBox2) + textwidth(textBox3);
    if(block->type == calculation){
        block->width += 20;
    }

    block->bottom = block->yPoz+block->height/2;
    block->top = block->yPoz-block->height/2;

    if(block->type == calculation){
        block->maxLeft = block->xPoz - block->width/2;
        block->maxRight = block->xPoz + block->width/2;
    }
    if(block->type == start || block->type == stop){
        block->maxLeft = block->xPoz - block->width/2 - block->height;
        block->maxRight = block->xPoz + block->width/2 + block->height;
    }
    if(block->type == input || block->type == output){
        block->maxLeft = block->xPoz - block->width/2 - 10 - textheight(block->text1);
        block->maxRight = block->xPoz + block->width/2 + 10 + textheight(block->text1);
    }
    if(block->type == decision){
        block->top = block->yPoz - 3*textheight(block->text1)/2;
        block->maxLeft = block->xPoz - 3*block->width/2 - 10;
        block->maxRight = block->xPoz + 3*block->width/2 + 10;
    }
    blocks.push_back(block);
}

bool cmpByStringLength(pair<string, double>& a, pair<string, double>& b){
    return a.first.length() > b.first.length();
}
string outputString;

void executeLogicalScheme(struct Block *root){
    root->color = executeBlockColor;
    vector<pair<string, double>> variablesSortedByLength;
    map<string, double>::iterator itr;
    for(itr = variables.begin(); itr != variables.end(); itr++){
        variablesSortedByLength.push_back(make_pair(itr->first, itr->second));
    }
    sort(variablesSortedByLength.begin(), variablesSortedByLength.end(), cmpByStringLength);
    switch(root->type){
        case start:
        executeLogicalScheme(root->leftNext);
        break;
        case stop:
            return;
        case input:
            variables.find(root->text1)->second = getValueForVariable(root->text1);
            executeLogicalScheme(root->leftNext);
            break;
        case output:
            if(root->text1[0] == '\"'){
                outputString += root->text1;
                outputString += "\n";
                cout << "\n\tSe afiseaza textul " << root->text1;
            }
            else{
                outputString += root->text1 ;
                outputString += " = " + to_string(variables.find(root->text1)->second);
                outputString += "\n";
                cout << "\n\tValoarea variabilei " << root->text1 << " este " << variables.find(root->text1)->second;
            }
            executeLogicalScheme(root->leftNext);
            break;
        case calculation:{
            string expression = root->text2;
            for(int index = 0; index < variablesSortedByLength.size(); index++){
                expression = regex_replace(expression, regex(variablesSortedByLength.at(index).first), (char*)to_string(variablesSortedByLength.at(index).second).data());
            }

            variables.find(root->text1)->second = te_interp((char*)expression.data(), 0);
            cout <<"\n\t!!!-- " <<expression << " ----- "<< variables.find(root->text1)->second << '\n';
            executeLogicalScheme(root->leftNext);
            break;
            }
        case decision:
            string expression1 = root->text1;
            string expression2 = root->text3;
                for(int index = 0; index < variablesSortedByLength.size(); index++){
                    expression1 = regex_replace(expression1, regex(variablesSortedByLength.at(index).first), (char*)to_string(variablesSortedByLength.at(index).second).data());
                    expression2 = regex_replace(expression2, regex(variablesSortedByLength.at(index).first), (char*)to_string(variablesSortedByLength.at(index).second).data());
                }
                double valText1 = te_interp((char*)expression1.data(), 0);
                double valText2 = te_interp((char*)expression2.data(), 0);
                cout <<"\n\t!!!!!!!"<<(char*)expression1.data() << " => " << valText1 << "    " <<(char*)expression2.data() << " => " << valText2 << '\n';
            if(strcmp(root->text2, "==") == 0){
                if(valText1 == valText2){
                    executeLogicalScheme(root->leftNext);
                }
                else{
                    executeLogicalScheme(root->rightNext);
                }
            }
            else if(strcmp(root->text2, "<=") == 0){
                if(valText1 <= valText2){
                    executeLogicalScheme(root->leftNext);
                }
                else{
                    executeLogicalScheme(root->rightNext);
                }
            }
            else if(strcmp(root->text2, ">=") == 0){
                if(valText1 >= valText2){
                    executeLogicalScheme(root->leftNext);
                }
                else{
                    executeLogicalScheme(root->rightNext);
                }
            }
            else if(strcmp(root->text2, "<") == 0){
                if(valText1 < valText2){
                    executeLogicalScheme(root->leftNext);
                }
                else{
                    executeLogicalScheme(root->rightNext);
                }
            }
            else if(strcmp(root->text2, ">") == 0){
                if(valText1 > valText2){
                    executeLogicalScheme(root->leftNext);
                }
                else{
                    executeLogicalScheme(root->rightNext);
                }
            }

            break;
    }
}

HWND hMainWindow;
void saveFile(vector<Block*> blocks1, HWND hWnd){
    for(int index = 0; index < blocks1.size(); index++){
        if(blocks1.at(index)->leftNext != NULL){
            for(int index2 = 0; index2 < blocks1.size(); index2++){
                if(blocks1.at(index2) == blocks1.at(index)->leftNext){
                    blocks1.at(index)->index_leftNext = index2;
                }
            }
        }
        if(blocks1.at(index)->rightNext != NULL){
            for(int index2 = 0; index2 < blocks1.size(); index2++){
                if(blocks1.at(index2) == blocks1.at(index)->rightNext){
                    blocks1.at(index)->index_rightNext = index2;
                }
            }
        }
    }
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    char fileName[100];
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 1000;
    ofn.lpstrFilter = "Schema Logica\0*.SCH\0";
    ofn.nFilterIndex = 1;
    GetSaveFileName(&ofn);
    strcat(ofn.lpstrFile, ".SCH");
    Block* arr[100];
    for (int index = 0; index < blocks1.size(); index++)
        arr[index] = blocks1[index];
    ofstream fout(ofn.lpstrFile);
    for (int index = 0; index < blocks1.size(); index++)
        fout<<arr[index]->type<<' '<<arr[index]->text1<<' '<<arr[index]->text2<<' '<<arr[index]->text3<<' '<<arr[index]->xPoz<<' '<<arr[index]->yPoz<<' '<<arr[index]->index_leftNext<<' '<<arr[index]->index_rightNext<<'\n';
    fout.close();
}

void openFile(HWND hWnd){
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    char fileName[100];
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 2000;
    ofn.lpstrFilter = "Schema Logica\0*.SCH\0";
    ofn.nFilterIndex = 1;
    GetOpenFileName(&ofn);
    FILE *file;
    file = fopen(ofn.lpstrFile,"r");
    fseek(file,0,SEEK_END);
    long long _size = ftell(file);
    rewind(file);
    blocks.clear();
    char string_block[256];
    while(fgets(string_block, 256, file) != NULL){
        char text1[100] = {}, text2[100] = {}, text3[100] = {};
        int x, y;
        int index_leftNext, index_rightNext;
        if(string_block[0]-'0' == start or string_block[0]-'0' == stop or string_block[0]-'0' == input or string_block[0]-'0' == output){
                char* p;
                p = strtok(string_block+1, " ");
                strcpy(text1, p);
                p = strtok(NULL, " ");
                x = atoi(p);
                p = strtok(NULL, " ");
                y = atoi(p);
                p = strtok(NULL, " ");
                index_leftNext = atoi(p);
                p = strtok(NULL, " ");
                index_rightNext = atoi(p);
        }
        if(string_block[0]-'0' == calculation){
                char* p;
                p=strtok(string_block+1," ");
                strcpy(text1,p);
                p=strtok(NULL," ");
                strcpy(text2,p);
                p=strtok(NULL," ");
                x = atoi(p);
                p=strtok(NULL," ");
                y = atoi(p);
                p = strtok(NULL, " ");
                index_leftNext = atoi(p);
                p = strtok(NULL, " ");
                index_rightNext = atoi(p);
        }
        if(string_block[0]-'0' == decision){
                char* p;
                p=strtok(string_block+1," ");
                strcpy(text1,p);
                p=strtok(NULL," ");
                strcpy(text2,p);
                p=strtok(NULL," ");
                strcpy(text3,p);
                p=strtok(NULL," ");
                x = atoi(p);
                p=strtok(NULL," ");
                y = atoi(p);
                p = strtok(NULL, " ");
                index_leftNext = atoi(p);
                p = strtok(NULL, " ");
                index_rightNext = atoi(p);
        }
        generateBlock(blockType(string_block[0]-'0'), text1, text2, text3, x, y, index_leftNext, index_rightNext);
    }
    drawBlocks();
}


string code;
map<int, int> loops;
string variablesName = "    double ";

void addLoopsAndVariables(){
    map<int, string> codeLine;
    std::istringstream iss(code);
    int index = 0;
    for (std::string line; std::getline(iss, line); ){
        codeLine.insert(pair<int, string>(index++, line));
    }
    int loop_index = 0;
    map<int, int>::iterator itr;
    for (itr = loops.begin(); itr != loops.end(); ++itr) {
        int tabsBefore = count(codeLine.find(itr->first)->second.begin(), codeLine.find(itr->first)->second.end(), ' ');
        if(codeLine.find(itr->first)->second.find(" if(") != std::string::npos || codeLine.find(itr->first)->second.find(" else{") != std::string::npos){
            tabsBefore += 4;
        }
        codeLine.find(itr->first)->second += "\n";
        for(int index_t = 0; index_t < tabsBefore/4 -  1; index_t++){
            codeLine.find(itr->first)->second += "    ";
        }
        codeLine.find(itr->first)->second += "    goto loop" + to_string(loop_index) + ";";
        tabsBefore = count(codeLine.find(itr->second)->second.begin(), codeLine.find(itr->second)->second.end(), ' ');
        string aux;
        for(int index_t = 0; index_t < tabsBefore/4 - 1; index_t++){
            aux += "    ";
        }
        aux = "    loop" + to_string(loop_index++) + ":\n" + codeLine.find(itr->second)->second;
        codeLine.find(itr->second)->second = aux;
    }
    variablesName[variablesName.size() - 2] = ';';
    codeLine.find(5)->second = variablesName;
    variablesName = "    double ";
    code = "";
    map<int, string>::iterator itr3;
    for(itr3 = codeLine.begin(); itr3 != codeLine.end(); ++itr3){
        code.append(itr3->second + "\n");
    }
    loops.clear();
}

void transformLogicalSchemeInCode(struct Block* root, int tabs){
    string tabs_s = "";;
    for(int itr = 0; itr < tabs; itr++){
        tabs_s += "    ";
    }
    switch(root->type){
        case start:
            code.append("#include<iostream>\n#include<cmath>\n\nusing namespace std;\nint main(){\n");
            root->level_tree = 0;
            if(root->leftNext->level_tree == -1){
                root->leftNext->level_tree = root->level_tree + 1;
            }transformLogicalSchemeInCode(root->leftNext, tabs);
            break;
        case stop:
            code.append("\n");
            code.append(tabs_s);
            code.append("    return 0;");
            break;
        case input:{
            string var = " ";
            var.append(root->text1);
            var += ",";
            if (variablesName.find(var) == std::string::npos){
                variablesName.append(root->text1);
                variablesName += ", ";
            }
            code.append("\n");
            code.append(tabs_s);
            code.append("    cin>>");
            root->code_line_index = count(code.begin(), code.end(), '\n');
            code.append(root->text1);
            code.append(";");
            if(root->leftNext->level_tree == -1){
                root->leftNext->level_tree = root->level_tree + 1;
            }
            else if(root->leftNext->level_tree < root->level_tree){
                loops.insert(pair<int, int>(root->code_line_index, root->leftNext->code_line_index));
                break;
            }
            transformLogicalSchemeInCode(root->leftNext, tabs);
            break;
            }
        case output:
            code.append("\n");
            code.append(tabs_s);
            code.append("    cout<<");
            root->code_line_index = count(code.begin(), code.end(), '\n');
            code.append(root->text1);
            code.append("<<\"\\n\";");
            if(root->leftNext->level_tree == -1){
                root->leftNext->level_tree = root->level_tree + 1;
            }
            else if(root->leftNext->level_tree < root->level_tree){
                loops.insert(pair<int, int>(root->code_line_index, root->leftNext->code_line_index));
                break;
            }
            transformLogicalSchemeInCode(root->leftNext, tabs);
            break;
        case calculation:{
            string var = " ";
            var.append(root->text1);
            var += ",";
            if (variablesName.find(var) == std::string::npos){
                variablesName.append(root->text1);
                variablesName += ", ";
            }
            code.append("\n");
            code.append(tabs_s);
            code.append("    ");
            root->code_line_index = count(code.begin(), code.end(), '\n');
            code.append(root->text1);
            code.append("=");
            code.append(root->text2);
            code.append(";");
            if(root->leftNext->level_tree == -1){
                root->leftNext->level_tree = root->level_tree + 1;
            }
            else if(root->leftNext->level_tree < root->level_tree){
                loops.insert(pair<int, int>(root->code_line_index, root->leftNext->code_line_index));
                break;
            }
            transformLogicalSchemeInCode(root->leftNext, tabs);
            break;
        }
        case decision:
            code.append("\n");
            code.append(tabs_s);
            code.append("    if(");
            root->code_line_index = count(code.begin(), code.end(), '\n');
            code.append(root->text1);
            code.append(root->text2);
            code.append(root->text3);
            code.append("){");
            if(root->leftNext->level_tree == -1){
                root->leftNext->level_tree = root->level_tree + 1;
                transformLogicalSchemeInCode(root->leftNext, tabs+1);
            }
            else if(root->leftNext->level_tree < root->level_tree){
                loops.insert(pair<int, int>(root->code_line_index, root->leftNext->code_line_index));
            }
            code.append("\n");
            code.append(tabs_s);
            code.append("    }");
            code.append("\n");
            code.append(tabs_s);
            code.append("    else{");
            if(root->rightNext->level_tree == -1){
                root->rightNext->level_tree = root->level_tree + 1;
                transformLogicalSchemeInCode(root->rightNext, tabs+1);
            }
            else if(root->rightNext->level_tree < root->level_tree){
                root->code_line_index = count(code.begin(), code.end(), '\n');
                loops.insert(pair<int, int>(root->code_line_index, root->rightNext->code_line_index));
            }
            else{
                transformLogicalSchemeInCode(root->rightNext, tabs+1);
            }
            code.append("\n");
            code.append(tabs_s);
            code.append("    }");
            break;
    }
}

void openOutpuWindow(string textOutput){
    int outputWindow = initwindow(420, count(textOutput.begin(), textOutput.end(), '\n') * textwidth("A") + 60, "", 850, 185);
    setcurrentwindow(outputWindow);
    setbkcolor(COLOR(backgroundColor.red, backgroundColor.green, backgroundColor.blue));
    cleardevice();
    settextstyle(8, 0, 2);
    istringstream iss(textOutput);
    int lineIndex = 0;
    for (string line; getline(iss, line); ){
        outtextxy(10, 30 + lineIndex * textheight("A"), (char*)line.data());
        lineIndex++;
    }
    getch();
    closegraph(outputWindow);
    setcurrentwindow(principalWindow);
}

void drawAll(){
    setbkcolor(COLOR(backgroundColor.red, backgroundColor.green, backgroundColor.blue));
    cleardevice();
    drawButtons();
    drawConstructionBlock(currentBlock);
    drawBlocks();
    swapbuffers();
}

void findExtremPoints(Block *block){
    block->bottom = block->yPoz+block->height/2+4;
    block->top = block->yPoz-block->height/2;
    if(block->type == calculation){
        block->maxLeft = block->xPoz - block->width/2-10;
        block->maxRight = block->xPoz + block->width/2+10;
    }
    if(block->type == start || block->type == stop){
        block->maxLeft = block->xPoz - block->width/2 - block->height/2;
        block->maxRight = block->xPoz + block->width/2 + block->height/2;
    }
    if(block->type == input || block->type == output){
        block->bottom = block->yPoz + textheight(block->text1)/2+5;
        block->top = block->yPoz - textheight(block->text1)/2;
        block->maxLeft = block->xPoz - block->width/2 - 10 - textheight(block->text1);
        block->maxRight = block->xPoz + block->width/2 + 10 + textheight(block->text1);
    }
    if(block->type == decision){
        block->top = block->yPoz - 3*textheight(block->text1)/2;
        block->maxLeft = block->xPoz - 3*block->width/2 - 10;
        block->maxRight = block->xPoz + 3*block->width/2 + 10;
    }
}

void appLoop(){
    findExtremPoints(startBlock);
    findExtremPoints(stopBlock);
    char character;
    int xMouse = mousex();
    int yMouse = mousey();
    for(ever){
        xMouse = mousex();
        yMouse = mousey();
        if(isAChange){
                drawAll();
                isAChange = false;
        }
        for(short index = 0; index < buttons.size(); index++){
            if(yMouse > 60  && yMouse < 120 && xMouse <710 && ismouseclick(WM_LBUTTONDOWN)){
                clearmouseclick(WM_LBUTTONDOWN);
                for(int indexTextBox = 0; indexTextBox < inputTextBoxes.size(); indexTextBox++){
                    if(inputTextBoxes.at(indexTextBox).isMouseOver(xMouse, yMouse) && !inputTextBoxes.at(indexTextBox).isSelected && inputTextBoxes.at(indexTextBox).isActive){
                        uncheckAllTextBoxes();
                        inputTextBoxes.at(indexTextBox).isSelected = true;
                        listenKeys = true;
                        cout << "A fost apasata casuta " << indexTextBox + 1 << " \n";
                        isAChange = true;
                    }
                }
            }
            if(buttons.at(index).isMouseOver == false && xMouse > buttons.at(index).x1 && xMouse < buttons.at(index).x2 && yMouse > buttons.at(index).y1 && yMouse < buttons.at(index).y2){
                clearmouseclick(WM_LBUTTONDOWN);
                buttons.at(index).color = mouseOverButtonColor;
                buttons.at(index).textColor = mouseOverTextButtonColor;
                buttons.at(index).isMouseOver = true;
                isAChange = true;
            }
            else if(buttons.at(index).isMouseOver == true && (xMouse < buttons.at(index).x1 || xMouse > buttons.at(index).x2 || yMouse < buttons.at(index).y1 || yMouse > buttons.at(index).y2)){
                buttons.at(index).color = reliseButtonColor;
                buttons.at(index).textColor = reliseTextButtonColor;
                buttons.at(index).isMouseOver = false;
                isAChange = true;
            }
            if(buttons.at(index).isMouseOver == true && ismouseclick(WM_LBUTTONDOWN)){
                if(strcmp(buttons.at(index).text, "Genereaza") == 0){
                    cout << "\n\n Se genereaza un block de tipul " << currentBlock << " cu datele " << inputTextBoxes.at(0).text << " "<< inputTextBoxes.at(1).text << " " <<inputTextBoxes.at(2).text << '\n';
                    if(currentBlock == input || currentBlock == calculation){
                        string variableName = inputTextBoxes.at(0).text;
                        variables.insert(pair<string, int>(variableName, 0));
                    }
                    generateBlock(currentBlock, inputTextBoxes.at(0).text, inputTextBoxes.at(1).text, inputTextBoxes.at(2).text, 200, 200, -1, -1);
                    currentBlock = null;
                }
                cout << "\nA fost apasat butonul " << buttons.at(index).text << '\n';
                if(strcmp(buttons.at(index).text, "Intrare") == 0){
                    currentBlock = input;
                }
                if(strcmp(buttons.at(index).text, "Executare") == 0){
                    for(int indexBlock = 0; indexBlock < blocks.size(); indexBlock++){
                        blocks.at(indexBlock)->color = blockColor;
                    }
                    outputString = "";
                    executeLogicalScheme(startBlock);
                    drawAll();
                    openOutpuWindow(outputString);
                    for(int indexBlock = 0; indexBlock < blocks.size(); indexBlock++){
                        blocks.at(indexBlock)->color = blockColor;
                    }
                    isAChange = true;
                }
                else if(strcmp(buttons.at(index).text, "Iesire") == 0){
                    currentBlock = output;
                }
                else if(strcmp(buttons.at(index).text, "Calcul") == 0){
                    currentBlock = calculation;
                }
                else if(strcmp(buttons.at(index).text, "Decizie") == 0){
                    currentBlock = decision;
                }
                else if(strcmp(buttons.at(index).text, "Deschide") == 0){
                    openFile(hMainWindow);
                    for(int index = 0; index < blocks.size(); index++){
                        if(blocks.at(index)->index_leftNext != -1){
                            blocks.at(index)->leftNext = blocks.at(blocks.at(index)->index_leftNext);
                        }
                        if(blocks.at(index)->index_rightNext != -1){
                            blocks.at(index)->rightNext = blocks.at(blocks.at(index)->index_rightNext);
                        }
                        if(blocks.at(index)->type == input ||  blocks.at(index)->type == calculation){
                            string variableName = blocks.at(index)->text1;
                            variables.insert(pair<string, int>(variableName, 0));
                        }
                    }
                    startBlock = blocks.at(0);
                    stopBlock = blocks.at(1);
                    drawArrow2();
                }
                else if(strcmp(buttons.at(index).text, "Salveaza") == 0){
                    saveFile(blocks,hMainWindow);
                }
                else if(strcmp(buttons.at(index).text, "Vezi codul C++") == 0){
                    code = "";
                    for(int index_block = 0; index_block < blocks.size(); index_block++){
                        blocks.at(index_block)->code_line_index = 0;
                        blocks.at(index_block)->level_tree = -1;
                    }
                    transformLogicalSchemeInCode(startBlock, 0);
                    ofstream fout("CodulC++.cpp");
                    code.append("\n}");
                    addLoopsAndVariables();
                    fout << code;
                    fout.close();
                    ShellExecute(NULL, "open", "CodulC++.cpp", NULL, NULL, SW_SHOWDEFAULT);
                }
                clearmouseclick(WM_LBUTTONDOWN);
                uncheckAllTextBoxes();
                clearTextTextBoxes();
                listenKeys = false;
                isAChange = true;
            }
        }
        if(yMouse > 120 && ismouseclick(WM_RBUTTONDOWN)){ ///Daca se apasa click dreapta pe un block se sterge
            clearmouseclick(WM_RBUTTONDOWN);
              for(int indexBlock = 0; indexBlock < blocks.size(); indexBlock++){
                if(blocks.at(indexBlock)->isMouseOver(xMouse, yMouse) && blocks.at(indexBlock)->type != start && blocks.at(indexBlock)->type != stop){
                    for(int indexBlock2 = 0; indexBlock2 < blocks.size(); indexBlock2++){
                        if(blocks.at(indexBlock2)->leftNext == blocks.at(indexBlock)){
                            blocks.at(indexBlock2)->leftNext = NULL;
                        }
                        if(blocks.at(indexBlock2)->rightNext == blocks.at(indexBlock)){
                            blocks.at(indexBlock2)->rightNext = NULL;
                        }
                    }
                    blocks.erase(blocks.begin() + indexBlock);
                    isAChange = true;
                }
            }
        }
        if(yMouse > 120 && ismouseclick(WM_LBUTTONDBLCLK)){ ///Daca se apasa dublu click stanga pe un block se duce in editare
            clearmouseclick(WM_LBUTTONDBLCLK);
              for(int indexBlock = 0; indexBlock < blocks.size(); indexBlock++){
                if(blocks.at(indexBlock)->isMouseOver(xMouse, yMouse) && blocks.at(indexBlock)->type != start && blocks.at(indexBlock)->type != stop){
                    currentBlock = blocks.at(indexBlock)->type;
                    strcpy(inputTextBoxes.at(0).text, blocks.at(indexBlock)->text1);
                    strcpy(inputTextBoxes.at(1).text, blocks.at(indexBlock)->text2);
                    strcpy(inputTextBoxes.at(2).text, blocks.at(indexBlock)->text3);
                    for(int indexBlock2 = 0; indexBlock2 < blocks.size(); indexBlock2++){
                        if(blocks.at(indexBlock2)->leftNext == blocks.at(indexBlock)){
                            blocks.at(indexBlock2)->leftNext = NULL;
                        }
                        if(blocks.at(indexBlock2)->rightNext == blocks.at(indexBlock)){
                            blocks.at(indexBlock2)->rightNext = NULL;
                        }
                    }
                    blocks.erase(blocks.begin() + indexBlock);
                    isAChange = true;
                }
            }
        }
        if(yMouse > 120 && ismouseclick(WM_LBUTTONDOWN)){
            clearmouseclick(WM_LBUTTONDOWN);
            clearmouseclick(WM_LBUTTONUP);
            for(int indexBlock = 0; indexBlock < blocks.size(); indexBlock++){
                if(blocks.at(indexBlock)->isMouseOver(xMouse, yMouse)){
                    if(getpixel(mousex(), mousey()) == 53652685){ /// 53652685 - codul de culoare pentru cerc
                        Beep(500, 10);
                        cout << "\nA fost apasat cerculetul din blocul " << blocks.at(indexBlock)->text1;
                        int mouseX = mousex();
                        int mouseY = mousey();
                        while(!ismouseclick(WM_LBUTTONUP) && mousey() > 120){
                            POINT p;
                            GetCursorPos(&p);
                            while(mousey()  < 140){
                                SetCursorPos(p.x, p.y + (140 - mousey()) + 5);
                            }
                            while(mousey() > WINDOW_HEIGHT - 80){
                                SetCursorPos(p.x, WINDOW_HEIGHT - (WINDOW_HEIGHT - mousey()) + 30);
                            }
                            if(mousex() < 20 || mousex() > WINDOW_WIDTH - 20){
                                break;
                            }
                            drawAll();
                            swapbuffers();
                            if(blocks.at(indexBlock)->type == decision){
                                if(mouseX < blocks.at(indexBlock)->xPoz){
                                        drawArrow(blocks.at(indexBlock)->xPoz - 3*blocks.at(indexBlock)->width/2 + 15, blocks.at(indexBlock)->yPoz + 11, mousex(), mousey());
                                    }
                                    else{
                                        drawArrow(blocks.at(indexBlock)->xPoz + 3*blocks.at(indexBlock)->width/2 - 15, blocks.at(indexBlock)->yPoz + 11, mousex(), mousey());
                                    }
                            }else{
                                drawArrow(blocks.at(indexBlock)->xPoz, blocks.at(indexBlock)->yPoz + 11, mousex(), mousey());
                            }

                            swapbuffers();
                            isAChange = true;
                        }
                        for(int indexBlock2 = 1; indexBlock2 < blocks.size(); indexBlock2++){ /// indexBlock2 este initializat cu 1 pentru ca pe pozitia 0 se afla blocul de start, astfel nu vom putea sa tragem sageata catre el
                            if(indexBlock2 != indexBlock && blocks.at(indexBlock2)->isMouseOver(mousex(), mousey())){
                                Beep(500, 10);
                                if(blocks.at(indexBlock)->type == decision){
                                    if(mouseX < blocks.at(indexBlock)->xPoz){
                                        blocks.at(indexBlock)->leftNext = blocks.at(indexBlock2);
                                    }
                                    else{
                                        blocks.at(indexBlock)->rightNext = blocks.at(indexBlock2);
                                    }
                                }
                                else{
                                    blocks.at(indexBlock)->leftNext = blocks.at(indexBlock2);
                                }
                            }
                        }
                        break;
                    }
                    while(!ismouseclick(WM_LBUTTONUP)){
                        blocks.at(indexBlock)->xPoz = mousex();
                        blocks.at(indexBlock)->yPoz = mousey();

                        findExtremPoints(blocks.at(indexBlock));
                        drawAll();
                        if(blocks.at(indexBlock)->maxLeft < 25){
                            blocks.at(indexBlock)->xPoz +=5 + 25 - blocks.at(indexBlock)->maxLeft;
                            findExtremPoints(blocks.at(indexBlock));
                            drawAll();
                            break;
                        }

                        findExtremPoints(blocks.at(indexBlock));
                        drawAll();
                        if(blocks.at(indexBlock)->maxRight > WINDOW_WIDTH-25){
                            blocks.at(indexBlock)->xPoz +=5 + WINDOW_WIDTH - 35 - blocks.at(indexBlock)->maxRight;
                            findExtremPoints(blocks.at(indexBlock));
                            drawAll();
                            break;
                        }

                        findExtremPoints(blocks.at(indexBlock));
                        drawAll();
                        if(blocks.at(indexBlock)->yPoz < 160){
                            blocks.at(indexBlock)->yPoz +=10;
                            findExtremPoints(blocks.at(indexBlock));
                            drawAll();
                            break;
                        }

                        findExtremPoints(blocks.at(indexBlock));
                        drawAll();
                        if(blocks.at(indexBlock)->yPoz > WINDOW_HEIGHT-100){
                            blocks.at(indexBlock)->yPoz -=5;
                            findExtremPoints(blocks.at(indexBlock));
                            drawAll();
                            break;
                        }
                    }
                }
            }
        }
        if(listenKeys && kbhit()){
            for(int indexTextBox = 0; indexTextBox < inputTextBoxes.size(); indexTextBox++){
                if(inputTextBoxes.at(indexTextBox).isSelected){
                    character = (char) getch();
                     Beep(character*10, 10);
                    if((int)character == 8){ ///delite
                        inputTextBoxes.at(indexTextBox).text[strlen(inputTextBoxes.at(indexTextBox).text) - 1] = '\0';
                    }
                    else{
                        inputTextBoxes.at(indexTextBox).text[strlen(inputTextBoxes.at(indexTextBox).text) + 1] = '\0';
                        inputTextBoxes.at(indexTextBox).text[strlen(inputTextBoxes.at(indexTextBox).text)] = character;
                    }
                    isAChange = true;
                }
            }
        }
        if(kbhit()){
            getch();
        }
    }
}

int main(){
    ShowWindow (GetConsoleWindow(), SW_HIDE);
    setcurrentwindow(principalWindow);
    setbkcolor(COLOR(backgroundColor.red, backgroundColor.green, backgroundColor.blue));
    cleardevice();
    initialization();
    appLoop();
    return 0;
}
