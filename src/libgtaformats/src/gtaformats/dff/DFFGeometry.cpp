/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "DFFGeometry.h"
#include "DFFException.h"
#include "DFFMesh.h"
#include <nxcommon/exception/OutOfBoundsException.h>
#include <cstring>



DFFGeometry::DFFGeometry(uint32_t numVertices, float* vertices, float* normals, float* uvCoords,
			uint8_t uvSetCount, uint8_t* vertexColors, uint8_t* boneIndices, float* boneWeights)
		: flags(0), uvSetCount(0), vertexCount(numVertices), frameCount(0), ambientLight(0.0f),
		  diffuseLight(0.0f), specularLight(0.0f), bounds(NULL), boneCount(0), inverseBoneMatrices(NULL),
		  associatedFrame(NULL), mesh(NULL)
{
	setVertices(numVertices, vertices, normals, uvCoords, uvSetCount, vertexColors, boneIndices, boneWeights);
}


DFFGeometry::DFFGeometry(const DFFGeometry& other)
		: flags(other.flags), uvSetCount(other.uvSetCount),
		  vertexCount(other.vertexCount), frameCount(other.frameCount),
		  ambientLight(other.ambientLight), diffuseLight(other.diffuseLight),
		  specularLight(other.specularLight),
		  vertexColors(other.vertexColors == NULL ? NULL : new uint8_t[vertexCount*4]),
		  uvCoordSets(other.uvCoordSets == NULL ? NULL : new float[vertexCount*uvSetCount*2]),
		  bounds(new DFFBoundingSphere),
		  vertices(new float[vertexCount*3]),
		  normals(other.normals == NULL ? NULL : new float[vertexCount*3]),
		  boneIndices(other.boneIndices == NULL ? NULL : new uint8_t[4*vertexCount]),
		  boneWeights(other.boneWeights == NULL ? NULL : new float[4*vertexCount]),
		  boneCount(other.boneCount),
		  inverseBoneMatrices(other.inverseBoneMatrices ? new Matrix4*[other.boneCount] : NULL),
		  associatedFrame(other.associatedFrame), mesh(NULL)
{
	if (vertexColors) {
		memcpy(vertexColors, other.vertexColors, vertexCount*4);
	}
	if (uvCoordSets) {
		memcpy(uvCoordSets, other.uvCoordSets, vertexCount*uvSetCount*2*4);
	}
	if (normals) {
		memcpy(normals, other.normals, vertexCount*3*4);
	}
	if (boneIndices) {
		memcpy(boneIndices, other.boneIndices, vertexCount*4);
	}
	if (boneWeights) {
		memcpy(boneWeights, other.boneWeights, vertexCount*4*sizeof(float));
	}
	if (inverseBoneMatrices) {
		for (uint8_t i = 0 ; i < boneCount ; i++) {
			inverseBoneMatrices[i] = new Matrix4(*other.inverseBoneMatrices[i]);
		}
	}

	memcpy(vertices, other.vertices, vertexCount*3*4);

	ConstMaterialIterator mit;
	for (mit = other.getMaterialBegin() ; mit != other.getMaterialEnd() ; mit++) {
		materials.push_back(new DFFMaterial(**mit));
	}

	ConstPartIterator pit;
	for (pit = other.getPartBegin() ; pit != other.getPartEnd() ; pit++) {
		parts.push_back(new DFFGeometryPart(**pit));
	}

	memcpy(bounds, other.bounds, sizeof(DFFBoundingSphere));
}


DFFGeometry::~DFFGeometry()
{
	delete bounds;

	if (vertexColors != NULL) {
		delete[] vertexColors;
	}
	if (uvCoordSets != NULL) {
		delete[] uvCoordSets;
	}
	if (vertices != NULL) {
		delete[] vertices;
	}
	if (normals != NULL) {
		delete[] normals;
	}
	if (boneIndices != NULL) {
		delete[] boneIndices;
	}
	if (boneWeights != NULL) {
		delete[] boneWeights;
	}

	if (inverseBoneMatrices != NULL) {
		for (uint8_t i = 0 ; i < boneCount ; i++) {
			delete inverseBoneMatrices[i];
		}

		delete[] inverseBoneMatrices;
	}

	MaterialIterator it;
	for (it = materials.begin() ; it != materials.end() ; it++) {
		(*it)->reparent(NULL);
		delete *it;
	}

	PartIterator pit;
	for (pit = parts.begin() ; pit != parts.end() ; pit++) {
		(*pit)->reparent(NULL);
		delete *pit;
	}

	/*for (int32_t i = 0 ; i < materialCount ; i++) {
		delete materials[i];
	}

	delete[] materials;


	for (int32_t i = 0 ; i < partCount ; i++) {
		delete parts[i];
	}

	delete[] parts;*/
}


void DFFGeometry::setVertices(uint32_t numVertices, float* vertices, float* normals, float* uvCoords,
		uint8_t uvSetCount, uint8_t* vertexColors, uint8_t* boneIndices, float* boneWeights)
{
	if (uvCoords != NULL  &&  uvSetCount == 0) {
		throw DFFException("DFFGeometry::setVertices(): UV coordinates given even though UV set count is 0!",
				__FILE__, __LINE__);
	}

	vertexCount = numVertices;
	this->vertices = vertices;
	this->normals = normals;
	this->uvCoordSets = uvCoords;
	this->uvSetCount = uvSetCount;
	this->vertexColors = vertexColors;
	this->boneIndices = boneIndices;
	this->boneWeights = boneWeights;
}


void DFFGeometry::mirrorYZ()
{
	if (!vertices && !normals) {
		return;
	}

	for (uint32_t i = 0 ; i < vertexCount ; i++) {
		if (vertices != NULL) {
			float y = vertices[(i*3) + 1];
			float z = vertices[(i*3) + 2];
			vertices[(i*3) + 1] = z;
			vertices[(i*3) + 2] = y;
		}

		if (normals != NULL) {
			float ny = normals[(i*3) + 1];
			float nz = normals[(i*3) + 2];
			normals[(i*3) + 1] = nz;
			normals[(i*3) + 2] = ny;
		}
	}
}


void DFFGeometry::scale(float x, float y, float z)
{
	for (uint32_t i = 0 ; i < vertexCount ; i++) {
		vertices[(i*3) + 0] *= x;
		vertices[(i*3) + 1] *= y;
		vertices[(i*3) + 2] *= z;
	}
}


void DFFGeometry::mirrorUVHorizontal()
{
	uint32_t numCoords = uvSetCount*vertexCount;

	for (uint32_t i = 0 ; i < numCoords ; i++) {
		uvCoordSets[i*2] = 1.0f - uvCoordSets[i*2];
	}

	/*int singleFloats = numFloats%4;
	Vec4SF vec1;
	vec1.f[0] = 1.0f;
	vec1.f[1] = 1.0f;
	vec1.f[2] = 1.0f;
	vec1.f[3] = 1.0f;

	for (int i = 0 ; i < numFloats; i += 4) {
		v4sf val;
		memcpy(&val, uvCoordSets+i, 16);
		v4sf* dest = (v4sf*) (uvCoordSets+i);
		*dest = vec1.v - val;
	}

	for (int i = 0 ; i < singleFloats ; i++) {
		float* ptr = uvCoordSets+(numFloats-(singleFloats-i));
		*ptr = 1.0f-*ptr;
	}*/
}


int32_t DFFGeometry::indexOf(DFFMaterial* mat) const
{
	ConstMaterialIterator it;
	int i = 0;

	for (it = materials.begin() ; it != materials.end() ; it++, i++) {
		if (*it == mat) {
			return i;
		}
	}

	return -1;
}


void DFFGeometry::removeMaterial(DFFMaterial* material)
{
	MaterialIterator it;

	for (it = materials.begin() ; it != materials.end() ; it++) {
		if (*it == material) {
			material->reparent(NULL);
			materials.erase(it);
			return;
		}
	}
}


void DFFGeometry::removeMaterials()
{
	MaterialIterator it;

	for (it = materials.begin() ; it != materials.end() ; it++) {
		(*it)->reparent(NULL);
	}

	materials.clear();
}


void DFFGeometry::removePart(DFFGeometryPart* part)
{
	PartIterator it;

	for (it = parts.begin() ; it != parts.end() ; it++) {
		if (*it == part) {
			parts.erase(it);
			part->reparent(NULL);
			return;
		}
	}
}


void DFFGeometry::removeParts()
{
	PartIterator it;

	for (it = parts.begin() ; it != parts.end() ; it++) {
		(*it)->reparent(NULL);
	}

	parts.clear();
}


float* DFFGeometry::getUVCoordSet(uint8_t idx)
{
	if (idx >= uvSetCount) {
		throw OutOfBoundsException(idx, __FILE__, __LINE__);
	}

	return uvCoordSets+idx*vertexCount;
}


const float* DFFGeometry::getUVCoordSet(uint8_t idx) const
{
	if (idx >= uvSetCount) {
		throw OutOfBoundsException(idx, __FILE__, __LINE__);
	}

	return uvCoordSets+idx*vertexCount;
}


DFFMaterial* DFFGeometry::getMaterial(uint32_t index)
{
	if (index >= materials.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return materials[index];
}


const DFFMaterial* DFFGeometry::getMaterial(uint32_t index) const
{
	if (index >= materials.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return materials[index];
}


DFFGeometryPart* DFFGeometry::getPart(uint32_t index)
{
	if (index >= parts.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return parts[index];
}


const DFFGeometryPart* DFFGeometry::getPart(uint32_t index) const
{
	if (index >= parts.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return parts[index];
}


void DFFGeometry::reparent(DFFMesh* mesh)
{
	if (this->mesh  &&  mesh) {
		throw DFFException("Attempt to reparent a DFFGeometry which still has a parent! Remove it from "
				"it's old DFFMesh parent first.", __FILE__, __LINE__);
	}

	this->mesh = mesh;
}
