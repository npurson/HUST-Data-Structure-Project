#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // MenuBar
    QMenuBar *mBar = menuBar();
    mBar->setStyleSheet("QWidget{background:rgb(243, 243, 243)}");

    // Menu
    QMenu* file  = mBar->addMenu("文件(&F)");
    QMenu* edit  = mBar->addMenu("编辑(&E)");
    QMenu* build = mBar->addMenu("构建(&B)");
    QMenu* view  = mBar->addMenu("查看(&V)");
    QMenu* debug = mBar->addMenu("调试(&D)");
    QMenu* help  = mBar->addMenu("帮助(&H)");

    // Action
    // File
    QAction* newFile  = file->addAction("新建文件                Ctrl + N");
    QAction* openFile = file->addAction("打开文件                Ctrl + O");
    file->addSeparator();
    QAction* save     = file->addAction("保存                        Ctrl + S");
    QAction* saveAs   = file->addAction("另存为");
    file->addSeparator();
    QAction* exit     = file->addAction("退出                        Ctrl + W");

    // Edit
    QAction* undo    = edit->addAction("撤销                        Ctrl + Z");
    QAction* redo    = edit->addAction("重做                        Ctrl + Y");
    edit->addSeparator();
    QAction* cut     = edit->addAction("剪切                        Ctrl + X");
    QAction* copy    = edit->addAction("复制                        Ctrl + C");
    QAction* paste   = edit->addAction("粘贴                        Ctrl + V");
    edit->addSeparator();
    QAction* find    = edit->addAction("查找                        Ctrl + F");
    QAction* replace = edit->addAction("替换                        Ctrl + H");

    // Build
    QAction* buildAll    = build->addAction("🔨  构建项目");
    QAction* buildFile   = build->addAction("       构建当前文件        Ctrl + B");
    QAction* run         = build->addAction("🚀  运行                       Ctrl + R");
    QAction* deployAll   = build->addAction("       部署项目");
    QAction* deployFile  = build->addAction("       部署当前文件");
    build->addSeparator();
    QAction* rebuildAll  = build->addAction("🛠  重新构建项目");
    QAction* rebuildFile = build->addAction("       重新构建当前文件");
    QAction* clearAll    = build->addAction("💸  清理项目");
    QAction* clearFile   = build->addAction("       清理当前文件");

    // View
    QAction* cmdPlatte  = view->addAction("命令面板                Ctrl + P");
    QAction* appearance = view->addAction("外观");
    QAction* layout     = view->addAction("编辑器布局");
    view->addSeparator();
    QAction* explorer   = view->addAction("资源管理器             Ctrl + E");
    QAction* search     = view->addAction("搜索                        Ctrl + F");
    QAction* scm        = view->addAction("源代码管理             Ctrl + G");
    QAction* debugMode  = view->addAction("调试                        Ctrl + D");
    QAction* extensions = view->addAction("扩展                        Ctrl + X");

    // Debug
    QAction* startDebug   = debug->addAction("启动调试                F5");
    QAction* stopDebug    = debug->addAction("停止调试");
    QAction* restartDebug = debug->addAction("重启调试");
    debug->addSeparator();
    QAction* swtichStop   = debug->addAction("切换断点");
    QAction* newStop      = debug->addAction("新建断点");

    // Help
    QAction* welcome      = help->addAction("欢迎使用                           ");
    QAction* doc          = help->addAction("文档");
    QAction* releaseNotes = help->addAction("发行说明");
    help->addSeparator();
    QAction* license      = help->addAction("查看许可证");
    QAction* privacyStmt  = help->addAction("隐私声明");
    help->addSeparator();
    QAction* checkUpdate  = help->addAction("检查更新");
    QAction* about        = help->addAction("关于");

    connect(newFile, &QAction::triggered,
            [=]() {
                int c = QMessageBox::question(this, "方舟编译器", "是否要保存对当前文件的更改？\n\n如果不保存，更改将丢失。\n",
                                              QMessageBox::Save, QMessageBox::No, QMessageBox::Cancel);
                if (c == QMessageBox::Save)
                    QFileDialog::getOpenFileName(this, "保存文件", "../",
                                                 "源文件(*.c, *.cpp, *.h);;文本文件(*.txt);;所有文件(*.*)");
                if (c == QMessageBox::No) textEdit->clear();
            });

    connect(openFile, &QAction::triggered, this, &MainWindow::openF);

    connect(save, &QAction::triggered,
            [=]() {
                QFileDialog::getOpenFileName(this, "保存", "../",
                                             "源文件(*.c, *.cpp, *.h);;文本文件(*.txt);;所有文件(*.*)");
            });

    connect(saveAs, &QAction::triggered,
            [=]() {
                QFileDialog::getOpenFileName(this, "另存为", "../",
                                             "源文件(*.c, *.cpp, *.h);;文本文件(*.txt);;所有文件(*.*)");
            });

    connect(buildAll, &QAction::triggered,
            [=]() mutable {
                QObject::blockSignals(true);
                textEdit->setPlainText(analysis[0]);
                QObject::blockSignals(false);
            });

    connect(welcome, &QAction::triggered,
            [=]() {
                QMessageBox::about(this, "欢迎使用", "\n🤪  欢迎欢迎，热烈欢迎！\n");
            });

    connect(doc, &QAction::triggered,
            [=]() {
                QMessageBox::about(this, "方舟编译器 文档",
                                   "😆  方舟编译器 1.0.2 release\n\n"
                                   "       方舟编译器（SquareBoat Compiler），是第一款基于微内核的全场景分布式编译器，是我们自主研发的编译器。 "
                                   "2020年3月22日，方舟编译器在我们的Github上正式发布，我们会率先部署在智慧屏、车载终端、穿戴等智能终端上，"
                                   "未来会有越来越多的智能设备使用开源的方舟编译器。\n\n"
                                   "       方舟编译器实现模块化耦合，对应不同设备可弹性部署，方舟编译器有三层架构，第一层是内核，第二层是基础服务，"
                                   "第三层是程序框架。可用于大屏、PC、汽车等各种不同的设备上。 还可以随时用在手机上。\n\n"
                                   "       方舟编译器底层由方舟微内核、Linux 内核、Lite OS 组成，未来将发展为完全的方舟微内核架构。\n");
            });

    connect(releaseNotes, &QAction::triggered,
            [=]() {
                QMessageBox::about(this, "发行说明",
                                   "方舟编译器 1.0.2 release\n\n"
                                   "🔥  编辑器\n修复 由于文本格式设定错误造成的 MessageBox 信息显示错误\n"
                                   "重构 用户交互界面，优化了软件交互逻辑，修改了 Action 显示文本\n\n"
                                   "🛫  语法分析\n修复 表达式无法解析的错误\n"
                                   "修复 输入不符合语法规则的错误语句时无法解析，造成软件随机崩溃的问题\n"
                                   "新增 智能错误跳出功能，允许输入不符合语法规则的错误语句\n\n"
                                   "🍕  报错信息\n修复 行号显示错误。\n\n");
            });

    connect(license, &QAction::triggered,
            [=]() {
                QMessageBox::about(this, "许可证", "🧐  方舟编译器许可条款\n"
                                   "SquareBoat Compiler by LaoZiKaiFa.inc\n\n"
                                   "这些许可条款是您与我们达成的协议。这些条款适用于上述软件。"
                                   "这些条款也适用于针对该软件的任何我们服务或更新，但有不同条款的服务或更新除外。\n\n"
                                   "如果您遵守这些许可条款，您将拥有以下权利。\n"
                                   "    1.安装和使用权利。\n"
                                   "        a.通则。 您可以使用该软件任意数量的副本，以开发并测试您的应用程序，包括在您的内部公司网络内部署。\n"
                                   "        b.演示用途。 上面允许的用途包括使用该软件演示您的应用程序。\n"
                                   "        c.第三方程序。 该软件可能包含具有单独的法律声明或受其他协议约束的第三方组件，详见该软件随附的第三方声明文件（如有）。\n\n"
                                   "    2.数据收集。 该软件可能收集有关您和您对该软件的使用的信息，并将其发送给我们。 我们可能使用此信息提供服务和改进我们的产品和服务。\n\n"
                                   "    3.软件更新。 该软件会定期检查软件更新，并为您下载和安装这些更新。 "
                                   "您只能从我们或授权来源处获得软件更新。 我们可能需要更新您的系统来为您提供更新资讯。\n\n"
                                   "    4.反馈。 如果您向我们提供有关该软件的反馈，则意味着您向我们无偿提供以任何方式、"
                                   "出于任何目的使用和分享您的反馈并将其商业化的权利。\n\n"
                                   "    5.许可范围。 该软件只授予使用许可，不允许出售。 本协议只授予您使用该软件的某些权利。 我们保留所有其他权利。\n"
                                   );
            });

    connect(privacyStmt, &QAction::triggered,
            [=]() {
                QMessageBox::about(this, "隐私声明",
                                   "\n       我们十分重视您的隐私。 本隐私声明阐述了我们处理的个人数据以及我们处理个人数据的方式和目的。\n\n"
                                   "       我们提供了多种产品，其中包括用于帮助在全球范围内运营企业的服务器产品、家用设备、学生在学校使用的软件，"
                                   "以及开发人员用于创建和托管下一代产品的服务。 本声明中所提到的我们的产品包括服务、网站、应用、软件、服务器和设备。\n\n"
                                   "       请阅读本隐私声明中特定于产品的详细信息，这些详细信息提供了其他相关信息。 "
                                   "此声明适用于我们与您之间的互动、下列产品以及显示此声明的其他产品。\n");
            });

    connect(checkUpdate, &QAction::triggered,
            [=]() {
                QMessageBox::about(this, "检查更新", "\n👍  您的编译器已是最新版本。\n");
            });

    connect(about, &QAction::triggered,
            [=]() {
                QMessageBox::about(this, "关于 方舟编译器",
                                   "😭  方舟编译器 1.0.2 release\n\n"
                                   "       方舟编译器（SquareBoat Compiler），是第一款基于微内核的全场景分布式编译器，是我们自主研发的编译器。 "
                                   "2020年3月22日，方舟编译器在我们的Github上正式发布，我们会率先部署在智慧屏、车载终端、穿戴等智能终端上，"
                                   "未来会有越来越多的智能设备使用开源的方舟编译器。\n\n"
                                   "       方舟编译器实现模块化耦合，对应不同设备可弹性部署，方舟编译器有三层架构，第一层是内核，第二层是基础服务，"
                                   "第三层是程序框架。 可用于大屏、PC、汽车等各种不同的设备上。 还可以随时用在手机上。\n\n"
                                   "       方舟编译器底层由方舟微内核、Linux 内核、Lite OS 组成，未来将发展为完全的方舟微内核架构。\n");
            });


    // StatusBar
    QStatusBar* sBar = statusBar();
    sBar->setStyleSheet("QWidget{color:white; background:rgb(0, 122, 202)}");
    sBar->setSizeGripEnabled(false);
    sBar->addWidget(new QLabel(" 🔥 错误列表 ", this));
    sBar->addWidget(new QLabel(" 🍍 输出 ", this));
    sBar->addPermanentWidget(new QLabel(" ☭ ", this));
    sBar->addPermanentWidget(new QLabel(" UTF-8 ", this));
    sBar->addPermanentWidget(new QLabel(" Windows(CRLF) ", this));
    sBar->addPermanentWidget(new QLabel(" C/C++ ", this));
    sBar->addPermanentWidget(new QLabel(" ㍿ ㍰ ", this));

    // CoreWidget
    textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);

    // FloatingDock
    QDockWidget* syntaxDock = new QDockWidget;
    addDockWidget(Qt::RightDockWidgetArea, syntaxDock);
    QTextBrowser* syntaxText = new QTextBrowser(this);
    syntaxText->setStyleSheet("QWidget{color:black; background:rgb(243, 243, 243)}");
    syntaxDock->setWidget(syntaxText);

    QDockWidget* errDock = new QDockWidget;
    addDockWidget(Qt::BottomDockWidgetArea, errDock);
    QTextBrowser* errText = new QTextBrowser(this);
    errText->setStyleSheet("QWidget{color:black; background:rgb(243, 243, 243)}");
    errDock->setWidget(errText);

    connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::updateText);

    connect(this, &MainWindow::textChanged,
            [=]() mutable {
                syntaxText->setText(QString(analysis[1]));
                errText->setText(QString(analysis[2]));
            }
    );
}


void MainWindow::updateText()
{
    strcpy(text, textEdit->toPlainText().toLatin1().data());
    analysis = syntaxAnalysis(text);
    emit textChanged();
}


void MainWindow::openF()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, "打开文件", "../",
                                            "源文件(*.h, *.cpp, *.c);;文本文件(*.txt);;所有文件(*.*)");
    if(fileName != "") {
       QFile file(fileName);
       if(!file.open(QIODevice::ReadOnly | QIODevice::Text) || !file.isReadable())
           QMessageBox::warning(this, tr("错误"), tr("打开文件失败"));
       else {
            QTextStream textStream(&file);
            while(!textStream.atEnd())
                textEdit->setPlainText(textStream.readAll());
            textEdit->show();
            file.close();
       }
    }
    return;
}


MainWindow::~MainWindow()
{
}
