#include "OpenGLWindow.h"
OpenGLWindow::OpenGLWindow(QWidget *parent)
    :QOpenGLWidget(parent)
{
    currentMode = -1;
    currentPointSize = 5;
    currentID = 0;
    currentColor[0] = 1;
    currentColor[1] = 170/255;
    currentColor[2] = 0;
    AngleX = 25.0f;
    AngleY = 300.0f;
    rectSize = 40;
    isChoosingPoints = false;

    setMouseTracking(false);
}

OpenGLWindow::~OpenGLWindow()
{

}

void OpenGLWindow::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0, 1.0, 1.0,1.0);
    glPointSize(currentPointSize);
    glColor3d(currentColor[0], currentColor[1], currentColor[2]);
}

void OpenGLWindow::paintGL()
{

    for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end(); ite++)
    {
        if(0 == (*ite)->type)
        {
            if((*ite)->chosen == false)
                glColor3d((*ite)->color[0], (*ite)->color[1], (*ite)->color[2]);
            else
                glColor3d(0, 0, 0);
            glPointSize((*ite)->size);
            glBegin(GL_POINTS);
            glVertex3d((*ite)->x, (*ite)->y, 0);
            glEnd();
        }
    }
    glColor3d(currentColor[0], currentColor[1], currentColor[2]);
    glPointSize(currentPointSize);
}

void OpenGLWindow::resizeGL(int w, int h)
{

        glViewport(0, 0, (GLsizei)w, (GLsizei)h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, this->size().width(), 0, this->size().height());
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

}

void OpenGLWindow::mousePressEvent(QMouseEvent *event)
{
    double loc_x = originX = event->localPos().x();
    double loc_y = originY = this->size().height() - event->localPos().y();

    if(event->button() == Qt::MiddleButton)
    {
        chooseCancel();
        cleanTempPoints();
    }
    else if(event->button() == Qt::LeftButton)
    {
        cleanTrashPoints();
        if(currentMode >= 0 && currentMode <= 10)
            chooseCancel();
        isLeftButtonPressed = true;
    }
    else if(event->button() == Qt::RightButton)
    {
        isRightButtonPressed = true;
    }

    if(currentMode == 0) 
    {
        if(event->button() == Qt::LeftButton)
            drawPoint(loc_x, loc_y);
    }
    else if(currentMode == 1) 
    {
        if(event->button() == Qt::LeftButton)
        {
            if(tempPoints.size() == 2)
                cleanTempPoints();
            if(tempPoints.size() == 0)
            {
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
            }
            else if(tempPoints.size() == 1)
            {
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
                drawLine((*(tempPoints.begin())).first, (*(tempPoints.begin())).second, (*(++tempPoints.begin())).first, (*(++tempPoints.begin())).second);
                currentID++;
            }
        }
        else if(event->button() == Qt::RightButton)
        {
            if(tempPoints.size() == 2)
            {
                traceUndo();
                cleanTrashPoints();
                drawLine(tempPoints[0].first, tempPoints[0].second, loc_x, loc_y);
                currentID++;
            }
        }
    }
    else if(currentMode == 2) 
    {
        if(event->button() == Qt::LeftButton)
        {
            if(tempPoints.size() == 4)
                cleanTempPoints();
            if(tempPoints.size() < 3)
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
            else if(tempPoints.size() == 3)
            {
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
                drawCruve();
                currentID++;
            }
        }
        else if(event->button() == Qt::RightButton)
        {
            if(tempPoints.size() == 4)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[3].first = loc_x;
                tempPoints[3].second = loc_y;
                drawCruve();
                currentID++;
            }
        }
    }

    else if(currentMode == 3) 
    {
        if(event->button() == Qt::LeftButton)
        {
            if(tempPoints.size() == 2)
                cleanTempPoints();
            if(tempPoints.size() == 0)
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
            else if(tempPoints.size() == 1)
            {
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
                double CircleRadius = sqrt(pow((loc_x - tempPoints[0].first), 2) + pow((loc_y - tempPoints[0].second), 2));
                drawCircle(tempPoints[0].first, tempPoints[0].second, CircleRadius);
                currentID++;
            }
        }
        else if(event->button() == Qt::RightButton)
        {
            if(tempPoints.size() == 2)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[1].first = loc_x;
                tempPoints[1].second = loc_y;
                double CircleRadius = sqrt(pow((loc_x - tempPoints[0].first), 2) + pow((loc_y - tempPoints[0].second), 2));
                drawCircle(tempPoints[0].first, tempPoints[0].second, CircleRadius);
                currentID++;
            }
        }
    }
    else if(currentMode == 4) 
    {
        if(event->button() == Qt::LeftButton)
        {
            if(tempPoints.size() == 3)
                cleanTempPoints();
            if(tempPoints.size() < 2)
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
            else if(tempPoints.size() == 2)
            {
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
                double ra = sqrt(pow((tempPoints[1].first - tempPoints[0].first), 2) + pow((tempPoints[1].second - tempPoints[0].second), 2));
                double rb = sqrt(pow((loc_x - tempPoints[0].first), 2) + pow((loc_y - tempPoints[0].second), 2));
                drawEllipse(tempPoints[0].first, tempPoints[0].second, ra, rb);
                currentID++;
            }
        }
        else if(event->button() == Qt::RightButton)
        {
            if(tempPoints.size() == 3)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[2].first = loc_x;
                tempPoints[2].second = loc_y;
                double ra = sqrt(pow((tempPoints[1].first - tempPoints[0].first), 2) + pow((tempPoints[1].second - tempPoints[0].second), 2));
                double rb = sqrt(pow((loc_x - tempPoints[0].first), 2) + pow((loc_y - tempPoints[0].second), 2));
                drawEllipse(tempPoints[0].first, tempPoints[0].second, ra, rb);
                currentID++;
            }
        }
    }
    else if(currentMode == 5 || currentMode == 6 || currentMode == 9) 
    {
        if(event->button() == Qt::LeftButton)
        {
            if(tempPoints.size() == 2)
                cleanTempPoints();
            if(tempPoints.size() == 0)
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
            else if(tempPoints.size() == 1)
            {
                tempPoints.push_back(std::make_pair(loc_x, loc_y));
                if(currentMode == 5)
                    drawRect(tempPoints[0].first, tempPoints[0].second, tempPoints[1].first, tempPoints[1].second);
                else if(currentMode == 6)
                    drawFilledRect(tempPoints[0].first, tempPoints[0].second, tempPoints[1].first, tempPoints[1].second);
                else if(currentMode == 9)
                {
                    chooseRect(tempPoints[0].first, tempPoints[0].second, tempPoints[1].first, tempPoints[1].second);
                    --currentID;
                }
                currentID++;
            }
        }
        else if(event->button() == Qt::RightButton)
        {
            if(tempPoints.size() == 2)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[1].first = loc_x;
                tempPoints[1].second = loc_y;
                if(currentMode == 5)
                    drawRect(tempPoints[0].first, tempPoints[0].second, tempPoints[1].first, tempPoints[1].second);
                else if(currentMode == 6)
                    drawFilledRect(tempPoints[0].first, tempPoints[0].second, tempPoints[1].first, tempPoints[1].second);
                currentID++;
            }
        }
    }
    else if(currentMode == 7 || currentMode == 10) 
    {
        if(event->button() == Qt::LeftButton)
        {
            if(isDrawPoligonJustNow)
            {
                cleanTempPoints();
                isDrawPoligonJustNow = false;
            }
            tempPoints.push_back(std::make_pair(loc_x, loc_y));
        }
        else if(event->button() == Qt::RightButton)
        {
            if(isDrawPoligonJustNow)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[tempPoints.size()-1].first = loc_x;
                tempPoints[tempPoints.size()-1].second = loc_y;
                if(currentMode == 7)
                    drawPoligon();
                currentID++;
            }
            else if(tempPoints.size() >= 3)
            {
                if(currentMode == 7 && isDrawPoligonJustNow == false)
                {
                    drawPoligon();
                    currentID++;
                    isDrawPoligonJustNow = true;
                }
                else if(currentMode == 10)
                {
                    choosePoligon();
                    cleanTempPoints();
                }
            }
        }
    }
    else if(currentMode == 8)
    {
        if(event->button() == Qt::LeftButton)
        {
            this->addNewText(event->localPos().x(),
                             event->localPos().y(),
                             loc_x,
                             loc_y);
            currentID++;
        }

    }
    else if(currentMode == 11)
    {

        chooseEntityWithSamePID(loc_x, loc_y);

    }
    else if(currentMode == -1) 
    {
        if(isNewChosen)
        {
            pickChosenPoints();
            ++currentID; 
            isNewChosen = false;
        }
    }

    update();
}

void OpenGLWindow::editFinished()
{
    edit->setEnabled(false);
    edit->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    this->update(); 
}

void OpenGLWindow::mouseMoveEvent(QMouseEvent *event)
{
    double loc_x = event->localPos().x();
    double loc_y = this->size().height() - event->localPos().y();

        if(currentMode == -1) 
        {
            if(isLeftButtonPressed)
            {
                moveChosenZone(event->localPos().x(),
                               event->localPos().y(),
                               event->localPos().x() - originX,
                               height() - event->localPos().y() - originY);

            }

        }
        else if(currentMode == 0) 
        {
            if(isLeftButtonPressed)
                drawPoint(loc_x, loc_y);
        }
        else if(currentMode == 1) 
        {
            if(tempPoints.size() == 2)
            {
                traceUndo();
                cleanTrashPoints();
                drawLine(tempPoints[0].first, tempPoints[0].second, loc_x, loc_y);
                currentID++;
            }
        }
        else if(currentMode == 2) 
        {
            if(tempPoints.size() == 4)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[3].first = loc_x;
                tempPoints[3].second = loc_y;
                drawCruve();
                currentID++;
            }
        }
        else if(currentMode == 3) 
        {

            if(tempPoints.size() == 2)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[1].first = loc_x;
                tempPoints[1].second = loc_y;
                double CircleRadius = sqrt(pow((loc_x - tempPoints[0].first), 2) + pow((loc_y - tempPoints[0].second), 2));
                drawCircle(tempPoints[0].first, tempPoints[0].second, CircleRadius);
                currentID++;
            }
        }
        else if(currentMode == 4) 
        {
            if(tempPoints.size() == 3)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[2].first = loc_x;
                tempPoints[2].second = loc_y;
                double ra = sqrt(pow((tempPoints[1].first - tempPoints[0].first), 2) + pow((tempPoints[1].second - tempPoints[0].second), 2));
                double rb = sqrt(pow((loc_x - tempPoints[0].first), 2) + pow((loc_y - tempPoints[0].second), 2));
                drawEllipse(tempPoints[0].first, tempPoints[0].second, ra, rb);
                currentID++;
            }
        }
        else if(currentMode == 5 || currentMode == 6) 
        {
             qDebug()<<"rect mouseMoveEvent currentMode --> "<<currentMode;
            if(tempPoints.size() == 2)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[1].first = loc_x;
                tempPoints[1].second = loc_y;
                if(currentMode == 5)
                    drawRect(tempPoints[0].first, tempPoints[0].second, tempPoints[1].first, tempPoints[1].second);
                else if(currentMode == 6)
                    drawFilledRect(tempPoints[0].first, tempPoints[0].second, tempPoints[1].first, tempPoints[1].second);
                currentID++;
            }
        }
        else if(currentMode == 7) 
        {
            if(tempPoints.size() >= 3 && isDrawPoligonJustNow)
            {
                traceUndo();
                cleanTrashPoints();
                tempPoints[tempPoints.size()-1].first = loc_x;
                tempPoints[tempPoints.size()-1].second = loc_y;
                if(currentMode == 7)
                    drawPoligon();
                else if(currentMode == 8)
                    drawFilledPoligon();
                currentID++;
            }
        }
        originX = loc_x;
        originY = loc_y;

    update();
}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent *event)
{
    isLeftButtonPressed = false;
    isRightButtonPressed = false;
    if(event->button() == Qt::LeftButton && currentMode == 0)
    {
        ++currentID;
    }
    else if(event->button() == Qt::LeftButton && currentMode == -1)
    {
        moveChosenZone(event->localPos().x(),
                       event->localPos().y(),
                       event->localPos().x() - originX,
                       height() - event->localPos().y() - originY);
    }
    update();
}

void OpenGLWindow::chooseEntityWithSamePID(double x, double y)
{
    int currPid = -1;
    for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end(); ite++)
    {
        if(abs((*ite)->x - x) <= 1 && abs((*ite)->y - y) <= 1)
        {
            currPid = (*ite)->pid;
        }
    }
    qDebug()<<"chooseEntityWithSamePID -->pid -->"<<currPid ;
    if(currPid == -1) return;
    chooseCancel();
    for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end(); ite++)
    {
        if((int)((*ite)->pid) == currPid)
        {
            (*ite)->chosen = true;
            isChoosingPoints = true;
            isNewChosen = true;
        }
    }
}

void OpenGLWindow::chooseRect(double x1, double y1, double x2, double y2)
{
    if(isChoosingPoints)
        chooseCancel();
    double smallX = (x1 < x2) ? x1 : x2;
    double bigX = (x1 < x2) ? x2 : x1;
    double smallY = (y1 < y2) ? y1 : y2;
    double bigY = (y1 < y2) ? y2 : y1;
    for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end(); ite++)
    {
        if((*ite)->x >= smallX && (*ite)->x <= bigX && (*ite)->y >= smallY && (*ite)->y <= bigY)
        {
            (*ite)->chosen = true;
            isChoosingPoints = true;
            isNewChosen = true;
        }
    }
}

void OpenGLWindow::choosePoligon()
{
    if(isChoosingPoints)
        chooseCancel();
    for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end(); ite++)
    {
        if(isPointInPoligon((*ite)->x, (*ite)->y))
        {
            (*ite)->chosen = true;
            isChoosingPoints = true;
            isNewChosen = true;
        }
    }
}

bool OpenGLWindow::isPointInPoligon(double x, double y)
{
    int nextPoint = tempPoints.size() - 1 ;
    int cross = 0;
    for (unsigned lastPoint = 0; lastPoint < tempPoints.size(); lastPoint++) 
    {
        double a = tempPoints[lastPoint].first;
        double b = tempPoints[lastPoint].second;
        double c = tempPoints[nextPoint].first;
        double d = tempPoints[nextPoint].second;
        if(((b < y && d >= y) || (d < y && b >= y)) 
                && ((y - b) / (d - b) * (c - a) < (x - a)))
            cross++;
        nextPoint = lastPoint;
    }
    return (cross % 2);
}

void OpenGLWindow::moveChosenZone(double localPosX, double localPosY,double offsetX, double offsetY)
{
    for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end(); ite++)
    {
        if((*ite)->type == 0)
        {
            if((*ite)->chosen == true)
            {
                qDebug()<<"now Move Current Id -->"<<(*ite)->pid;
                (*ite)->x += offsetX;
                (*ite)->y += offsetY;
            }
        }
        else
        {
            if((*ite)->chosen == true)
            {
                this->moveTxtPos(localPosX, localPosY,  (*ite)->tmpedit);
                break;
            }
        }


    }
}

void OpenGLWindow::moveTxtPos(double localPosX, double localPosY, QLineEdit *ite)
{
    ite->move(localPosX, localPosY);
}

void OpenGLWindow::rotateChosenZone(double angle)
{
    if(isChoosingPoints == false) return;
    if(isNewChosen)
    {
        pickChosenPoints();
        ++currentID;
        isNewChosen = false;
    }
    double left = width(), right = 0, top = 0, bottom = height();
    for(unsigned i = 0; i < points.size(); ++i)
    {
        if(points[i]->chosen == true)
        {
            if(points[i]->x < left)
                left = points[i]->x;
            if(points[i]->x > right)
                right = points[i]->x;
            if(points[i]->y > top)
                top = points[i]->y;
            if(points[i]->y < bottom)
                bottom = points[i]->y;
        }
    }
    double midx = (left+right)/2.0;
    double midy = (top+bottom)/2.0;

    for(unsigned i = 0; i < points.size(); ++i)
    {
        if(points[i]->chosen == true)
        {
             double dis = sqrt(pow(points[i]->x - midx, 2.0) + pow(points[i]->y - midy, 2.0));
             double ang = atan2(points[i]->y - midy, points[i]->x - midx);
             ang += ((-angle)/180.0)*3.141592653;
             points[i]->x = midx + dis * cos(-ang);
             points[i]->y = midy + dis * sin(ang);
        }
    }
    update();
}

void OpenGLWindow::zoomChosenZone(double pix)
{
    if(isChoosingPoints == false) return;
    if(isNewChosen)
    {
        pickChosenPoints();
        ++currentID;
        isNewChosen = false;
    }
    double left = width(), right = 0, top = 0, bottom = height();
    for(unsigned i = 0; i < points.size(); ++i)
    {
        if(points[i]->chosen == true)
        {
            if(points[i]->x < left)
                left = points[i]->x;
            if(points[i]->x > right)
                right = points[i]->x;
            if(points[i]->y > top)
                top = points[i]->y;
            if(points[i]->y < bottom)
                bottom = points[i]->y;
        }
    }
    double midx = (left+right)/2.0;
    double midy = (top+bottom)/2.0;
    for(unsigned i = 0; i < points.size(); ++i)
    {
        if(points[i]->chosen == true)
        {
             double dis = sqrt(pow(points[i]->x - midx, 2.0) + pow(points[i]->y - midy, 2.0)) * pix;
             double ang = atan2(points[i]->y - midy, points[i]->x - midx);
             points[i]->x = midx + dis * cos(ang);
             points[i]->y = midy + dis * sin(ang);
        }
    }
    update();
}

void OpenGLWindow::deleteChosenZone()
{
    if(isChoosingPoints == false) return;
    for(unsigned i = 0; i < points.size(); ++i)
    {
        if(points[i]->chosen == true)
        {
            delete points[i];
            points.erase(points.begin()+i);
            --i;
        }
    }
    isChoosingPoints = false;
    isNewChosen = false;
    cleanChosenPoints();
    update();
}

void OpenGLWindow::pickChosenPoints()
{
    for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end();)
    {
        if((*ite)->pid < currentID && (*ite)->chosen == true)
        {
            Entity *tmp = new Entity;
            tmp->chosen = true;
            tmp->type = (*ite)->type;
            tmp->color[0] = (*ite)->color[0];
            tmp->color[1] = (*ite)->color[1];
            tmp->color[2] = (*ite)->color[2];
            tmp->size = (*ite)->size;
            tmp->x = (*ite)->x;
            tmp->y = (*ite)->y;
            tmp->pid = currentID;
            tmp->type = (*ite)->type;
            tmp->tmpedit = (*ite)->tmpedit;
            tmp->editText = (*ite)->editText;


            chosenPoints.push_back(*ite);
            ite = points.erase(ite);
            points.push_back(tmp);
        }
        else ++ite;
    }
}

void OpenGLWindow::traceUndo()
{
    if(points.size() == 0) return;
    currentID -= 1;
    std::vector<Entity*>::iterator ite = points.end() - 1;

    bool tmpDeleteEdit = true;
    QString tmpText = "";
    qDebug()<<"traceUndo (*ite)->pid -->"<<((*ite)->pid );
    qDebug()<<"traceUndo  currentID  -->"<<( currentID);

    while((*ite)->pid == currentID)
    {
        if(tmpDeleteEdit && ((*ite)->type == 1)  && (*ite)->isStartPoint)
        {
            qDebug()<<"traceUndo  (*ite)->tmpedit-- >"<<((*ite)->tmpedit);
            qDebug()<<"traceUndo  (*ite)->tmpedit->text()  -->"<<( (*ite)->tmpedit->text());
            tmpText = (*ite)->tmpedit->text();
            delete (*ite)->tmpedit;
            tmpDeleteEdit= false;
        }
        (*ite)->editText = tmpText;
        trashPoints.push_back(*ite);

        points.pop_back();
        if(points.size() > 0)
            ite = points.end() - 1;

        else break;
    }
    if(isChoosingPoints == true)
    {
        for(std::vector<Entity*>::iterator ite = chosenPoints.begin(); ite != chosenPoints.end();)
        {
            points.insert(points.begin() + getPosByPID((*ite)->pid), *ite);
            ite = chosenPoints.erase(ite);
        }
        if(getTrashPointsAmounts() == 1)
            isNewChosen = true;
    }
    update();
}

void OpenGLWindow::traceRedo()
{
    if(trashPoints.size() == 0) return;
    if(getTrashPointsAmounts() == 1)
        isNewChosen = true;
    std::vector<Entity*>::iterator ite = trashPoints.end() - 1;
    bool tmpFirst = true;

    qDebug()<<"traceRedo (*ite)->pid -->"<<((*ite)->pid );
    qDebug()<<"traceRedo  currentID  -->"<<( currentID);


    while((*ite)->pid == currentID)
    {
        if(tmpFirst && ((*ite)->type == 1) && (*ite)->isStartPoint)
        {
            edit = new QLineEdit(this);
            edit->setEnabled(false);
            QFont font;
            font.setPointSize((*ite)->size);


            QBrush myBrush;
            QPalette palette;
            myBrush = QBrush(QColor((*ite)->color[0] * 255,
                             (*ite)->color[1] * 255,
                    (*ite)->color[2] * 255),Qt::DiagCrossPattern);
            palette.setBrush(QPalette::Text,  myBrush);
            edit->setPalette(palette);

            edit->setStyleSheet("background:transparent;");
            edit->hide();
            edit->setText((*ite)->editText);
            qDebug()<<"traceRedo LineEdit X Y  "<<(*ite)->x<<"  "<<this->size().height() - (*ite)->y;


            edit ->move((*ite)->x, this->size().height() - (*ite)->y);
            edit ->show();
            edit ->raise();  
            edit->setFocus();

            connect(edit, SIGNAL(editingFinished()), this, SLOT(editFinished()));
            tmpFirst = false;
            (*ite)->tmpedit = edit;
        }

        points.push_back(*ite);
        trashPoints.pop_back();
        if(trashPoints.size() > 0)
            ite = trashPoints.end() - 1;
        else break;
    }
    if(getTrashPointsAmounts() == 0 && isChoosingPoints) 
    {
        for(std::vector<Entity*>::iterator ite = points.begin(); ite != points.end();)
        {
            if((*ite)->pid < currentID && (*ite)->chosen == true)
            {
                chosenPoints.push_back(*ite);
                ite = points.erase(ite);
            }
            else ++ite;
        }
        isNewChosen = false;
    }
    currentID += 1;
    update();
}

void OpenGLWindow::drawPoint(double x, double y)
{
    Entity *newEntity = new Entity;
    newEntity->x = x;
    newEntity->y = y;
    newEntity->color[0] = currentColor[0];
    newEntity->color[1] = currentColor[1];
    newEntity->color[2] = currentColor[2];
    newEntity->size = currentPointSize;
    newEntity->pid = currentID;
    newEntity->chosen = false;
    newEntity->type = 0;
    points.push_back(newEntity);
}

void OpenGLWindow::addNewText( double localPosX, double localPosY,double loc_x, double loc_y)
{

    edit = new QLineEdit(this);
    edit->setEnabled(true);
    cleanTempPoints();
    QFont font;
    font.setPointSize( currentPointSize );


    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(QColor(currentColor[0] * 255,currentColor[1] * 255,currentColor[2] * 255),Qt::DiagCrossPattern);
    palette.setBrush(QPalette::Text,  myBrush);
    edit->setPalette(palette);

    edit->setStyleSheet("background:transparent;");
    edit->hide();
    edit->setText("");

    edit ->move(localPosX, localPosY);
    qDebug()<<" LineEdit X Y  "<<localPosX<<"  "<<localPosY;
    edit ->show();
    edit ->raise();   
    edit->setFocus();

    connect(edit, SIGNAL(editingFinished()), this, SLOT(editFinished()));

    int tmpStartPointX = loc_x;
    int tmpStartPointY = loc_y;


    for(tmpStartPointX; tmpStartPointX<(loc_x + edit->width()); tmpStartPointX++)
    {
        tmpStartPointY = loc_y;

        for(tmpStartPointY; tmpStartPointY>(loc_y - edit->height()); tmpStartPointY--)
        {

            Entity *newEntity = new Entity;
            newEntity->x = tmpStartPointX;
            newEntity->y = tmpStartPointY;
            newEntity->color[0] = currentColor[0];
            newEntity->color[1] = currentColor[1];
            newEntity->color[2] = currentColor[2];
            newEntity->size = currentPointSize;
            newEntity->pid = currentID;
            newEntity->chosen = false;
            newEntity->type = 1;

            if((tmpStartPointX == loc_x) && (tmpStartPointY == loc_y))
            {
                newEntity->isStartPoint = true;
                newEntity->tmpedit = edit;
            }
            else
            {
                newEntity->isStartPoint = false;
            }
            points.push_back(newEntity);
        }
    }


    qDebug()<<"edit currentID --> "<<currentID;
}

void OpenGLWindow::editorLostFocus(drawText *item)
{

}


void OpenGLWindow::drawLine(double x1, double y1, double x2, double y2)
{
    drawPoint(x1, y1);
    drawPoint(x2, y2);
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int flag = (dx >= dy) ? 0 : 1;
    int a = (x2 > x1) ? 1 : -1;
    int b = (y2 > y1) ? 1 : -1;
    int x = x1;
    int y = y1;
    if(flag){
        int temp = dx;
        dx = dy;
        dy = temp;
    }
    int h = 2 * dy - dx;
    for (int i = 1; i <= dx; ++i) {
        drawPoint(x, y);
        if (h >= 0) {
            if (!flag)
                    y += b;
            else
                    x += a;
            h -= 2 * dx;
        }
        if (!flag)
            x += a;
        else
            y += b;
        h += 2 * dy;
    }
}

void OpenGLWindow::drawCruve()
{
    for (GLfloat t = 0; t <= 1.0; t += 0.001)
    {
        GLfloat x = tempPoints[0].first*pow(1.0f - t, 3) + 3 * tempPoints[1].first*t*pow(1.0f - t, 2) + 3 * tempPoints[2].first*t*t*(1.0f - t) + tempPoints[3].first*pow(t, 3);
        GLfloat y = tempPoints[0].second*pow(1.0f - t, 3) + 3 * tempPoints[1].second*t*pow(1.0f - t, 2) + 3 * tempPoints[2].second*t*t*(1.0f - t) + tempPoints[3].second*pow(t, 3);
        drawPoint(x, y);
    }
}

void OpenGLWindow::drawCircle(double xc, double yc, double r)
{
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    drawPoint(xc + x, yc + y);
    drawPoint(xc - x, yc + y);
    drawPoint(xc + x, yc - y);
    drawPoint(xc - x, yc - y);
    drawPoint(xc + y, yc + x);
    drawPoint(xc - y, yc + x);
    drawPoint(xc + y, yc - x);
    drawPoint(xc - y, yc - x);
    while (x < y)
    {
        if (d < 0)
            d = d + 4 * x + 6;
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
        drawPoint(xc + x, yc + y);
        drawPoint(xc - x, yc + y);
        drawPoint(xc + x, yc - y);
        drawPoint(xc - x, yc - y);
        drawPoint(xc + y, yc + x);
        drawPoint(xc - y, yc + x);
        drawPoint(xc + y, yc - x);
        drawPoint(xc - y, yc - x);
    }
}

void OpenGLWindow::drawEllipse(double x0, double y0, double a, double b)
{
    double sqa = a*a;
    double sqb = b*b;
    double d = sqb + sqa*(0.25 - b);
    int x = 0;
    int y = b;
    drawPoint((x0 + x), (y0 + y));
    drawPoint((x0 + x), (y0 - y));
    drawPoint((x0 - x), (y0 - y));
    drawPoint((x0 - x), (y0 + y));
    while (sqb*(x + 1) < sqa*(y - 0.5))
    {
        if (d < 0)
            d += sqb*(2 * x + 3);
        else
        {
            d += (sqb*(2 * x + 3) + sqa*((-2)*y + 2));
            --y;
        }
        ++x;
        drawPoint((x0 + x), (y0 + y));
        drawPoint((x0 + x), (y0 - y));
        drawPoint((x0 - x), (y0 - y));
        drawPoint((x0 - x), (y0 + y));
    }
    d = (b * (x + 0.5)) * 2 + (a * (y - 1)) * 2 - (a * b) * 2;
    while (y > 0)
    {
        if (d < 0)
        {
            d += sqb * (2 * x + 2) + sqa * ((-2) * y + 3);
            ++x;
        }
        else
            d += sqa * ((-2) * y + 3);
        --y;
        drawPoint((x0 + x), (y0 + y));
        drawPoint((x0 + x), (y0 - y));
        drawPoint((x0 - x), (y0 - y));
        drawPoint((x0 - x), (y0 + y));
    }
}

void OpenGLWindow::drawRect(double x1, double y1, double x2, double y2)
{
    drawLine(x1, y1, x1, y2);
    drawLine(x1, y1, x2, y1);
    drawLine(x1, y2, x2, y2);
    drawLine(x2, y1, x2, y2);
}

void OpenGLWindow::drawFilledRect(double x1, double y1, double x2, double y2)
{
    double smallX = (x1 < x2) ? x1 : x2;
    double bigX = (x1 < x2) ? x2 : x1;
    double smallY = (y1 < y2) ? y1 : y2;
    double bigY = (y1 < y2) ? y2 : y1;
    for (double i = smallX; i <= bigX; i += 1)
            for (double j = smallY; j <= bigY; j += 1)
                    drawPoint(i, j);
}

void OpenGLWindow::drawPoligon()
{
    for (unsigned i = 1; i < tempPoints.size(); ++i) {
        drawLine(tempPoints[i - 1].first, tempPoints[i - 1].second, tempPoints[i].first, tempPoints[i].second);
    }
    drawLine(tempPoints[tempPoints.size()-1].first, tempPoints[tempPoints.size()-1].second, tempPoints[0].first, tempPoints[0].second);
}

void OpenGLWindow::drawFilledPoligon()
{

}

void OpenGLWindow::saveToFile(char *filepath)
{
    GLint PixelDataLength = width() * 3 * height();
    GLubyte *pPixelData = (GLubyte *)malloc(PixelDataLength);
    FILE *pWritingFile = fopen(filepath, "wb");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glReadPixels(10, 40, width(), height(), GL_BGR, GL_UNSIGNED_BYTE, pPixelData); 
    char BMP_Header[54] = {(char)0x42, (char)0x4D, (char)0x36, (char)0x2C, (char)0x22, (char)0x00, (char)0x00, (char)0x00, (char)0x00,\
                           (char)0x00, (char)0x36, (char)0x00, (char)0x00, (char)0x00, (char)0x28, (char)0x00, (char)0x00, (char)0x00,\
                           (char)0x58, (char)0x02, (char)0x00, (char)0x00, (char)0x90, (char)0x01, (char)0x00, (char)0x00, (char)0x01,\
                           (char)0x00, (char)0x18, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x2C,\
                           (char)0x22, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00,\
                           (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00}; 
    fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
    fseek(pWritingFile, 0, SEEK_END);
    fwrite(pPixelData, PixelDataLength, 1, pWritingFile);
    fclose(pWritingFile);
    free(pPixelData);
}

void OpenGLWindow::openFile(char *filepath)
{
    FILE *pOpenFile = fopen(filepath, "rb");
    for(int i = 0; i < 54; ++i) fgetc(pOpenFile); 
    for(int i = 0; i < width() * height(); ++i)
    {
        double Blue, Green, Red;
        GLubyte buffer;
        fread(&buffer, sizeof(GLubyte), 1, pOpenFile);
        Blue = (double)buffer/255.0;
        fread(&buffer, sizeof(GLubyte), 1, pOpenFile);
        Green = (double)buffer/255.0;
        fread(&buffer, sizeof(GLubyte), 1, pOpenFile);
        Red = (double)buffer/255.0;
        if(!(Blue == Green && Green == Red && Green > 0.9))
        {
            Entity *temp = new Entity;
            temp->chosen = false;
            temp->type = 0;
            temp->color[0] = Red;
            temp->color[1] = Green;
            temp->color[2] = Blue;
            temp->pid = currentID;
            temp->size = 2;
            temp->x = i % width();
            temp->y = i / width();
            points.push_back(temp);
        }
    }
    fclose(pOpenFile);
    ++currentID;
    update();
}

void OpenGLWindow::newPaint()
{

    cleanPoints();
    cleanTempPoints();
    cleanTrashPoints();
    cleanChosenPoints();

    currentMode = -1;
    currentPointSize = 5;
    currentID = 0;
    currentColor[0] = 1;
    currentColor[1] = 170/255;
    currentColor[2] = 0;
    isChoosingPoints = false;
    isNewChosen = false;
    setMouseTracking(false);

    update();
}

int OpenGLWindow::getTrashPointsAmounts()
{
    int rtn = 0;
    int cr = -1;
    for(unsigned i = 0; i < trashPoints.size(); ++i)
        if((int)(trashPoints[i]->pid) != cr)
        {
            rtn++;
            cr = trashPoints[i]->pid;
        }
    return rtn;
}

unsigned OpenGLWindow::getPosByPID(unsigned id)
{
    if(points.size() == 0)
        return 0;
    if(id < points[0]->pid)
        return 0;
    for(unsigned i = 1; i < points.size(); ++i)
    {
        if(id < points[i]->pid)
            return i;
    }
    return points.size();
}

void OpenGLWindow::show3D()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DITHER);
    glShadeModel(GL_SMOOTH);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    glRotatef(AngleX,1.0f,0.0f,0.0f);
    glRotatef(AngleY,0.0f,1.0f,0.0f);

    glBegin(GL_POLYGON); 
    glColor3ub((GLubyte)255,(GLubyte)255,(GLubyte)255);
    glVertex3f(rectSize,rectSize,rectSize);
    glColor3ub((GLubyte)255,(GLubyte)255,(GLubyte)0);
    glVertex3f(rectSize,-rectSize,rectSize);
    glColor3ub((GLubyte)255,(GLubyte)0,(GLubyte)0);
    glVertex3f(-rectSize,-rectSize,rectSize);
    glColor3ub((GLubyte)255,(GLubyte)0,(GLubyte)255);
    glVertex3f(-rectSize,rectSize,rectSize);
    glEnd();

    glBegin(GL_POLYGON); 
    glColor3f(0.0f,1.0f,1.0f);
    glVertex3f(rectSize,rectSize,-rectSize);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f(rectSize,-rectSize,-rectSize);
    glColor3f(0.0f,0.0f,0.0f);
    glVertex3f(-rectSize,-rectSize,-rectSize);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(-rectSize,rectSize,-rectSize);
    glEnd();

    glBegin(GL_POLYGON); 
    glColor3d(0.0,1.0,1.0);
    glVertex3f(rectSize,rectSize,-rectSize);
    glColor3d(1.0,1.0,1.0);
    glVertex3f(rectSize,rectSize,rectSize);
    glColor3d(1.0,0.0,1.0);
    glVertex3f(-rectSize,rectSize,rectSize);
    glColor3d(0.0,0.0,1.0);
    glVertex3f(-rectSize,rectSize,-rectSize);
    glEnd();

    glBegin(GL_POLYGON); 
    glColor3ub(0u,255u,0u);
    glVertex3f(rectSize,-rectSize,-rectSize);
    glColor3ub(255u,255u,0u);
    glVertex3f(rectSize,-rectSize,rectSize);
    glColor3ub(255u,0u,0u);
    glVertex3f(-rectSize,-rectSize,rectSize);
    glColor3ub(0u,0u,0u);
    glVertex3f(-rectSize,-rectSize,-rectSize);
    glEnd();

    glBegin(GL_POLYGON); 
    glColor3ub((GLubyte)255,(GLubyte)255,(GLubyte)255);
    glVertex3f(rectSize,rectSize,rectSize);
    glColor3ub((GLubyte)0,(GLubyte)255,(GLubyte)255);
    glVertex3f(rectSize,rectSize,-rectSize);
    glColor3ub((GLubyte)0,(GLubyte)255,(GLubyte)0);
    glVertex3f(rectSize,-rectSize,-rectSize);
    glColor3ub((GLubyte)255,(GLubyte)255,(GLubyte)0);
    glVertex3f(rectSize,-rectSize,rectSize);
    glEnd();

    glBegin(GL_POLYGON); 
    glColor3f(1.0f,0.0f,1.0f);
    glVertex3f(-rectSize,rectSize,rectSize);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(-rectSize,rectSize,-rectSize);
    glColor3f(0.0f,0.0f,0.0f);
    glVertex3f(-rectSize,-rectSize,-rectSize);
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f(-rectSize,-rectSize,rectSize);
    glEnd();

    glPopMatrix();
}
