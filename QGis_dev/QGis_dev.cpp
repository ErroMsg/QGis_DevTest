#include "QGis_dev.h"
#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "gui/qgsmapcanvas.h"
#include "core/qgsvectorlayer.h"
#include "core/qgsmaplayerregistry.h"
#include "core/raster/qgsrasterlayer.h"

QGis_dev::QGis_dev(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	mapCanvas = new QgsMapCanvas(); 
	this->setCentralWidget(mapCanvas); 
	mapCanvas->enableAntiAliasing(true); 
	mapCanvas->setCanvasColor(QColor(255, 255, 255)); 
	mapCanvas->setVisible(true);
	mapCanvas->setDestinationCrs(QgsCoordinateReferenceSystem(3857, QgsCoordinateReferenceSystem::EpsgCrsId));
	
	initUi();
}

void QGis_dev::initUi()
{
	QAction *pActAddVector = new QAction(this);
	pActAddVector->setText("add vector layer");
	ui.mainToolBar->addAction(pActAddVector);

	QAction *pActAddRaster = new QAction(this);
	pActAddRaster->setText("add raster layer");
	ui.mainToolBar->addAction(pActAddRaster);

	connect(pActAddVector, &QAction::triggered, this, &QGis_dev::displayVectorLayer);
	connect(pActAddRaster, &QAction::triggered, this, &QGis_dev::displayRasterLayer);
}

void QGis_dev::displayVectorLayer()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("open vector"), "", "*.shp"); 
	QStringList temp = filename.split(QDir::separator()); 
	QString basename = temp.at(temp.size() - 1); 
	QgsVectorLayer *vecLayer = new QgsVectorLayer(filename, basename, "ogr", false); 
	if (!vecLayer->isValid()) 
	{ 
		QMessageBox::critical(this, "error", "layer is invalid"); return; 
	} 
	QgsMapLayerRegistry::instance()->addMapLayer(vecLayer);

	QgsMapCanvasLayer layer(vecLayer);
	mapCanvasLayerSet.append(layer);

	mapCanvas->setExtent(vecLayer->extent());
	mapCanvas->setLayerSet(mapCanvasLayerSet);
	mapCanvas->setVisible(true);
	mapCanvas->freeze(false);
	mapCanvas->refresh();
}

void QGis_dev::displayRasterLayer()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("open vector"), "", "*.tif"); 
	QStringList temp = filename.split(QDir::separator()); 
	QString basename = temp.at(temp.size() - 1); 
	QgsRasterLayer* rasterLayer = new QgsRasterLayer(filename, basename, "gdal", false);
	if (!rasterLayer->isValid()) 
	{ 
		QMessageBox::critical(this, "error", "layer is invalid"); 
		return; 
	} 
	QgsMapLayerRegistry::instance()->addMapLayer(rasterLayer); 
	
	QgsMapCanvasLayer layer(rasterLayer);
	mapCanvasLayerSet.append(layer);

	mapCanvas->setExtent(rasterLayer->extent()); 
	mapCanvas->setLayerSet(mapCanvasLayerSet);
	mapCanvas->setVisible(true); 
	mapCanvas->freeze(false); 
	mapCanvas->refresh();

}
