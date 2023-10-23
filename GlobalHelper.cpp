#include <QFile>
#include <QDebug>
#include "GlobalHelper.h"


GlobalHelper::GlobalHelper()
{

}

QString GlobalHelper::GetQssStr(QString strQssPath)
{
    QString strQss;
    QFile FileQss(strQssPath);
    if (FileQss.open(QIODevice::ReadOnly))
    {
        strQss = FileQss.readAll();
        FileQss.close();
    }
    else
    {
        qDebug() << "读取样式表失败" << strQssPath;
    }
    return strQss;
}
