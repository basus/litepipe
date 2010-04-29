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


#ifndef LITEPIPE_CLIENT_H
#define LITEPIPE_CLIENT_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QStatusBar>
#include <QTextEdit>
#include <map>
//#include <list>

extern "C" {
#include "protocol.h"
}

class QAction;
class QMenu;
class QTextEdit;

class HtmlViewer;

class PageNode;

//struct pthread_t;

class litepipe_client:public QMainWindow
{
      Q_OBJECT

public:
      litepipe_client();
      ~litepipe_client();
      void setStatus(QString status);
      
      static int execute(int argc, char **argv);
      static void handleCommunicationEvent(int status, void *data);
      
      void request(int protocol, const char *message); 
      void request(QString pageName);

      std::map<QString, int> requestedResources;


private slots:
      
    void goBack();
    void goForward();
    void reload();
    void infoActive(bool);
    void httpActive(bool);

    void connectToServer(bool);
    
      /*
      void newFile();
      void open();
      bool save();
      bool saveAs();
      void about();
      void documentWasModified();
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

class HtmlViewer: public QTextEdit {
    public:
        HtmlViewer(litepipe_client *mainWindow);
        ~HtmlViewer();
        
        QVariant loadResource(int type, const QUrl &name); 
        
        
        
    protected:
        void mousePressEvent(QMouseEvent *event);
        
    private:
        litepipe_client *mainWindow;
};

class PageNode {
    public:
        PageNode(PageNode *prev, PageNode *next, QString pageName) {
            p = prev;
            n = next;
            this->pageName = pageName;
        }
        
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
