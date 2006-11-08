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
#include "TorrentConsoleDownloadEngine.h"
#include "Util.h"
#include <signal.h>

volatile sig_atomic_t btHaltRequested = 0;

TorrentConsoleDownloadEngine::TorrentConsoleDownloadEngine() {}

TorrentConsoleDownloadEngine::~TorrentConsoleDownloadEngine() {}

void TorrentConsoleDownloadEngine::sendStatistics() {
  printf("\r                                                                             ");
  printf("\r");
  if(pieceStorage->downloadFinished()) {
    printf("Download Completed.");
  } else {
    printf("%s/%sB %d%% %s D:%.2f",
	   Util::llitos(downloadLength, true).c_str(),
	   Util::llitos(totalLength, true).c_str(),
	   (totalLength == 0 ?
	    0 : (int)((downloadLength*100)/totalLength)),
	   avgSpeed == 0 ? "-" : Util::secfmt(eta).c_str(),
	   downloadSpeed/1024.0);
  }
  printf(" U:%.2f(%s) %d peers",
	 uploadSpeed/1024.0,
	 Util::llitos(uploadLength, true).c_str(),
	 btRuntime->getConnections());
  fflush(stdout);	 
}

void TorrentConsoleDownloadEngine::afterEachIteration() {
  if(btHaltRequested) {
    btRuntime->setHalt(true);
  }
  TorrentDownloadEngine::afterEachIteration();
}
