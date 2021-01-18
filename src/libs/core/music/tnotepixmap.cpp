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


#include "tnotepixmap.h"
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtGui/qguiapplication.h>
#include <QtCore/qbuffer.h>
#include <QtCore/qdebug.h>


inline QString altStr(char accid) {
  static const std::string accArr[6] = { "n", "B", "b", "", "#", "x" };
  return QString::fromStdString(accArr[static_cast<int>(accid) + 3]);
}


QPixmap getNotePixmap(Tnote note, bool clef, TkeySignature key, qreal factor) {
  int noteNr = note.octave() * 7 + note.note();
  QString accidString = altStr(note.alter());
  if (note.alter()) {
      if (qAbs(note.alter()) == 1) { // double accidentals already assigned
        if (note.alter() == TkeySignature::scalesDefArr[static_cast<int>(key.value()) + 7][static_cast<int>(note.note()) - 1])
          accidString.clear(); // accidental(s) in key signature
      }
  } else { // no accidental
      if (TkeySignature::scalesDefArr[static_cast<int>(key.value()) + 7][static_cast<int>(note.note()) - 1] != 0)
        accidString = altStr(3); // paint natural then
  }

  int h = qRound(factor * 18.0); // height
  int w = qRound(factor * 13.0); // width
  int xPosOfNote = 8;
  if (key.value()) {
    w += qRound(factor * (2.0 * static_cast<qreal>(qAbs(key.value()))));
    xPosOfNote += 2 * qAbs(key.value());
  }
  if (!accidString.isEmpty()) {
    w += qRound(factor * 3.0);
    xPosOfNote += 3;
  }
  if (noteNr > 12)
    h = qRound(factor * static_cast<qreal>(18 + (noteNr - 12)));
  if (noteNr < -1)
    h = qRound(factor * static_cast<qreal>(18 + (-1 - noteNr)));

  QPixmap pix(w, h);
  pix.fill(Qt::transparent); // white background

  int noteOffset = 10 - noteNr;
  int hiLinePos = 4;
  if (noteNr > 12)
    hiLinePos = 4 + noteNr - 12;

  QPainter painter(&pix);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setWindow(0, 0, w, h);

  painter.setPen(QGuiApplication::palette().text().color());
  painter.setBrush(QGuiApplication::palette().text().color());
  // main staff lines
  for (int i = hiLinePos; i < (hiLinePos + 10); i += 2)
    painter.drawLine(0 ,i * factor, w, i * factor);
  // upper lines if needed
  if (noteNr > 12)
    for (int i = hiLinePos - 2; i > 2; i -= 2)
      painter.drawLine(qRound((xPosOfNote - 1) * factor), i * factor, (xPosOfNote + 4) * factor, i * factor);
    // lower lines if needed
  if (noteNr < 2) {
    for (int i = hiLinePos + 10; i <= (hiLinePos + 10 + (1 + (-1 * noteNr))); i += 2)
      painter.drawLine((xPosOfNote - 1) * factor, i * factor, (xPosOfNote + 4) * factor, i * factor);
  }

  double fontFactor = 1; // font factor
  // Unfortunately clef and accidental glyphs from nootka font
  // depends on system font size. To scale it fontF exists
  QFont cFont = QFont("nootka", factor * 15.5, QFont::Normal);
  QFontMetricsF cMetr(cFont);
  double clefWidth = cMetr.width(QString(QChar(0xe1a7)));
  fontFactor = 80.0 / clefWidth;
  if ((fontFactor < 1) || (fontFactor > 1.25)) {
    cFont.setPointSizeF(cFont.pointSizeF() * fontFactor);
    cMetr = QFontMetricsF(cFont);
    clefWidth = cMetr.width(QString(QChar(0xe1a7)));
  }
  if (clef) {
    painter.setFont(cFont);
// #if defined(Q_OS_WIN32)
//         painter.drawText(QRectF(0, (hiLinePos + 3.4) * factor - cMetr.height() / 2, clefWidth, cMetr.height()),
//                          Qt::AlignCenter, QString(QChar(0xe1a7))); // 4.4
// #else
    painter.drawText(QRectF(0, (hiLinePos + 4) * factor - cMetr.height() / 2, clefWidth, cMetr.height()),
                        Qt::AlignCenter, QString(QChar(0xe1a7))); // 4.4
// #endif
  }

  // ALL ACCIDENTALS
  QFont accFont = QFont("nootka");
  accFont.setPointSizeF(5 * factor * fontFactor);
  painter.setFont(accFont);
  QFontMetricsF metrics(accFont);
  // key signature
  if (key.value()) {
    QString keyAccidString;
    char ff;
    if (key.value() < 0) {
      keyAccidString = altStr(-1); // flat
      ff = -1;
    }
    else {
      keyAccidString = altStr(1); // sharp
      ff = 1;
    }
//     double accWidth = metrics.width(keyAccidString);
//     for (int i = 1; i <= (qAbs(key.value())); i++) {
// #if defined(Q_OS_WIN32)
//             painter.drawText(QRectF( (4 + i*1.7) * factor,
//                                      (TkeySignatureView::getPosOfAccid((7 + ((i)*ff))%8) - 19.5 + hiLinePos) * factor,
//                                      accWidth, metrics.height()),
//                             Qt::AlignCenter, keyAccidString);
// #else
//       painter.drawText(QRectF( (4 + i*1.7) * factor,
//                                 (TkeySignatureView::getPosOfAccid((7 + ((i)*ff))%8) - 19 + hiLinePos) * factor,
//                                 accWidth, metrics.height()),
//                       Qt::AlignCenter, keyAccidString);
// #endif
//     }
  }
    // note
  painter.drawEllipse( xPosOfNote * factor, (hiLinePos + noteOffset) * factor, factor * 3, factor * 2);
  // accidental
  if (!accidString.isEmpty()) {
    double accWidth = metrics.width(accidString);
// #if defined(Q_OS_WIN32)
//         painter.drawText(QRectF((xPosOfNote) * factor - accWidth / 2,
//                                 (hiLinePos + noteOffset - 2.5) * factor,
//                                 accWidth, metrics.height() ),
//                          Qt::AlignCenter, accidString );
// #else
    painter.drawText(QRectF((xPosOfNote) * factor - accWidth / 2,
                                (hiLinePos + noteOffset - 2.2) * factor,
                                accWidth, metrics.height() ),
                         Qt::AlignCenter, accidString );
// #endif
  }

  return pix;
}


QString pixToHtml(const QPixmap& pix) {
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  pix.save(&buffer, "PNG");
  return QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";
}


QString htmlNote(Tnote note, bool clef, TkeySignature key, qreal factor) {
  return pixToHtml(getNotePixmap(note, clef, key, factor));
}
