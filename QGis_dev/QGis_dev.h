#pragma once

#include <QtWidgets/QMainWindow>
#include <QList>
#include "gui/qgsmapcanvas.h"
#include "ui_QGis_dev.h"


class QgsMapCanvas;
//class QgsMapCanvasLayer;
class QGis_dev : public QMainWindow
{
	Q_OBJECT

public:
	QGis_dev(QWidget *parent = Q_NULLPTR);
private:
	void initUi();

public slots:
	void displayVectorLayer();
	void displayRasterLayer();

private:
	Ui::QGis_devClass ui;

	QgsMapCanvas *mapCanvas; // 地图画布
	 // 地图画布所用的图层集合
};
