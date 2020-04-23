/*
   Copyright 2011, David Sansome <me@davidsansome.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef LOGGING_H
#define LOGGING_H

#include <chrono>

#include <QtGlobal>
#include <QIODevice>
#include <QString>
#include <QDebug>

#ifdef QT_NO_DEBUG_STREAM
#  define qLog(level) while (false) QNoDebug()
#  define qLogCat(level, category) while (false) QNoDebug()
#else
#  define qLog(level) logging::CreateLogger##level(__LINE__, __PRETTY_FUNCTION__, nullptr)

// This macro specifies a separate category for message filtering.
// The default qLog will use the class name extracted from the function name for this purpose.
// The category is also printed in the message along with the class name.
#  define qLogCat(level, category) logging::CreateLogger##level(__LINE__, __PRETTY_FUNCTION__, category)

#endif  // QT_NO_DEBUG_STREAM

namespace logging {

class NullDevice : public QIODevice {
 protected:
  qint64 readData(char*, qint64) { return -1; }
  qint64 writeData(const char*, qint64 len) { return len; }
};

enum Level {
  Level_Fatal = -1,
  Level_Error = 0,
  Level_Warning,
  Level_Info,
  Level_Debug,
};

  void Init();
  void SetLevels(const QString& levels);

  void DumpStackTrace();

QDebug CreateLoggerFatal(int line, const char *pretty_function, const char* category);
QDebug CreateLoggerError(int line, const char *pretty_function, const char* category);

#ifdef QT_NO_WARNING_OUTPUT
  QNoDebug CreateLoggerWarning(int, const char*, const char*);
#else
  QDebug CreateLoggerWarning(int line, const char *pretty_function, const char* category);
#endif // QT_NO_WARNING_OUTPUT

#ifdef QT_NO_DEBUG_OUTPUT
  QNoDebug CreateLoggerInfo(int, const char*, const char*);
  QNoDebug CreateLoggerDebug(int, const char*, const char*);
#else
  QDebug CreateLoggerInfo(int line, const char *pretty_function, const char* category);
  QDebug CreateLoggerDebug(int line, const char *pretty_function, const char* category);
#endif  // QT_NO_DEBUG_OUTPUT


void GLog(const char* domain, int level, const char* message, void* user_data);

extern const char *kDefaultLogLevels;

}  // namespace logging

QDebug operator<<(QDebug debug, std::chrono::seconds secs);

#endif  // LOGGING_H

