// xlsxzipreader.cpp

#include "xlsxzipreader_p.h"

#ifdef USE_LOCAL_ZIP
#include <QFile>
#else
#include <private/qzipreader_p.h>
#endif

QT_BEGIN_NAMESPACE_XLSX

#ifdef USE_LOCAL_ZIP

ZipReader::ZipReader(const QString &filePath) :
#ifdef UNICODE
    m_reader(OpenZip(reinterpret_cast<const wchar_t *>(filePath.utf16()), nullptr))
#else
    m_reader(OpenZip(filePath.toUtf8().constData(), nullptr))
#endif
    , m_result(ZR_OK)
{
    init();
}
ZipReader::ZipReader(QIODevice *device)
{
    QFile *file = dynamic_cast<QFile *>(device);
    if (nullptr != file)
    {
        QString str_filename = file->fileName();
        file->close();
    #ifdef UNICODE
        m_reader = OpenZip(reinterpret_cast<const wchar_t *>(str_filename.utf16()), nullptr);
    #else
        m_reader = OpenZip(str_filename.toUtf8().constData(), nullptr);
    #endif
    }
    else
    {
        // 如果使用下面的方式，那么会读取不到数据，需要解决，目前走的是上面的分支
        m_reader = OpenZip(device->readAll().data(), device->size(), nullptr);
    }
    init();
}
ZipReader::~ZipReader()
{
    CloseZip(m_reader);
}
void ZipReader::init()
{
    ZIPENTRY entry;
    GetZipItem(m_reader, -1, &entry);
    int i_numitems = entry.index;
    for (int zi = 0; zi <i_numitems; zi++)
    {
        GetZipItem(m_reader, zi, &entry);
        m_filePaths.append(QString::fromUtf8(entry.name));
    }
}
bool ZipReader::exists() const
{
    return IsZipHandleU(m_reader);
}
QStringList ZipReader::filePaths() const
{
    return m_filePaths;
}
QByteArray ZipReader::fileData(const QString &fileName) const
{
    ZIPENTRY entry;
    int i = -1;
    // 使用fileName.toUtf8().constData()， 而不是fileName.toStdString().c_str()，否则会读取不到
    FindZipItem(m_reader, fileName.toUtf8().constData(), true, &i, &entry);
    if (entry.unc_size < 0 || i < 0)
    {
        return QByteArray("");
    }
    char *p_buf = new char[entry.unc_size + 1];
    UnzipItem(m_reader,i, p_buf, entry.unc_size);
    QByteArray byte_array("");
    byte_array.append(p_buf, entry.unc_size);
    delete[] p_buf;
    p_buf = nullptr;
    return byte_array;
}

#else
ZipReader::ZipReader(const QString &filePath) :
    m_reader(new QZipReader(filePath))
{
    init();
}

ZipReader::ZipReader(QIODevice *device) :
    m_reader(new QZipReader(device))
{
    init();
}

ZipReader::~ZipReader()
{

}

void ZipReader::init()
{
    const auto& allFiles = m_reader->fileInfoList();
    for (const auto &fi : allFiles) {
        if (fi.isFile || (!fi.isDir && !fi.isFile && !fi.isSymLink))
            m_filePaths.append(fi.filePath);
    }
}

bool ZipReader::exists() const
{
    return m_reader->exists();
}

QStringList ZipReader::filePaths() const
{
    return m_filePaths;
}

QByteArray ZipReader::fileData(const QString &fileName) const
{
    return m_reader->fileData(fileName);
}
#endif

QT_END_NAMESPACE_XLSX
