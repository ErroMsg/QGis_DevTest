#include "QGis_dev.h"
#include <QtWidgets/QApplication>
#include "qgsapplication.h"

int main(int argc, char *argv[])
{
	QgsApplication a(argc, argv, true); 
	QString exeRoot = QApplication::applicationDirPath();
	QgsApplication::setPrefixPath(exeRoot, true);
	QgsApplication::initQgis(); //初始化QGIS应用 

	QGis_dev w; //创建一个窗体，类似于Qt w.show();
	w.show();
	return a.exec();
}
