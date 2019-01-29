#include "QGis_dev.h"
#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "gui/qgsmapcanvas.h"
#include "core/qgsvectorlayer.h"
#include "core/qgsmaplayerregistry.h"
#include "core/raster/qgsrasterlayer.h"
#include "gui/layertree/qgslayertreeview.h"
#include "core/layertree/qgslayertreemodel.h"
#include "gui/qgsdockwidget.h"
#include "core/qgsproject.h"
#include "gui/layertree/qgslayertreemapcanvasbridge.h"
#include "gui/layertree/qgscustomlayerorderwidget.h"
#include "gui/layertree/qgslayertreeviewdefaultactions.h"
#include "core/layertree/qgslayertreeregistrybridge.h"
#include <QVBoxLayout>



QGis_dev::QGis_dev(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	mMapCanvas = new QgsMapCanvas(); 
	this->setCentralWidget(mMapCanvas); 
	mMapCanvas->enableAntiAliasing(true); 
	mMapCanvas->setCanvasColor(QColor(255, 255, 255)); 
	mMapCanvas->setVisible(true);
	mMapCanvas->setDestinationCrs(QgsCoordinateReferenceSystem(3857, QgsCoordinateReferenceSystem::EpsgCrsId));
	
	initUi();

	initLayerTreeView();
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

void QGis_dev::initLayerTreeView()
{
	mLayerTreeView = new QgsLayerTreeView(this);
	mLayerTreeView->setObjectName("theLayerTreeView"); // "theLayerTreeView" used to find this canonical instance later

	mLayerTreeView->setWhatsThis(tr("Map legend that displays all the layers currently on the map canvas. Click on the check box to turn a layer on or off. Double click on a layer in the legend to customize its appearance and set other properties."));

	mLayerTreeDock = new QgsDockWidget(QString::fromLocal8Bit("图层管理器"), this);
	mLayerTreeDock->setObjectName("Layers");
	mLayerTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	QgsLayerTreeModel* model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
#ifdef ENABLE_MODELTEST
	new ModelTest(model, this);
#endif
	model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
	model->setFlag(QgsLayerTreeModel::AllowNodeRename);
	model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
	model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
	model->setFlag(QgsLayerTreeModel::UseEmbeddedWidgets);
	model->setAutoCollapseLegendNodes(10);

	mLayerTreeView->setModel(model);
	//QgsAppLayerTreeViewMenuProvider 需要重写
	//mLayerTreeView->setMenuProvider(new QgsAppLayerTreeViewMenuProvider(mLayerTreeView, mMapCanvas));

	connect(mLayerTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(layerTreeViewDoubleClicked(QModelIndex)));
	connect(mLayerTreeView, SIGNAL(currentLayerChanged(QgsMapLayer*)), this, SLOT(activeLayerChanged(QgsMapLayer*)));
	connect(mLayerTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(updateNewLayerInsertionPoint()));
	connect(QgsProject::instance()->layerTreeRegistryBridge(), SIGNAL(addedLayersToLayerTree(QList<QgsMapLayer*>)),
		this, SLOT(autoSelectAddedLayer(QList<QgsMapLayer*>)));

	// add group action
	QAction* actionAddGroup = new QAction(tr("Add Group"), this);
	actionAddGroup->setIcon(QIcon(":/Layer/images/themes/default/mActionAddGroup.svg"));
	actionAddGroup->setToolTip(QString::fromLocal8Bit("添加组"));
	connect(actionAddGroup, SIGNAL(triggered(bool)), mLayerTreeView->defaultActions(), SLOT(addGroup()));

	// visibility groups tool button
	/*QToolButton* btnVisibilityPresets = new QToolButton;
	btnVisibilityPresets->setAutoRaise(true);
	btnVisibilityPresets->setToolTip(tr("Manage Layer Visibility"));
	btnVisibilityPresets->setIcon(QgsApplication::getThemeIcon("/mActionShowAllLayers.svg"));
	btnVisibilityPresets->setPopupMode(QToolButton::InstantPopup);*/
	//btnVisibilityPresets->setMenu(QgsVisibilityPresets::instance()->menu());

	/*mLegendExpressionFilterButton = new QgsLegendFilterButton(this);
	mLegendExpressionFilterButton->setToolTip(tr("Filter legend by expression"));
	connect(mLegendExpressionFilterButton, SIGNAL(toggled(bool)), this, SLOT(toggleFilterLegendByExpression(bool)));*/

	/*mActionStyleDock = new QAction(tr("Layer Styling"), this);
	mActionStyleDock->setCheckable(true);
	mActionStyleDock->setToolTip(tr("Open the layer styling dock"));
	mActionStyleDock->setShortcut(QString("F7"));
	mActionStyleDock->setIcon(QgsApplication::getThemeIcon("propertyicons/symbology.svg"));
	connect(mActionStyleDock, SIGNAL(toggled(bool)), this, SLOT(mapStyleDock(bool)));*/

	// expand / collapse tool buttons
	QAction* actionExpandAll = new QAction(tr("Expand All"), this);
	actionExpandAll->setIcon(QIcon(":/Layer/images/themes/default/mActionExpandTree.svg"));
	actionExpandAll->setToolTip(QString::fromLocal8Bit("全部展开"));
	connect(actionExpandAll, SIGNAL(triggered(bool)), mLayerTreeView, SLOT(expandAllNodes()));
	QAction* actionCollapseAll = new QAction(tr("Collapse All"), this);
	actionCollapseAll->setIcon(QIcon(":/Layer/images/themes/default/mActionCollapseTree.svg"));
	actionCollapseAll->setToolTip(QString::fromLocal8Bit("全部折叠"));
	connect(actionCollapseAll, SIGNAL(triggered(bool)), mLayerTreeView, SLOT(collapseAllNodes()));

	/*QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);*/

	QAction *mActionRemoveLayer = new QAction(tr("Remove Layer"), this);
	mActionRemoveLayer->setIcon(QIcon(":/Layer/images/themes/default/mActionRemoveLayer.svg"));
	mActionRemoveLayer->setToolTip(QString::fromLocal8Bit("移除"));

	QToolBar* toolbar = new QToolBar();
	toolbar->setIconSize(QSize(16, 16));
	//toolbar->addAction(mActionStyleDock);
	toolbar->addAction(actionAddGroup);
	//toolbar->addWidget(btnVisibilityPresets);
	//toolbar->addAction(mActionFilterLegend);
	//toolbar->addWidget(mLegendExpressionFilterButton);
	toolbar->addAction(actionExpandAll);
	toolbar->addAction(actionCollapseAll);
	toolbar->addAction(mActionRemoveLayer);

	QVBoxLayout* vboxLayout = new QVBoxLayout;
	vboxLayout->setMargin(0);
	vboxLayout->setContentsMargins(0, 0, 0, 0);
	vboxLayout->setSpacing(0);
	vboxLayout->addWidget(toolbar);
	vboxLayout->addWidget(mLayerTreeView);

	QWidget* w = new QWidget;
	w->setLayout(vboxLayout);
	mLayerTreeDock->setWidget(w);
	addDockWidget(Qt::LeftDockWidgetArea, mLayerTreeDock);

	//这个是连接 mMapCanvas 与 mLayerTreeView相关交互的类
	mLayerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), mMapCanvas, this);
	connect(QgsProject::instance(), SIGNAL(writeProject(QDomDocument&)), mLayerTreeCanvasBridge, SLOT(writeProject(QDomDocument&)));
	connect(QgsProject::instance(), SIGNAL(readProject(QDomDocument)), mLayerTreeCanvasBridge, SLOT(readProject(QDomDocument)));

	bool otfTransformAutoEnable = QSettings().value("/Projections/otfTransformAutoEnable", true).toBool();
	mLayerTreeCanvasBridge->setAutoEnableCrsTransform(otfTransformAutoEnable);

	mMapLayerOrder = new QgsCustomLayerOrderWidget(mLayerTreeCanvasBridge, this);
	mMapLayerOrder->setObjectName("theMapLayerOrder");

	mMapLayerOrder->setWhatsThis(tr("Map layer list that displays all layers in drawing order."));
	mLayerOrderDock = new QgsDockWidget(tr("Layer Order Panel"), this);
	mLayerOrderDock->setObjectName("LayerOrder");
	mLayerOrderDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	mLayerOrderDock->setWidget(mMapLayerOrder);
	addDockWidget(Qt::LeftDockWidgetArea, mLayerOrderDock);
	mLayerOrderDock->show();

	connect(mMapCanvas, SIGNAL(mapCanvasRefreshed()), this, SLOT(updateFilterLegend()));
	connect(mActionRemoveLayer, SIGNAL(triggered()), this, SLOT(removeLayer()));
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

	mMapCanvas->setExtent(vecLayer->extent());
	/* 	mapCanvasLayerSet.append(vecLayer);
		mMapCanvas->setLayerSet(mapCanvasLayerSet);*/

	mMapCanvas->setVisible(true);
	mMapCanvas->freeze(false);
	mMapCanvas->refresh();
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
	/*mapCanvasLayerSet.append(rasterLayer);
	mMapCanvas->setLayerSet(mapCanvasLayerSet);*/
	mMapCanvas->setExtent(rasterLayer->extent());
	mMapCanvas->setVisible(true); 
	mMapCanvas->freeze(false); 
	mMapCanvas->refresh();

}
