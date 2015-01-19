﻿#include "LoginUI.h"
#include <QMessageBox>
LoginUI::LoginUI(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    pushButton_login->setShortcut(QKeySequence::InsertParagraphSeparator);	//将回车键与登录按钮绑定在一起
}

void LoginUI::on_pushButton_login_clicked()
{
    //自动填写。。。
    lineEdit_ID->setText("123");
    lineEdit_PW->setText("123");
    USER::GetInstance().setId(lineEdit_ID->text().toInt());
    USER::GetInstance().setPassword(lineEdit_PW->text());
    emit this->loginSignal(USER::GetInstance());
}

