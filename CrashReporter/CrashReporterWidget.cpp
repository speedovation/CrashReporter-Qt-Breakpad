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

CrashReporterWidget::CrashReporterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CrashReporterWidget)
{
    ui->setupUi(this);
}

CrashReporterWidget::~CrashReporterWidget()
{
    delete ui;
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

    QString url_str = "http://www.example.com/path/to/page.php";

    HttpRequestInput input(url_str, "POST");

    input.add_var("key1", "value1");
    input.add_var("key2", "value2");

    input.add_file("file1", "/path/to/file1.png", NULL, "image/png");
    input.add_file("file2", "/path/to/file2.png", NULL, "image/png");

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
