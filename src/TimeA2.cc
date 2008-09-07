/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */

#include "TimeA2.h"
#include "Util.h"
#include "array_fun.h"
#include <cstring>

namespace aria2 {

Time::Time() {
  reset();
}

Time::Time(const Time& time) {
  tv = time.tv;
}

Time::Time(time_t sec) {
  setTimeInSec(sec);
}

Time::Time(const struct timeval& tv) {
  this->tv = tv;
}

Time::~Time() {}

Time& Time::operator=(const Time& time)
{
  if(this != &time) {
    tv = time.tv;
  }
  return *this;
}

bool Time::operator<(const Time& time) const
{
  return Util::difftv(time.tv, tv) > 0;
}

void Time::reset() {
  gettimeofday(&tv, 0);
}

struct timeval Time::getCurrentTime() const {
  struct timeval now;
  gettimeofday(&now, 0);
  return now;
}

bool Time::elapsed(time_t sec) const {
  // Because of gettimeofday called from getCurrentTime() is slow, and most of
  // the time this function is called before specified time passes, we first do
  // simple test using time.
  // Then only when the further test is required, call gettimeofday.
  time_t now = time(0);
  if(tv.tv_sec+sec < now) {
    return true;
  } else if(tv.tv_sec+sec == now) {
    return Util::difftvsec(getCurrentTime(), tv) >= sec;
  } else {
    return false;
  }
}

bool Time::elapsedInMillis(int64_t millis) const {
  return Util::difftv(getCurrentTime(), tv)/1000 >= millis;
}

bool Time::isNewer(const Time& time) const {
  return Util::difftv(this->tv, time.tv) > 0;
}

time_t Time::difference() const {
  return Util::difftvsec(getCurrentTime(), tv);
}

time_t Time::difference(const struct timeval& now) const
{
  return Util::difftvsec(now, tv);
}

int64_t Time::differenceInMillis() const {
  return Util::difftv(getCurrentTime(), tv)/1000;
}

int64_t Time::differenceInMillis(const struct timeval& now) const
{
  return Util::difftv(now, tv)/1000;
}

bool Time::isZero() const
{
  return tv.tv_sec == 0 && tv.tv_usec == 0;
}

int64_t Time::getTimeInMicros() const
{
  return (int64_t)tv.tv_sec*1000*1000+tv.tv_usec;
}

int64_t Time::getTimeInMillis() const
{
  return (int64_t)tv.tv_sec*1000+tv.tv_usec/1000;
}

time_t Time::getTime() const
{
  return tv.tv_sec;
}

void Time::setTimeInSec(time_t sec) {
  tv.tv_sec = sec;
  tv.tv_usec = 0;
}

bool Time::good() const
{
  return tv.tv_sec >= 0;
}

Time Time::parse(const std::string& datetime, const std::string& format)
{
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  char* r = strptime(datetime.c_str(), format.c_str(), &tm);
  if(r != datetime.c_str()+datetime.size()) {
    return Time(-1);
  }
  time_t thetime = timegm(&tm);
  if(thetime == -1) {
    if(tm.tm_year >= 2038-1900) {
      thetime = INT32_MAX;
    }
  }
  return Time(thetime);  
}

Time Time::parseRFC1123(const std::string& datetime)
{
  return parse(datetime, "%a, %d %b %Y %H:%M:%S GMT");
}

Time Time::parseRFC850(const std::string& datetime)
{
  return parse(datetime, "%a, %d-%b-%y %H:%M:%S GMT");
}

Time Time::parseRFC850Ext(const std::string& datetime)
{
  return parse(datetime, "%a, %d-%b-%Y %H:%M:%S GMT");
}

Time Time::parseHTTPDate(const std::string& datetime)
{
  Time (*funcs[])(const std::string&) = {
    &parseRFC1123,
    &parseRFC850Ext,
    &parseRFC850,
  };
  for(Time (**funcsp)(const std::string&) = &funcs[0];
      funcsp != &funcs[arrayLength(funcs)]; ++funcsp) {
    Time t = (*funcsp)(datetime);
    if(t.good()) {
      return t;
    }
  }
  return Time(-1);
}

} // namespace aria2
