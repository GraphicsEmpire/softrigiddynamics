/*
 * SGBulletRigidBodyCuttableMesh.cpp
 *
 *  Created on: Sep 24, 2014
 *      Author: pourya
 */

#include <deformable/SGBulletRigidBodyCuttableMesh.h>

namespace PS {
namespace MESH {

SGBulletRigidBodyCuttableMesh::SGBulletRigidBodyCuttableMesh(const VolMesh& volmesh, double mass): CuttableMesh(volmesh) {
	// TODO Auto-generated constructor stub
	m_lpRigidBody = NULL;
	this->setup(volmesh, mass);
}

SGBulletRigidBodyCuttableMesh::~SGBulletRigidBodyCuttableMesh() {
	// TODO Auto-generated destructor stub
}

void SGBulletRigidBodyCuttableMesh::cleanup() {

	btCollisionShape* pShape = NULL;
	if(m_lpRigidBody) {
		pShape = m_lpRigidBody->getCollisionShape();
		delete (m_lpRigidBody->getMotionState());
		SAFE_DELETE(m_lpRigidBody);
	}
	SAFE_DELETE(pShape);
}

void SGBulletRigidBodyCuttableMesh::setup(const VolMesh& vmesh, double mass) {
	//1
	btQuaternion rotation(0.0, 0.0, 0.0, 1.0);

	//2
	vec3f t = this->transform()->getTranslate();
	btVector3 position = btVector3(t.x, t.y, t.z);

	//3
	btDefaultMotionState* motionState = new btDefaultMotionState(
			btTransform(rotation, position));

	//4
	btScalar bodyMass = mass;
	btVector3 bodyInertia(0, 0, 0);
	btConvexHullShape* pCDShape = new btConvexHullShape();
	for (U32 i = 0; i < vmesh.countNodes(); i++) {
		vec3d p = vmesh.const_nodeAt(i).pos;
		btVector3 btv = btVector3(p.x, p.y, p.z);
		pCDShape->addPoint(btv);
	}
	pCDShape->calculateLocalInertia(bodyMass, bodyInertia);
	pCDShape->setMargin(0.04);

	//5
	btRigidBody::btRigidBodyConstructionInfo bodyCI =
			btRigidBody::btRigidBodyConstructionInfo(bodyMass, motionState,
					pCDShape, bodyInertia);

	//6
	bodyCI.m_restitution = 1.0f;
	bodyCI.m_friction = 0.5f;

	//7
	m_lpRigidBody = new btRigidBody(bodyCI);

	//8
	m_lpRigidBody->setUserPointer((void*)this);

	//9
	m_lpRigidBody->setLinearFactor(btVector3(1, 1, 0));

	//animate
	setAnimate(true);

	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }
}

void SGBulletRigidBodyCuttableMesh::updateNodeTransformFromMotionState() {

	//btScalar m[16];
	btTransform trans = m_lpRigidBody->getWorldTransform();

	//rotate
	btQuaternion q = trans.getRotation();
	transform()->setRotate(quat(q.x(), q.y(), q.z(), q.w()));

	//translate
	btVector3 t = trans.getOrigin();
	transform()->setTranslate(vec3f(t.x(), t.y(), t.z()));
}

void SGBulletRigidBodyCuttableMesh::updateMotionStateFromNodeTransform() {
	if(!m_lpRigidBody->getMotionState())
		return;

	vec3f t = transform()->getTranslate();

	btTransform trans(btQuaternion(0, 0, 0, 1), btVector3(t.x, t.y, t.z));
	m_lpRigidBody->getMotionState()->setWorldTransform(trans);
}


void SGBulletRigidBodyCuttableMesh::draw() {
	CuttableMesh::draw();
}

void SGBulletRigidBodyCuttableMesh::timestep() {
	if(m_lpRigidBody == NULL)
		return;

	updateNodeTransformFromMotionState();
}

} /* namespace MESH */
} /* namespace PS */
