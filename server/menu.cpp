﻿#include "menu.h"

Menu::Menu(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    if(User::GetInstance().getType() == 1){
        stackedWidget->setCurrentIndex(1);
    }
    else {
        stackedWidget->setCurrentIndex(0);
    }
}

Menu::~Menu()
{

}

void Menu::on_pushButton_mem_clicked()
{
    emit this->action_memmanager();
}

void Menu::on_pushButton_examsetting_clicked()
{
    emit this->action_examsetting();
}

void Menu::on_pushButton_examctrl_clicked()
{
    emit this->action_examctrl();
}

void Menu::on_pushButton_config_clicked()
{
    emit this->action_config();
}

void Menu::on_pushButton_question_clicked()
{
    emit this->action_QuestionsManager();
}

void Menu::on_pushButton_paper_clicked()
{
    emit this->action_makepaper();
}

void Menu::on_pushButton_papersetting_clicked()
{
    emit this->action_papersetting();
}

void Menu::on_pushButton_subscore_clicked()
{
    emit this->action_subscore();
}

void Menu::on_pushButton_scoremanage_clicked()
{
    emit this->action_scoremanage();
}
