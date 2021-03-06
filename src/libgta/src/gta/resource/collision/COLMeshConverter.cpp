/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "COLMeshConverter.h"
#include "../mesh/Submesh.h"
#include "../../MeshGenerator.h"
#include "../../Engine.h"



Mesh* COLMeshConverter::convert(const float* vertices, unsigned int vertexCount, const COLFace* faces, unsigned int faceCount)
{
	unsigned int modelVertexCount, modelIndexCount;
	float* modelVertices;
	uint8_t* modelColors;
	uint32_t* modelIndices;
	convertVertexModel(vertices, vertexCount, faces, faceCount, modelVertexCount, modelVertices, modelColors,
			modelIndices, modelIndexCount);

	Mesh* mesh = new Mesh(modelVertexCount, VertexFormatTriangles, MeshVertexColors, modelVertices, NULL,
			NULL, modelColors);
	Submesh* submesh = new Submesh(mesh, modelIndexCount, modelIndices);
	mesh->link();

	delete[] modelVertices;
	delete[] modelColors;
	delete[] modelIndices;

	return mesh;
}


Mesh* COLMeshConverter::convert(const COLSphere& sphere)
{
	MeshGenerator gen;
	float* vertices;
	float* normals;
	uint32_t* indices;
	unsigned int vertexCount, indexCount;
	gen.createSphere(vertices, normals, vertexCount, indices, indexCount, sphere.getRadius(), 4, 4);

	const COLSurface& surface = sphere.getSurface();
	uint8_t matNum = surface.getMaterial();
	uint8_t* colors = new uint8_t[vertexCount*4];
	uint8_t r, g, b;
	getMaterialColors(matNum, r, g, b);

	for (unsigned int i = 0 ; i < vertexCount ; i++) {
		colors[i*4] = r;
		colors[i*4 + 1] = g;
		colors[i*4 + 2] = b;
		colors[i*4 + 3] = 255;
	}

	//printf("glBufferData COLMeshConverter 1\n");

	GLuint dataBuffer;
	glGenBuffers(1, &dataBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);

	glBufferData(GL_ARRAY_BUFFER, vertexCount*3*4 + vertexCount*4, NULL, GL_STATIC_DRAW);
	Engine::getInstance()->increaseTestMem(vertexCount*3*4 + vertexCount*4, __FILE__, __LINE__);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount*3*4, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vertexCount*3*4, vertexCount*4, colors);

	Mesh* mesh = new Mesh(vertexCount, VertexFormatTriangles, 0, dataBuffer, -1, -1, vertexCount*3*4);

	Submesh* submesh = new Submesh(mesh, indexCount, indices);
	mesh->link();

	const Vector3& center = sphere.getCenter();
	mesh->setBounds(center[0], center[1], center[2], sphere.getRadius());

	delete[] vertices;
	delete[] normals;
	delete[] indices;

	return mesh;
}


Mesh* COLMeshConverter::convert(const COLBox& box)
{
	MeshGenerator gen;
	float* vertices;
	uint32_t* indices;
	unsigned int vertexCount, indexCount;
	gen.createBox(vertices, vertexCount, indices, indexCount, box.getMinimum(), box.getMaximum());

	const COLSurface& surface = box.getSurface();
	uint8_t matNum = surface.getMaterial();
	uint8_t* colors = new uint8_t[vertexCount*4];
	uint8_t r, g, b;
	getMaterialColors(matNum, r, g, b);

	for (unsigned int i = 0 ; i < vertexCount ; i++) {
		colors[i*4] = r;
		colors[i*4 + 1] = g;
		colors[i*4 + 2] = b;
		colors[i*4 + 3] = 255;
	}

	//printf("glBufferData COLMeshConverter 2\n");

	GLuint dataBuffer;
	glGenBuffers(1, &dataBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);

	glBufferData(GL_ARRAY_BUFFER, vertexCount*3*4 + vertexCount*4, NULL, GL_STATIC_DRAW);
	Engine::getInstance()->increaseTestMem(vertexCount*3*4 + vertexCount*4, __FILE__, __LINE__);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount*3*4, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vertexCount*3*4, vertexCount*4, colors);

	//Mesh* mesh = new Mesh(vertexCount, VertexFormatTriangles, 0, dataBuffer, -1, -1, vertexCount*3*4);
	Mesh* mesh = new Mesh(vertexCount, VertexFormatTriangles, 0, dataBuffer, vertexCount*3*4 + vertexCount*4,
			0, 0, -1, -1, -1, -1, vertexCount*3*4, 0);

	Submesh* submesh = new Submesh(mesh, indexCount, indices);
	mesh->link();

	//mesh->setBounds(center[0], center[1], center[2], sphere.getRadius());

	delete[] vertices;
	delete[] indices;

	return mesh;
}


Mesh* COLMeshConverter::convert(const COLModel& model)
{
	const COLSphere* spheres = model.getSphereCount() != 0 ? &model.getSpheres()[0] : NULL;
	const COLBox* boxes = model.getBoxCount() != 0 ? &model.getBoxes()[0] : NULL;

	GLuint dataBuffer;
    glGenBuffers(1, &dataBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);

    unsigned int vertexCount = 0;
    uint32_t sphereCount = model.getSphereCount();
    uint32_t boxCount = model.getBoxCount();

    float** boxVertexArrays = new float*[boxCount];
    float** sphereVertexArrays = new float*[sphereCount];
    uint32_t** boxIndexArrays = new uint32_t*[boxCount];
    uint32_t** sphereIndexArrays = new uint32_t*[sphereCount];
    unsigned int* boxVertexCounts = new unsigned int[boxCount];
    unsigned int* sphereVertexCounts = new unsigned int[sphereCount];
    unsigned int* boxIndexCounts = new unsigned int[boxCount];
    unsigned int* sphereIndexCounts = new unsigned int[sphereCount];

    MeshGenerator gen;

    for (uint32_t i = 0 ; i < sphereCount ; i++) {
		const COLSphere& sphere = spheres[i];
		float* tmpNormals;
		gen.createSphere(sphereVertexArrays[i], tmpNormals, sphereVertexCounts[i], sphereIndexArrays[i],
				sphereIndexCounts[i], sphere.getRadius(), 4, 4, sphere.getCenter());
		delete[] tmpNormals;
        vertexCount += sphereVertexCounts[i];
    }

    for (uint32_t i = 0 ; i < boxCount ; i++) {
        const COLBox& box = boxes[i];
        gen.createBox(boxVertexArrays[i], boxVertexCounts[i], boxIndexArrays[i], boxIndexCounts[i],
                  box.getMinimum(), box.getMaximum());
        vertexCount += boxVertexCounts[i];
    }

    unsigned int modelVertexCount;
    float* modelVertices;
    uint8_t* modelColors;
    uint32_t* modelIndices;
    unsigned int modelIndexCount;

    convertVertexModel(model.getVertices(), model.getVertexCount(),
    		model.getFaceCount() != 0 ? &model.getFaces()[0] : NULL, model.getFaceCount(),
    		modelVertexCount, modelVertices, modelColors, modelIndices, modelIndexCount);
    vertexCount += modelVertexCount;

    //printf("glBufferData COLMeshConverter 3\n");

    glBufferData(GL_ARRAY_BUFFER, vertexCount*3*4 + vertexCount*4, NULL, GL_STATIC_DRAW);
    Engine::getInstance()->increaseTestMem(vertexCount*3*4 + vertexCount*4, __FILE__, __LINE__);
    int vertexOffset = 0;
    int colorOffset = vertexCount*3*4;

    glBufferSubData(GL_ARRAY_BUFFER, 0, modelVertexCount*3*4, modelVertices);
    glBufferSubData(GL_ARRAY_BUFFER, colorOffset, modelVertexCount*4, modelColors);
    vertexOffset = modelVertexCount;

    /*Mesh* mesh = new Mesh(vertexCount, VertexFormatTriangles, MeshVertexColors, dataBuffer, -1, -1,
    		colorOffset);*/

    Mesh* mesh = new Mesh(vertexCount, VertexFormatTriangles, 0, dataBuffer, vertexCount*3*4 + vertexCount*4,
			0, 0, -1, -1, -1, -1, vertexCount*3*4, 0);

    delete[] modelVertices;
    delete[] modelColors;

    uint8_t r, g, b;

    for (uint32_t i = 0 ; i < sphereCount ; i++) {
    	const COLSphere& sphere = spheres[i];
    	unsigned int vertexCount = sphereVertexCounts[i];

    	// Create the vertex colors array
    	uint8_t* colors = new uint8_t[vertexCount*4];
    	const COLSurface& surface = sphere.getSurface();
    	uint8_t matNum = surface.getMaterial();
    	getMaterialColors(matNum, r, g, b);

        for (unsigned int j = 0 ; j < vertexCount ; j++) {
        	colors[j*4] = r;
        	colors[j*4+1] = g;
        	colors[j*4+2] = b;
        	colors[j*4+3] = 255;
        }

        // Fill the buffers with vertex data
        glBufferSubData(GL_ARRAY_BUFFER, vertexOffset*3*4, vertexCount*3*4, sphereVertexArrays[i]);
        glBufferSubData(GL_ARRAY_BUFFER, colorOffset + vertexOffset*4, vertexCount*4, colors);
        vertexOffset += vertexCount;

        delete[] colors;
    }

    for (uint32_t i = 0 ; i < boxCount ; i++) {
        const COLBox& box = boxes[i];
    	unsigned int vertexCount = boxVertexCounts[i];

    	// Create the vertex colors array
    	uint8_t* colors = new uint8_t[vertexCount*4];
    	const COLSurface& surface = box.getSurface();
    	uint8_t matNum = surface.getMaterial();
    	getMaterialColors(matNum, r, g, b);

        for (unsigned int j = 0 ; j < vertexCount ; j++) {
        	colors[j*4] = r;
        	colors[j*4+1] = g;
        	colors[j*4+2] = b;
        	colors[j*4+3] = 255;
        }

        // Fill the buffers with vertex data
        glBufferSubData(GL_ARRAY_BUFFER, vertexOffset*3*4, vertexCount*3*4, boxVertexArrays[i]);
        glBufferSubData(GL_ARRAY_BUFFER, colorOffset + vertexOffset*4, vertexCount*4, colors);
        vertexOffset += vertexCount;

        delete[] colors;
    }

    Submesh* submesh = new Submesh(mesh, modelIndexCount, modelIndices);
    vertexOffset = modelVertexCount;

    delete[] modelIndices;

    for (uint32_t i = 0 ; i < sphereCount ; i++) {
    	unsigned int indexCount = sphereIndexCounts[i];
    	uint32_t* indices = sphereIndexArrays[i];
    	for (unsigned int j = 0 ; j < indexCount ; j++) {
    		indices[j] += vertexOffset;
    	}

    	Submesh* submesh = new Submesh(mesh, indexCount, indices);
    	vertexOffset += sphereVertexCounts[i];

    	delete[] sphereVertexArrays[i];
    	delete[] sphereIndexArrays[i];
    }

    delete[] sphereVertexArrays;
    delete[] sphereIndexArrays;
    delete[] sphereVertexCounts;
    delete[] sphereIndexCounts;

    for (uint32_t i = 0 ; i < boxCount ; i++) {
    	unsigned int indexCount = boxIndexCounts[i];
    	uint32_t* indices = boxIndexArrays[i];
    	for (unsigned int j = 0 ; j < indexCount ; j++) {
    		indices[j] += vertexOffset;
    	}

        Submesh* submesh = new Submesh(mesh, indexCount, indices);
        vertexOffset += boxVertexCounts[i];

        delete[] boxVertexArrays[i];
        delete[] boxIndexArrays[i];
    }

    delete[] boxVertexArrays;
    delete[] boxIndexArrays;
    delete[] boxVertexCounts;
    delete[] boxIndexCounts;

    mesh->link();

    const COLBounds& bounds = model.getBounds();
    const Vector3& center = bounds.getCenter();
    mesh->setBounds(center.getX(), center.getY(), center.getZ(), bounds.getRadius());

    return mesh;
}


void COLMeshConverter::getMaterialColors(uint8_t mat, uint8_t& r, uint8_t& g, uint8_t& b)
{
	switch (mat) {
	case 0:
	case 1:
	case 2:
	case 3:
		//Default
		r = 0x50;
		g = 0x50;
		b = 0x50;
		break;

	case 4:
	case 5:
	case 7:
	case 8:
	case 34:
	case 89:
	case 127:
	case 135:
	case 136:
	case 137:
	case 138:
	case 139:
	case 144:
	case 165:
		// Concrete
		r = 0x90;
		g = 0x90;
		b = 0x90;
		break;

	case 6:
	case 85:
	case 101:
	case 134:
	case 140:
		// Gravel
		r = 0x64;
		g = 0x5E;
		b = 0x53;
		break;

	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 20:
	case 80:
	case 81:
	case 82:
	case 115:
	case 116:
	case 117:
	case 118:
	case 119:
	case 120:
	case 121:
	case 122:
	case 125:
	case 146:
	case 147:
	case 148:
	case 149:
	case 150:
	case 151:
	case 152:
	case 153:
	case 160:
		// Grass
		r = 0x92;
		g = 0xC0;
		b = 0x32;
		break;

	case 19:
	case 21:
	case 22:
	case 24:
	case 25:
	case 26:
	case 27:
	case 40:
	case 83:
	case 84:
	case 87:
	case 88:
	case 100:
	case 110:
	case 123:
	case 124:
	case 126:
	case 128:
	case 129:
	case 130:
	case 132:
	case 133:
	case 141:
	case 142:
	case 145:
	case 155:
	case 156:
		// Dirt
		r = 0x77;
		g = 0x5F;
		b = 0x40;
		break;

	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 74:
	case 75:
	case 76:
	case 77:
	case 78:
	case 79:
	case 86:
	case 96:
	case 97:
	case 98:
	case 99:
	case 131:
	case 143:
	case 157:
		// Sand
		r = 0xE7;
		g = 0xE1;
		b = 0x7E;
		break;

	case 45:
	case 46:
	case 47:
	case 175:
		// Glass
		r = 0xA7;
		g = 0xE9;
		b = 0xFC;
		break;

	case 42:
	case 43:
	case 44:
	case 70:
	case 72:
	case 73:
	case 172:
	case 173:
	case 174:
		// Wood
		r = 0x93;
		g = 0x69;
		b = 0x44;
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case 58:
	case 59:
	case 63:
	case 64:
	case 65:
	case 162:
	case 164:
	case 167:
	case 168:
	case 171:
		// Metal
		r = 0xBF;
		g = 0xC8;
		b = 0xD5;
		break;

	case 18:
	case 35:
	case 36:
	case 37:
	case 69:
	case 109:
	case 154:
		// Stone
		r = 0xAF;
		g = 0xAA;
		b = 0xA0;
		break;

	case 23:
	case 41:
	case 111:
	case 112:
	case 113:
	case 114:
		// Vegetation
		r = 0x2E;
		g = 0xA5;
		b = 0x63;
		break;

	case 38:
	case 39:
		// Water
		r = 0x64;
		g = 0x93;
		b = 0xE1;
		break;

	case 48:
	case 49:
	case 60:
	case 61:
	case 62:
	case 66:
	case 67:
	case 68:
	case 71:
	case 90:
	case 91:
	case 92:
	case 93:
	case 94:
	case 95:
	case 102:
	case 103:
	case 104:
	case 105:
	case 106:
	case 107:
	case 108:
	case 158:
	case 159:
	case 163:
	case 166:
	case 169:
	case 170:
	case 176:
	case 177:
	case 178:
		// Misc
		r = 0xF1;
		g = 0xAB;
		b = 0x07;
		break;

	default:
		r = 0xFF;
		g = 0xFF;
		b = 0xFF;
		break;
	}
}


void COLMeshConverter::convertVertexModel(const float* inVertices, unsigned int inVertexCount,
		const COLFace* inFaces, unsigned int inFaceCount, unsigned int& outVertexCount, float*& outVertices,
		uint8_t*& outColors, uint32_t*& outIndices, unsigned int& outIndexCount)
{
	outVertexCount = inFaceCount*3;
	outVertices = new float[outVertexCount*3];
	outColors = new uint8_t[outVertexCount*4];
	outIndices = new uint32_t[outVertexCount];
	outIndexCount = outVertexCount;

	for (unsigned int numFace = 0 ; numFace < inFaceCount ; numFace++) {
		const COLFace& face = inFaces[numFace];
		const uint32_t* srcIndices = face.getIndices();

		memcpy(outVertices + numFace*3 * 3, inVertices + srcIndices[0]*3, 3*4);
		memcpy(outVertices + (numFace*3+1) * 3, inVertices + srcIndices[1]*3, 3*4);
		memcpy(outVertices + (numFace*3+2) * 3, inVertices + srcIndices[2]*3, 3*4);
		outIndices[numFace*3] = numFace*3;
		outIndices[numFace*3+1] = numFace*3+1;
		outIndices[numFace*3+2] = numFace*3+2;

		const COLSurface& surface = face.getSurface();
		uint8_t mat = surface.getMaterial();
		uint8_t r, g, b;
		getMaterialColors(mat, r, g, b);

		outColors[numFace*3*4] = r;
		outColors[numFace*3*4+1] = g;
		outColors[numFace*3*4+2] = b;
		outColors[numFace*3*4+3] = 255;

		outColors[(numFace*3+1)*4] = r;
		outColors[(numFace*3+1)*4+1] = g;
		outColors[(numFace*3+1)*4+2] = b;
		outColors[(numFace*3+1)*4+3] = 255;

		outColors[(numFace*3+2)*4] = r;
		outColors[(numFace*3+2)*4+1] = g;
		outColors[(numFace*3+2)*4+2] = b;
		outColors[(numFace*3+2)*4+3] = 255;
	}
}
