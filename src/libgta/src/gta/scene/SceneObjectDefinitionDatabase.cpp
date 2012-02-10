/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "SceneObjectDefinitionDatabase.h"
#include <utility>

using std::pair;




void SceneObjectDefinitionDatabase::addFileGroup(SceneObjectFileGroup* group)
{
	fileGroups.insert(pair<CString, SceneObjectFileGroup*>(group->getRelativePath(), group));
}


SceneObjectFileGroup* SceneObjectDefinitionDatabase::getFileGroup(const CString& relPath)
{
	FileGroupMap::iterator it = fileGroups.find(relPath);

	if (it == fileGroups.end()) {
		return NULL;
	}

	return it->second;
}


void SceneObjectDefinitionDatabase::addGroupDependency(SceneObjectGroupDependency* dep)
{
	groupDeps.insert(pair<CString, SceneObjectGroupDependency*>(dep->getRelativePath(), dep));
}


SceneObjectGroupDependency* SceneObjectDefinitionDatabase::getGroupDependency(const CString& relPath)
{
	GroupDepIterator it = groupDeps.find(relPath);

	if (it == groupDeps.end())
		return NULL;

	return it->second;
}