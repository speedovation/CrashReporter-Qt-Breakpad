/****************************************************************************
**
** Copyright (C) 2007-2015 Speedovation
** Contact: Speedovation Lab (info@speedovation.com)
**
** KineticWing IDE CrashHandler
** http:// kineticwing.com
** This file is part of the core classes of the KiWi Editor (IDE)
**
** Author: Yash Bhardwaj
** License : Apache License 2.0
**
** All rights are reserved.
*/

#include "CrashReporterWidget.h"
#include "ui_CrashReporterWidget.h"

#include <QNetworkReply>
#include <QMessageBox>

#include "../../version.h"

CrashReporterWidget::CrashReporterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrashReporterWidget)
{
    ui->setupUi(this);

    setWindowTitle(VER_PRODUCTNAME_STR);

}

CrashReporterWidget::~CrashReporterWidget()
{
    delete ui;
}

void CrashReporterWidget::setDmpPath(const QString& path)
{
    _dmpPath = path;
}

void CrashReporterWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void CrashReporterWidget::on_btnSendReport_clicked()
{
    // trigger the request - see the examples in the following sections

    QString url_str = CRASHREPORTER_SUBMIT_URL;

    HttpRequestInput input(url_str, "POST");

    input.add_var( "BuildID", BUILD_ID );
    input.add_var( "ProductName", VER_PRODUCTNAME_STR );
    input.add_var( "version", VER_FILEVERSION_STR );


    input.add_var( "email", ui->email->text() );
    input.add_var( "desc", ui->desc->toPlainText() );


    input.add_file("dmpFile", _dmpPath, NULL, "binary/octet-stream");

    HttpRequestWorker *worker = new HttpRequestWorker(this);
    connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result(HttpRequestWorker*)));
    worker->execute(&input);

}



void CrashReporterWidget::handle_result(HttpRequestWorker *worker) {
    QString msg;

    if (worker->error_type == QNetworkReply::NoError) {
        // communication was successful
        msg = "Success - Response: " + worker->response;
    }
    else {
        // an error occurred
        msg = "Error: " + worker->error_str;
    }

    QMessageBox::information(this, "", msg);
}
