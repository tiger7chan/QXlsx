// xlsxzipreader_p.h

#ifndef QXLSX_XLSXZIPREADER_P_H
#define QXLSX_XLSXZIPREADER_P_H

#include <QScopedPointer>
#include <QStringList>
#include <QIODevice>

#include "xlsxglobal.h"

#include <QVector>
#ifdef USE_LOCAL_ZIP
#include "unzip.h"
#endif

QT_BEGIN_NAMESPACE_XLSX

#ifdef USE_LOCAL_ZIP

class  ZipReader
{
public:
    explicit ZipReader(const QString &fileName);
    explicit ZipReader(QIODevice *device);
    ~ZipReader();
    bool exists() const;
    QStringList filePaths() const;
    QByteArray fileData(const QString &fileName) const;
private:
    Q_DISABLE_COPY(ZipReader)
    void init();
    HZIP m_reader;
    QStringList m_filePaths;
    ZRESULT m_result;
};

#else
class QZipReader;

class  ZipReader
{
public:
    explicit ZipReader(const QString &fileName);
    explicit ZipReader(QIODevice *device);
    ~ZipReader();
    bool exists() const;
    QStringList filePaths() const;
    QByteArray fileData(const QString &fileName) const;

private:
    Q_DISABLE_COPY(ZipReader)
    void init();
    QScopedPointer<QZipReader> m_reader;
    QStringList m_filePaths;
};
#endif

QT_END_NAMESPACE_XLSX

#endif // QXLSX_XLSXZIPREADER_P_H
