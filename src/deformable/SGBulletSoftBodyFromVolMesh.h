/*
 * SGBulletVolMesh.h
 *
 *  Created on: Sep 4, 2014
 *      Author: pourya
 */

#ifndef SGBulletSoftBodyFromVolMesh_H
#define SGBulletSoftBodyFromVolMesh_H

#include "BulletSoftBody/btSoftBody.h"
#include "VolMeshSamples.h"

namespace PS {
namespace MESH {


struct SGBulletSoftBodyFromVolMesh {

	static btSoftBody*	CreateFromVolMesh(btSoftBodyWorldInfo& worldInfo, const VolMesh& vmesh);

};

}
}

#endif /* SGBulletSoftBodyFromVolMesh_H */
