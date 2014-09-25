/*
 * SGBulletSoftBodyCuttableMesh.h
 *
 *  Created on: Sep 7, 2014
 *      Author: pourya
 */

#ifndef SGBULLETSOFTBODYCUTTABLEMESH_H_
#define SGBULLETSOFTBODYCUTTABLEMESH_H_

#include "CuttableMesh.h"
#include <btBulletDynamicsCommon.h>
#include "BulletSoftBody/btSoftBody.h"

using namespace PS::MESH;

namespace PS {
namespace SG {

class SGBulletSoftBodyCuttableMesh : public CuttableMesh {
public:
	SGBulletSoftBodyCuttableMesh(const VolMesh& volmesh);
	virtual ~SGBulletSoftBodyCuttableMesh();
	void setup(const VolMesh& volmesh);

	void draw();
	void timestep();
	void sync();

	btSoftBody* getB3SoftBody() const {return m_lpSoftBody;}
protected:
	bool setSBFromVolMesh(const VolMesh& vmesh);
	virtual void setup(const btSoftBody* pBody, float mass = 1.0f);

protected:
	btSoftBody* m_lpSoftBody;
};

}
}



#endif /* SGBULLETSOFTBODYCUTTABLEMESH_H_ */
