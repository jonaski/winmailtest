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

#pragma once

#include <memory>

#include <QObject>
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QList>
#include <QAxObject>

#include "outlook.h"

class QWidget;
class QShowEvent;
class QCloseEvent;
class QLabel;
class QLineEdit;
class Ui_MainWindow;
class AboutDialog;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 protected:
  void showEvent(QShowEvent *e);
  void closeEvent(QCloseEvent *e);

 private slots:
  void Exit();
  void TestOutlookException(const int code, const QString &source, const QString &desc, const QString &help);
  void SelectAttachment();
  void TestMAPI();
  void TestOutlook();

 private:
  void ShowError(const QString &title, const QString &text = QString());
  bool CheckRequiredFields();

 private:
  Ui_MainWindow *ui_;
  AboutDialog *aboutdialog_;

  Outlook::Application outlook_;
  std::unique_ptr<Outlook::MailItem> mailitemptr_;
  std::unique_ptr<IDispatch> mailitem_dispatch_ptr_;
  typedef QPair<QLabel*, QLineEdit*> Field;
  typedef QList<Field> FieldList;
  FieldList required_fields_;
  ///QAxBase *ax_base_;

};
