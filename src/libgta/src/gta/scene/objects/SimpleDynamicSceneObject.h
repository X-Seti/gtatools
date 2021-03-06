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

#ifndef SIMPLEDYNAMICSCENEOBJECT_H_
#define SIMPLEDYNAMICSCENEOBJECT_H_

#include "../parts/RigidBodySceneObject.h"
#include "../parts/VisualSceneObject.h"
#include "../../resource/mesh/MeshPointer.h"
#include "../../resource/texture/TextureSource.h"
#include "../../resource/collision/CollisionShapePointer.h"
#include "../../resource/smesh/ShadowMeshPointer.h"
#include "../../resource/physics/PhysicsPointer.h"
#include "../../scene/StreamingManager.h"


class SimpleDynamicSceneObject : public virtual VisualSceneObject, public virtual RigidBodySceneObject
{
public:
	SimpleDynamicSceneObject(MeshPointer* mptr, TextureSource* tsrc, CollisionShapePointer* cptr,
			ShadowMeshPointer* smptr, PhysicsPointer* pptr, float sd, float mass, const Matrix4& mm);
	SimpleDynamicSceneObject(const SimpleDynamicSceneObject& other);

	virtual SceneObject* clone() const { return new SimpleDynamicSceneObject(*this); }
	virtual typeflags_t getTypeFlags() const { return TypeFlagVisual | TypeFlagRigidBody; }
	virtual float getStreamingDistance() const { return streamingDist; }
	virtual uint32_t getStreamingBuckets() const { return StreamingManager::VisibleBucket | StreamingManager::PhysicsBucket; }
	virtual Matrix4& getModelMatrix() { return mm; }
	virtual const Matrix4& getModelMatrix() const { return mm; }
	virtual void setModelMatrix(const Matrix4& matrix) { mm = matrix; }

	virtual bool hasAlphaTransparency() const { return true; }
	virtual void getBoundingSphere(Vector3& center, float& radius);
	virtual ShadowMeshPointer* getCurrentShadowMeshPointer() { return smeshPtr; }

	virtual void updateRenderingDistance(float dist, float sdMultiplier) {}
	virtual void resetRenderingDistance() {}

	virtual CollisionShapePointer* getCollisionShapePointer() { return colPtr; }
	virtual float getMass() const { return mass; }
	virtual void setMass(float m) {}
	virtual void getCollisionBoundingSphere(Vector3& center, float& radius) { getBoundingSphere(center, radius); }
	virtual void getCollisionBoundingBox(Vector3& min, Vector3& extX, Vector3& extY, Vector3& extZ) {}

	void setBoundingSphere(const Vector3& center, float radius) { boundingSphereCenter = center; boundingSphereRadius = radius; }

	MeshPointer* getMeshPointer() { return meshPtr; }
	TextureSource* getTextureSource() { return texSrc; }
	PhysicsPointer* getPhysicsPointer() { return physicsPtr; }

	btRigidBody* getRigidBody() { return rb; }

	void setCenterOfMassOffset(const Vector3& comOffset);

	virtual void getWorldTransform(btTransform& trans) const;
	virtual void setWorldTransform(const btTransform& trans);

private:
	btRigidBody* rb;
	float streamingDist;
	float mass;
	Matrix4 mm;

	Vector3 comOffset;

	Vector3 boundingSphereCenter;
	float boundingSphereRadius;
	Vector3 boundingBoxMin;
	Vector3 boundingBoxMax;

	MeshPointer* meshPtr;
	TextureSource* texSrc;
	CollisionShapePointer* colPtr;
	ShadowMeshPointer* smeshPtr;
	PhysicsPointer* physicsPtr;
};

#endif /* SIMPLEDYNAMICSCENEOBJECT_H_ */
