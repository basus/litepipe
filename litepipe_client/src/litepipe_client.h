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

class QAction;
class QMenu;
class QTextEdit;

class HtmlViewer;

class litepipe_client:public QMainWindow
{
      Q_OBJECT

public:
      litepipe_client();
      ~litepipe_client();
      void setStatus(QString status);
      
      static int execute();
      static void handleCommunicationEvent(int status, void *data);

protected:
      void closeEvent(QCloseEvent *event);

private slots:
      void newFile();
      void open();
      bool save();
      bool saveAs();
      void about();
      void documentWasModified();
      void handleCommunicationEventSlot(int status, void *data);

signals:
    void handleCommunicationEventSignal(int status, void *data);
      
private:
      void createActions();
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void readSettings();
      void writeSettings();
      bool maybeSave();
      void loadFile(const QString &fileName);
      bool saveFile(const QString &fileName);
      void setCurrentFile(const QString &fileName);
      QString strippedName(const QString &fullFileName);

      HtmlViewer *textEdit;
      QString curFile;

      QMenu *fileMenu;
      QMenu *editMenu;
      QMenu *helpMenu;
      QToolBar *fileToolBar;
      QToolBar *editToolBar;
      QAction *newAct;
      QAction *openAct;
      QAction *saveAct;
      QAction *saveAsAct;
      QAction *exitAct;
      QAction *cutAct;
      QAction *copyAct;
      QAction *pasteAct;
      QAction *aboutAct;
      QAction *aboutQtAct;
      
      static litepipe_client *instance;
};

class HtmlViewer: public QTextEdit {
    public:
        HtmlViewer(litepipe_client *mainWindow);
        ~HtmlViewer();
        
    protected:
        void mousePressEvent(QMouseEvent *event);
        
    private:
        litepipe_client *mainWindow;
};

#endif
