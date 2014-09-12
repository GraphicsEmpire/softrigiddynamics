/*
 * SGBulletSoftBodyFromVolMesh.cpp
 *
 *  Created on: Sep 4, 2014
 *      Author: pourya
 */

#include "SGBulletSoftBodyFromVolMesh.h"

using namespace PS::MESH;

btSoftBody* SGBulletSoftBodyFromVolMesh::CreateFromVolMesh(
		btSoftBodyWorldInfo& worldInfo, const VolMesh& vmesh) {

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
}

