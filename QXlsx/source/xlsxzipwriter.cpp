// xlsxzipwriter.cpp

#include "xlsxzipwriter_p.h"

#include <QtGlobal>
#include <QDebug>
#ifdef USE_LOCAL_ZIP
#include <QFile>
#include <QDebug>
#else
#include <private/qzipwriter_p.h>
#endif

QT_BEGIN_NAMESPACE_XLSX

#ifdef USE_LOCAL_ZIP

ZipWriter::ZipWriter(const QString &filePath)
{
#ifdef UNICODE
    m_writer = CreateZip(reinterpret_cast<const wchar_t *>(filePath.utf16()), nullptr);
#else
    m_writer = CreateZip(filePath.toUtf8().constData(), nullptr);
#endif
}
ZipWriter::ZipWriter(QIODevice *device)
{
    QFile *file = dynamic_cast<QFile *>(device);
    if (nullptr != file)
    {
        QString str_filename = file->fileName();
        // 需要先close，否则Create会失败
        file->close();
    #ifdef UNICODE
        m_writer = CreateZip(reinterpret_cast<const wchar_t *>(str_filename.utf16()), nullptr);
    #else
        m_writer = CreateZip(str_filename.toUtf8().constData(), nullptr);
    #endif
    }
    else
    {
    }
}
ZipWriter::~ZipWriter()
{
    CloseZip(m_writer);
}
bool ZipWriter::error() const
{
    return !IsZipHandleZ(m_writer);
}
void ZipWriter::addFile(const QString &filePath, QIODevice *device)
{
    ZipAdd(m_writer, filePath.toUtf8().constData(),  (void *)device->readAll().data(), device->size());
}
void ZipWriter::addFile(const QString &filePath, const QByteArray &data)
{
    ZipAdd(m_writer, filePath.toUtf8().constData(),  (void *)data.data(), data.size());
}
void ZipWriter::close()
{
    CloseZip(m_writer);
}

#else
ZipWriter::ZipWriter(const QString &filePath)
{
    m_writer = new QZipWriter(filePath, QIODevice::WriteOnly);
    m_writer->setCompressionPolicy(QZipWriter::AutoCompress);
}

ZipWriter::ZipWriter(QIODevice *device)
{
    m_writer = new QZipWriter(device);
    m_writer->setCompressionPolicy(QZipWriter::AutoCompress);
}

ZipWriter::~ZipWriter()
{
    delete m_writer;
}

bool ZipWriter::error() const
{
    return m_writer->status() != QZipWriter::NoError;
}

void ZipWriter::addFile(const QString &filePath, QIODevice *device)
{
    m_writer->addFile(filePath, device);
}

void ZipWriter::addFile(const QString &filePath, const QByteArray &data)
{
    m_writer->addFile(filePath, data);
}

void ZipWriter::close()
{
    m_writer->close();
}
#endif

QT_END_NAMESPACE_XLSX
