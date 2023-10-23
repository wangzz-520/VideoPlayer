#ifndef GLOBALHELPER_H
#define GLOBALHELPER_H

#define MAX_SLIDER_VALUE 65536

#pragma execution_character_set("utf-8")

#include <QString>
#include <QPushButton>
#include <QDebug>
#include <QStringList>

class GlobalHelper
{
public:
    GlobalHelper();
	/**
	 * 获取样式表
	 * 
	 * @param	strQssPath 样式表文件路径
	 * @return	样式表
	 * @note 	
	 */
    static QString GetQssStr(QString strQssPath);
};



#endif // GLOBALHELPER_H
