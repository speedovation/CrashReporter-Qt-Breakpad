/****************************************************************************
**
** Original Author :  Aleksey Palazhchenk
** Copyright (C) 2009, Aleksey Palazhchenk  BSD 2-Clause License
**
** Modifications
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


#include "CrashHandler.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>

#if defined(Q_OS_MAC)
#include "client/mac/handler/exception_handler.h"
#elif defined(Q_OS_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#endif

namespace CrashManager
{

    class GlobalHandlerPrivate
    {
        public:
            GlobalHandlerPrivate();
            ~GlobalHandlerPrivate();

        public:
            static char reporter_[1024];
            static char reporterArguments_[8*1024];
            static google_breakpad::ExceptionHandler* handler_;
            static ReportCrashesToSystem reportCrashesToSystem_;
    };

    char GlobalHandlerPrivate::reporter_[1024] = {0};
    char GlobalHandlerPrivate::reporterArguments_[8*1024] = {0};
    google_breakpad::ExceptionHandler* GlobalHandlerPrivate::handler_ = 0;
    ReportCrashesToSystem GlobalHandlerPrivate::reportCrashesToSystem_ = ReportUnhandled;


    bool launcher(const char* program, const char* const arguments[])
    {
        // TODO launcher
        //	if(!GlobalHandlerPrivate::reporter_.isEmpty()) {
        //		QProcess::startDetached(GlobalHandlerPrivate::reporter_);	// very likely we will die there
        //	}

        Q_UNUSED(program);
        Q_UNUSED(arguments);
        return false;
    }


#if defined(Q_OS_WIN32)
    bool DumpCallback(const wchar_t* _dump_dir,
                      const wchar_t* _minidump_id,
                      void* context,
                      EXCEPTION_POINTERS* exinfo,
                      MDRawAssertionInfo* assertion,
                      bool success)
#else
    bool DumpCallback(const char* _dump_dir,
                      const char* _minidump_id,
                      void *context, bool success)
#endif
    {
        Q_UNUSED(_dump_dir);
        Q_UNUSED(_minidump_id);
        Q_UNUSED(context);
#if defined(Q_OS_WIN32)
        Q_UNUSED(assertion);
        Q_UNUSED(exinfo);
#endif

        /*
        NO STACK USE, NO HEAP USE THERE !!!
        Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
    */

        launcher(GlobalHandlerPrivate::reporter_, 0);
        return (GlobalHandlerPrivate::reportCrashesToSystem_ == ReportUnhandled) ? success : false;
    }


    GlobalHandlerPrivate::GlobalHandlerPrivate()
    {
        std::string path = QString::fromAscii("").toStdString();
        handler_ = new google_breakpad::ExceptionHandler(/*DumpPath*/ path, /*FilterCallback*/ 0, DumpCallback, /*context*/ 0, true, 0);
    }

    GlobalHandlerPrivate::~GlobalHandlerPrivate()
    {
        delete handler_;
        handler_ = 0;
    }


    GlobalHandler* GlobalHandler::instance()
    {
        static GlobalHandler globalHandler;
        return &globalHandler;
    }

    GlobalHandler::GlobalHandler()
    {
        d = new GlobalHandlerPrivate();
    }

    GlobalHandler::~GlobalHandler()
    {
        delete d;
        d = 0;
    }

    void GlobalHandler::setDumpPath(const QString& path)
    {
        QString absPath = path;

        if(!QDir::isAbsolutePath(absPath)) {
            absPath = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + path);
            qDebug("BreakpadQt: setDumpPath: %s -> %s", qPrintable(path), qPrintable(absPath));
        }


        Q_ASSERT(QDir::isAbsolutePath(absPath));

        QDir().mkpath(absPath);
        Q_ASSERT(QDir().exists(absPath));

#if defined(Q_OS_WIN32)
        d->handler_->set_dump_path(absPath.toStdWString());
#else
        d->handler_->set_dump_path(absPath.toStdString());
#endif
    }

    void GlobalHandler::setReporter(const QString& reporter)
    {
        QString rep = reporter;

        if(!QDir::isAbsolutePath(rep)) {
#if defined(Q_OS_MAC)
            // TODO(AlekSi) What to do if we are not inside bundle?
            rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/../Resources/") + rep);
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
            // MAYBE(AlekSi) Better place for Linux? libexec? or what?
            rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + rep);
#else
            What is this?!
             #endif

                         qDebug("BreakpadQt: setReporter: %s -> %s", qPrintable(reporter), qPrintable(rep));
        }
        Q_ASSERT(QDir::isAbsolutePath(rep));

        // add .exe for Windows if needed
#if defined(Q_OS_WIN32)
        if(!QDir().exists(rep)) {
            rep += QLatin1String(".exe");
        }
#endif
        Q_ASSERT(QDir().exists(rep));

        qstrcpy(d->reporter_, QFile::encodeName(rep));
    }

    void GlobalHandler::setReportCrashesToSystem(ReportCrashesToSystem report)
    {
        d->reportCrashesToSystem_ = report;
    }

    bool GlobalHandler::writeMinidump()
    {
        bool res = d->handler_->WriteMinidump();
        if (res) {
            qDebug("BreakpadQt: writeMinidump() successed.");
        } else {
            qWarning("BreakpadQt: writeMinidump() failed.");
        }
        return res;
    }

}	// namespace
