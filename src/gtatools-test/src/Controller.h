/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <SDL.h>
#include <gtaformats/util/File.h>
#include <gta/scene/BasicTransparencyAlgorithm.h>
#include <gta/scene/DepthPeelingAlgorithm.h>
#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>
//#include <gta/scene/WeightedAverageAlgorithm.h>


class Controller {
public:
	Controller(QWidget* mainWidget);
	void init();
	void reshape(int w, int h);
	bool paint();
	void keyPressed(QKeyEvent* evt);
	void keyReleased(QKeyEvent* evt);
	void mouseButtonPressed(Qt::MouseButton button, int x, int y);
	void mouseButtonReleased(Qt::MouseButton button, int x, int y);
	void mouseMotion(int x, int y);

private:
	void addResource(const File& file);

private:
	QWidget* mainWidget;
	uint64_t lastFrameStart, lastMeasuredFrameStart;
	float moveFactor;
	float moveForwardFactor, moveSidewardFactor, moveUpFactor;
	bool firstFrame;
	int framesSinceLastMeasure;
	int lastMouseX, lastMouseY;
	bool printCacheStatistics;
	//BulletGLDebugDraw* debugDrawer;
	bool programRunning;
	bool forceStatisticsUpdate;

	BasicTransparencyAlgorithm* basicTransAlgo;
	DepthPeelingAlgorithm* dpAlgo;
	//WeightedAverageAlgorithm* wavgAlgo;
};

#endif /* CONTROLLER_H_ */
