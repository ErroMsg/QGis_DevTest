#include "QGis_dev.h"
#include <QtWidgets/QApplication>
#include "qgsapplication.h"

int main(int argc, char *argv[])
{
	QgsApplication a(argc, argv, true); 
	QString exeRoot = QApplication::applicationDirPath();
	QgsApplication::setPrefixPath(exeRoot, true);
	QgsApplication::initQgis(); //��ʼ��QGISӦ�� 

	QGis_dev w; //����һ�����壬������Qt w.show();
	w.show();
	return a.exec();
}
