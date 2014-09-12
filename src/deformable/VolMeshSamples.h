/*
 * VolMeshSamples.h
 *
 *  Created on: Aug 7, 2014
 *      Author: hupf2020
 */

#ifndef VOLMESHSAMPLES_H_
#define VOLMESHSAMPLES_H_


#include "VolMesh.h"

namespace PS {
namespace MESH {


class VolMeshSamples{
public:
	VolMeshSamples() {}

	static VolMesh* CreateOneTetra();
	static VolMesh* CreateTwoTetra(const vec3d& origin = vec3d(0.0));
	static VolMesh* CreateTruthCube(int nx, int ny, int nz, double cellsize);
};

}
}
#endif /* VOLMESHSAMPLES_H_ */
