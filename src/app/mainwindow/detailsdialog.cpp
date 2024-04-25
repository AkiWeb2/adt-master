#include "detailsdialog.h"
#include "ui_detailsdialog.h"

#include <QFont>
#include <QTextCursor>

DetailsDialog::DetailsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DetailsDialog)
{
    ui->setupUi(this);
    ui->closePushButton->setFocus();
}

DetailsDialog::~DetailsDialog()
{
    delete ui;
}

void DetailsDialog::setDetailsText(ADTExecutable *test)
{
    //ui->detailsPlainTextEdit->appendPlainText(test->m_log);
}

void DetailsDialog::clearDetailsText()
{
    ui->detailsPlainTextEdit->clear();
}

void DetailsDialog::on_closePushButton_clicked()
{
    close();
}

void DetailsDialog::on_getStdout(QString line)
{
    ui->detailsPlainTextEdit->blockSignals(true);
    QTextDocument *document = ui->detailsPlainTextEdit->document();
    QFont font              = document->defaultFont();
    font.setFamily("Courier New");
    document->setDefaultFont(font);
    ui->detailsPlainTextEdit->appendHtml("<p style=\"color:black;white-space:pre\">" + line.trimmed() + "</p>");
    ui->detailsPlainTextEdit->blockSignals(false);
}

void DetailsDialog::on_getStderr(QString line)
{
    ui->detailsPlainTextEdit->blockSignals(true);
    QTextDocument *document = ui->detailsPlainTextEdit->document();
    QFont font              = document->defaultFont();
    font.setFamily("Courier New");
    document->setDefaultFont(font);
    ui->detailsPlainTextEdit->appendHtml("<p style=\"color:red;white-space:pre\">" + line.trimmed() + "</p>");
    ui->detailsPlainTextEdit->blockSignals(false);
}
