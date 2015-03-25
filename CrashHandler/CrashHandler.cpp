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

#include <string.h>
#include "client/mac/handler/exception_handler.h"

#elif defined(Q_OS_LINUX)

#include <sys/types.h>
#include <sys/wait.h>
#include "client/linux/handler/exception_handler.h"

#elif defined(Q_OS_WIN32)

#include "Tchar.h"
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


#ifdef Q_OS_WIN


    bool launcher(wchar_t* program)
    {

        STARTUPINFO si = {};
        si.cb = sizeof si;

        PROCESS_INFORMATION pi = {};

        if ( !CreateProcess(NULL, program , 0, FALSE, 0,
                            CREATE_DEFAULT_ERROR_MODE | CREATE_NO_WINDOW | DETACHED_PROCESS,
                            0, 0, &si, &pi) )
        {
            std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
        }
        else
        {
            //std::cout << "Waiting on process for 5 seconds.." << std::endl;
           // WaitForSingleObject(pi.hProcess, 5 * 1000);
            /*
               if ( TerminateProcess(pi.hProcess, 0) ) // Evil
                   cout << "Process terminated!";
            */
            if ( PostThreadMessage(pi.dwThreadId, WM_QUIT, 0, 0) ) // Good
                std::cout << "Request to terminate process has been sent!";

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }


        //ref http://stackoverflow.com/questions/1067789/how-to-create-a-process-in-c-on-windows
       /*
        * Code Here for info purpose
        *

        SHELLEXECUTEINFO shExecInfo;

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


          const size_t cSize = strlen(program)+1;
          wchar_t* wc = new wchar_t[cSize];
          mbstowcs (wc, program, cSize);

         We will do something like this
          LPTSTR szCmdline[] = _tcsdup(TEXT("\"C:\\Program Files\\MyApp\" -L -S"));
          CreateProcess(NULL, szCmdline, /*...* /);

        LPCTSTR lpApplicationName = program; // The program to be executed

        LPSTARTUPINFO lpStartupInfo;
        LPPROCESS_INFORMATION lpProcessInfo;

        memset(&lpStartupInfo, 0, sizeof(lpStartupInfo));
        memset(&lpProcessInfo, 0, sizeof(lpProcessInfo));

        // Create the process
        if (!CreateProcess(lpApplicationName,
                           NULL, NULL, NULL,
                           NULL, NULL, NULL, NULL,
                           lpStartupInfo,
                           lpProcessInformation
                           )
                ) {
            std::cerr << "Uh-Oh! CreateProcess() failed to start program \"" << lpApplicationName << "\"\n";
            exit(1);
        }

        std::cout << "Started program \"" << lpApplicationName << "\" successfully\n";

        */


#else

    bool launcher(const char* program, const char* path)
    {
        // TODO launcher
        //	if(!GlobalHandlerPrivate::reporter_.isEmpty()) {
        //		QProcess::startDetached(GlobalHandlerPrivate::reporter_);	// very likely we will die there
        //	}

        //LINUX and WIN Ref : http://www.cplusplus.com/forum/lounge/17684/


        //FOR LINUX and MAC
        //        char* programPath = "/bin/bash";


        //Cout is not visible in qtcreator output window..may be QtC.. bug or I don't know
        //Visible on terminal output
        std::cout << "CrashReporter: "   << program
                  << "Dmppath: "      << path;

        pid_t pid = fork(); /* Create a child process */

        switch (pid) {
            case -1: /* Error */
                std::cerr << "Uh-Oh! fork() failed.\n";
                exit(1);
            case 0: /* Child process */
            {



                execl(program,program, path,(char*) 0 ); /* Execute the program */
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

        //Q_UNUSED(path);
#endif


        Q_UNUSED(program);
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

 char* path;

#ifdef defined(Q_OS_LINUX)
        path =  CrashHandlerPrivate::pHandler->minidump_descriptor().path()
        launcher(CrashHandlerPrivate::reporter_,path);
#elif defined(Q_OS_WIN)

        const size_t cSize = strlen(CrashHandlerPrivate::reporter_)+1;
        wchar_t* program = new wchar_t[cSize];
        mbstowcs ( program, CrashHandlerPrivate::reporter_, cSize);

        wchar_t* wpath = new wchar_t[MAX_PATH * 10];

        wcscpy( wpath, program);

        wcscat( wpath, L" ");
        wcscat( wpath, _dump_dir );
        wcscat( wpath, L"/" );
        wcscat( wpath, _minidump_id );
        wcscat( wpath, L".dmp" );


        wcstombs(path,wpath, sizeof(path) );


        launcher(wpath);
#elif defined(Q_OS_MAC)

         strcpy (path,_dump_dir);
         strcat (path,"/");
         strcat (path,_minidump_id);
         strcat (path,".dmp");

         launcher(CrashHandlerPrivate::reporter_,path);


#endif

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
#elif defined(Q_OS_LINUX)
            // MAYBE(AlekSi) Better place for Linux? libexec? or what?
            rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + rep);
#elif defined(Q_OS_WIN)
            // add .exe for Windows if needed
            if(!QDir().exists(rep)) {
                rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + rep + QLatin1String(".exe"));
            }
#endif

            qDebug("BreakpadQt: setReporter: %s -> %s", qPrintable(reporter), qPrintable(rep));
        }


        Q_ASSERT(QDir::isAbsolutePath(rep));

        Q_ASSERT(QDir().exists(rep));

        qstrcpy(d->reporter_, QFile::encodeName(rep).data());
    }



}
