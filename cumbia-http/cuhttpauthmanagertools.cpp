#include "cuhttpauthmanagertools.h"
#include <cumacros.h>
#include <termios.h>
#include <unistd.h>

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#endif

#define BUF_SIZ 256

class CuHttpAuthInputFactoryP {
public:
    CuHttpAuthInputFactoryP() : prompt(nullptr) {};
    CuHttpAuthPrompt_I *prompt;
};

CuHttpAuthInputFactory::CuHttpAuthInputFactory() {
    d = new CuHttpAuthInputFactoryP;
}

CuHttpAuthInputFactory::~CuHttpAuthInputFactory() {
    delete d;
}

CuHttpAuthPrompt_I *CuHttpAuthInputFactory::create_prompt(bool cli , QObject *parent) const
{
    if(d->prompt)
        return d->prompt;

#ifdef QT_WIDGETS_LIB
    if(qobject_cast<QApplication *>(qApp) && !cli) {
        CuHttpCredInputDialog *dlg = new CuHttpCredInputDialog(nullptr);
        connect(dlg, SIGNAL(destroyed()), this, SLOT(onPromptDestroyed()));
        d->prompt = dlg;
    }
#endif
    if(!d->prompt) {
        CuHttpCredInputCLIPrompt *clip = new CuHttpCredInputCLIPrompt(parent);
        connect(clip, SIGNAL(destroyed()), this, SLOT(onPromptDestroyed()));
        d->prompt = clip;
    }
    return d->prompt;
}

bool CuHttpAuthInputFactory::inExecution() const {
    return d->prompt != nullptr;
}

void CuHttpAuthInputFactory::onPromptDestroyed() {
    d->prompt = nullptr;
}

#ifdef QT_WIDGETS_LIB

CuHttpCredInputDialog::CuHttpCredInputDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    QGridLayout *lo = new QGridLayout(this);
    QLabel *lu = new QLabel("User name:", this);
    QLabel *lp = new QLabel("Password:", this);
    lu->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    lp->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QLineEdit *leu = new QLineEdit(this);
    leu->setObjectName("leu");
    leu->setPlaceholderText("Type the username");
    QLineEdit *lep = new QLineEdit(this);
    lep->setObjectName("lep");
    lep->setPlaceholderText("[password]");
    lep->setEchoMode(QLineEdit::Password);
    lo->addWidget(lu, 0, 0, 1, 1);
    lo->addWidget(leu, 0, 1, 1, 3);
    lo->addWidget(lp, 1, 0, 1, 1);
    lo->addWidget(lep, 1, 1, 1, 3);
    QPushButton *pbok = new QPushButton("OK", this);
    QPushButton *pbca = new QPushButton("Cancel", this);
    connect(pbok, SIGNAL(clicked()), this, SLOT(accept()));
    connect(pbca, SIGNAL(clicked()), this, SLOT(reject()));
    lo->addWidget(pbok, 2, 2, 1, 1);
    lo->addWidget(pbca, 2, 3, 1, 1);
}

CuHttpCredInputDialog::~CuHttpCredInputDialog() {
    pdelete("~CuHttpCredInputDialog %p", this);
}

void CuHttpCredInputDialog::getCreds() {
    this->show();
}

QObject *CuHttpCredInputDialog::qobj() {
    return this;
}

void CuHttpCredInputDialog::accept() {
    QString user = findChild<QLineEdit *>("leu")->text();
    QString passw = findChild<QLineEdit *>("lep")->text();
    emit onCredsReady(user, passw);
    QDialog::accept();
}

void CuHttpCredInputDialog::reject() {
    emit onCredsReady(QString(), QString());
    QDialog::reject();
}

bool CuHttpCredInputDialog::inExecution() const
{

}

#endif

CuHttpCredInputCLIPrompt::CuHttpCredInputCLIPrompt(QObject *parent) : QThread(parent) {
    connect(this, SIGNAL(finished()), this, SLOT(onInputFinished()));
}

CuHttpCredInputCLIPrompt::~CuHttpCredInputCLIPrompt() {
    pdelete("~CuHttpCredInputCLIPrompt %p", this);
}

void CuHttpCredInputCLIPrompt::getCreds() {
    start();
}

QObject *CuHttpCredInputCLIPrompt::qobj() {
    return qobject_cast<QObject *>(this);
}

bool CuHttpCredInputCLIPrompt::inExecution() const {
    return !this->isFinished();
}

void CuHttpCredInputCLIPrompt::onInputFinished() {
    emit onCredsReady(m_user, m_pass);
    deleteLater();
}

void CuHttpCredInputCLIPrompt::run()
{
    char buf[BUF_SIZ];
    memset(buf, 0, sizeof(char) * BUF_SIZ);
    m_user = m_pass = QString();
    printf("username  [return to cancel]: \e[1;32m");
    scanf("%255s", buf); // BUF_SIZ - 1
    while(getchar() != '\n');
    printf("\e[0m");
    if(strlen(buf) > 0) {
        m_user = QString(buf);
        printf("password [will not echo]:");
        struct termios oldattr, newattr;
        tcgetattr(STDIN_FILENO, &oldattr );
        newattr = oldattr;
        newattr.c_lflag &= ~ECHO;
        tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
        char c; int i = 0;
        while( (c = getchar() ) != '\n' && i < BUF_SIZ - 1) {
            buf[i] = c;
            i++;
        }
        buf[i] = '\0';
        tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
        m_pass = QString(buf);
    }
}
