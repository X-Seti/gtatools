/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DFFGUIModule.h"
#include <qmenu.h>
#include "../../gui/MainWindow.h"
#include "DFFWidget.h"
#include "../System.h"



DFFGUIModule::DFFGUIModule()
		: installCount(0)
{
	texturedAction = new QAction(tr("Show textured"), NULL);
	texturedAction->setCheckable(true);
	texturedAction->setChecked(true);

	wireframeAction = new QAction(tr("Show wireframe"), NULL);
	wireframeAction->setCheckable(true);
	wireframeAction->setChecked(false);

	dumpAction = new QAction(tr("Dump XML"), NULL);

	connect(texturedAction, SIGNAL(triggered(bool)), this, SLOT(texturedPropertyChangedSlot(bool)));
	connect(wireframeAction, SIGNAL(triggered(bool)), this, SLOT(wireframePropertyChangedSlot(bool)));
	connect(dumpAction, SIGNAL(triggered(bool)), this, SLOT(dumpRequestedSlot(bool)));
}


DFFGUIModule::~DFFGUIModule()
{
	delete texturedAction;
	delete wireframeAction;
	delete dumpAction;
}


void DFFGUIModule::installOnce()
{
	if (installCount == 0) {
		System::getInstance()->installGUIModule(this);
	}

	installCount++;
}


void DFFGUIModule::uninstallOnce()
{
	installCount--;

	if (installCount == 0) {
		System::getInstance()->uninstallGUIModule(this);
	}
}


void DFFGUIModule::doInstall()
{
	QMenu* viewMenu = mainWindow->getViewMenu();
	QMenu* fileMenu = mainWindow->getFileMenu();

	viewMenu->addAction(texturedAction);
	texturedAction->setParent(mainWindow);

	viewMenu->addAction(wireframeAction);
	wireframeAction->setParent(mainWindow);

	fileMenu->addAction(dumpAction);
	dumpAction->setParent(mainWindow);
}


void DFFGUIModule::doUninstall()
{
	QMenu* viewMenu = mainWindow->getViewMenu();
	QMenu* fileMenu = mainWindow->getFileMenu();

	viewMenu->removeAction(texturedAction);
	texturedAction->setParent(NULL);

	viewMenu->removeAction(wireframeAction);
	wireframeAction->setParent(NULL);

	fileMenu->removeAction(dumpAction);
	dumpAction->setParent(NULL);
}

