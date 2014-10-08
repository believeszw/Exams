﻿#include"mainapp.h"
#include <QMessageBox>
#include <QDebug>
MainApp::MainApp()
{


    readConfig();
    iniMainWindow();
    iniDBManager();
    iniServer();
    IOM=new IOManager;

    serverState=STATE_NOEXAM;
    //在信息字符串前预留一位用来填写考试状态
    infolist.append("*,");
}

MainApp::~MainApp()
{
    delete(DBM);
    delete(server);
    delete(IOM);

}

void MainApp::readConfig()
{
    QFile Config("./config.ini");
    if(!Config.open(QIODevice::ReadOnly|QIODevice::Text))
        return;
    QTextStream in(&Config);
    QString temp;
    temp=in.readLine();
    temp=temp.mid(temp.indexOf(" ")+1);
    DBname=temp;
    temp=in.readLine();
    temp=temp.mid(temp.indexOf(" ")+1);
    DBuser=temp;
    temp=in.readLine();
    temp=temp.mid(temp.indexOf(" ")+1);
    DBpassword=temp;
    temp=in.readLine();
    temp=temp.mid(temp.indexOf(" ")+1);
    port=temp.toInt();
    Config.close();
}

void MainApp::iniDBManager()
{
    DBM=new DBManager(DBname,DBuser,DBpassword);

}

void MainApp::iniServer()
{
    server=new Server(this,port);
    connect(this,SIGNAL(sendData(int,qint32,QVariant)),server,SIGNAL(sendData(int,qint32,QVariant)));
    connect(server,SIGNAL(messageArrive(int,qint32,QVariant)),this,SLOT(messageArrive(int,qint32,QVariant)),Qt::QueuedConnection);
    connect(server,SIGNAL(removeUser(int)),this,SLOT(removeUser(int)));
}

void MainApp::iniMainWindow()
{
    //与题库管理界面连接的信号和槽
    connect(&w,SIGNAL(getQuestions()),this,SLOT(getQuestions()));
    connect(this,SIGNAL(showQuestions(QList<Ob_questions*>,QList<Sub_questions*>)),&w,SIGNAL(showQuestions(QList<Ob_questions*>,QList<Sub_questions*>)));
    connect(&w,SIGNAL(addOb_Questoins(Ob_questions*)),this,SLOT(addOb_Questions(Ob_questions*)));
    connect(&w,SIGNAL(addSub_Questoins(Sub_questions*)),this,SLOT(addSub_Questions(Sub_questions*)));
    connect(&w,SIGNAL(modifyOb_Questoins(Ob_questions*)),this,SLOT(modifyOb_Questoins(Ob_questions*)));
    connect(&w,SIGNAL(modifySub_Questoins(Sub_questions*)),this,SLOT(modifySub_Questoins(Sub_questions*)));
    connect(&w,SIGNAL(deleteOb_Questoins(int)),this,SLOT(deleteOb_Questoins(int)));
    connect(&w,SIGNAL(deleteSub_Questoins(int)),this,SLOT(deleteSub_Questoins(int)));
    //与试卷管理界面连接的信号和槽
    connect(&w,SIGNAL(addPaper(Paper)),this,SLOT(addPaper(Paper)));
    connect(&w,SIGNAL(getAllPaper()),this,SLOT(getAllPaper()));
    connect(this,SIGNAL(showAllPaper(QList<Paper*>)),&w,SIGNAL(showAllPaper(QList<Paper*>)));
    connect(&w,SIGNAL(getPaperById(int)),this,SLOT(getPaperById(int)));
    connect(&w,SIGNAL(deletePaper(int)),this,SLOT(deletePaper(int)));
    connect(&w,SIGNAL(modifyPaper(Paper)),this,SLOT(modifyPaper(Paper)));
    connect(this,SIGNAL(showCurrentPaper(Paper)),&w,SIGNAL(showCurrentPaper(Paper)));
    connect(&w,SIGNAL(saveUsertoPaperMark(int,QList<User*>)),this,SLOT(saveUsertoPaperMark(int,QList<User*>)));

    ////////examctrl
    connect(&w,SIGNAL(sendPaper(int)),this,SLOT(sendPaper(int)));
    connect(&w,SIGNAL(beginExam()),this,SLOT(beginExam()));
    connect(&w,SIGNAL(getUserList()),this,SLOT(getUserList()));
    connect(this,SIGNAL(updateUserTable(QList<User*>)),&w,SIGNAL(updateUserTable(QList<User*>)));
    connect(&w,SIGNAL(endExam()),this,SLOT(endExam()));
    connect(this,SIGNAL(getcurrentPaperTime(int)),&w,SIGNAL(getcurrentPaperTime(int)));
    connect(&w,SIGNAL(sendPaperTime(int,int)),this,SLOT(sendPaperTime(int,int)));
    connect(&w,SIGNAL(sendInfo(QStringList)),this,SLOT(sendInfo(QStringList)));
    //subcore
    connect(&w,SIGNAL(getUserByPaperId(int,QString)),this,SLOT(getUserByPaperId(int,QString)));
    connect(this,SIGNAL(showUserByPaperId(QList<User*>)),&w,SIGNAL(showUserByPaperId(QList<User*>)));
    connect(&w,SIGNAL(getSubAnswer(int,QString)),this,SLOT(getSubAnswer(int,QString)));
    connect(this,SIGNAL(showSubAnswer(QVector<QString>)),&w,SIGNAL(showSubAnswer(QVector<QString>)));
    connect(&w,SIGNAL(submitSubMark(QStringList)),this,SLOT(submitSubMark(QStringList)));
    ///mem
    connect(&w,SIGNAL(getUser()),this,SLOT(getUser()));//mainwindow发，mainapp收
    connect(this,SIGNAL(showUser(QList<User*>,QList<Manager*>)),&w,SIGNAL(showUser(QList<User*>,QList<Manager*>)));
    connect(&w,SIGNAL(addUser(User*)),this,SLOT(addUser(User *)));
    connect(&w,SIGNAL(addManager(Manager*)),this,SLOT(addManager(Manager*)));
    connect(&w,SIGNAL(deleteUserId(QString)),this,SLOT(deleteUserId(QString)));
    connect(&w,SIGNAL(deleteManagerId(int)),this,SLOT(deleteManagerId(int)));
    //login
    connect(&w,SIGNAL(loginSignal(Manager)),this,SLOT(managerLogin(Manager)));
    connect(this,SIGNAL(LoginOK()),&w,SLOT(LoginOK()));
    //scomem
    connect(&w,SIGNAL(getCombo_id(QString)),this,SLOT(getCombo_id(QString)));
    connect(this,SIGNAL(showCombo(QList<Combo*>)),&w,SIGNAL(showCombo(QList<Combo*>)));
    connect(&w,SIGNAL(getCombo_paperid(int)),this,SLOT(getCombo_paperid(int)));
    connect(&w,SIGNAL(delete_score(int,qlonglong)),this,SLOT(delete_score(int,qlonglong)));
    //InOutPutUI
    connect(&w,SIGNAL(inputUser(QString)),this,SLOT(inputUser(QString)));
    connect(&w,SIGNAL(inputOb(QString)),this,SLOT(inputOb(QString)));
    connect(&w,SIGNAL(inputSub(QString)),this,SLOT(inputSub(QString)));
    connect(&w,SIGNAL(inputPaper(QString)),this,SLOT(inputPaper(QString)));
    connect(&w,SIGNAL(outputUser()),this,SLOT(outputUser()));
    connect(&w,SIGNAL(outputOb()),this,SLOT(outputOb()));
    connect(&w,SIGNAL(outputSub()),this,SLOT(outputSub()));
    connect(&w,SIGNAL(outputPaper()),this,SLOT(outputPaper()));
    w.show();
}

void MainApp::messageArrive(int descriptor,qint32 m, QVariant v)
{
    QMessageBox msg;
    User u;
    All_answers allans;
    switch(m)
    {
    case MSG_NEWCONNECT:
         infolist.replace(0,1,QString::number(serverState));
        v.setValue(infolist);
        emit this->sendData(descriptor,MSG_NEWCONNECT,v);
        break;
    case MSG_LOGIN:
        u=v.value<User>();
        if(this->userLogin(u))
        {
            if(serverState==STATE_EXAMING)
                {

                    QPushButton *agree=new QPushButton(QString("批准"));
                    QPushButton *reject=new QPushButton(QString("拒绝"));
                    msg.setText(QString("考生 %1 请求考试，\n是否批准？").arg(u.getID()));
                    msg.addButton(agree,QMessageBox::AcceptRole);
                    msg.addButton(reject,QMessageBox::RejectRole);
                    int ret=msg.exec();
                    if(ret==QMessageBox::AcceptRole)
                        {
                            emit this->sendData(descriptor,MSG_BEGINEXAM,0);
                            this->userStateChange(descriptor,QString("考试中"));
                        }
                    if(ret==QMessageBox::RejectRole)
                        {
                            QString errorstring=QString("考试要求被拒绝\n请联系工作人员");
                            v.setValue(errorstring);
                            emit this->sendData(u.getSockDescriptor(),MSG_ERROR,v);
                        }
                }
        }

        break;
    case MSG_GETPAPER:
        if(serverState==STATE_PAPERREADY)
        {
            v.setValue(mainPaper);
            emit this->sendData(descriptor,MSG_GETPAPER,v);
        }
        else if(serverState==STATE_EXAMING)
            {
                emit this->getcurrentPaperTime(descriptor);
            }
        break;

    case MSG_ANSWER:
        allans=v.value<All_answers>();
        this->userStateChange(descriptor,QString("已经交卷"));
        this->updateUserTable(userList);
        this->dealObAnswers(allans.getObanswer());
        this->dealSubAnswers(allans.getSubanswer());
        break;

    case MSG_ANSWERSINGLE:
           allans=v.value<All_answers>();
           this->updateUserTable(userList);
           this->dealObAnswers(allans.getObanswer());
           this->dealSubAnswers(allans.getSubanswer());
          break;
    }

}

void MainApp::getQuestions()
{
    QList<Ob_questions*> obList;
    QList<Sub_questions*> subList;
    QSqlQuery query;
    query=DBM->SelectOb_questions();

        while(query.next())
        {
            Ob_questions *ob_que=new Ob_questions;
            ob_que->setOb_id(query.value(0).toInt());
            ob_que->setTitle(query.value(1).toString());
            ob_que->setAnswer(query.value(2).toString());
            ob_que->setType(query.value(3).toString());
            obList.append(ob_que);
        }

        query.clear();
        query=DBM->SelectSub_questions();
        while(query.next())
        {
            Sub_questions *sub_que=new Sub_questions;
            sub_que->setSub_id(query.value(0).toInt());           
            sub_que->setTitle(query.value(1).toString());
            sub_que->setType(query.value(2).toString());
            subList.append(sub_que);
        }
        emit this->showQuestions(obList,subList);

}

void MainApp::addOb_Questions(Ob_questions *o_que)
{
    DBM->InserOb(NULL,o_que->getType(),o_que->getTitle(),o_que->getAnswer());
}

void MainApp::addSub_Questions(Sub_questions *s_que)
{
    DBM->InserSub(NULL,s_que->getType(),s_que->getTitle());
}

void MainApp::modifyOb_Questoins(Ob_questions *o_que)
{
    DBM->AlterOb_questions(o_que->getOb_id(),o_que->getType(),o_que->getTitle(),o_que->getAnswer());
}

void MainApp::modifySub_Questoins(Sub_questions *s_que)
{
    DBM->AlterSub_questions(s_que->getSub_id(),s_que->getType(),s_que->getTitle());
}

void MainApp::deleteOb_Questoins(int id)
{
    DBM->DeleteOb_questions(id);
}

void MainApp::deleteSub_Questoins(int id)
{
    DBM->DeleteSub_questions(id);
}

void MainApp::addPaper(Paper paper)
{
   DBM->InserPaper(
               paper.getOb_qu_ids(),
               paper.getSub_qu_ids(),
               paper.getTotal_mark(),
               paper.getPercent(),
               paper.getDescription(),
               paper.getTime());
}

void MainApp::getAllPaper()
{
    QList<Paper*> paperList;
    QSqlQuery query;
    query=DBM->SelectPaper();
    while(query.next())
    {

        Paper *p=new Paper;
        p->setPaper_id(query.value(0).toInt());
        p->setOb_qu_ids(query.value(1).toString());
        p->setSub_qu_ids(query.value(2).toString());
        p->setTotal_mark(query.value(3).toInt());
        p->setPercent(query.value(4).toInt());
        p->setDescription(query.value(5).toString());
        p->setTime(query.value(6).toInt());
        paperList.append(p);
    }
    emit this->showAllPaper(paperList);
}

void MainApp::deletePaper(int id)
{
    DBM->DeletePaperById(id);
}

void MainApp::getPaperById(int id)
{
    QSqlQuery query;
    query=DBM->SelectPaperById(id);
    query.next();
    if(!query.isValid())
    {
        qDebug("null");
        return;
    }
    Paper paper;
    paper.setPaper_id(query.value(0).toInt());
    paper.setOb_qu_ids(query.value(1).toString());
    paper.setSub_qu_ids(query.value(2).toString());
    paper.setTotal_mark(query.value(3).toInt());
    paper.setPercent(query.value(4).toInt());
    paper.setDescription(query.value(5).toString());
    paper.setTime(query.value(6).toInt());
    emit this->showCurrentPaper(paper);
}

void MainApp::modifyPaper(Paper p)
{
    DBM->AlterPaper(p.getPaper_id(),
                    p.getOb_qu_ids(),
                    p.getSub_qu_ids(),
                    p.getTotal_mark(),
                    p.getPercent(),
                    p.getDescription(),
                    p.getTime());
}

Paper MainApp::preparePaper(int id)
{
    QSqlQuery query;
    query=DBM->SelectPaperById(id);
    query.next();
    if(!query.isValid())
    {
        qDebug("null");

    }
    Paper paper;
    paper.setPaper_id(query.value(0).toInt());
    paper.setOb_qu_ids(query.value(1).toString());
    paper.setSub_qu_ids(query.value(2).toString());
    paper.setTotal_mark(query.value(3).toInt());
    paper.setPercent(query.value(4).toInt());
    paper.setDescription(query.value(5).toString());
    paper.setTime(query.value(6).toInt());
    query.clear();

    query=DBM->SelectOb_questions();
        while(query.next())
        {
            if(paper.getOb_qu_ids().indexOf(query.value(0).toString())>=0)
            {
            Ob_questions *ob_que=new Ob_questions;
            ob_que->setOb_id(query.value(0).toInt());
            ob_que->setTitle(query.value(1).toString());
            ob_que->setAnswer(query.value(2).toString());
            ob_que->setType(query.value(3).toString());

            paper.obList.append(*ob_que);
            }

        }

        query.clear();
        query=DBM->SelectSub_questions();
        while(query.next())
        {
            if(paper.getSub_qu_ids().indexOf(query.value(0).toString())>=0)
            {
            Sub_questions *sub_que=new Sub_questions;
            sub_que->setSub_id(query.value(0).toInt());
            sub_que->setTitle(query.value(1).toString());
            sub_que->setType(query.value(2).toString());
            paper.subList.append(*sub_que);
            }
        }
        return paper;
}

void MainApp::sendPaper(int id)
{
   mainPaper=this->preparePaper(id);
   userList= this->getUserByPaperId(id,QString("未完成"));
   this->userStateChange(-1,QString("未登录"));

   QVariant v;
   v.setValue(mainPaper);
   emit this->sendData(-1,MSG_GETPAPER,v);
   serverState=STATE_PAPERREADY;
}

void MainApp::getUserList()
{
    emit this->updateUserTable(userList);
}

void MainApp::beginExam()
{
    serverState=STATE_EXAMING;
    for(int i=0;i<userList.count();i++)
        {
            if(userList.at(i)->getState()==QString("等待"))
                this->userStateChange(userList.at(i)->getSockDescriptor(),QString("考试中"));
        }
    emit this->sendData(-1,MSG_BEGINEXAM,0);

}

void MainApp::endExam()
{
    serverState=STATE_NOEXAM;
    for(int i=0;i<userList.count();i++)
        {
            if(userList.at(i)->getState()==QString("考试中"))
                {
                    emit this->sendData(userList.at(i)->getSockDescriptor(),MSG_ENDEXAM,0);
                }
        }
    userList.clear();
}

bool MainApp::userLogin(User u)
{
    int isIn=0;
    QVariant v;
    QSqlQuery query=DBM->Login(u.getID(),u.getPassword());
    if(query.size()>0)
    {
        for(int i=0;i<userList.count();i++)
            {
                if(userList.at(i)->getID()==u.getID())
                    {
                        if(userList.at(i)->getState()==QString("已经交卷"))
                            {
                                QString errorstring=QString("你已经交卷了");
                                v.setValue(errorstring);
                                emit this->sendData(u.getSockDescriptor(),MSG_ERROR,v);
                                return false;
                            }
                        if(userList.at(i)->getState()!=QString("未登录"))
                            {
                                QString errorstring=QString("你已经登录了");
                                v.setValue(errorstring);
                                emit this->sendData(u.getSockDescriptor(),MSG_ERROR,v);
                                return false;
                            }

                        isIn=1;
                        userList.at(i)->setHostname(u.getHostname());
                        userList.at(i)->setSockDescriptor(u.getSockDescriptor());

                        this->userStateChange(userList.at(i)->getSockDescriptor(),QString("等待"));
                        v.setValue(*userList.at(i));

                    }
            }
        if(isIn==1)
            {
                this->sendData(u.getSockDescriptor(),MSG_LOGIN,v);
                return true;
            }
        else
            {
                QString errorstring=QString("你不能参加这个考试");
                v.setValue(errorstring);
                emit this->sendData(u.getSockDescriptor(),MSG_ERROR,v);
                return false;
            }

    }
    else
    {
        QString errorstring=QString("用户名或密码错误");
        v.setValue(errorstring);
        emit this->sendData(u.getSockDescriptor(),MSG_ERROR,v);
        return false;
    }
}

void MainApp::sendPaperTime(int descriptor,int time)
{
    QVariant v;
    mainPaper.setTime(time);
    v.setValue(mainPaper);
    emit this->sendData(descriptor,MSG_GETPAPER,v);
}

bool MainApp::managerLogin(Manager m)
{
    QSqlQuery query=DBM->managerLogin(m.getId(),m.getPassword());
    if(query.size()>0)
    {
        emit this->LoginOK();
        return true;
    }
    else
        {
        QMessageBox msg;
        msg.setText(QString("用户名或密码错误"));
        msg.exec();
        return false;
        }
}

void MainApp::userStateChange(int descriptor, QString state)
{
    for(int i=0;i<userList.count();i++)
    {
        if(descriptor==userList.at(i)->getSockDescriptor()||descriptor==-1)
        {
            userList.at(i)->setState(state);
        }
    }

    emit this->updateUserTable(userList);
}

void MainApp::removeUser(int descriptor)
{
    for(int i=0;i<userList.count();i++)
        {
            if(userList.at(i)->getSockDescriptor()==descriptor)
                {
                    if(userList.at(i)->getState()!=QString("已经交卷"))
                        {
                        userList.at(i)->setHostname("");
                        this->userStateChange(descriptor,QString("未登录"));
                        }
                }
        }
}

void MainApp::saveUsertoPaperMark(int pid, QList<User *> ulist)
{

    bool isdelete=DBM->deletePapermark(pid);
    QMessageBox msg;
        for(int i=0;i<ulist.count();i++)
            {
               if(DBM->InserPaper_mark(NULL,NULL,NULL,pid,ulist.at(i)->getID())){
                DBM->InserOb_answers(pid,ulist.at(i)->getID(),NULL);
                DBM->InserSub_answers(pid,ulist.at(i)->getID());

            }else{
                   QString name=ulist.at(i)->getName();
                   QString text=QString("学生 '%1' 添加失败，已完成该次考试的学生不能再次添加").arg(name);
                   msg.setText(text);
                   msg.exec();}}
        msg.setText(QString("操作完成"));
        msg.exec();
       // QMessageBox msg;
       // msg.setText(QString("已完成该次考试的学生不能再次添加"));
       // msg.exec();};

}

void MainApp::dealObAnswers(Ob_answers obans)
{
    DBM->updateOb_answers(obans.getPaper_id(),obans.getStudent_id(),obans.getAnswers());

    QString ans_string=obans.getAnswers();
    QStringList ansList;
    int temp=0;
    while(temp<ans_string.length())
    {
        ansList.append(ans_string.mid(temp,ans_string.indexOf(",")));
        temp=ans_string.indexOf(",",temp)+1;
    }
    QString eachObmark;
    QString obMarkString;
    eachObmark=QString::number(mainPaper.getTotal_mark()*mainPaper.getPercent()/100/mainPaper.obList.count());
    for(int i=0;i<ansList.count();i++)
    {
        QString correctAns=mainPaper.obList.value(i).getAnswer();
        if(ansList.at(i)==correctAns)
        {

            obMarkString.append(eachObmark);
        }
        else
            obMarkString.append("0");

        obMarkString.append(",");
    }
   DBM->updatePaper_mark_obmark(obMarkString,obans.getPaper_id(),obans.getStudent_id());
}

void MainApp::dealSubAnswers(Sub_answers subans)
{

    for(int i=0;i<subans.getSubanslist().count();i++)
    {
            DBM->updateSub_answers(subans.getPaper_id(),subans.getStudent_id(),i+1,subans.getSubanslist().at(i));
    }
    DBM->updatePapermark_done(QDate::currentDate().toString(),subans.getPaper_id(),subans.getStudent_id());
}

void MainApp::submitSubMark(QStringList submark)
{
    DBM->updatePaper_mark_Submark(submark.at(2),submark.at(0).toInt(),submark.at(1));
    QSqlQuery query=DBM->SearchPaper_mark(submark.at(0).toInt(),submark.at(1));
    query.next();
    int totalmark=0;
    QString ob=query.value(0).toString();
    QString sub=query.value(1).toString();
    int temp=0;
    while(temp<ob.length())
    {
        totalmark+=ob.mid(temp,ob.indexOf(",",temp)-temp).toInt();
        temp=ob.indexOf(",",temp)+1;
    }
    temp=0;
    while(temp<sub.length())
    {
        totalmark+=sub.mid(temp,sub.indexOf(",",temp)-temp).toInt();
        temp=sub.indexOf(",",temp)+1;
    }
    DBM->updatePaper_mark_totalmark(totalmark,submark.at(0).toInt(),submark.at(1));
}

QList<User*> MainApp::getUserByPaperId(int id,QString state)
{
    QSqlQuery query=DBM->Query_papermark1(id);
    QList<User*> ulist;
    while(query.next())
        {
            User *u=new User;
            u->setID(query.value(8).toString());
            u->setName(query.value(9).toString());
            u->setGrade(query.value(10).toInt());
            u->setClass(query.value(11).toInt());
            u->setState(query.value(5).toString());
            if(u->getState()==state)
                {
                    u->setState(query.value(6).toString());
                    ulist.append(u);
                }

        }

    emit this->showUserByPaperId(ulist);
    return ulist;

}

void MainApp::getSubAnswer(int pid,QString uid)
{
    QVector<QString> sub;
    QSqlQuery query=DBM->SelectPaperById(pid);
    query.next();
     QString sub_ids=query.value(2).toString();
    // sub.resize(2*sub_ids.count(","));

     query.clear();
     query=DBM->SelectSub_questions();
     while(query.next())
         {
             if(sub_ids.indexOf(query.value(0).toString())>=0)
             {
                  sub.append(query.value(1).toString());
             }
         }
     query.clear();
     query=DBM->QuerySub_answers(pid,uid);
     query.next();
     int m=2;
     int count=sub.size();
     for(int i=count;i<count*2;i++)
         {
             sub.append(query.value(m).toString());
             m++;
         }
     emit this->showSubAnswer(sub);
  }

  ////用户id 查找
  void MainApp::getCombo_id(QString a)
  {
      QList <Combo*> comboList;
      QSqlQuery query;
      query = DBM->Query_papermark2(a);

      QSqlQuery s = DBM->SelectUserId(a);
      QString temp;
      if(s.next())
      {
          temp = s.value(1).toString();
      }

      while(query.next())
      {
          if(query.value(6).toString()==QString("已批改"))
          {
              Combo *b = new Combo;

          //from user
          b->setUser_id(a);
          b->setName(temp);
          b->setGrade(s.value(2).toInt());
          b->setClass(s.value(3).toInt());
          int obmark=0;
          int submark=0;
          QString ob=query.value(0).toString();
          QString sub=query.value(1).toString();
          int t=0;
          while(t<ob.length())
          {
              obmark+=ob.mid(t,ob.indexOf(",",t)-t).toInt();
              t=ob.indexOf(",",t)+1;
          }
          t=0;
          while(t<sub.length())
          {
              submark+=sub.mid(t,sub.indexOf(",",t)-t).toInt();
              t=sub.indexOf(",",t)+1;
          }
          b->setObmark(obmark);
          b->setSubmark(submark);
          //from paper
          b->setPaper_id(query.value(3).toInt());
          QSqlQuery h = DBM->SelectPaperById(query.value(3).toInt());
          if(h.next())
              b->setPaperName(h.value(5).toString());

          //from self (paper mark)
          b->setPaper_mark(query.value(2).toInt());
          comboList.append(b);
        }
      }
      emit this->showCombo(comboList);
}

void MainApp::getCombo_paperid(int id)
{
    QList <Combo*> comboList;
    QSqlQuery query;
    query = DBM->Query_papermark1(id);

    QSqlQuery s = DBM->SelectPaperById(id);
    QString temp;
    if(s.next())
    {
        temp = s.value(5).toString();
    }

    while(query.next())
    {
        if(query.value(6).toString()==QString("已批改"))
        {
            Combo *b = new Combo;
        b->setPaper_id(id);
        b->setPaperName(temp);
        int obmark=0;
        int submark=0;
        QString ob=query.value(0).toString();
        QString sub=query.value(1).toString();
        int t=0;
        while(t<ob.length())
        {
            obmark+=ob.mid(t,ob.indexOf(",",t)-t).toInt();
            t=ob.indexOf(",",t)+1;
        }
        t=0;
        while(t<sub.length())
        {
            submark+=sub.mid(t,sub.indexOf(",",t)-t).toInt();
            t=sub.indexOf(",",t)+1;
        }
        b->setObmark(obmark);
        b->setSubmark(submark);
        //from paper
        b->setPaper_id(query.value(3).toInt());
        QSqlQuery h = DBM->SelectUserId(query.value(4).toString());
        if(h.next())
           {
               b->setUser_id(h.value(0).toString());
               b->setName(h.value(1).toString());
               b->setGrade(h.value(2).toInt());
               b->setClass(h.value(3).toInt());
           }
        //from self (paper mark)
        b->setPaper_mark(query.value(2).toInt());
        comboList.append(b);
        }
    }
    emit this->showCombo(comboList);
}

void MainApp::getUser()
{
    QList<User*> userList;
    QList<Manager*> managerList;

    QSqlQuery query;
    query = DBM->SelectUser();

    while(query.next())
    {
        User *userptr = new User;
        userptr->setID(query.value(0).toString());
        userptr->setName(query.value(1).toString());
        userptr->setGrade(query.value(2).toInt());
        userptr->setClass(query.value(3).toInt());
        userptr->setPassword(query.value(4).toString());
        userList.append(userptr);
    }

    query.clear();
    query = DBM->SelectManager();
    while(query.next())
    {
        Manager *managerptr = new Manager;
        managerptr->setId(query.value(0).toInt());
        managerptr->setName(query.value(1).toString());
        managerptr->setPassword(query.value(2).toString());
        managerList.append(managerptr);
    }
/*      QSqlTableModel *ob_model=new QSqlTableModel(this,DBM->db);
        ob_model->setTable("user");
        ob_model->select();
        QTableView *view = new QTableView;
        view->setModel(ob_model);
        view->show();
*/
    emit this->showUser(userList,managerList);
}

void MainApp::addUser(User *user)
{
    DBM->InsertUser(
                user->getID(),
                user->getName(),
                user->getGrade(),
                user->getClass(),
                user->getPassword());
}

void MainApp::addManager(Manager *m)
{
    DBM->InsertManager(m->getId(),m->getName(),m->getPassword());
}

void MainApp::modifyUser(User u)
{
    DBM->ModifyUser(u.getID(),u.getName(),u.getGrade(),u.getClass(),u.getPassword());
}

void MainApp::deleteUserId(QString a)
{
    DBM->DeleteUserId(a);
}

void MainApp::deleteManagerId(int a)
{
    DBM->DeleteManagerId(a);
}

void MainApp::outputUser()
{
    QList<User*> userList;

    QSqlQuery query;
    query = DBM->SelectUser();

    while(query.next())
    {
        User *userptr = new User;
        userptr->setID(query.value(0).toString());
        userptr->setName(query.value(1).toString());
        userptr->setGrade(query.value(2).toInt());
        userptr->setClass(query.value(3).toInt());
        userptr->setPassword(query.value(4).toString());
        userList.append(userptr);
    }
    IOM->outputUser(userList);
    QMessageBox msg;
    msg.setText(QString("导出成功。"));
    msg.exec();
}

void MainApp::outputOb()
{
    QList<Ob_questions*> obList;
    QSqlQuery query;
    query=DBM->SelectOb_questions();
        while(query.next())
        {
            Ob_questions *ob_que=new Ob_questions;
            ob_que->setOb_id(query.value(0).toInt());
            ob_que->setTitle(query.value(1).toString());
            ob_que->setAnswer(query.value(2).toString());
            ob_que->setType(query.value(3).toString());
            obList.append(ob_que);
        }
    IOM->outputOb(obList);
    QMessageBox msg;
    msg.setText(QString("导出成功。"));
    msg.exec();
}

void MainApp::outputSub()
{
    QList<Sub_questions*> subList;
    QSqlQuery query=DBM->SelectSub_questions();
    while(query.next())
    {
        Sub_questions *sub_que=new Sub_questions;
        sub_que->setSub_id(query.value(0).toInt());
        sub_que->setTitle(query.value(1).toString());
        sub_que->setType(query.value(2).toString());
        subList.append(sub_que);
    }
    IOM->outputSub(subList);
    QMessageBox msg;
    msg.setText(QString("导出成功。"));
    msg.exec();
}

void MainApp::outputPaper()
{
    QList<Paper*> paperList;
    QSqlQuery query;
    query=DBM->SelectPaper();
    while(query.next())
    {
        Paper *p=new Paper;
        p->setPaper_id(query.value(0).toInt());
        p->setOb_qu_ids(query.value(1).toString());
        p->setSub_qu_ids(query.value(2).toString());
        p->setTotal_mark(query.value(3).toInt());
        p->setPercent(query.value(4).toInt());
        p->setDescription(query.value(5).toString());
        p->setTime(query.value(6).toInt());
        paperList.append(p);
    }
        IOM->outputPaper(paperList);
        QMessageBox msg;
        msg.setText(QString("导出成功。"));
        msg.exec();
}

void MainApp::inputUser(QString path)
{
    QList<User*> userlist;
    userlist=IOM->inputUser(path);
    for(int i=0;i<userlist.count();i++)
        {
        /*qDebug()<< userlist.at(i)->getID()<<
                userlist.at(i)->getName()<<
                userlist.at(i)->getGrade()<<
                userlist.at(i)->getClass()<<
                userlist.at(i)->getPassword()<<"\n";*/
            DBM->InsertUser(
                        userlist.at(i)->getID(),
                        userlist.at(i)->getName(),
                        userlist.at(i)->getGrade(),
                        userlist.at(i)->getClass(),
                        userlist.at(i)->getPassword());
        }
    QMessageBox msg;
    msg.setText(QString("导入成功。"));
    msg.exec();
}

void MainApp::inputOb(QString path)
{
    QList<Ob_questions*> oblist;
    oblist=IOM->inputOb(path);
    for(int i=0;i<oblist.count();i++)
        {
            DBM->InserOb(oblist.at(i)->getOb_id(),oblist.at(i)->getType(),oblist.at(i)->getTitle(),oblist.at(i)->getAnswer());
        }
    QMessageBox msg;
    msg.setText(QString("导入成功。"));
    msg.exec();
}

void MainApp::inputSub(QString path)
{
    QList<Sub_questions*> sublist;
    sublist=IOM->inputSub(path);
    for(int i=0;i<sublist.count();i++)
        {
            DBM->InserSub(sublist.at(i)->getSub_id(),sublist.at(i)->getType(),sublist.at(i)->getTitle());
        }
    QMessageBox msg;
    msg.setText(QString("导入成功。"));
    msg.exec();
}

void MainApp::inputPaper(QString path)
{
    QList<Paper*> plist;
    plist=IOM->inputPaper(path);
    for(int i=0;i<plist.count();i++)
        {
            DBM->InserPaper(
                        plist.at(i)->getOb_qu_ids(),
                        plist.at(i)->getSub_qu_ids(),
                        plist.at(i)->getTotal_mark(),
                        plist.at(i)->getPercent(),
                        plist.at(i)->getDescription(),
                        plist.at(i)->getTime());
        }
    QMessageBox msg;
    msg.setText(QString("导入成功。"));
    msg.exec();
}

void MainApp::sendInfo(QStringList list)
{
    infolist.clear();
    infolist.append("*,");
    for(int i=0;i<list.count();i++)
        {
            infolist.append(list.at(i));
            infolist.append(",");
        }
}

void MainApp::delete_score(int pid,qlonglong uid){
   if(DBM->delete_score(pid,uid)){
   QMessageBox msg;
   msg.setText("删除成功");
   msg.exec();
   }
}
