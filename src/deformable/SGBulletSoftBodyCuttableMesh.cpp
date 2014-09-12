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
	psb->m_materials[0]->m_kLST	= 0.45;
	psb->m_cfg.kVC				= 20;
	psb->setTotalMass(50, true);
	psb->setPose(true, false);

	SGBulletSoftMesh::setup(psb, 50.0f);
}


void SGBulletSoftBodyCuttableMesh::draw() {
	CuttableMesh::draw();
}

void SGBulletSoftBodyCuttableMesh::timestep() {
	//sync the bullet nodes with volmesh
	if(!m_lpSoftBody)
		return;

	if(m_lpSoftBody->m_nodes.size() != (int)countNodes()) {
		sync();
		return;
	}

	//apply deformations
	for(U32 i=0; i < countNodes(); i++) {
		NODE& node = VolMesh::nodeAt(i);
		const btSoftBody::Node&	n = m_lpSoftBody->m_nodes[i];
		node.pos = vec3d(n.m_x.x(), n.m_x.y(), n.m_x.z());
	}

	VolMesh::setAABB(VolMesh::computeAABB());
}

void SGBulletSoftBodyCuttableMesh::sync() {

	SGBulletSoftMesh* psb = dynamic_cast<SGBulletSoftMesh*>(this);
	TheSceneGraph::Instance().getWorld()->removeSoftBody(psb);
	SAFE_DELETE(m_lpSoftBody);




	SGBulletSoftBodyCuttableMesh::setup(*this);
	TheSceneGraph::Instance().getWorld()->addSoftBody(psb);
	/*
	vector<vector<U32>> parts;
	VolMesh::get_disjoint_parts(parts);
	*/
}

bool SGBulletSoftBodyCuttableMesh::setSBFromVolMesh(const VolMesh& vmesh) {
	/*
	int ctNodes = vmesh.countNodes();
	int ctCells = vmesh.countCells();
	btAlignedObjectArray<btVector3> arrNodes;
	arrNodes.resize(ctNodes);



	for(U32 i=0; i < vmesh.countNodes(); i++) {
		vec3d pos = vmesh.const_nodeAt(i).pos;
		arrNodes[i] = btVector3(pos.x, pos.y, pos.z);
	}

	//create the softbody mesh
	btSoftBody* psb = new btSoftBody(&worldInfo, ctNodes, &arrNodes[0], 0);
	for (U32 i = 0; i < (U32)ctCells; i++) {

		const CELL& cell = vmesh.const_cellAt(i);
		psb->appendTetra(cell.nodes[0], cell.nodes[1], cell.nodes[2], cell.nodes[3]);
	}

	//edges
	for(U32 i=0; i < vmesh.countEdges(); i++) {
		const EDGE& edge = vmesh.const_edgeAt(i);
		psb->appendLink(edge.from, edge.to, 0, true);
	}

	//faces
	for(U32 i=0; i < vmesh.countFaces(); i++) {
		U32 fn[3];
		vmesh.getFaceNodes(i, fn);
		psb->appendFace(fn[0], fn[1], fn[2], 0);
	}


	printf("Nodes:  %u\r\n", psb->m_nodes.size());
	printf("Links:  %u\r\n", psb->m_links.size());
	printf("Faces:  %u\r\n", psb->m_faces.size());
	printf("Tetras: %u\r\n", psb->m_tetras.size());
	return (psb);
*/
}
