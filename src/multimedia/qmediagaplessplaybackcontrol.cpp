/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmediagaplessplaybackcontrol.h"
#include "qmediacontrol_p.h"

/*!
    \class QMediaGaplessPlaybackControl
    \inmodule QtMultimedia
    \ingroup multimedia
    \since 5.0

    \brief The QMediaGaplessPlaybackControl class provides access to the gapless playback
    related control of a QMediaService.

    If a QMediaService supports gapless playback it will implement QMediaGaplessPlaybackControl.
    This control provides a means to set the \l {setNextMedia()}{next media} or
    \l {setCrossfadeTime()}{crossfade time} for smooth transitions between tracks.

    The functionality provided by this control is exposed to application
    code through the QMediaPlayer class.

    The interface name of QMediaGaplessPlaybackControl is
    \c com.nokia.Qt.QMediaGaplessPlaybackControl/1.0 as defined in QMediaGaplessPlaybackControl_iid.

    \sa QMediaService::requestControl(), QMediaPlayer
*/

/*!
    \macro QMediaGaplessPlaybackControl_iid

    \c com.nokia.Qt.QMediaGaplessPlaybackControl/1.0

    Defines the interface name of the QMediaGaplessPlaybackControl class.

    \relates QMediaGaplessPlaybackControl
*/

/*!
    Destroys a gapless playback control.
*/
QMediaGaplessPlaybackControl::~QMediaGaplessPlaybackControl()
{
}

/*!
    Constructs a new gapless playback control with the given \a parent.
*/
QMediaGaplessPlaybackControl::QMediaGaplessPlaybackControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    \fn QMediaGaplessPlaybackControl::nextMedia() const

    Returns the content of the next media
    \since 1.0
*/

/*!
    \fn QMediaGaplessPlaybackControl::setNextMedia(const QMediaContent& media)

    Sets the next media for smooth transition.
    \since 1.0
*/

/*!
    \fn QMediaGaplessPlaybackControl::nextMediaChanged(const QMediaContent& media)

    Signals that the next media has changed (either explicitly via \l setNextMedia() or when the
    player clears the next media while advancing to it).

    \since 1.0
    \sa nextMedia()
*/

/*!
    \fn QMediaGaplessPlaybackControl::advancedToNextMedia()

    Signals when the player advances to the next media (the content of next media will be cleared).

    \since 1.0
    \sa nextMedia()
*/

/*!
    \fn QMediaGaplessPlaybackControl::isCrossfadeSupported() const

    Indicates whether crossfading is supported or not.
    If crossfading is not supported, \l setCrossfadTime() will be ignored and \l crossfadeTime() will
    always return 0.

    \since 1.0
*/

/*!
    \fn QMediaGaplessPlaybackControl::setCrossfadeTime(qreal crossfadeTime)

    Sets the crossfade time in seconds for smooth transition.

    Positive value means how much time it will take for the next media to transit from silent to
    full volume and vice versa for current one. So both current and the next one will be playing
    during this period of time.

    A crossfade time of zero or negative will result in gapless playback (suitable for some
    continuous media).

    \since 1.0
*/

/*!
    \fn QMediaGaplessPlaybackControl::crossfadeTime() const

    Returns current crossfade time in seconds.
    \since 1.0
*/

/*!
    \fn QMediaGaplessPlaybackControl::crossfadeTimeChanged(qreal crossfadeTime)

    Signals that the crossfade time has changed.

    \since 1.0
    \sa crossfadeTime()
*/

