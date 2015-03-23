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


#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QtCore/QString>
namespace google_breakpad { class ExceptionHandler; }

namespace CrashManager
{

    class GlobalHandlerPrivate;

    enum ReportCrashesToSystem
    {
        ReportUnhandled = 1,
        AlwaysReport = 2
    };

    class GlobalHandler
    {
        public:
            static GlobalHandler* instance();

            void setDumpPath(const QString& path);
            void setReporter(const QString& reporter);
            void setReportCrashesToSystem(ReportCrashesToSystem report);
            bool writeMinidump();

        private:
            GlobalHandler();
            ~GlobalHandler();
            Q_DISABLE_COPY(GlobalHandler)

            GlobalHandlerPrivate* d;
    };

}	// namespace

#endif	// CRASHHANDLER_H
