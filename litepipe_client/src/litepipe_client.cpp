/***************************************************************************
 *   Copyright (C) 2010 by Berkin Ilbeyi,,,   *
 *   nickrebp@nick-xubuntu   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QtGui>
#include "litepipe_client.h"

#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QString>

#include <iostream>

#include <string.h>
#include <unistd.h>

#include <time.h>

#define BACK 1
#define FORWARD 2
#define RELOAD 3

litepipe_client *litepipe_client::instance = NULL;

litepipe_client::litepipe_client()
{
    textEdit = new HtmlViewer(this);
    setCentralWidget(textEdit);
    textEdit->setReadOnly(TRUE);
      
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
      
    readSettings();
      
      
      
    connect(this, SIGNAL(handleCommunicationEventSignal(int, void*)), this, SLOT(handleCommunicationEventSlot(int, void*)));
      
    
    timeConn = NULL;
    infoConn = NULL;
    httpConn = NULL;
    
    httpNode = new PageNode(NULL, NULL, tr("index.html"));
    infoNode = new PageNode(NULL, NULL, tr("HELP"));

    mode = 0;
    setMode(HTTP);
    //reload();
}


void litepipe_client::goBack() {
    navKeyClicked = BACK;
    if (currentNode()->prev() != NULL)
        request(currentNode()->prev()->pageName);
}

void litepipe_client::goForward() {
    navKeyClicked = FORWARD;
    if (currentNode()->next() != NULL)
        request(currentNode()->next()->pageName);
}
    
void litepipe_client::reload() {
    navKeyClicked = RELOAD;
    request(currentNode()->pageName);
}

void litepipe_client::setMode(int newMode) {
    if (mode == newMode)
        return;
    mode = newMode;
    infoAct->setChecked(mode == INFO);
    httpAct->setChecked(mode == HTTP);
    reload();
}
    
void litepipe_client::infoActive(bool active) {
    if (active)
        setMode(INFO);
}
    
void litepipe_client::httpActive(bool active) {
    if (active)
        setMode(HTTP);
}

void HtmlViewer::mousePressEvent(QMouseEvent *event) {
    //std::cout << "mouse event" << std::endl;
    QString anchor = anchorAt(event->pos());
    //if (!anchor.isEmpty())
    //mainWindow->setStatus(QString("Anchor clicked (%1, %2): %3").arg(event->pos().x()).arg(event->pos().y()).arg(anchor));
    
    if (!anchor.isEmpty())
        mainWindow->request(anchor);
    mainWindow->request(TIME, "a");
    QTextEdit::mousePressEvent(event);
}
/*
void litepipe_client::about()
{
    QMessageBox::about(this, tr("About Application"),
                       tr("The <b>Application</b> example demonstrates how to "
                               "write modern GUI applications using Qt, with a menu bar, "
                               "toolbars, and a status bar."));
}
*/
void litepipe_client::createActions()
{
    
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    
    backAct = new QAction(QIcon("../images/back_3_48.png"), tr("Back"), this);
    backAct->setShortcut(tr("Alt+Left"));
    backAct->setStatusTip(tr("Go back"));
    connect(backAct, SIGNAL(triggered()), this, SLOT(goBack()));
    
    forwardAct = new QAction(QIcon("../images/forward_23_48.png"), tr("Forward"), this);
    forwardAct->setShortcut(tr("Alt+Right"));
    forwardAct->setStatusTip(tr("Go forward"));
    connect(forwardAct, SIGNAL(triggered()), this, SLOT(goForward()));
    
    reloadAct = new QAction(QIcon("../images/reload_46_48.png"), tr("&Reload"), this);
    reloadAct->setShortcut(tr("Ctrl+R"));
    reloadAct->setStatusTip(tr("Reload the page"));
    connect(reloadAct, SIGNAL(triggered()), this, SLOT(reload()));
    
    infoAct = new QAction(QIcon("../images/gear_24_48.png"), tr("&Info Mode"), this);
    infoAct->setShortcut(tr("Ctrl+I"));
    infoAct->setStatusTip(tr("Get info on the server"));
    infoAct->setCheckable(TRUE);
    connect(infoAct, SIGNAL(toggled(bool)), this, SLOT(infoActive(bool)));
    
    httpAct = new QAction(QIcon("../images/ftp_1661_48.png"), tr("&HTTP Mode"), this);
    httpAct->setShortcut(tr("Ctrl+H"));
    httpAct->setStatusTip(tr("Browse the HTTP"));
    httpAct->setCheckable(TRUE);
    connect(httpAct, SIGNAL(toggled(bool)), this, SLOT(httpActive(bool)));
    
    /*
    newAct = new QAction(QIcon(":/filenew.xpm"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/fileopen.xpm"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/filesave.xpm"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

   

    cutAct = new QAction(QIcon(":/editcut.xpm"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/editcopy.xpm"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
            "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/editpaste.xpm"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
            "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
    */
}

void litepipe_client::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(backAct);
    fileMenu->addAction(forwardAct);
    fileMenu->addAction(reloadAct);
    fileMenu->addSeparator();
    fileMenu->addAction(httpAct);
    fileMenu->addAction(infoAct);
    /*
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    */
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    /*
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    */
}

void litepipe_client::createToolBars()
{
    navigationToolBar = addToolBar(tr("Navigation"));
    navigationToolBar->addAction(backAct);
    navigationToolBar->addAction(forwardAct);
    navigationToolBar->addSeparator();
    navigationToolBar->addAction(reloadAct);
    
    modeToolBar = addToolBar(tr("Mode"));
    modeToolBar->addAction(httpAct);
    modeToolBar->addAction(infoAct);
    
    /*
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    */
}

void litepipe_client::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void litepipe_client::readSettings()
{
    QSettings settings("Trolltech", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void litepipe_client::writeSettings()
{
    QSettings settings("Trolltech", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}



QString litepipe_client::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void litepipe_client::setStatus(QString status) {
    statusBar()->showMessage(status);
}

litepipe_client::~litepipe_client()
{

}

void litepipe_client::handleCommunicationEvent(int status, void *data) {
    emit instance->handleCommunicationEventSignal(status, data);
    
}

void litepipe_client::request(QString pageName) {
    request(0, pageName.toStdString().c_str());
}

void litepipe_client::request(int protocol, const char *message) {
    if (protocol == 0)
        protocol = mode;
    RemoteConnection *remote;
    switch (protocol) {
        case TIME:
            remote = timeConn;
            break;
        case INFO:
            remote = infoConn;
            break;
        case HTTP:
            remote = httpConn;
            break;
        default:
            return;
    }
    if (remote != NULL)
        sendData(remote, message, strlen(message) + 1);
}

bool litepipe_client::handleIncomingPage(QString pageName) {
    PageNode **node = (mode == HTTP) ? &httpNode : &infoNode;
    
    if (navKeyClicked == 0) {
        *node = (*node)->setNext(pageName);
    } else if (navKeyClicked == BACK) {
        if ((*node)->prev() == NULL || (*node)->prev()->pageName != pageName)
            return FALSE;
        *node = (*node)->prev();
    } else if (navKeyClicked == FORWARD) {
        if ((*node)->next() == NULL || (*node)->next()->pageName != pageName)
            return FALSE;
        *node = (*node)->next();
    } else if (navKeyClicked == RELOAD) {
        if ((*node)->pageName != pageName)
            return FALSE;
    }
    
    backAct->setEnabled((*node)->prev() != NULL);
    forwardAct->setEnabled((*node)->next() != NULL);
    
    navKeyClicked = 0;
    return TRUE;
}

void litepipe_client::handleCommunicationEventSlot(int status, void *data) {
    if (status == HANDLE_NEW_CONNECTION) {
        struct RemoteConnection *remote = (struct RemoteConnection *) data;
        switch (remote->protocol) {
            case TIME:
                timeConn = remote;
                
                //initialize the timer call here
                break;
            case INFO:
                infoConn = remote;
                
                
                break;
            case HTTP:
                httpConn = remote;
                reload();
                //request(HTTP, "index.html");
                break;
            default:
                break;
        }
        
    } else if (status == HANDLE_NEW_DATA) {
        struct IncomingData *inData = (struct IncomingData *) data;
        char *localBuf = (char *) inData->data;
                //new char[inData->ndata]; //TODO: try again with inData
        //memcpy(localBuf, inData->data, inData->ndata);
        
        if (inData->remoteConnection.protocol == TIME) {
                setStatus(tr(ctime((const time_t *) localBuf)));
                //textEdit->setText(QString("%1").arg());

        } else if (inData->remoteConnection.protocol == mode) {
            
                std::cerr << (char *) data << std::endl;
                QString fileName(localBuf);
                localBuf += fileName.size() + 1;
                int dataLen = inData->ndata - fileName.size() - 1;
                if (requestedResources.find(fileName) != requestedResources.end()) {
                    std::cerr << "resource matched: " << fileName.toStdString() << " size: " << dataLen << std::endl;
                    textEdit->document()->addResource(requestedResources[fileName], QUrl(fileName), QVariant(QByteArray((const char *) (inData->data + fileName.size() + 1), dataLen - 1)));
                    requestedResources.erase(fileName);
                    //textEdit->repaint();
                    reload(); //NOTE: this is a little ugly, but whatever...
                } else {
                    if (!handleIncomingPage(fileName))
                        return;
                    textEdit->setText((char*)localBuf);
                    setWindowTitle(fileName);
                    
                }
            
        }
    } else if (status == HANDLE_CONNECTION_BROKEN) {
        
        
    }
}

int litepipe_client::execute() {
    QApplication app(0, NULL);
    instance = new litepipe_client();
    instance->show();
    setHandler(&litepipe_client::handleCommunicationEvent);
    issueCommunicationThread(CLIENT, "localhost", TIME);
    issueCommunicationThread(CLIENT, "localhost", INFO);
    issueCommunicationThread(CLIENT, "localhost", HTTP);
    
    instance->request(TIME, "a");
    
    return app.exec();
}


QVariant HtmlViewer::loadResource(int type, const QUrl &name) {
    std::cerr << "load resource " << type << " " << name.toString().toStdString() << std::endl;
    if (mainWindow->requestedResources.find(name.toString()) ==   mainWindow->requestedResources.end()) {
        mainWindow->requestedResources[name.toString()] = type;
        mainWindow->request(0, name.toString().toStdString().c_str());
    }
    return QTextEdit::loadResource(type, name);
    
}

HtmlViewer::HtmlViewer(litepipe_client *mainWindow): QTextEdit() {
    this->mainWindow = mainWindow;
}

HtmlViewer::~HtmlViewer() {

}
    

