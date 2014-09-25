/*
 * SGBulletRigidBodyCuttableMesh.h
 *
 *  Created on: Sep 24, 2014
 *      Author: pourya
 */

#ifndef SGBULLETRIGIDBODYCUTTABLEMESH_H_
#define SGBULLETRIGIDBODYCUTTABLEMESH_H_

#include <deformable/CuttableMesh.h>
#include <btBulletDynamicsCommon.h>

namespace PS {
namespace MESH {

class SGBulletRigidBodyCuttableMesh: public CuttableMesh {
public:
	SGBulletRigidBodyCuttableMesh(const VolMesh& volmesh, double mass);
	virtual ~SGBulletRigidBodyCuttableMesh();

	void setup(const VolMesh& vmesh, double mass);

	void updateNodeTransformFromMotionState();
	void updateMotionStateFromNodeTransform();

	btRigidBody* getB3RigidBody() const {return m_lpRigidBody;}

	void draw();
	void timestep();
protected:
	void init();
	void cleanup();


protected:
	btRigidBody* m_lpRigidBody;

};

} /* namespace MESH */
} /* namespace PS */

#endif /* SGBULLETRIGIDBODYCUTTABLEMESH_H_ */
