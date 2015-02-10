﻿#include"mainapp.h"
#include "data.h"
#include <QMessageBox>
#include <QDebug>
MainApp::MainApp()
{
    readConfig();
    iniMainWindow();
    iniDBManager();
    iniServer();
    _IOM = new IOManager;
    _serverState = STATE_NOEXAM;
    _infoList.append("*,");
}

MainApp::~MainApp()
{
    delete(_DBM);
    delete(_server);
    delete(_IOM);
}

void MainApp::readConfig()
{
    QFile Config("./config.ini");
    if(!Config.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox msg;
        msg.setText(QStringLiteral("读取配置文件失败！将使用默认配置"));
        msg.exec();
        return;
    }
    QTextStream in(&Config);
    QString temp;
    temp = in.readLine();
    temp = temp.mid(temp.indexOf(" ")+1);
    _dbName = temp;
    temp = in.readLine();
    temp = temp.mid(temp.indexOf(" ")+1);
    _dbUser = temp;
    temp = in.readLine();
    temp = temp.mid(temp.indexOf(" ")+1);
    _dbPassword = temp;
    temp = in.readLine();
    temp = temp.mid(temp.indexOf(" ")+1);
    _port = temp.toInt();
    Config.close();
}

void MainApp::iniDBManager()
{
    _DBM = new DBManager();
    _DBM->setConfig(_dbName, _dbUser, _dbPassword);
}

void MainApp::iniServer()
{
    _server = new Server(this,_port);
    connect(this,SIGNAL(sendData(int,qint32,QVariant)),_server,SIGNAL(sendData(int,qint32,QVariant)));
    connect(_server,SIGNAL(messageArrive(int,qint32,QVariant)),this,SLOT(messageArrive(int,qint32,QVariant)),Qt::QueuedConnection);
    connect(_server,SIGNAL(removeUser(int)),this,SLOT(removeUser(int)));
}

void MainApp::iniMainWindow()
{
    //与题库管理界面连接的信号和槽
    connect(&_window,SIGNAL(getQuestions()),this,SLOT(getQuestions()));
    connect(this,SIGNAL(showQuestions(QList<ChoiceQuestions*>,QList<EssayQuestions*>)),&_window,SIGNAL(showQuestions(QList<ChoiceQuestions*>,QList<EssayQuestions*>)));
    connect(&_window,SIGNAL(addOb_Questoins(ChoiceQuestions*)),this,SLOT(addOb_Questions(ChoiceQuestions*)));
    connect(&_window,SIGNAL(addSub_Questoins(EssayQuestions*)),this,SLOT(addSub_Questions(EssayQuestions*)));
    connect(&_window,SIGNAL(modifyOb_Questoins(ChoiceQuestions*)),this,SLOT(modifyOb_Questoins(ChoiceQuestions*)));
    connect(&_window,SIGNAL(modifySub_Questoins(EssayQuestions*)),this,SLOT(modifySub_Questoins(EssayQuestions*)));
    connect(&_window,SIGNAL(deleteOb_Questoins(int)),this,SLOT(deleteOb_Questoins(int)));
    connect(&_window,SIGNAL(deleteSub_Questoins(int)),this,SLOT(deleteSub_Questoins(int)));
    //与试卷管理界面连接的信号和槽
    connect(&_window,SIGNAL(addPaper(Paper)),this,SLOT(addPaper(Paper)));
    connect(&_window,SIGNAL(getAllPaper()),this,SLOT(getAllPaper()));
    connect(this,SIGNAL(showAllPaper(QList<Paper*>)),&_window,SIGNAL(showAllPaper(QList<Paper*>)));
    connect(&_window,SIGNAL(getPaperById(int)),this,SLOT(getPaperById(int)));
    connect(&_window,SIGNAL(deletePaper(int)),this,SLOT(deletePaper(int)));
    connect(&_window,SIGNAL(modifyPaper(Paper)),this,SLOT(modifyPaper(Paper)));
    connect(this,SIGNAL(showCurrentPaper(Paper)),&_window,SIGNAL(showCurrentPaper(Paper)));
    connect(&_window,SIGNAL(saveUsertoPaperMark(int,QList<Student*>)),this,SLOT(saveUsertoPaperMark(int,QList<Student*>)));

    ////////examctrl
    connect(&_window,SIGNAL(sendPaper(int)),this,SLOT(sendPaper(int)));
    connect(&_window,SIGNAL(beginExam()),this,SLOT(beginExam()));
    connect(&_window,SIGNAL(getUserList()),this,SLOT(getUserList()));
    connect(this,SIGNAL(updateUserTable(QList<Student*>)),&_window,SIGNAL(updateUserTable(QList<Student*>)));
    connect(&_window,SIGNAL(endExam()),this,SLOT(endExam()));
    connect(this,SIGNAL(getcurrentPaperTime(int)),&_window,SIGNAL(getcurrentPaperTime(int)));
    connect(&_window,SIGNAL(sendPaperTime(int,int)),this,SLOT(sendPaperTime(int,int)));
    connect(&_window,SIGNAL(sendInfo(QStringList)),this,SLOT(sendInfo(QStringList)));
    //subcore
    connect(&_window,SIGNAL(getUserByPaperId(int,QString)),this,SLOT(getUserByPaperId(int,QString)));
    connect(this,SIGNAL(showUserByPaperId(QList<Student*>)),&_window,SIGNAL(showUserByPaperId(QList<Student*>)));
    connect(&_window,SIGNAL(getSubAnswer(int,QString)),this,SLOT(getSubAnswer(int,QString)));
    connect(this,SIGNAL(showSubAnswer(QVector<QString>)),&_window,SIGNAL(showSubAnswer(QVector<QString>)));
    connect(&_window,SIGNAL(submitSubMark(QStringList)),this,SLOT(submitSubMark(QStringList)));
    ///mem
    connect(&_window, SIGNAL(getType()), this, SLOT(getType()));
    connect(this, SIGNAL(showType(QMap<int, QString>)), &_window, SIGNAL(showType(QMap<int, QString>)));
    connect(&_window, SIGNAL(getSubject()), this, SLOT(getSubject()));
    connect(this, SIGNAL(showSubject(QList<QString>)), &_window, SIGNAL(showSubject(QList<QString>)));
    connect(&_window,SIGNAL(getUser()),this,SLOT(getUser()));//mainwindow发，mainapp收
    connect(&_window, SIGNAL(getManager()), this, SLOT(getManager()));
    connect(this,SIGNAL(showManager(QList<User*>)), &_window, SIGNAL(showManager(QList<User*>)));
    connect(this,SIGNAL(showUser(QList<Student*>,QList<User*>)),&_window,SIGNAL(showUser(QList<Student*>,QList<User*>)));
    connect(&_window,SIGNAL(addStudent(Student*)),this,SLOT(addStudent(Student *)));
    connect(&_window,SIGNAL(addTeacher(User*)),this,SLOT(addTeacher(User*)));
    connect(&_window, SIGNAL(addManger(User*)), this, SLOT(addManager(User*)));
    connect(&_window,SIGNAL(deleteUserId(QString)),this,SLOT(deleteUserId(QString)));
    connect(&_window,SIGNAL(deleteManagerId(int)),this,SLOT(deleteManagerId(int)));
    connect(&_window, SIGNAL(addType(int,QString)), this, SLOT(addType(int,QString)));
    connect(&_window, SIGNAL(deleteType(int)), this, SLOT(deleteType(int)));
    //login
    connect(&_window,SIGNAL(loginSignal(User)),this,SLOT(managerLogin(User)));
    connect(this,SIGNAL(LoginOK()),&_window,SLOT(LoginOK()));
    //scomem
    connect(&_window,SIGNAL(getCombo_id(QString)),this,SLOT(getCombo_id(QString)));
    connect(this,SIGNAL(showCombo(QList<Combo*>)),&_window,SIGNAL(showCombo(QList<Combo*>)));
    connect(&_window,SIGNAL(getCombo_paperid(int)),this,SLOT(getCombo_paperid(int)));
    connect(&_window,SIGNAL(delete_score(int,qlonglong)),this,SLOT(delete_score(int,qlonglong)));
    //InOutPutUI
    connect(&_window,SIGNAL(inputUser(QString)),this,SLOT(inputUser(QString)));
    connect(&_window,SIGNAL(inputOb(QString)),this,SLOT(inputOb(QString)));
    connect(&_window,SIGNAL(inputSub(QString)),this,SLOT(inputSub(QString)));
    connect(&_window,SIGNAL(inputPaper(QString)),this,SLOT(inputPaper(QString)));
    connect(&_window,SIGNAL(outputUser()),this,SLOT(outputUser()));
    connect(&_window,SIGNAL(outputOb()),this,SLOT(outputOb()));
    connect(&_window,SIGNAL(outputSub()),this,SLOT(outputSub()));
    connect(&_window,SIGNAL(outputPaper()),this,SLOT(outputPaper()));
    _window.show();
}

void MainApp::messageArrive(int descriptor,qint32 m, QVariant v)
{
    QMessageBox msg;
    Student student;
    AllAnswers answers;
    switch(m)
    {
    case MSG_NEWCONNECT:
        _infoList.replace(0,1,QString::number(_serverState));
        v.setValue(_infoList);
        emit this->sendData(descriptor,MSG_NEWCONNECT,v);
        break;
    case MSG_LOGIN:
        student = v.value<Student>();
        if(this->userLogin(student))
        {
            if(_serverState == STATE_EXAMING)
            {

                QPushButton *agree = new QPushButton(QStringLiteral("批准"));
                QPushButton *reject = new QPushButton(QStringLiteral("拒绝"));
                msg.setText(QStringLiteral("考生 %1 请求考试，\n是否批准？").arg(student.getID()));
                msg.addButton(agree,QMessageBox::AcceptRole);
                msg.addButton(reject,QMessageBox::RejectRole);
                int ret = msg.exec();
                if(ret == QMessageBox::AcceptRole)
                {
                    emit this->sendData(descriptor,MSG_BEGINEXAM,0);
                    this->userStateChange(descriptor,QStringLiteral("考试中"));
                }
                if(ret == QMessageBox::RejectRole)
                {
                    QString errorstring = QStringLiteral("考试要求被拒绝\n请联系工作人员");
                    v.setValue(errorstring);
                    emit this->sendData(student.getSockDescriptor(),MSG_ERROR,v);
                }
            }
        }

        break;
    case MSG_GETPAPER:
        if(_serverState == STATE_PAPERREADY)
        {
            v.setValue(_mainPaper);
            emit this->sendData(descriptor,MSG_GETPAPER,v);
        }
        else if(_serverState == STATE_EXAMING)
        {
            emit this->getcurrentPaperTime(descriptor);
        }
        break;

    case MSG_ANSWER:
        answers = v.value<AllAnswers>();
        this->userStateChange(descriptor,QStringLiteral("已经交卷"));
        this->updateUserTable(_userList);
        this->dealObAnswers(answers.getObanswer());
        this->dealSubAnswers(answers.getSubanswer());
        break;

    case MSG_ANSWERSINGLE:
        answers = v.value<AllAnswers>();
        this->updateUserTable(_userList);
        this->dealObAnswers(answers.getObanswer());
        this->dealSubAnswers(answers.getSubanswer());
        break;
    }

}

void MainApp::getQuestions()
{
    QList<ChoiceQuestions*> choiceList;
    QList<EssayQuestions*> essayList;
    QSqlQuery query;
    query = _DBM->selectObQuestions();

    while(query.next())
    {
        ChoiceQuestions *choicequestion = new ChoiceQuestions;
        choicequestion->setQuestionId(query.value(0).toInt());
        choicequestion->setQuestionTitle(query.value(1).toString());
        choicequestion->setAnswer(query.value(2).toString());
        choicequestion->setSubjectID(query.value(3).toString());
        choiceList.append(choicequestion);
    }

    query.clear();
    query = _DBM->selectSubQuestions();
    while(query.next())
    {
        EssayQuestions *sub_que = new EssayQuestions;
        sub_que->setQuestionId(query.value(0).toInt());
        sub_que->setQuestionTitle(query.value(1).toString());
        sub_que->setSubjectID(query.value(2).toString());
        essayList.append(sub_que);
    }
    emit this->showQuestions(choiceList,essayList);

}

void MainApp::addOb_Questions(ChoiceQuestions *o_que)
{
    _DBM->insertOb(NULL,o_que->getSubjectID(),o_que->getQuestionTitle(),o_que->getAnswer());
}

void MainApp::addSub_Questions(EssayQuestions *s_que)
{
    _DBM->insertSub(NULL,s_que->getSubjectID(),s_que->getQuestionTitle());
}

void MainApp::modifyOb_Questoins(ChoiceQuestions *o_que)
{
    _DBM->alterObQuestions(o_que->getQuestionId(),o_que->getSubjectID(),o_que->getQuestionTitle(),o_que->getAnswer());
}

void MainApp::modifySub_Questoins(EssayQuestions *s_que)
{
    _DBM->alterSubQuestions(s_que->getQuestionId(),s_que->getSubjectID(),s_que->getQuestionTitle());
}

void MainApp::deleteOb_Questoins(int id)
{
    _DBM->deleteObQuestionsByID(id);
}

void MainApp::deleteSub_Questoins(int id)
{
    _DBM->deleteSubQuestionsByID(id);
}

void MainApp::addPaper(Paper paper)
{
    _DBM->insertPaper(
        paper.getObQuIds(),
        paper.getSubQuIds(),
        paper.getTotalMark(),
        paper.getPercent(),
        paper.getDescription(),
        paper.getTime());
}

void MainApp::getAllPaper()
{
    QList<Paper*> paperList;
    QSqlQuery query;
    query = _DBM->selectPaper();
    while(query.next())
    {
        Paper *p = new Paper;
        p->setPaperId(query.value(0).toInt());
        p->setObQuIds(query.value(1).toString());
        p->setSubQuIds(query.value(2).toString());
        p->setTotalMark(query.value(3).toInt());
        p->setPercent(query.value(4).toInt());
        p->setDescription(query.value(5).toString());
        p->setTime(query.value(6).toInt());
        paperList.append(p);
    }
    emit this->showAllPaper(paperList);
}

void MainApp::deletePaper(int id)
{
    _DBM->deletePaperById(id);
}

void MainApp::getPaperById(int id)
{
    QSqlQuery query;
    query = _DBM->selectPaperById(id);
    query.next();
    if(!query.isValid())
    {
        qDebug("null");
        return;
    }
    Paper paper;
    paper.setPaperId(query.value(0).toInt());
    paper.setObQuIds(query.value(1).toString());
    paper.setSubQuIds(query.value(2).toString());
    paper.setTotalMark(query.value(3).toInt());
    paper.setPercent(query.value(4).toInt());
    paper.setDescription(query.value(5).toString());
    paper.setTime(query.value(6).toInt());
    emit this->showCurrentPaper(paper);
}

void MainApp::modifyPaper(Paper p)
{
    _DBM->alterPaper(p.getPaperId(),
                    p.getObQuIds(),
                    p.getSubQuIds(),
                    p.getTotalMark(),
                    p.getPercent(),
                    p.getDescription(),
                    p.getTime());
}

Paper MainApp::preparePaper(int id)
{
    QSqlQuery query;
    query = _DBM->selectPaperById(id);
    query.next();
    if(!query.isValid())
    {
        qDebug("null");

    }
    Paper paper;
    paper.setPaperId(query.value(0).toInt());
    paper.setObQuIds(query.value(1).toString());
    paper.setSubQuIds(query.value(2).toString());
    paper.setTotalMark(query.value(3).toInt());
    paper.setPercent(query.value(4).toInt());
    paper.setDescription(query.value(5).toString());
    paper.setTime(query.value(6).toInt());
    query.clear();

    query = _DBM->selectObQuestions();
    while(query.next())
    {
        if(paper.getObQuIds().indexOf(query.value(0).toString()) >= 0)
        {
            ChoiceQuestions *ob_que = new ChoiceQuestions;
            ob_que->setQuestionId(query.value(0).toInt());
            ob_que->setQuestionTitle(query.value(1).toString());
            ob_que->setAnswer(query.value(2).toString());
            ob_que->setSubjectID(query.value(3).toString());

            paper.choiceQuestionList.append(*ob_que);
        }

    }

    query.clear();
    query = _DBM->selectSubQuestions();
    while(query.next())
    {
        if(paper.getSubQuIds().indexOf(query.value(0).toString()) >= 0)
        {
            EssayQuestions *sub_que = new EssayQuestions;
            sub_que->setQuestionId(query.value(0).toInt());
            sub_que->setQuestionTitle(query.value(1).toString());
            sub_que->setSubjectID(query.value(2).toString());
            paper.essayQuestionbList.append(*sub_que);
        }
    }
    return paper;
}

void MainApp::sendPaper(int id)
{
    _mainPaper = this->preparePaper(id);
    _userList = this->getUserByPaperId(id, QStringLiteral("未完成"));
    this->userStateChange(-1, QStringLiteral("未登录"));

    QVariant v;
    v.setValue(_mainPaper);
    emit this->sendData(-1, MSG_GETPAPER,v);
    _serverState = STATE_PAPERREADY;
}

void MainApp::getUserList()
{
    emit this->updateUserTable(_userList);
}

void MainApp::beginExam()
{
    _serverState = STATE_EXAMING;
    for(int i = 0; i < _userList.count(); i++)
    {
        if(_userList.at(i)->getState() == QStringLiteral("等待"))
            this->userStateChange(_userList.at(i)->getSockDescriptor(),QStringLiteral("考试中"));
    }
    emit this->sendData(-1, MSG_BEGINEXAM, 0);

}

void MainApp::endExam()
{
    _serverState = STATE_NOEXAM;
    for(int i = 0; i < _userList.count(); i++)
    {
        if(_userList.at(i)->getState() == QStringLiteral("考试中"))
        {
            emit this->sendData(_userList.at(i)->getSockDescriptor(),MSG_ENDEXAM,0);
        }
    }
    _userList.clear();
}

bool MainApp::userLogin(Student student)
{
    int isIn = 0;
    QVariant v;
    QSqlQuery query = _DBM->studentLogin(student.getID(),student.getPassword());
    if(query.size() > 0)
    {
        for(int i = 0; i < _userList.count(); i++)
        {
            if(_userList.at(i)->getID() == student.getID())
            {
                if(_userList.at(i)->getState() == QStringLiteral("已经交卷"))
                {
                    QString errorstring = QStringLiteral("你已经交卷了");
                    v.setValue(errorstring);
                    emit this->sendData(student.getSockDescriptor(),MSG_ERROR,v);
                    return false;
                }
                if(_userList.at(i)->getState() != QStringLiteral("未登录"))
                {
                    QString errorstring = QStringLiteral("你已经登录了");
                    v.setValue(errorstring);
                    emit this->sendData(student.getSockDescriptor(),MSG_ERROR,v);
                    return false;
                }

                isIn = 1;
                _userList.at(i)->setHostName(student.getHostname());
                _userList.at(i)->setSockDescriptor(student.getSockDescriptor());

                this->userStateChange(_userList.at(i)->getSockDescriptor(),QStringLiteral("等待"));
                v.setValue(*_userList.at(i));

            }
        }
        if(isIn == 1)
        {
            this->sendData(student.getSockDescriptor(),MSG_LOGIN,v);
            return true;
        }
        else
        {
            QString errorstring = QStringLiteral("你不能参加这个考试");
            v.setValue(errorstring);
            emit this->sendData(student.getSockDescriptor(),MSG_ERROR,v);
            return false;
        }

    }
    else
    {
        QString errorstring = QStringLiteral("用户名或密码错误");
        v.setValue(errorstring);
        emit this->sendData(student.getSockDescriptor(),MSG_ERROR,v);
        return false;
    }
}

void MainApp::sendPaperTime(int descriptor,int time)
{
    QVariant v;
    _mainPaper.setTime(time);
    v.setValue(_mainPaper);
    emit this->sendData(descriptor,MSG_GETPAPER,v);
}

bool MainApp::managerLogin(User m)
{
    QSqlQuery query = _DBM->managerLogin(m.getId(),m.getPassword());
    if(query.size() > 0)
    {
        emit this->LoginOK();
        return true;
    }
    else
    {
        QMessageBox msg;
        msg.setText(QStringLiteral("用户名或密码错误"));
        msg.exec();
        return false;
    }
}

void MainApp::userStateChange(int descriptor, QString state)
{
    for(int i = 0; i < _userList.count(); i++)
    {
        if(descriptor == _userList.at(i)->getSockDescriptor() || descriptor == -1)
        {
            _userList.at(i)->setState(state);
        }
    }

    emit this->updateUserTable(_userList);
}

void MainApp::removeUser(int descriptor)
{
    for(int i = 0; i < _userList.count(); i++)
    {
        if(_userList.at(i)->getSockDescriptor() == descriptor)
        {
            if(_userList.at(i)->getState() != QStringLiteral("已经交卷"))
            {
                _userList.at(i)->setHostName("");
                this->userStateChange(descriptor,QStringLiteral("未登录"));
            }
        }
    }
}

void MainApp::saveUsertoPaperMark(int pid, QList<Student *> ulist)
{
    QMessageBox msg;
    for(int i = 0; i < ulist.count(); i++)
    {
        if(_DBM->insertPaperMark(NULL,NULL,NULL,pid,ulist.at(i)->getID()))
        {
            _DBM->insertObAnswers(pid,ulist.at(i)->getID(),NULL);
            _DBM->insertSubAnswers(pid,ulist.at(i)->getID());

        }
        else
        {
            QString name = ulist.at(i)->getName();
            QString text = QStringLiteral("学生 '%1' 添加失败，已完成该次考试的学生不能再次添加").arg(name);
            msg.setText(text);
            msg.exec();
        }
    }
    msg.setText(QStringLiteral("操作完成"));
    msg.exec();
}

void MainApp::dealObAnswers(EssayAnswers obans)
{
    _DBM->updateObAnswers(obans.getPaperId(),obans.getStudentId(),obans.getAnswers());
    QString ans_string = obans.getAnswers();
    QStringList ansList;
    int temp = 0;
    while(temp < ans_string.length())
    {
        ansList.append(ans_string.mid(temp,ans_string.indexOf(",")));
        temp = ans_string.indexOf(",",temp)+1;
    }
    QString eachObmark;
    QString obMarkString;
    eachObmark = QString::number(_mainPaper.getTotalMark() * _mainPaper.getPercent() / 100 / _mainPaper.choiceQuestionList.count());
    for(int i = 0; i < ansList.count(); i++)
    {
        QString correctAns = _mainPaper.choiceQuestionList.value(i).getAnswer();
        if(ansList.at(i) == correctAns)
        {

            obMarkString.append(eachObmark);
        }
        else
            obMarkString.append("0");

        obMarkString.append(",");
    }
    _DBM->updatePaperMarkObmark(obMarkString,obans.getPaperId(),obans.getStudentId());
}

void MainApp::dealSubAnswers(ChoiceAnswers subans)
{

    for(int i = 0; i < subans.getAnswerList().count(); i++)
    {
        _DBM->updateSubAnswers(subans.getPaperId(),subans.getStudentId(),i+1,subans.getAnswerList().at(i));
    }
    _DBM->updatePaperMarkDone(QDate::currentDate().toString(),subans.getPaperId(),subans.getStudentId());
}

void MainApp::submitSubMark(QStringList submark)
{
    _DBM->updatePaperMarkSubmark(submark.at(2),submark.at(0).toInt(),submark.at(1));
    QSqlQuery query = _DBM->searchPaperMark(submark.at(0).toInt(),submark.at(1));
    query.next();
    int totalmark = 0;
    QString ob = query.value(0).toString();
    QString sub = query.value(1).toString();
    int temp = 0;
    while(temp < ob.length())
    {
        totalmark += ob.mid(temp,ob.indexOf(",",temp)-temp).toInt();
        temp = ob.indexOf(",",temp)+1;
    }
    temp = 0;
    while(temp < sub.length())
    {
        totalmark += sub.mid(temp,sub.indexOf(",",temp)-temp).toInt();
        temp = sub.indexOf(",",temp)+1;
    }
    _DBM->updatePaperMarkTotalmark(totalmark,submark.at(0).toInt(),submark.at(1));
}

QList<Student*> MainApp::getUserByPaperId(int id,QString state)
{
    QSqlQuery query = _DBM->queryPaperMark1(id);
    QList<Student*> ulist;
    while(query.next())
    {
        Student *u = new Student;
        u->setID(query.value(8).toString());
        u->setName(query.value(9).toString());
        u->setGrade(query.value(10).toInt());
        u->setClass(query.value(11).toInt());
        u->setState(query.value(5).toString());
        if(u->getState() ==state)
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
    QSqlQuery query = _DBM->selectPaperById(pid);
    query.next();
    QString sub_ids = query.value(2).toString();
    query.clear();
    query = _DBM->selectSubQuestions();
    while(query.next())
    {
        if(sub_ids.indexOf(query.value(0).toString()) >= 0)
        {
            sub.append(query.value(1).toString());
        }
    }
    query.clear();
    query = _DBM->querySubAnswers(pid,uid);
    query.next();
    int m = 2;
    int count = sub.size();
    for(int i = count; i < count*2; i++)
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
    query = _DBM->queryPaperMark2(a);

    QSqlQuery s = _DBM->selectStudentByID(a);
    QString temp;
    if(s.next())
    {
        temp = s.value(1).toString();
    }

    while(query.next())
    {
        if(query.value(6).toString() == QStringLiteral("已批改"))
        {
            Combo *b = new Combo;
            //from user
            b->setUserId(a);
            b->setName(temp);
            b->setGrade(s.value(2).toInt());
            b->setClass(s.value(3).toInt());
            int obmark = 0;
            int submark = 0;
            QString ob = query.value(0).toString();
            QString sub = query.value(1).toString();
            int t = 0;
            while(t < ob.length())
            {
                obmark += ob.mid(t,ob.indexOf(",",t)-t).toInt();
                t = ob.indexOf(",",t)+1;
            }
            t = 0;
            while(t < sub.length())
            {
                submark += sub.mid(t,sub.indexOf(",",t)-t).toInt();
                t = sub.indexOf(",",t)+1;
            }
            b->setObmark(obmark);
            b->setSubmark(submark);
            //from paper
            b->setPaperId(query.value(3).toInt());
            QSqlQuery h = _DBM->selectPaperById(query.value(3).toInt());
            if(h.next())
                b->setPaperName(h.value(5).toString());

            //from self (paper mark)
            b->setPaperMark(query.value(2).toInt());
            comboList.append(b);
        }
    }
    emit this->showCombo(comboList);
}

void MainApp::getCombo_paperid(int id)
{
    QList <Combo*> comboList;
    QSqlQuery query;
    query = _DBM->queryPaperMark1(id);

    QSqlQuery s = _DBM->selectPaperById(id);
    QString temp;
    if(s.next())
    {
        temp = s.value(5).toString();
    }

    while(query.next())
    {
        if(query.value(6).toString() == QStringLiteral("已批改"))
        {
            Combo *b = new Combo;
            b->setPaperId(id);
            b->setPaperName(temp);
            int obmark = 0;
            int submark = 0;
            QString ob = query.value(0).toString();
            QString sub = query.value(1).toString();
            int t = 0;
            while(t < ob.length())
            {
                obmark += ob.mid(t,ob.indexOf(",",t)-t).toInt();
                t = ob.indexOf(",",t)+1;
            }
            t = 0;
            while(t < sub.length())
            {
                submark += sub.mid(t,sub.indexOf(",",t)-t).toInt();
                t = sub.indexOf(",",t)+1;
            }
            b->setObmark(obmark);
            b->setSubmark(submark);
            //from paper
            b->setPaperId(query.value(3).toInt());
            QSqlQuery h = _DBM->selectStudentByID(query.value(4).toString());
            if(h.next())
            {
                b->setUserId(h.value(0).toString());
                b->setName(h.value(1).toString());
                b->setGrade(h.value(2).toInt());
                b->setClass(h.value(3).toInt());
            }
            //from self (paper mark)
            b->setPaperMark(query.value(2).toInt());
            comboList.append(b);
        }
    }
    emit this->showCombo(comboList);
}

void MainApp::getManager(){
    QList<User *> managerList;
    QSqlQuery query;
    query = _DBM->selectManager();

    while(query.next()){
        User * managerptr = new User();
        managerptr->setId(query.value(0).toInt());
        managerptr->setName(query.value(1).toString());
        managerptr->setPassword(query.value(2).toString());
        managerList.append(managerptr);
    }

    emit this->showManager(managerList);
}

void MainApp::getSubject()
{
    QList<QString> subjectList;
    QSqlQuery query;
    query = _DBM->selectSubject();

    while(query.next()){
        subjectList.append(query.value(1).toString());
    }

    emit this->showSubject(subjectList);
}

void MainApp::getType()
{
    QMap<int, QString> type;
    QSqlQuery query;
    query = _DBM->selectType();

    while(query.next()){
        type[query.value(0).toInt()] = query.value(1).toString();
    }

    emit this->showType(type);
}

void MainApp::addType(int id, QString type)
{
    _DBM->insertType(id, type);
}

void MainApp::deleteType(int id)
{
    _DBM->deleteType(id);
}

void MainApp::getUser()
{
    QList<Student*> studentList;
    QList<User*> userList;

    QSqlQuery query;
    query = _DBM->selectStudent();

    while(query.next())
    {
        Student *studentptr = new Student();
        studentptr->setID(query.value(0).toString());
        studentptr->setName(query.value(1).toString());
        studentptr->setGrade(query.value(2).toInt());
        studentptr->setClass(query.value(3).toInt());
        studentptr->setPassword(query.value(4).toString());
        studentList.append(studentptr);
    }

    query.clear();
    query = _DBM->selectUser();
    while(query.next())
    {
        User *userptr = new User();
        userptr->setId(query.value(0).toInt());
        userptr->setName(query.value(1).toString());
        userptr->setPassword(query.value(2).toString());
        userptr->setSubject(query.value(3).toString());
        userList.append(userptr);
    }
    emit this->showUser(studentList,userList);
}

void MainApp::addStudent(Student *user)
{
    _DBM->insertStudent(user->getID(), user->getName(), user->getGrade(), user->getClass(), user->getPassword());
}

void MainApp::addTeacher(User *m)
{
    QSqlQuery query;
    query = _DBM->selectUserTypeBySubject(m->getSubject());
    query.first();
    _DBM->insertServerUser(m->getId(),m->getName(),m->getPassword(), query.value(0).toInt());
}

void MainApp::addManager(User * user)
{
    QSqlQuery query;
    query = _DBM->selectUserTypeBySubject(QStringLiteral("管理员"));
    query.first();
    _DBM->insertServerUser(user->getId(), user->getName(), user->getPassword(), query.value(0).toInt());
}

void MainApp::modifyUser(Student u)
{
    _DBM->modifyStudent(u.getID(),u.getName(),u.getGrade(),u.getClass(),u.getPassword());
}

void MainApp::deleteUserId(QString a)
{
    _DBM->deleteStudentById(a);
}

void MainApp::deleteManagerId(int a)
{
    _DBM->deleteServerUserByID(a);
}

void MainApp::outputUser()
{
    QList<Student*> userList;

    QSqlQuery query;
    query = _DBM->selectStudent();

    while(query.next())
    {
        Student *userptr = new Student();
        userptr->setID(query.value(0).toString());
        userptr->setName(query.value(1).toString());
        userptr->setGrade(query.value(2).toInt());
        userptr->setClass(query.value(3).toInt());
        userptr->setPassword(query.value(4).toString());
        userList.append(userptr);
    }
    _IOM->outputUser(userList);
    QMessageBox msg;
    msg.setText(QStringLiteral("导出成功。"));
    msg.exec();
}

void MainApp::outputOb()
{
    QList<ChoiceQuestions*> obList;
    QSqlQuery query;
    query = _DBM->selectObQuestions();
    while(query.next())
    {
        ChoiceQuestions *ob_que = new ChoiceQuestions;
        ob_que->setQuestionId(query.value(0).toInt());
        ob_que->setQuestionTitle(query.value(1).toString());
        ob_que->setAnswer(query.value(2).toString());
        ob_que->setSubjectID(query.value(3).toString());
        obList.append(ob_que);
    }
    _IOM->outputOb(obList);
    QMessageBox msg;
    msg.setText(QStringLiteral("导出成功。"));
    msg.exec();
}

void MainApp::outputSub()
{
    QList<EssayQuestions*> subList;
    QSqlQuery query = _DBM->selectSubQuestions();
    while(query.next())
    {
        EssayQuestions *sub_que = new EssayQuestions;
        sub_que->setQuestionId(query.value(0).toInt());
        sub_que->setQuestionTitle(query.value(1).toString());
        sub_que->setSubjectID(query.value(2).toString());
        subList.append(sub_que);
    }
    _IOM->outputSub(subList);
    QMessageBox msg;
    msg.setText(QStringLiteral("导出成功。"));
    msg.exec();
}

void MainApp::outputPaper()
{
    QList<Paper*> paperList;
    QSqlQuery query;
    query = _DBM->selectPaper();
    while(query.next())
    {
        Paper *p = new Paper;
        p->setPaperId(query.value(0).toInt());
        p->setObQuIds(query.value(1).toString());
        p->setSubQuIds(query.value(2).toString());
        p->setTotalMark(query.value(3).toInt());
        p->setPercent(query.value(4).toInt());
        p->setDescription(query.value(5).toString());
        p->setTime(query.value(6).toInt());
        paperList.append(p);
    }
    _IOM->outputPaper(paperList);
    QMessageBox msg;
    msg.setText(QStringLiteral("导出成功。"));
    msg.exec();
}

void MainApp::inputUser(QString path)
{
    QList<Student*> userlist;
    userlist = _IOM->inputUser(path);
    for(int i = 0; i < userlist.count(); i++)
    {
        _DBM->insertStudent(
            userlist.at(i)->getID(),
            userlist.at(i)->getName(),
            userlist.at(i)->getGrade(),
            userlist.at(i)->getClass(),
            userlist.at(i)->getPassword());
    }
    QMessageBox msg;
    msg.setText(QStringLiteral("导入成功。"));
    msg.exec();
}

void MainApp::inputOb(QString path)
{
    QList<ChoiceQuestions*> oblist;
    oblist = _IOM->inputOb(path);
    for(int i = 0; i < oblist.count(); i++)
    {
        _DBM->insertOb(oblist.at(i)->getQuestionId(),oblist.at(i)->getSubjectID(),oblist.at(i)->getQuestionTitle(),oblist.at(i)->getAnswer());
    }
    QMessageBox msg;
    msg.setText(QStringLiteral("导入成功。"));
    msg.exec();
}

void MainApp::inputSub(QString path)
{
    QList<EssayQuestions*> sublist;
    sublist = _IOM->inputSub(path);
    for(int i = 0; i < sublist.count(); i++)
    {
        _DBM->insertSub(sublist.at(i)->getQuestionId(),sublist.at(i)->getSubjectID(),sublist.at(i)->getQuestionTitle());
    }
    QMessageBox msg;
    msg.setText(QStringLiteral("导入成功。"));
    msg.exec();
}

void MainApp::inputPaper(QString path)
{
    QList<Paper*> plist;
    plist = _IOM->inputPaper(path);
    for(int i = 0; i < plist.count(); i++)
    {
        _DBM->insertPaper(
            plist.at(i)->getObQuIds(),
            plist.at(i)->getSubQuIds(),
            plist.at(i)->getTotalMark(),
            plist.at(i)->getPercent(),
            plist.at(i)->getDescription(),
            plist.at(i)->getTime());
    }
    QMessageBox msg;
    msg.setText(QStringLiteral("导入成功。"));
    msg.exec();
}

void MainApp::sendInfo(QStringList list)
{
    _infoList.clear();
    _infoList.append("*,");
    for(int i = 0; i < list.count(); i++)
    {
        _infoList.append(list.at(i));
        _infoList.append(",");
    }
}


void MainApp::delete_score(int pid,qlonglong uid)
{
    if(_DBM->deleteScore(pid,uid))
    {
        QMessageBox msg;
        msg.setText("删除成功");
        msg.exec();
    }
}
