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
**
** Work is based on
**   1. https://blog.inventic.eu/2012/08/qt-and-google-breakpad/
**   2. https://github.com/AlekSi/breakpad-qt - Aleksey Palazhchenko, BSD-2 License
**   3. http://www.cplusplus.com/forum/lounge/17684/
**   4. http://www.cplusplus.com/forum/beginner/48283/
**   5. http://www.siafoo.net/article/63
**
*/

#include "CrashHandler.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>

#include <QString>
#include <iostream>


#include "CrashHandler.h"
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>
#include <QString>

#include "../../version.h"

#if defined(Q_OS_MAC)

#include "client/mac/handler/exception_handler.h"

#elif defined(Q_OS_LINUX)

#include <sys/types.h>
#include <sys/wait.h>
#include "client/linux/handler/exception_handler.h"

#elif defined(Q_OS_WIN32)

#include "client/windows/handler/exception_handler.h"

#endif

namespace CrashManager
{
    /************************************************************************/
    /* CrashHandlerPrivate                                                  */
    /************************************************************************/
    class CrashHandlerPrivate
    {
        public:
            CrashHandlerPrivate()
            {
                pHandler = NULL;
            }

            ~CrashHandlerPrivate()
            {
                delete pHandler;
            }

            void InitCrashHandler(const QString& dumpPath);
            static google_breakpad::ExceptionHandler* pHandler;
            static bool bReportCrashesToSystem;

            static char reporter_[1024];
            static char reporterArguments_[8*1024];

    };

    google_breakpad::ExceptionHandler* CrashHandlerPrivate::pHandler = NULL;
    bool CrashHandlerPrivate::bReportCrashesToSystem = false;
    char CrashHandlerPrivate::reporter_[1024] = {0};
    char CrashHandlerPrivate::reporterArguments_[8*1024] = {0};

    /************************************************************************/
    /* DumpCallback                                                         */
    /************************************************************************/

    bool launcher(const char* program, const char* path)
    {
        // TODO launcher
        //	if(!GlobalHandlerPrivate::reporter_.isEmpty()) {
        //		QProcess::startDetached(GlobalHandlerPrivate::reporter_);	// very likely we will die there
        //	}

        //LINUX and WIN Ref : http://www.cplusplus.com/forum/lounge/17684/

#ifdef Q_OS_WIN

        //ref http://stackoverflow.com/questions/1067789/how-to-create-a-process-in-c-on-windows
       /* SHELLEXECUTEINFO shExecInfo;

        shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

        shExecInfo.fMask = NULL;
        shExecInfo.hwnd = NULL;
        shExecInfo.lpVerb = L"runas";
        shExecInfo.lpFile = L"regasm.exe";
        shExecInfo.lpParameters = L"testdll /tlb:test.tlb /codebase";
        shExecInfo.lpDirectory = NULL;
        shExecInfo.nShow = SW_NORMAL;
        shExecInfo.hInstApp = NULL;

        ShellExecuteEx(&shExecInfo);
        */



        //Ref : http://www.cplusplus.com/forum/beginner/48283/

        STARTUPINFO si = {};
        si.cb = sizeof si;

        PROCESS_INFORMATION pi = {};
        const TCHAR* target = _T(program);

        if ( !CreateProcess(target, 0, 0, FALSE, 0, 0, 0, 0, &si, &pi) )
        {
            cerr << "CreateProcess failed (" << GetLastError() << ").\n";
        }
        else
        {
            cout << "Waiting on process for 5 seconds.." << endl;
            WaitForSingleObject(pi.hProcess, 5 * 1000);
            /*
               if ( TerminateProcess(pi.hProcess, 0) ) // Evil
                   cout << "Process terminated!";
               */
            if ( PostThreadMessage(pi.dwThreadId, WM_QUIT, 0, 0) ) // Good
                cout << "Request to terminate process has been sent!";

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

       // cin.sync();
       // cin.ignore();


        //ALSO

//        LPCTSTR lpApplicationName = program; /* The program to be executed */

//        LPSTARTUPINFO lpStartupInfo;
//        LPPROCESS_INFORMATION lpProcessInfo;

//        memset(&lpStartupInfo, 0, sizeof(lpStartupInfo));
//        memset(&lpProcessInfo, 0, sizeof(lpProcessInfo));

//        /* Create the process */
//        if (!CreateProcess(lpApplicationName,
//                           NULL, NULL, NULL,
//                           NULL, NULL, NULL, NULL,
//                           lpStartupInfo,
//                           lpProcessInformation
//                           )
//                ) {
//            std::cerr << "Uh-Oh! CreateProcess() failed to start program \"" << lpApplicationName << "\"\n";
//            exit(1);
//        }

//        std::cout << "Started program \"" << lpApplicationName << "\" successfully\n";




#else
        //FOR LINUX and MAC
        //        char* programPath = "/bin/bash";


        //Cout is not visible in qtcreator output window..may be QtC.. bug or I don't know
        //Visible on terminal output
        std::cout << "CrashReporter: "   << CrashHandlerPrivate::reporter_
                  << "Dmppath: "      << CrashHandlerPrivate::pHandler->minidump_descriptor().path();

        pid_t pid = fork(); /* Create a child process */

        switch (pid) {
            case -1: /* Error */
                std::cerr << "Uh-Oh! fork() failed.\n";
                exit(1);
            case 0: /* Child process */
            {



                execl(program,program, CrashHandlerPrivate::pHandler->minidump_descriptor().path(),(char*) 0 ); /* Execute the program */
                std::cerr << "Uh-Oh! execl() failed!";
                /* execl doesn't return unless there's an error */
                //qApp->quit();
                exit(1);
            }
            default: /* Parent process */
                //Don't hand app
                //If required kill parent like below
                //kill it
                //int parent = getppid();
                //kill(parent, SIGKILL);
                return false;
        }

#endif


        Q_UNUSED(program);
        Q_UNUSED(path);
        return false;
    }




#if defined(Q_OS_WIN32)
    bool DumpCallback(const wchar_t* _dump_dir,const wchar_t* _minidump_id,void* context,EXCEPTION_POINTERS* exinfo,MDRawAssertionInfo* assertion,bool success)
#elif defined(Q_OS_LINUX)
    bool DumpCallback(const google_breakpad::MinidumpDescriptor &md,void *context, bool success)
#elif defined(Q_OS_MAC)
    bool DumpCallback(const char* _dump_dir,const char* _minidump_id,void *context, bool success)
#endif
    {
        Q_UNUSED(context);
#if defined(Q_OS_WIN32)
        Q_UNUSED(_dump_dir);
        Q_UNUSED(_minidump_id);
        Q_UNUSED(assertion);
        Q_UNUSED(exinfo);
#endif
        //qDebug("BreakpadQt crash");

        /*
        NO STACK USE, NO HEAP USE THERE !!!
        Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
        */


        const char* path;

#ifdef defined(Q_OS_LINUX)
        path =  CrashHandlerPrivate::pHandler->minidump_descriptor().path()
#elif defined(Q_OS_WIN)
         path = dump_dir + "/" + minidump_id  + ".dmp";

         qDebug("path" + path );
#endif


        launcher(CrashHandlerPrivate::reporter_,path);



        return CrashHandlerPrivate::bReportCrashesToSystem ? success : true;
    }

    void CrashHandlerPrivate::InitCrashHandler(const QString& dumpPath)
    {
        //if already init then skip rest
        if ( pHandler != NULL )
            return;

#if defined(Q_OS_WIN32)
        std::wstring pathAsStr = (const wchar_t*)dumpPath.utf16();
        pHandler = new google_breakpad::ExceptionHandler(
                    pathAsStr,
                    /*FilterCallback*/ 0,
                    DumpCallback,
                    /*context*/
                    0,
                    true
                    );
#elif defined(Q_OS_LINUX)
        std::string pathAsStr = dumpPath.toStdString();
        google_breakpad::MinidumpDescriptor md(pathAsStr);
        pHandler = new google_breakpad::ExceptionHandler(
                    md,
                    /*FilterCallback*/ 0,
                    DumpCallback,
                    /*context*/ 0,
                    true,
                    -1
                    );
#elif defined(Q_OS_MAC)
        std::string pathAsStr = dumpPath.toStdString();
        pHandler = new google_breakpad::ExceptionHandler(
                    pathAsStr,
                    /*FilterCallback*/ 0,
                    DumpCallback,
                    /*context*/
                    0,
                    true,
                    NULL
                    );
#endif



    }

    /************************************************************************/
    /* CrashHandler                                                         */
    /************************************************************************/
    CrashHandler* CrashHandler::instance()
    {
        static CrashHandler globalHandler;
        return &globalHandler;
    }

    CrashHandler::CrashHandler()
    {
        d = new CrashHandlerPrivate();
    }

    CrashHandler::~CrashHandler()
    {
        delete d;
    }

    void CrashHandler::setReportCrashesToSystem(bool report)
    {
        d->bReportCrashesToSystem = report;
    }

    bool CrashHandler::writeMinidump()
    {
        bool res = d->pHandler->WriteMinidump();
        if (res) {
            qDebug("BreakpadQt: writeMinidump() successed.");
        } else {
            qWarning("BreakpadQt: writeMinidump() failed.");
        }
        return res;
    }

    void CrashHandler::Init( const QString& reportPath )
    {
        d->InitCrashHandler(reportPath);

        setReporter(CRASHREPORTER_EXECUTABLE);
    }

    void CrashHandler::setReporter(const QString& reporter)
    {
        QString rep = reporter;

        if(!QDir::isAbsolutePath(rep))
        {

#if defined(Q_OS_MAC)
            // TODO(AlekSi) What to do if we are not inside bundle?
            rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/../Resources/") + rep);
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
            // MAYBE(AlekSi) Better place for Linux? libexec? or what?
            rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + rep);
#elif defined(Q_OS_WIN32)
            // add .exe for Windows if needed
            if(!QDir().exists(rep)) {
                rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + rep + QLatin1String(".exe");
            }
#endif

            qDebug("BreakpadQt: setReporter: %s -> %s", qPrintable(reporter), qPrintable(rep));
        }


        Q_ASSERT(QDir::isAbsolutePath(rep));

        Q_ASSERT(QFile().exists(rep));

        qstrcpy(d->reporter_, QFile::encodeName(rep).data());
    }



}
