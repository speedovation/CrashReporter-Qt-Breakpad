[![](https://img.shields.io/badge/Alpha-Release-green.svg?style=flat-square)](http://kineticwing.com)

## CrashReporter for Qt Apps
After getting frustated to get crash reporter (or with proper license). I decided to create one. This is result.

### Download
Clone repo or download from releases. [![](https://img.shields.io/badge/Download_Alpha-Release-green.svg?style=flat-square)](https://github.com/speedovation/CrashReporter-Qt-Breakpad/archive/v1.0.1.alpha.tar.gz)

### How to use

* Step 1: Include Header in app. Create version.h from sample_version.h file which suits your app.
```
#include "version.h"
#include "CrashHandler.h"
```

* Step 2: Init Crash Handler
```
CrashManager::CrashHandler::instance()->Init( QDir::tempPath());
```

* Step 3: Open CrashRepoter pro file(Qt Project) and build executable out of it. Put it in same dir of App executable.

* Step 4: Compile App.

* Step 5: Crash will open crashreporter.

That's it. :)


### Components
* **CrashReporter** - Support features like Send report with email,desc and dmp file to server, restart app.
* **CrashHandler**  - Add inside Qt App. It will create crash report and trigger crash reporter automatically.
* **breakpad** - Google breakpad library ( BSD 3 license)
* **WebInterface** - Receives dmp file, email, desc and sends email to user.

### TODO
* Tests
* Examples
* Add screenshots
* Share web server code for uploading dmp file and send mail to developers
* Any WIKI page about how easy to use breakpad with Qt.
* Enable create issue button in crash reporter

For all TODOs I'm pretty much rely of PULL requests so make one and contribute back.

### License
* Apache 2.0 license

### Finished
* Crash reporter
* Server side done. Dmp file upload and send mail is also done.
* Crash Handler
* WebInterface
