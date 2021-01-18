/***************************************************************************
 *   Copyright (C) 2012-2020 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TNOTEPIXMAP_H
#define TNOTEPIXMAP_H

#include <nootkacoreglobal.h>
#include "tnote.h"
#include "tkeysignature.h"
#include <QtGui/qpixmap.h>


class QPixmap;


/**
 * returns @p QPixmap with given note.
 * @param factor determines its size
 */
QPixmap NOOTKACORE_EXPORT getNotePixmap(Tnote note, bool clef = true,
                                        TkeySignature key = TkeySignature(0), qreal factor = 4.0);

QString NOOTKACORE_EXPORT pixToHtml(const QPixmap& pix);

QString NOOTKACORE_EXPORT htmlNote(Tnote note, bool clef = true,
                                   TkeySignature key = TkeySignature(0), qreal factor = 4.0);




#endif // TNOTEPIXMAP_H
