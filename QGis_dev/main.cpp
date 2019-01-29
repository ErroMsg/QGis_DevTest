#include "QGis_dev.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QGis_dev w;
	w.show();
	return a.exec();
}
