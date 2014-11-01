#include "gui/dialogs/connection_dialog.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QEvent>

#include "gui/gui_factory.h"
#include "common/qt/convert_string.h"
#include "common/utils.h"

namespace fastoredis
{
    ConnectionDialog::ConnectionDialog(const IConnectionSettingsBasePtr &connection, QWidget *parent)
        : QDialog(parent),
        connection_(connection)
    {
        using namespace common;
        setWindowIcon(GuiFactory::instance().serverIcon());
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove help button (?)

        connectionName_ = new QLineEdit;
        connectionName_->setText(convertFromString<QString>(connection_->connectionName()));

        typeConnection_ = new QComboBox;
        std::vector<std::string> supt = supportedConnectionTypes();
        for(std::vector<std::string>::const_iterator it = supt.begin(); it != supt.end(); ++it){
            typeConnection_->addItem(convertFromString<QString>(*it));
        }
        typeConnection_->setCurrentText(convertFromString16<QString>(common::convertToString16(connection_->connectionType())));
        VERIFY(connect(typeConnection_, SIGNAL(currentTextChanged(const QString&)), this, SLOT(typeConnectionChange(const QString&))));

        logging_ = new QCheckBox("Logging enabled");
        logging_->setChecked(connection_->loggingEnabled());

        commandLine_ = new QLineEdit;
        commandLine_->setText(convertFromString<QString>(connection_->commandLine()));

        QVBoxLayout *inputLayout = new QVBoxLayout;
        inputLayout->addWidget(connectionName_);
        inputLayout->addWidget(typeConnection_);
        inputLayout->addWidget(logging_);
        inputLayout->addWidget(commandLine_);

        testButton_ = new QPushButton("&Test");
        testButton_->setIcon(GuiFactory::instance().messageBoxInformationIcon());
        VERIFY(connect(testButton_, SIGNAL(clicked()), this, SLOT(testConnection())));

        QHBoxLayout *bottomLayout = new QHBoxLayout;
        bottomLayout->addWidget(testButton_, 1, Qt::AlignLeft);
        buttonBox_ = new QDialogButtonBox(this);
        buttonBox_->setOrientation(Qt::Horizontal);
        buttonBox_->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Save);
        VERIFY(connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept())));
        VERIFY(connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject())));
        bottomLayout->addWidget(buttonBox_);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(inputLayout);
        mainLayout->addLayout(bottomLayout);
        setLayout(mainLayout);

        //update controls
        typeConnectionChange(typeConnection_->currentText());
        retranslateUi();
    }

    void ConnectionDialog::typeConnectionChange(const QString &value)
    {
        connectionTypes currentType = common::convertFromString<connectionTypes>(common::convertToString(value));
        bool isValidType = currentType != badConnectionType();
        connectionName_->setEnabled(isValidType);
        testButton_->setEnabled(isValidType);
        commandLine_->setEnabled(isValidType);
        buttonBox_->button(QDialogButtonBox::Save)->setEnabled(isValidType);
        commandLine_->setToolTip(useHelpText(currentType));
    }

    void ConnectionDialog::accept()
    {
        if(validateAndApply()){
            connection_->setConnectionName(common::convertToString(connectionName_->text()));
            connection_->setCommandLine(common::convertToString(commandLine_->text()));
            connection_->setLoggingEnabled(logging_->isChecked());
            QDialog::accept();
        }
    }

    bool ConnectionDialog::validateAndApply()
    {
        return true;
    }

    void ConnectionDialog::testConnection()
    {
        if(validateAndApply()){
           // ConnectionDiagnosticDialog diag(_connection,this);
           // diag.exec();
        }
    }

    void ConnectionDialog::changeEvent(QEvent *e)
    {
        if(e->type() == QEvent::LanguageChange){
            retranslateUi();
        }
        QDialog::changeEvent(e);
    }

    void ConnectionDialog::retranslateUi()
    {
        setWindowTitle(tr("Connection Settings"));
    }
}
