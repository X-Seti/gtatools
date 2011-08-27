/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#ifndef COLLISIONMESHINDEXER_H_
#define COLLISIONMESHINDEXER_H_

#include "../ResourceObserver.h"
#include <gta/config.h>
#include <gtaformats/util/strutil.h>

#ifdef CXX0X_AVAILABLE
#include <unordered_map>
using std::unordered_map;
#else
#include <map>
using std::map;
#endif



class CollisionMeshIndexer : public ResourceObserver {
public:
	struct CollisionMeshIndexEntry
	{
		File* file;
		int index;
	};

private:
#ifdef CXX0X_AVAILABLE
	typedef unordered_map<hash_t, CollisionMeshIndexEntry*> IndexMap;
#else
	typedef map<hash_t, CollisionMeshIndexEntry*> IndexMap;
#endif

public:
	virtual void resourceAdded(const File& file);
	CollisionMeshIndexEntry* getCollisionMesh(hash_t name);
	CollisionMeshIndexEntry* operator[](hash_t name) { return getCollisionMesh(name); }

private:
	IndexMap index;
};

#endif /* COLLISIONMESHINDEXER_H_ */