[![](https://img.shields.io/badge/Release-Not%20ready-yellow.svg?style=flat-square)](http://kineticwing.com)

## CrashReporter for Qt Apps
After getting frustated to get crash reporter (or with proper license). I decided to create one. This is result.

Under dev. Not ready yet. Soon it will be. Stay tuned.

### Components
* **breakpad** - Google breakpad library ( BSD 3 license)
* **CrashReporter** - It will support features like send report,create github issue.
* **CrashHandler** - Need to add inside Qt App. It will create crash report and trigger crash reporter. As a base I've used work of [breakpad-qt](https://github.com/AlekSi/breakpad-qt) which is under BSD-2.

### Working
* Crash reporter features like send report, create issue.

### Finished
* Crash reporter
* Server side done. Dmp file upload and send mail is also done.

### TODO
* Examples
* Handler
* Add screenshots
* Share web server code for uploading dmp file and send mail to developers
* Any WIKI page about how easy to use breakpad with Qt.
* Enable create issue button in crash reporter

### License
* Apache 2.0 license
