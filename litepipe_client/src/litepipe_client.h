/***************************************************************************
 *   Copyright (C) 2010 by Berkin Ilbeyi*
 *
This class represents the GUI client implemented in Qt. For the HTML rendering, it
uses QTextEdit.
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


#ifndef LITEPIPE_CLIENT_H
#define LITEPIPE_CLIENT_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QStatusBar>
#include <QTextEdit>
#include <map>

extern "C" {
    #include "protocol.h"
}

class QAction;
class QMenu;
class QTextEdit;
class HtmlViewer;
class PageNode;

class litepipe_client:public QMainWindow {
    Q_OBJECT

public:
    /**
    * The constructor
    */
    litepipe_client();
    
    /**
    * The destructor
    */
    ~litepipe_client();
    
    /**
    * Sets the status string at the bottom of the window
    * @arg status the status message
    */
    void setStatus(QString status);

    /**
    * Static function to initiate the client. In addition to constructing litepipe_client,
    * also handles the Qt specific initialization.
    */
    static int execute(int argc, char **argv);
    
    /**
    * The handler of the client, that provides gui realization to the network events.
     * Will emit handleCommunicationEventSignal to switch the thread of the call
    */
    static void handleCommunicationEvent(int status, void *data);

    /**
    * Requests a resource from the server for the given resource name
    */
    void request(int protocol, const char *message);
    void request(QString pageName);

    /**
    * The map to store all the internally requested resources
    *
    */
    std::map<QString, int> requestedResources;


private slots:

    /**
    * The gui control slots
    *
    */
    void goBack();
    void goForward();
    void reload();
    void infoActive(bool);
    void httpActive(bool);

    void connectToServer(bool);

    /**
     * handleCommunicationEventSignal will arrive to this slot. This will do the
    * appropriate actions depending on the call.
    */
    void handleCommunicationEventSlot(int status, void *data);

signals:
    void handleCommunicationEventSignal(int status, void *data);

private:

    HtmlViewer *textEdit;
    QString hostName;
    static litepipe_client *instance;
    struct RemoteConnection *timeConn, *infoConn, *httpConn;
    pthread_t *timeThd, *infoThd, *httpThd;
    int mode;
    int navKeyClicked;
    bool ignoreConnectChange;
    bool ignoreBadFileDescriptor;

    PageNode *httpNode;
    PageNode *infoNode;

    void setMode(int);

    PageNode *currentNode() {
        return mode == HTTP ? httpNode : infoNode;
    }


    bool handleIncomingPage(QString pageName);

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    QString strippedName(const QString &fullFileName);



    QMenu *fileMenu;
    QToolBar *navigationToolBar;
    QToolBar *modeToolBar;


    QAction *backAct;
    QAction *forwardAct;
    QAction *reloadAct;
    QAction *infoAct;
    QAction *httpAct;
    QAction *exitAct;
    QAction *connectAct;
};

/**
* This is the html viewer of the client, extending Qt's QTextEdit
*
*
*/
class HtmlViewer: public QTextEdit {
public:
    /**
    * Constructor with a reference to the litepipe_client
    */
    HtmlViewer(litepipe_client *mainWindow);
    ~HtmlViewer();

    
    /**
    * The resource request call. This will request the additional resources (like images)
    * from the server.
    */
    QVariant loadResource(int type, const QUrl &name);

protected:
    /**
    * Catches the mouse presses, to check if a link is clicked, request that page if a link 
    * has been clicked.
    */
    void mousePressEvent(QMouseEvent *event);

private:
    litepipe_client *mainWindow;
};

/**
* The node for a simplistic doubly linked list for the back/forward buttons
*
*
*/
class PageNode {
public:
    /**
    * Constructs the node
    *
    */
    PageNode(PageNode *prev, PageNode *next, QString pageName) {
        p = prev;
        n = next;
        this->pageName = pageName;
    }

    /**
    * The destructor also destroys the following elements, useful to change the 
    * path of the history if a different link is clicked on a page that was accessed
    * with back button.
    */
    ~PageNode() {
        if (n != NULL)
            delete n;
    }

    PageNode *prev() {
        return p;
    }

    PageNode *next() {
        return n;
    }

    /**
    * Sets the next node element, clearing the old next if there was any.
    *
    */
    PageNode *setNext(QString pageName) {
        if (n != NULL)
            delete n;
        return n = new PageNode(this, NULL, pageName);
    }

    QString pageName;

private:
    PageNode *p, *n;

};

#endif
