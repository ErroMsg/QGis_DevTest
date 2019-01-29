#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QGis_dev.h"

class QGis_dev : public QMainWindow
{
	Q_OBJECT

public:
	QGis_dev(QWidget *parent = Q_NULLPTR);

private:
	Ui::QGis_devClass ui;
};
