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

extern "C" {
#include "protocol.h"
}

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
      
      connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
      
      
      connect(this, SIGNAL(handleCommunicationEventSignal(int, void*)), this, SLOT(handleCommunicationEventSlot(int, void*)));
      
      setCurrentFile("");
}

void litepipe_client::closeEvent(QCloseEvent *event)
{
      if (maybeSave()) {
            writeSettings();
            event->accept();
      } else {
            event->ignore();
      }
}

void HtmlViewer::mousePressEvent(QMouseEvent *event) {
    std::cout << "mouse event" << std::endl;
    QString anchor = anchorAt(event->pos());
    //if (!anchor.isEmpty())
        mainWindow->setStatus(QString("Anchor clicked (%1, %2): %3").arg(event->pos().x()).arg(event->pos().y()).arg(anchor));
    
}

void litepipe_client::newFile()
{
      if (maybeSave()) {
            textEdit->clear();
            setCurrentFile("");
      }
}

void litepipe_client::open()
{
      if (maybeSave()) {
            QString fileName = QFileDialog::getOpenFileName(this);
            if (!fileName.isEmpty())
            loadFile(fileName);
      }
}

bool litepipe_client::save()
{
      if (curFile.isEmpty()) {
            return saveAs();
      } else {
            return saveFile(curFile);
      }
}

bool litepipe_client::saveAs()
{
      QString fileName = QFileDialog::getSaveFileName(this);
      if (fileName.isEmpty())
            return false;

      return saveFile(fileName);
}

void litepipe_client::about()
{
      QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
                  "write modern GUI applications using Qt, with a menu bar, "
                  "toolbars, and a status bar."));
}

void litepipe_client::documentWasModified()
{
      setWindowModified(true);
}

void litepipe_client::createActions()
{
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

      exitAct = new QAction(tr("E&xit"), this);
      exitAct->setShortcut(tr("Ctrl+Q"));
      exitAct->setStatusTip(tr("Exit the application"));
      connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

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
}

void litepipe_client::createMenus()
{
      fileMenu = menuBar()->addMenu(tr("&File"));
      fileMenu->addAction(newAct);
      fileMenu->addAction(openAct);
      fileMenu->addAction(saveAct);
      fileMenu->addAction(saveAsAct);
      fileMenu->addSeparator();
      fileMenu->addAction(exitAct);

      editMenu = menuBar()->addMenu(tr("&Edit"));
      editMenu->addAction(cutAct);
      editMenu->addAction(copyAct);
      editMenu->addAction(pasteAct);

      menuBar()->addSeparator();

      helpMenu = menuBar()->addMenu(tr("&Help"));
      helpMenu->addAction(aboutAct);
      helpMenu->addAction(aboutQtAct);
}

void litepipe_client::createToolBars()
{
      fileToolBar = addToolBar(tr("File"));
      fileToolBar->addAction(newAct);
      fileToolBar->addAction(openAct);
      fileToolBar->addAction(saveAct);

      editToolBar = addToolBar(tr("Edit"));
      editToolBar->addAction(cutAct);
      editToolBar->addAction(copyAct);
      editToolBar->addAction(pasteAct);
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

bool litepipe_client::maybeSave()
{
      if (textEdit->document()->isModified()) {
            int ret = QMessageBox::warning(this, tr("Application"),
                        tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape);
            if (ret == QMessageBox::Yes)
            return save();
            else if (ret == QMessageBox::Cancel)
            return false;
      }
      return true;
}

void litepipe_client::loadFile(const QString &fileName)
{
      QFile file(fileName);
      if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Application"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
            return;
      }

      QTextStream in(&file);
      QApplication::setOverrideCursor(Qt::WaitCursor);
      if (fileName.endsWith(".htm") || fileName.endsWith(".html"))
          textEdit->setHtml(in.readAll());
      else
        textEdit->setPlainText(in.readAll());
      QApplication::restoreOverrideCursor();

      setCurrentFile(fileName);
      statusBar()->showMessage(tr("File loaded"), 2000);
}

bool litepipe_client::saveFile(const QString &fileName)
{
      QFile file(fileName);
      if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Application"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
            return false;
      }

      QTextStream out(&file);
      QApplication::setOverrideCursor(Qt::WaitCursor);
      out << textEdit->toPlainText();
      QApplication::restoreOverrideCursor();

      setCurrentFile(fileName);
      statusBar()->showMessage(tr("File saved"), 2000);
      return true;
}

void litepipe_client::setCurrentFile(const QString &fileName)
{
      curFile = fileName;
      textEdit->document()->setModified(false);
      setWindowModified(false);

      QString shownName;
      if (curFile.isEmpty())
            shownName = "untitled.txt";
      else
            shownName = strippedName(curFile);

      setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Application")));
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

void litepipe_client::handleCommunicationEventSlot(int status, void *data) {
    if (status == HANDLE_NEW_CONNECTION) {
        struct RemoteConnection *remote = (struct RemoteConnection *) data;
        sendData(remote, (void *) "a", 2);
    } else if (status == HANDLE_NEW_DATA) {
        struct IncomingData *inData = (struct IncomingData *) data;
        switch (inData->remoteConnection.protocol) {
            case TIME:
                 textEdit->setText(QString("%1").arg((long) data));
                 
                 
                
                break;
            case INFO:
                
                
                break;
            case HTTP:
                textEdit->setHtml(QString((char *) data));
                
                break;
            default:
            
                break;
        }
        
    } else if (status == HANDLE_CONNECTION_BROKEN) {
        
        
    }
}

int litepipe_client::execute() {
    QApplication app(0, NULL);
    instance = new litepipe_client();
    instance->show();
    
    setHandler(&litepipe_client::handleCommunicationEvent);
    issueCommunicationThread(CLIENT, "localhost", 70001);
    
    return app.exec();
}

HtmlViewer::HtmlViewer(litepipe_client *mainWindow): QTextEdit() {
    this->mainWindow = mainWindow;
}

HtmlViewer::~HtmlViewer() {

}
    

