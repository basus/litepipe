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

      
      
      
    connect(this, SIGNAL(handleCommunicationEventSignal(int, void*)), this, SLOT(handleCommunicationEventSlot(int, void*)));
      
    
    timeConn = NULL;
    infoConn = NULL;
    httpConn = NULL;
    timeThd = NULL;
    httpThd = NULL;
    infoThd = NULL;
    
    hostName = tr("localhost");
    
    
    backAct->setEnabled(FALSE);
    forwardAct->setEnabled(FALSE);
    reloadAct->setEnabled(FALSE);
    infoAct->setEnabled(FALSE);
    httpAct->setEnabled(FALSE);
    
    //httpNode = new PageNode(NULL, NULL, tr("index.html"));
    //infoNode = new PageNode(NULL, NULL, tr("HELP"));

    //mode = 0;
    //setMode(HTTP);
    //reload();
    ignoreConnectChange = FALSE;
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

void litepipe_client::connectToServer(bool active) {
    if (ignoreConnectChange) {
        ignoreConnectChange = FALSE;
        return;
    }
    if (active) {  //connect
        ignoreConnectChange = TRUE;
        connectAct->setChecked(FALSE);
        bool ok;
        QString newHostName = QInputDialog::getText(this, tr("Connect to server"),
                                            tr("Server name:"), QLineEdit::Normal,
                                                hostName, &ok);
        if (ok && !newHostName.isEmpty()) {
            hostName = newHostName;
            char *hostChArr = new char[hostName.size()];
            strcpy(hostChArr, hostName.toStdString().c_str());
            timeThd = issueCommunicationThread(CLIENT, hostChArr, TIME);
            infoThd = issueCommunicationThread(CLIENT, hostChArr, INFO);
            httpThd = issueCommunicationThread(CLIENT, hostChArr, HTTP);
        }
    } else {  //disconnect
        ignoreBadFileDescriptor = TRUE;
        killCommunication(timeConn);
        killCommunication(infoConn);
        killCommunication(httpConn);
        
        backAct->setEnabled(FALSE);
        forwardAct->setEnabled(FALSE);
        reloadAct->setEnabled(FALSE);
        infoAct->setEnabled(FALSE);
        httpAct->setEnabled(FALSE);
        
        textEdit->setPlainText(tr(""));
        
    }
}

void HtmlViewer::mousePressEvent(QMouseEvent *event) {
    //std::cout << "mouse event" << std::endl;
    QString anchor = anchorAt(event->pos());
    //if (!anchor.isEmpty())
    //mainWindow->setStatus(QString("Anchor clicked (%1, %2): %3").arg(event->pos().x()).arg(event->pos().y()).arg(anchor));
    
    if (!anchor.isEmpty())
        mainWindow->request(anchor);
    QTextEdit::mousePressEvent(event);
}

void litepipe_client::createActions()
{
    
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    
    backAct = new QAction(QIcon("images/back_3_48.png"), tr("Back"), this);
    backAct->setShortcut(tr("Alt+Left"));
    backAct->setStatusTip(tr("Go back"));
    connect(backAct, SIGNAL(triggered()), this, SLOT(goBack()));
    
    forwardAct = new QAction(QIcon("images/forward_23_48.png"), tr("Forward"), this);
    forwardAct->setShortcut(tr("Alt+Right"));
    forwardAct->setStatusTip(tr("Go forward"));
    connect(forwardAct, SIGNAL(triggered()), this, SLOT(goForward()));
    
    reloadAct = new QAction(QIcon("images/reload_46_48.png"), tr("&Reload"), this);
    reloadAct->setShortcut(tr("Ctrl+R"));
    reloadAct->setStatusTip(tr("Reload the page"));
    connect(reloadAct, SIGNAL(triggered()), this, SLOT(reload()));
    
    infoAct = new QAction(QIcon("images/gear_24_48.png"), tr("&Info Mode"), this);
    infoAct->setShortcut(tr("Ctrl+I"));
    infoAct->setStatusTip(tr("Get info on the server"));
    infoAct->setCheckable(TRUE);
    connect(infoAct, SIGNAL(toggled(bool)), this, SLOT(infoActive(bool)));
    
    httpAct = new QAction(QIcon("images/ftp_1661_48.png"), tr("&HTTP Mode"), this);
    httpAct->setShortcut(tr("Ctrl+H"));
    httpAct->setStatusTip(tr("Browse the HTTP"));
    httpAct->setCheckable(TRUE);
    connect(httpAct, SIGNAL(toggled(bool)), this, SLOT(httpActive(bool)));
    
    connectAct = new QAction(QIcon("images/Internet-48.png"), tr("Co&nnect"), this);
    connectAct->setShortcut(tr("Ctrl+N"));
    connectAct->setStatusTip(tr("Connect to the server"));
    connectAct->setCheckable(TRUE);
    connect(connectAct, SIGNAL(toggled(bool)), this, SLOT(connectToServer(bool)));
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
    fileMenu->addSeparator();
    fileMenu->addAction(connectAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);


}

void litepipe_client::createToolBars()
{
    navigationToolBar = addToolBar(tr("Navigation"));
    navigationToolBar->addAction(backAct);
    navigationToolBar->addAction(forwardAct);
    navigationToolBar->addSeparator();
    navigationToolBar->addAction(reloadAct);
    
    modeToolBar = addToolBar(tr("Connection"));
    modeToolBar->addAction(connectAct);
    modeToolBar->addSeparator();
    modeToolBar->addAction(httpAct);
    modeToolBar->addAction(infoAct);
    

}

void litepipe_client::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
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
    std::cout << "Handle incoming page: " << pageName.toStdString() << " key: " << navKeyClicked << std::endl;
    if (navKeyClicked == 0) {
        if ((*node)->pageName != pageName)
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
                instance->request(TIME, "a");
                //initialize the timer call here
                break;
            case INFO:
                infoConn = remote;
                
                
                break;
            case HTTP:
                httpConn = remote;
                
                ignoreConnectChange = TRUE;
                connectAct->setChecked(TRUE);
                
                reloadAct->setEnabled(TRUE);
                infoAct->setEnabled(TRUE);
                httpAct->setEnabled(TRUE);
                
                httpNode = new PageNode(NULL, NULL, tr("index.html"));
                infoNode = new PageNode(NULL, NULL, tr("HELP"));
                
                mode = 0;
                setMode(HTTP);
                
                //reload();
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
                    textEdit->document()->addResource(requestedResources[fileName], QUrl(fileName), QVariant(QByteArray((const char *) ((char *)inData->data + fileName.size() + 1), dataLen - 1)));
                    requestedResources.erase(fileName);
                    textEdit->setText(textEdit->toHtml());
                } else {
                    std::cerr << (char *) localBuf << std::endl;
                    if (!handleIncomingPage(fileName))
                        return;
                    textEdit->setText((char*)localBuf);
                    setWindowTitle(fileName);
                    
                }
            
        }
    } else if (status == HANDLE_CONNECTION_BROKEN) {
        std::cout << "connection broken handle" << std::endl;
        connectAct->setChecked(FALSE);
        /*
        backAct->setEnabled(FALSE);
        forwardAct->setEnabled(FALSE);
        reloadAct->setEnabled(FALSE);
        infoAct->setEnabled(FALSE);
        httpAct->setEnabled(FALSE);
        
        textEdit->setPlainText(tr(""));
        */
        
    } else if (status == HANDLE_ERROR) {
        std::cout << "error handle: " << (char *) data <<  std::endl;
        if (ignoreBadFileDescriptor && tr((char*) data) == tr("Bad file descriptor"))
            return;
        QMessageBox::critical(this, tr("Error"), tr((char *) data));
    }
}

int litepipe_client::execute(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setOverrideCursor(Qt::ArrowCursor);
    setHandler(&litepipe_client::handleCommunicationEvent);
    instance = new litepipe_client();
    instance->show();
    
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
    

