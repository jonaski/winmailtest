/*
   Win Mail Tester
   Copyright 2020, Jonas Kvinge <jonas@jkvinge.net>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <windows.h>
#include <mapi.h>
#include <memory>
#include <string.h>

#include <QtGlobal>
#include <QObject>
#include <QMainWindow>
#include <QApplication>
#include <QGuiApplication>
#include <QAxObject>
#include <QScreen>
#include <QList>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QVariant>
#include <QByteArray>
#include <QString>
#include <QIcon>
#include <QIODevice>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QAction>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QSettings>
#include <QDebug>
#include <QShowEvent>
#include <QCloseEvent>

#include "iconloader.h"
#include "logging.h"
#include "aboutdialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "outlook.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
  ui_(new Ui_MainWindow),
  aboutdialog_(new AboutDialog(this)),
  mapi_lib_(nullptr),
  mailitemptr_(nullptr),
  mailitem_dispatch_ptr_(nullptr) {

  ui_->setupUi(this);

  ui_->action_about_qt->setIcon(QIcon(":/qt-project.org/qmessagebox/images/qtlogo-64.png"));
  ui_->action_about->setIcon(IconLoader::Load("internet-mail"));
  ui_->action_exit->setIcon(IconLoader::Load("application-exit"));
  ui_->button_select_attachment->setIcon(IconLoader::Load("document-open-folder"));

  ui_->button_select_attachment->setIcon(IconLoader::Load("internet-mail"));
  ui_->button_test_mapi->setIcon(IconLoader::Load("internet-mail"));
  ui_->button_test_outlook->setIcon(IconLoader::Load("internet-mail"));
  ui_->button_exit->setIcon(IconLoader::Load("application-exit"));

  connect(ui_->button_exit, SIGNAL(clicked()), SLOT(Exit()));
  connect(ui_->action_exit, SIGNAL(triggered()), SLOT(Exit()));
  connect(ui_->action_about, SIGNAL(triggered()), aboutdialog_, SLOT(show()));
  connect(ui_->action_about_qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  connect(ui_->button_select_attachment, SIGNAL(clicked()), SLOT(SelectAttachment()));
  connect(ui_->button_test_mapi_logon, SIGNAL(clicked()), SLOT(TestMAPILogon()));
  connect(ui_->button_test_mapi, SIGNAL(clicked()), SLOT(TestMAPISendMail()));
  connect(ui_->button_test_outlook, SIGNAL(clicked()), SLOT(TestOutlook()));

  required_fields_ = FieldList() << qMakePair(ui_->label_to_name, ui_->to_name)
                                 << qMakePair(ui_->label_to_addr, ui_->to_addr)
                                 << qMakePair(ui_->label_subject, ui_->subject);

  mapi_lib_ = LoadLibrary(TEXT("mapi32.dll"));
  if (!mapi_lib_) {
    ShowError(tr("Unable to load mapi32.dll library"));
    qApp->quit();
  }

  show();

}

MainWindow::~MainWindow() {
  if (mapi_lib_) FreeLibrary(mapi_lib_);
  delete ui_;
}

void MainWindow::showEvent(QShowEvent*) {

  QSettings s;
  s.beginGroup(qApp->applicationName());
  if (s.contains("geometry")) {
    restoreGeometry(s.value("geometry").toByteArray());
  }
  s.endGroup();
  
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
  QScreen *screen = QGuiApplication::screenAt(pos());
  if (screen) {
    const QRect sr = screen->availableGeometry();
    const QRect wr({}, size().boundedTo(sr.size()));
    resize(wr.size());
    move(sr.center() - wr.center());
  }
#endif

}

void MainWindow::closeEvent(QCloseEvent*) {
  Exit();
}

void MainWindow::Exit() {

  QSettings s;
  s.beginGroup(qApp->applicationName());
  s.setValue("geometry", saveGeometry());
  s.endGroup();

  qApp->exit();

}

void MainWindow::TestOutlookException(const int code, const QString &source, const QString &desc, const QString &help) {

  ShowError(source, QString("Error code %1: %2 %3").arg(code).arg(desc).arg(help).trimmed());

}

void MainWindow::SelectAttachment() {

  QFileDialog dialog;
  dialog.setFileMode(QFileDialog::AnyFile);
  QString filename = dialog.getOpenFileName(this, tr("Select file for attachment."));
  if (filename.isEmpty()) return;

  QFileInfo info(filename);
  if (!info.exists()) {
    ShowError(tr("File does not exist."), tr("File %1 does not exist.").arg(filename));
    return;
  }

  if (!info.isReadable()) {
    ShowError(tr("File is not readable."), tr("File %1 is not readable."));
    return;
  }

  ui_->attachment->setText(QDir::toNativeSeparators(filename));

}

void MainWindow::ShowError(const QString &title, const QString &text) {

  QMessageBox box(QMessageBox::Critical, title, (text.isEmpty() ? title : text), QMessageBox::Close);
  box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
  box.exec();

}

bool MainWindow::CheckRequiredFields() {

  for (Field field : required_fields_) {
    if (field.second->text().isEmpty()) {
      ShowError(tr("Missing %1.").arg(field.first->text()));
      return false;
    }
  }
  if (ui_->text_body->toPlainText().isEmpty()) {
    ShowError(tr("Missing body text."));
    return false;
  }
  return true;

}

void MainWindow::TestMAPILogon() {

  if (!mapi_lib_) return;

#if 0
  HINSTANCE mapi_lib = LoadLibrary(TEXT("mapi32.dll"));
  if (!mapi_lib) {
    ShowError(tr("Unable to load mapi32.dll library"));
    return;
  }
#endif

  LPMAPILOGON mapi_logon = reinterpret_cast<LPMAPILOGON>(GetProcAddress(mapi_lib_, TEXT("MAPILogon")));
  if (!mapi_logon) {
    ShowError(tr("Unable to load MAPILogon"));
    //FreeLibrary(mapi_lib);
    return;
  }

  LHANDLE mapi_session;
  ULONG ret = mapi_logon(0, nullptr, nullptr, MAPI_NEW_SESSION, 0, &mapi_session);

  if (ret != SUCCESS_SUCCESS && ret != MAPI_E_USER_ABORT) {
    ShowError(tr("MAPILogon failed: %1").arg(ret));
  }
  else {
    QMessageBox box(QMessageBox::Information, "MAPILogon", "MAPILogon was successful!", QMessageBox::Close);
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.exec();
  }

  //FreeLibrary(mapi_lib);

}

void MainWindow::TestMAPISendMail() {

  if (!CheckRequiredFields()) return;
  if (!mapi_lib_) return;

#if 0
  HINSTANCE mapi_lib = LoadLibrary(TEXT("mapi32.dll"));
  if (!mapi_lib) {
    ShowError(tr("Unable to load mapi32.dll library"));
    return;
  }
#endif

  LPMAPISENDMAIL mapi_sendmail = reinterpret_cast<LPMAPISENDMAIL>(GetProcAddress(mapi_lib_, TEXT("MAPISendMail")));
  if (!mapi_sendmail) {
    ShowError(tr("Unable to load MAPISendMail"));
    //FreeLibrary(mapi_lib);
    return;
  }

  QString to_addr = "smtp:" + ui_->to_addr->text().trimmed();
  char sz_address[MAX_PATH] = {};
  strcat_s(sz_address, to_addr.toStdString().c_str());

  QString to_name = ui_->to_name->text().trimmed();
  char sz_name[MAX_PATH] = {};
  strcat_s(sz_name, to_name.toStdString().c_str());

  MapiRecipDesc mapi_recipient[1] = {};
  mapi_recipient[0].ulRecipClass = MAPI_TO;
  mapi_recipient[0].lpszAddress = sz_address;
  mapi_recipient[0].lpszName = sz_name;

  char mapi_subject[MAX_PATH] = {};
  strcat_s(mapi_subject, ui_->subject->text().toLocal8Bit().constData());

  char mapi_text[MAX_PATH] = {};
  strcat_s(mapi_text, ui_->text_body->toPlainText().toLocal8Bit().constData());

  MapiMessage mapi_msg = {};
  mapi_msg.lpszSubject = mapi_subject;
  mapi_msg.lpRecips = mapi_recipient;
  mapi_msg.nRecipCount = 1;
  mapi_msg.lpszNoteText = mapi_text;
  mapi_msg.lpszDateReceived = 0;

  MapiFileDesc mapi_file = {};
  memset(&mapi_file, 0, sizeof(MapiFileDesc));
  if (!ui_->attachment->text().isEmpty()) {
    char drive[_MAX_DRIVE] = {};
    char dir[_MAX_DIR] = {};
    char name[_MAX_FNAME] = {};
    char ext[_MAX_EXT] = {};
    LPCSTR lpsz_fullfileName = ui_->attachment->text().toLocal8Bit().constData();
    _splitpath_s(lpsz_fullfileName, drive, dir, name, ext);
    char filename[MAX_PATH] = {};
    strcat_s(filename, name);
    strcat_s(filename, ext);
    char fullfilename[MAX_PATH] = {};
    strcat_s(fullfilename, lpsz_fullfileName);
    mapi_file.nPosition = 0xFFFFFFFF;
    mapi_file.lpszPathName = fullfilename;
    mapi_file.lpszFileName = filename;
    mapi_msg.nFileCount = 1;
    mapi_msg.lpFiles = &mapi_file;
  }

  ULONG result = mapi_sendmail(0, 0, &mapi_msg, MAPI_LOGON_UI | MAPI_DIALOG, 0);
  if (result != SUCCESS_SUCCESS && result != MAPI_E_USER_ABORT) {
    ShowError(tr("MAPISendMail failure"), tr("MAPISendMail failed error code %1").arg(result));
  }

  //FreeLibrary(mapi_lib);

}

void MainWindow::TestOutlook() {

  if (!CheckRequiredFields()) return;

  mailitemptr_.reset();
  mailitem_dispatch_ptr_.reset(outlook_.CreateItem(Outlook::olMailItem));
  if (!mailitem_dispatch_ptr_) {
    ShowError(tr("CreateItem failed"));
    return;
  }
  mailitemptr_.reset(new Outlook::MailItem(mailitem_dispatch_ptr_.get(), nullptr));
  connect(mailitemptr_.get(), SIGNAL(exception(int, QString, QString, QString)), SLOT(TestOutlookException(int, QString, QString, QString)));

  mailitemptr_->SetSubject(ui_->subject->text().toLocal8Bit());
  mailitemptr_->SetTo(ui_->to_addr->text().trimmed().toLocal8Bit());
  mailitemptr_->SetBody(ui_->text_body->toPlainText().toLocal8Bit());
  mailitemptr_->SetDeleteAfterSubmit(VARIANT_FALSE);
  if (!ui_->attachment->text().isEmpty()) {
    LPCSTR attachment_file = ui_->attachment->text().toLocal8Bit();
    mailitemptr_->Attachments()->Add(attachment_file, Outlook::OlAttachmentType::olByValue);
  }

  mailitemptr_->Send();

}
