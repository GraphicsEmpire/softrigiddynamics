/*
 * SGBulletSoftBodyCuttableMesh.cpp
 *
 *  Created on: Sep 7, 2014
 *      Author: pourya
 */

#include "SGBulletSoftBodyCuttableMesh.h"
#include "SGBulletSoftBodyFromVolMesh.h"
#include "graphics/SceneGraph.h"

using namespace PS::SG;

SGBulletSoftBodyCuttableMesh::SGBulletSoftBodyCuttableMesh(const VolMesh& volmesh):CuttableMesh(volmesh) {
	setup(volmesh);
}

SGBulletSoftBodyCuttableMesh::~SGBulletSoftBodyCuttableMesh() {

}

void SGBulletSoftBodyCuttableMesh::setup(const VolMesh& volmesh) {

	const SGBulletSoftRigidDynamics* pw = TheSceneGraph::Instance().getWorld();

	btSoftBody* psb = SGBulletSoftBodyFromVolMesh::CreateFromVolMesh(const_cast<btSoftBodyWorldInfo&>(pw->getSoftBodyWorldInfo()), volmesh);
	psb->m_materials[0]->m_kLST	=	0.45;
	psb->m_cfg.kVC				=	20;
	psb->setTotalMass(50,true);
	psb->setPose(true,false);

	SGBulletSoftMesh::setup(psb, 50.0f);
}


void SGBulletSoftBodyCuttableMesh::draw() {
	CuttableMesh::draw();
}

void SGBulletSoftBodyCuttableMesh::timestep() {
	//sync the bullet nodes with volmesh
}
