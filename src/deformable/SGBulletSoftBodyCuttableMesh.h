/*
 * SGBulletSoftBodyCuttableMesh.h
 *
 *  Created on: Sep 7, 2014
 *      Author: pourya
 */

#ifndef SGBULLETSOFTBODYCUTTABLEMESH_H_
#define SGBULLETSOFTBODYCUTTABLEMESH_H_

#include "CuttableMesh.h"
#include "graphics/SGBulletSoftMesh.h"

using namespace PS::MESH;

namespace PS {
namespace SG {

class SGBulletSoftBodyCuttableMesh : public CuttableMesh, public SGBulletSoftMesh {
public:
	SGBulletSoftBodyCuttableMesh(const VolMesh& volmesh);
	virtual ~SGBulletSoftBodyCuttableMesh();
	void setup(const VolMesh& volmesh);

	void draw();
	void timestep();

};

}
}



#endif /* SGBULLETSOFTBODYCUTTABLEMESH_H_ */
