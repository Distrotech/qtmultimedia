/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qaudiodecoder_p.h"

#include "qmediaobject_p.h"
#include <qmediaservice.h>
#include "qaudiodecodercontrol_p.h"
#include <private/qmediaserviceprovider_p.h>

#include <QtCore/qcoreevent.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

/*!
    \class QAudioDecoder
    \brief The QAudioDecoder class allows decoding audio.
    \inmodule QtMultimedia
    \ingroup multimedia
    \ingroup multimedia_audio

    \preliminary

    The QAudioDecoder class is a high level class for decoding local
    audio media files.  It is similar to the QMediaPlayer class except
    that audio is provided back through this API rather than routed
    directly to audio hardware, and playlists and network and streaming
    based media is not supported.

    \sa QAudioBuffer, QAudioDecoder
*/

namespace
{
class AudioDecoderRegisterMetaTypes
{
public:
    AudioDecoderRegisterMetaTypes()
    {
        qRegisterMetaType<QAudioDecoder::State>("QAudioDecoder::State");
        qRegisterMetaType<QAudioDecoder::Error>("QAudioDecoder::Error");
    }
} _registerPlayerMetaTypes;
}

class QAudioDecoderPrivate : public QMediaObjectPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QAudioDecoder)

public:
    QAudioDecoderPrivate()
        : provider(0)
        , control(0)
        , state(QAudioDecoder::StoppedState)
        , error(QAudioDecoder::NoError)
    {}

    QMediaServiceProvider *provider;
    QAudioDecoderControl *control;
    QAudioDecoder::State state;
    QAudioDecoder::Error error;
    QString errorString;

    void _q_stateChanged(QAudioDecoder::State state);
    void _q_error(int error, const QString &errorString);
};

void QAudioDecoderPrivate::_q_stateChanged(QAudioDecoder::State ps)
{
    Q_Q(QAudioDecoder);

    if (ps != state) {
        state = ps;

        emit q->stateChanged(ps);
    }
}

void QAudioDecoderPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QAudioDecoder);

    this->error = QAudioDecoder::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}

/*!
    Construct an QAudioDecoder instance
    parented to \a parent and with \a flags.
*/
QAudioDecoder::QAudioDecoder(QObject *parent)
    : QMediaObject(*new QAudioDecoderPrivate,
                   parent,
                   QMediaServiceProvider::defaultServiceProvider()->requestService(Q_MEDIASERVICE_AUDIODECODER))
{
    Q_D(QAudioDecoder);

    d->provider = QMediaServiceProvider::defaultServiceProvider();
    if (d->service == 0) {
        d->error = ServiceMissingError;
    } else {
        d->control = qobject_cast<QAudioDecoderControl*>(d->service->requestControl(QAudioDecoderControl_iid));
        if (d->control != 0) {
            connect(d->control, SIGNAL(stateChanged(QAudioDecoder::State)), SLOT(_q_stateChanged(QAudioDecoder::State)));
            connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));

            connect(d->control, SIGNAL(formatChanged(QAudioFormat)), SIGNAL(formatChanged(QAudioFormat)));
            connect(d->control, SIGNAL(bufferReady()), this, SIGNAL(bufferReady()));
            connect(d->control ,SIGNAL(bufferAvailableChanged(bool)), this, SIGNAL(bufferAvailableChanged(bool)));
        }
    }
}


/*!
    Destroys the audio decoder object.
*/
QAudioDecoder::~QAudioDecoder()
{
    Q_D(QAudioDecoder);

    if (d->service) {
        if (d->control)
            d->service->releaseControl(d->control);

        d->provider->releaseService(d->service);
    }
}

QAudioDecoder::State QAudioDecoder::state() const
{
    return d_func()->state;
}

/*!
    Returns the current error state.
*/

QAudioDecoder::Error QAudioDecoder::error() const
{
    return d_func()->error;
}

QString QAudioDecoder::errorString() const
{
    return d_func()->errorString;
}

/*!
    Starts decoding the audio resource.

    As data gets decoded, the \l bufferReady() signal will be emitted
    when enough data has been decoded.  Calling \l read() will then return
    an audio buffer without blocking.

    Alternatively, if you wish to block until enough data has been decoded,
    you can call read() at any time to block until a buffer is ready.

    \sa read(), bufferSize()
*/
void QAudioDecoder::start()
{
    Q_D(QAudioDecoder);

    if (d->control == 0) {
        QMetaObject::invokeMethod(this, "_q_error", Qt::QueuedConnection,
                                    Q_ARG(int, QAudioDecoder::ServiceMissingError),
                                    Q_ARG(QString, tr("The QAudioDecoder object does not have a valid service")));
        return;
    }

    // Reset error conditions
    d->error = NoError;
    d->errorString.clear();

    d->control->start();
}

/*!
    Stop decoding audio.  Calling \l start() again will resume decoding from the beginning.
*/
void QAudioDecoder::stop()
{
    Q_D(QAudioDecoder);

    if (d->control != 0)
        d->control->stop();
}

/*!
    Returns the current file name to decode.
    If \l setSourceDevice was called, this will
    be empty.
*/
QString QAudioDecoder::sourceFilename() const
{
    Q_D(const QAudioDecoder);
    if (d->control)
        return d->control->sourceFilename();
    return QString();
}

/*!
    Sets the current audio file name to \a fileName.

    When this property is set any current decoding is stopped,
    and any audio buffers are discarded.

    You can only specify either a source filename or
    a source QIODevice.  Setting one will unset the other.
*/
void QAudioDecoder::setSourceFilename(const QString &fileName)
{
    Q_D(QAudioDecoder);

    if (d->control != 0)
        d_func()->control->setSourceFilename(fileName);
}

/*!
    Returns the current source QIODevice, if one was set.
    If \l setSourceFilename was called, this will be 0.
*/
QIODevice *QAudioDecoder::sourceDevice() const
{
    Q_D(const QAudioDecoder);
    if (d->control)
        return d->control->sourceDevice();
    return 0;
}

/*!
    Sets the current audio QIODevice to \a device.

    When this property is set any current decoding is stopped,
    and any audio buffers are discarded.

    You can only specify either a source filename or
    a source QIODevice.  Setting one will unset the other.
*/
void QAudioDecoder::setSourceDevice(QIODevice *device)
{
    Q_D(QAudioDecoder);

    if (d->control != 0)
        d_func()->control->setSourceDevice(device);
}

/*!
    Returns the current audio format of the decoded stream.

    Any buffers returned should have this format.

    \sa setAudioFormat, audioFormatChanged
*/
QAudioFormat QAudioDecoder::audioFormat() const
{
    Q_D(const QAudioDecoder);
    if (d->control)
        return d->control->audioFormat();
    return QAudioFormat();
}

/*!
    Set the desired audio format for decoded samples to \a format.

    This property can only be set while the decoder is stopped.
    Setting this property at other times will be ignored.

    If the decoder does not support this format, \l error() will
    be set to \c FormatError.

    If you do not specify a format, the format of the decoded
    audio itself will be used.  Otherwise, some format conversion
    will be applied.

    If you wish to reset the decoded format to that of the original
    audio file, you can specify an invalid \a format.
*/
void QAudioDecoder::setAudioFormat(const QAudioFormat &format)
{
    Q_D(QAudioDecoder);

    if (state() != QAudioDecoder::StoppedState)
        return;

    if (d->control != 0)
        d_func()->control->setAudioFormat(format);
}

/*!
    \internal
*/

bool QAudioDecoder::bind(QObject *obj)
{
    return QMediaObject::bind(obj);
}

/*!
    \internal
*/

void QAudioDecoder::unbind(QObject *obj)
{
    QMediaObject::unbind(obj);
}

/*!
    Returns the level of support an audio decoder has for a \a mimeType and a set of \a codecs.
*/
QtMultimedia::SupportEstimate QAudioDecoder::hasSupport(const QString &mimeType,
                                               const QStringList& codecs)
{
    return QMediaServiceProvider::defaultServiceProvider()->hasSupport(QByteArray(Q_MEDIASERVICE_AUDIODECODER),
                                                                    mimeType,
                                                                    codecs);
}

/*!
    Returns true if a buffer is available to be read,
    and false otherwise.  If there is no buffer available, calling
    the \l read() function may block until a buffer is available or
    the end of the media is reached
*/
bool QAudioDecoder::bufferAvailable() const
{
    Q_D(const QAudioDecoder);
    if (d->control)
        return d->control->bufferAvailable();
    return false;
}

/*!
    Read a buffer from the decoder, with the success or failure stored in \a ok.
*/
QAudioBuffer QAudioDecoder::read(bool *ok) const
{
    Q_D(const QAudioDecoder);

    if (d->control) {
        return d->control->read(ok);
    } else {
        if (ok)
            *ok = false;
        return QAudioBuffer();
    }
}

// Enums
/*!
    \enum QAudioDecoder::State

    Defines the current state of a media player.

    \value DecodingState The audio player is currently decoding media.
    \value StoppedState The decoder is not decoding.  Decoding will
           start at the start of the media.
    \value WaitingState The decoder is either waiting for more data
           to decode, or has filled the required number of buffers.
*/

/*!
    \enum QAudioDecoder::Error

    Defines a media player error condition.

    \value NoError No error has occurred.
    \value ResourceError A media resource couldn't be resolved.
    \value FormatError The format of a media resource isn't supported.
    \value AccessDeniedError There are not the appropriate permissions to play a media resource.
    \value ServiceMissingError A valid playback service was not found, playback cannot proceed.
*/

// Signals
/*!
    \fn QAudioDecoder::error(QAudioDecoder::Error error)

    Signals that an \a error condition has occurred.

    \sa errorString()
*/

/*!
    \fn void QAudioDecoder::stateChanged(State state)

    Signal the \a state of the decoder object has changed.
*/

/*!
    \fn void QAudioDecoder::sourceChanged()

    Signals that the current source of the decoder has changed.

    \sa sourceFilename(), sourceDevice()
*/

/*!
    \fn void QAudioDecoder::formatChanged(const QAudioFormat &format)

    Signals that the current audio format of the decoder has changed to \a format.

    \sa audioFormat(), setAudioFormat
*/

/*!
    \fn void QAudioDecoder::bufferReady()

    Signals that a new decoded audio buffer is available to be read.

    \sa read(), bufferAvailable()
*/

/*!
    \fn void QAudioDecoder::bufferAvailableChanged(bool available)

    Signals the availability (if \a available is true) of a new buffer.

    If \a available is false, there are no buffers available.

    \sa bufferAvailable(), bufferReady()
*/



// Properties
/*!
    \property QAudioDecoder::state
    \brief the audio decoder's playback state.

    By default this property is QAudioDecoder::Stopped

    \sa start(), stop()
*/

/*!
    \property QAudioDecoder::error
    \brief a string describing the last error condition.

    \sa error()
*/

/*!
    \property QAudioDecoder::sourceFilename
    \brief the active filename being decoded by the decoder object.
*/

#include "moc_qaudiodecoder_p.cpp"
QT_END_NAMESPACE
