#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QColor>
#include <QGLWidget>
#include <QKeyEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QString>
#include <iostream>
#include <windows.h>
#include <glut.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "common.h"
#include <QDebug>
#include "drawtext.h"
#include <QLineEdit>
#include <QFont>
struct Edge{
    int ymax;
    double x;
    double dx; 
    Edge *next; 
};

struct Entity{ 
    double x; 
    double y; 
    double color[3]; 
    unsigned size; 
    unsigned pid; 
    bool chosen; 
    int type;
    QLineEdit *tmpedit = nullptr; 
    QString editText = "";
    bool isStartPoint = false;

};

class OpenGLWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
private:
    unsigned currentID; 
    int currentMode; 
    unsigned currentPointSize; 
    double currentColor[3]; 

    double originX; 
    double originY; 

    bool isLeftButtonPressed; 
    bool isRightButtonPressed; 
    bool isChoosingPoints; 
    bool isNewChosen; 
    bool isDrawPoligonJustNow; 

    bool moveTextFlag = false;

    GLfloat AngleX;
    GLfloat AngleY;
    int rectSize;

    std::vector<Entity*> points; 
    std::vector<Entity*> trashPoints; 
    std::vector<Entity*> chosenPoints; 
    std::vector<std::pair<double, double>> tempPoints; 

    QLineEdit *edit = nullptr;

public:
    OpenGLWindow(QWidget *parent = 0);
    ~OpenGLWindow();

    void cleanTempPoints(){
        while(!tempPoints.empty()) {tempPoints.pop_back();}
    }
    void cleanTrashPoints(){
        while(trashPoints.size() > 0){delete trashPoints[trashPoints.size()-1];trashPoints.pop_back();}}
    void cleanPoints(){
        while(points.size() > 0){delete points[points.size()-1];points.pop_back();}currentID = 0;}
    void cleanChosenPoints(){
        while(chosenPoints.size() > 0){delete chosenPoints[chosenPoints.size()-1];chosenPoints.pop_back();}}

    unsigned getPosByPID(unsigned id); 
    int getMode(){return currentMode;} 
    int getTrashPointsAmounts(); 
    int getRectSize(){return rectSize;}

    void setCurrentMode(int m){ 
        currentMode = m; cleanTempPoints(); isDrawPoligonJustNow = false;}
    void setCurrentColor(double r, double g, double b){
        currentColor[0] = r; currentColor[1] = g; currentColor[2] = b; glColor3d(r, g, b);}
    void setCurrentPointSize(int x){ 
        currentPointSize = x; glPointSize(currentPointSize);    qDebug()<<"ÉèÖÃ´óÐ¡ £º"<<x;}
    void setRectSize(int x){
        rectSize = x;}

    void drawPoint(double x, double y); 
    void addNewText( double localPosX, double localPosY,double loc_x, double loc_y);
    void drawLine(double x1, double y1, double x2, double y2); 
    void drawCruve(); 
    void drawCircle(double xc, double yc, double r); 
    void drawEllipse(double x0, double y0, double a, double b); 
    void drawRect(double x1, double y1, double x2, double y2); 
    void drawFilledRect(double x1, double y1, double x2, double y2); 
    void drawPoligon(); 
    void drawFilledPoligon(); 
    void editorLostFocus(drawText *item);

    void chooseEntityWithSamePID(double x, double y); 
    void chooseRect(double x1, double y1, double x2, double y2); 
    void choosePoligon(); 
    bool isPointInPoligon(double x, double y); 
    void chooseCancel() 
    {
        int lastInt = 0;
        bool firstCheck = true;
        int nowInt = 0;
        for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end(); ite++)
        {
            if(firstCheck)
            {
                if((*ite)->chosen)
                {
                    nowInt =  lastInt = (*ite)->pid;
                    qDebug()<<"now current Id choose Cancel -->"<< (*ite)->pid;
                    firstCheck = false;
                }
            }
            else
            {
                if((*ite)->chosen)
                {
                    if(lastInt != nowInt)
                    {
                        nowInt =  lastInt = (*ite)->pid;
                        qDebug()<<"now current Id choose Cancel -->"<< (*ite)->pid;
                        firstCheck = false;
                    }
                }
            }

            (*ite)->chosen = false;

        }

        isChoosingPoints = false;
        cleanChosenPoints();
        update();
    }
    void chooseInvert() 
    {
        cleanChosenPoints();
        for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end(); ite++)
        {
            (*ite)->chosen = !((*ite)->chosen);
            if((*ite)->chosen == true)
            {
                isChoosingPoints = true;
                isNewChosen = true;
            }
        }
        update();
    }

    void moveChosenZone(double localPosX, double localPosY,double offsetX, double offsetY); 
    void rotateChosenZone(double angle); 
    void zoomChosenZone(double pix); 
    void deleteChosenZone(); 
    void pickChosenPoints(); 


    void moveTxtPos(double localPosX, double localPosY, QLineEdit *ite);

    void saveToFile(char *filepath); 
    void openFile(char *filepath); 
    void newPaint();

    void traceUndo(); 
    void traceRedo(); 

    void show3D(); 

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
public slots:
    void editFinished();
};

#endif 
