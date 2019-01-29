#pragma once

#include <QtWidgets/QMainWindow>
#include <QList>
#include "gui/qgsmapcanvas.h"
#include "ui_QGis_dev.h"


class QgsMapCanvas;
class QgsMapCanvasLayer;
class QgsLayerTreeView;
class QgsLayerTreeMapCanvasBridge;
class QgsCustomLayerOrderWidget;
class QGis_dev : public QMainWindow
{
	Q_OBJECT

public:
	QGis_dev(QWidget *parent = Q_NULLPTR);
private:
	void initUi();
	void initLayerTreeView();

public slots:
	void displayVectorLayer();
	void displayRasterLayer();

private:
	Ui::QGis_devClass ui;

	QgsMapCanvas *mMapCanvas; // 地图画布
	QList<QgsMapCanvasLayer> mapCanvasLayerSet;// 地图画布所用的图层集合

	//layer tree
	QgsLayerTreeView* mLayerTreeView;
	QgsLayerTreeMapCanvasBridge* mLayerTreeCanvasBridge;
	QgsCustomLayerOrderWidget* mMapLayerOrder;
	QDockWidget* mLayerTreeDock;
	QDockWidget* mLayerOrderDock;
};
