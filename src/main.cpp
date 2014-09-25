//============================================================================
// Name        : tetcutter.cpp
// Author      : Pourya Shirazian
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "base/FileDirectory.h"
#include "base/Logger.h"
#include "graphics/SceneGraph.h"
#include "graphics/Gizmo.h"
#include "graphics/AppScreen.h"
#include "graphics/selectgl.h"
#include "graphics/SGRenderMask.h"
#include "deformable/AvatarScalpel.h"
#include "deformable/AvatarRing.h"
#include "deformable/VolMeshSamples.h"
#include "deformable/SGBulletSoftBodyCuttableMesh.h"
#include "deformable/SGBulletRigidBodyCuttableMesh.h"

using namespace PS;
using namespace PS::SG;
using namespace PS::MESH;
using namespace PS::FILESTRINGUTILS;
using namespace std;


AvatarRing* g_lpAvatar = NULL;
SGBulletRigidBodyCuttableMesh* g_lpTissue = NULL;

void draw() {
	TheSceneGraph::Instance().draw();
	TheGizmoManager::Instance().draw();

	glutSwapBuffers();

}

void timestep() {
	TheSceneGraph::Instance().timestep();

    //Update selection
	glutPostRedisplay();
}

void MousePress(int button, int state, int x, int y)
{
    TheGizmoManager::Instance().mousePress(button, state, x, y);
    TheSceneGraph::Instance().mousePress(button, state, x, y);

    //Update selection
	glutPostRedisplay();
}


void MousePassiveMove(int x, int y)
{
}

void MouseMove(int x, int y)
{
	TheGizmoManager::Instance().mouseMove(x, y);
	TheSceneGraph::Instance().mouseMove(x, y);

	glutPostRedisplay();
}

void MouseWheel(int button, int dir, int x, int y)
{
	TheSceneGraph::Instance().mouseWheel(button, dir, x, y);
	glutPostRedisplay();
}

void NormalKey(unsigned char key, int x, int y)
{
	switch(key)
	{
	case('g'):{
		TheGizmoManager::Instance().setType(gtTranslate);
	}
	break;

	case('s'):{
		TheGizmoManager::Instance().setType(gtScale);
		break;
	}

	case('r'):{
		TheGizmoManager::Instance().setType(gtRotate);
		break;
	}

	case('x'):{
		TheGizmoManager::Instance().setAxis(axisX);
	}
	break;
	case('y'):{
		TheGizmoManager::Instance().setAxis(axisY);
	}
	break;
	case('z'):{
		TheGizmoManager::Instance().setAxis(axisZ);
	}
	break;

	case('p'): {
		TheSceneGraph::Instance().print();
	}
	break;

	case('['):{
		TheSceneGraph::Instance().camera().incrZoom(0.5f);
	}
	break;
	case(']'):{
		TheSceneGraph::Instance().camera().incrZoom(-0.5f);
	}
	break;

	case(27):
	{
		//Saving Settings and Exit
		LogInfo("Saving settings and exit.");
		glutLeaveMainLoop();
	}
	break;


	}

	//Update Screen
	glutPostRedisplay();
}

void SpecialKey(int key, int x, int y)
{
	switch(key)
	{
		case(GLUT_KEY_F4):
		{
			//Set UIAxis
			int axis = (int)TheGizmoManager::Instance().axis();
			axis = (axis + 1) % axisCount;
			TheGizmoManager::Instance().setAxis((GizmoAxis)axis);
			LogInfoArg1("Change haptic axis to %d", TheGizmoManager::Instance().axis());
			break;
		}

		case(GLUT_KEY_F5):
		{
			GizmoAxis axis = TheGizmoManager::Instance().axis();
			vec3f inc(0,0,0);
			if(axis < axisFree)
				inc.setElement(axis, -0.1);
			else
				inc = vec3f(-0.1, -0.1, -0.1);
			inc = inc * 0.5;
			TheGizmoManager::Instance().transform()->scale(inc);
			break;
		}


		case(GLUT_KEY_F6):
		{
			GizmoAxis axis = TheGizmoManager::Instance().axis();
			vec3f inc(0,0,0);
			if(axis < axisFree)
				inc.setElement(axis, 0.1);
			else
				inc = vec3f(0.1, 0.1, 0.1);
			inc = inc * 0.5;
			TheGizmoManager::Instance().transform()->scale(inc);
			break;
		}

		case(GLUT_KEY_F7):
		{
			g_lpTissue->setFlagSplitMeshAfterCut(!g_lpTissue->getFlagSplitMeshAfterCut());
			LogInfoArg1("Tissue splitting is set to: %d", g_lpTissue->getFlagSplitMeshAfterCut());
			break;
		}

		case(GLUT_KEY_F8):
		{
			bool flag = !TheSceneGraph::Instance().get("floor")->isVisible();
			TheSceneGraph::Instance().get("floor")->setVisible(flag);
			LogInfoArg1("Set floor to %s", flag ? "show" : "hide");
			break;
		}

		case(GLUT_KEY_F9):
		{
			bool flag = !TheGizmoManager::Instance().isVisible();
			TheGizmoManager::Instance().setVisible(flag);
			LogInfoArg1("Set gizmos to %s", flag ? "show" : "hide");
			break;
		}

		case(GLUT_KEY_F11):
		{
			g_lpAvatar->grip();
			LogInfo("gripped avatar!");
			break;
		}

	}

	//Modifier
	TheSceneGraph::Instance().setModifier(glutGetModifiers());

	glutPostRedisplay();
}


void closeApp() {

}

void finishedcut() {
	if(g_lpTissue == NULL)
		return;

	LogInfoArg1("Finished cutting %d", g_lpTissue->countCompletedCuts());

//	if(!g_parser.value<int>("disjoint"))
//		return;


	vector<CuttableMesh*> vMeshes;
	g_lpTissue->convertDisjointPartsToMeshes(vMeshes);

	if(vMeshes.size() == 0)
		return;

	U32 ctMaxCells = 0;
	U32 idxMaxCell = 0;

	vector<SGBulletRigidBodyCuttableMesh*> vPhysicsMeshes;
	vPhysicsMeshes.reserve(vMeshes.size());
	for(U32 i=0; i < vMeshes.size(); i++) {
		vMeshes[i]->computeAABB();

		SGBulletRigidBodyCuttableMesh* a = new SGBulletRigidBodyCuttableMesh(*vMeshes[i], vMeshes[i]->countNodes());
		a->setFlagDrawSweepSurf(true);
		a->setFlagSplitMeshAfterCut(true);

		TheSceneGraph::Instance().world()->addRawRigidBody(a->getB3RigidBody());
		TheSceneGraph::Instance().add(a);
		vPhysicsMeshes.push_back(a);

		if(a->countCells() > ctMaxCells) {
			ctMaxCells = a->countCells();
			idxMaxCell = i;
		}

		SAFE_DELETE(vMeshes[i]);
	}
	vMeshes.clear();

	g_lpTissue = vPhysicsMeshes[idxMaxCell];
	if(g_lpAvatar)
		g_lpAvatar->setTissue(g_lpTissue);
}

int main(int argc, char* argv[]) {
	cout << "Cutting tets" << endl;

	//Initialize app
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glutCreateWindow("OpenGL Framework");
	glutDisplayFunc(draw);
	glutReshapeFunc(def_resize);
	glutMouseFunc(MousePress);
	glutPassiveMotionFunc(MousePassiveMove);
	glutMotionFunc(MouseMove);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(NormalKey);
	glutSpecialFunc(SpecialKey);
	glutCloseFunc(closeApp);
	glutIdleFunc(timestep);

	def_initgl();

	//Build Shaders for drawing the mesh
	AnsiStr strRoot = ExtractOneLevelUp(ExtractFilePath(GetExePath()));
	AnsiStr strShaderRoot = strRoot + "data/shaders/";
	AnsiStr strMeshRoot = strRoot + "data/meshes/";
	AnsiStr strTextureRoot = strRoot + "data/textures/";

	AnsiStr strLeftPial = strMeshRoot + "brain/pial_Full_obj/lh.pial.obj";
	AnsiStr strRightPial = strMeshRoot + "brain/pial_Full_obj/rh.pial.obj";

	//Load Shaders
	TheShaderManager::Instance().addFromFolder(strShaderRoot.cptr());

	//Load Textures
	TheTexManager::Instance().add(strTextureRoot + "wood.png");
	TheTexManager::Instance().add(strTextureRoot + "rendermask.png");
	TheTexManager::Instance().add(strTextureRoot + "maskalpha.png");
	TheTexManager::Instance().add(strTextureRoot + "maskalphafilled.png");
	TheTexManager::Instance().add(strTextureRoot + "spin.png");

	//Ground and Room
	//TheSceneGraph::Instance().addFloor(32, 32, 0.5f);
	TheSceneGraph::Instance().addSceneBox(AABB(vec3f(-10, -10, -16), vec3f(10, 10, 16)));


	/*
	//load brain mesh
	SGMesh* leftpial = new SGMesh(strLeftPial);
	leftpial->transform()->setScale(vec3f(0.01));
	leftpial->transform()->rotate(vec3f(1, 0, 0), -90.0);
	leftpial->transform()->translate(vec3f(4, 1, 0));
	TheSceneGraph::Instance().add(leftpial);

	SGMesh* rightpial = new SGMesh(strRightPial);
	rightpial->transform()->setScale(vec3f(0.01));
	rightpial->transform()->rotate(vec3f(1, 0, 0), -90.0);
	rightpial->transform()->translate(vec3f(4, 1, 0));
	TheSceneGraph::Instance().add(rightpial);
	*/

	//floor
	Geometry g;
	g.addCube(vec3f(-8, -0.1, -8), vec3f(8, 0, 8));
	g.addPerVertexColor(vec4f(0.5, 0.5, 0.5, 1));
	SGBulletRigidMesh* floor = new SGBulletRigidMesh();
	floor->setup(g, 0.0);
	floor->setName("floor");
	TheSceneGraph::Instance().addRigidBody(floor);


	//create rigid bodies
	/*
	Geometry g1;
	g1.addCube(vec3f(0.0, 0.0, 0.0), 1.0);
	g1.addPerVertexColor(vec4f(0, 0, 1, 1));

	for(int i=0; i < 8; i ++) {
		for(int j=0; j < 8; j++) {
			g1.colors().clear();

			float r = RandRangeT<float>(0.0, 1.0);
			float g = RandRangeT<float>(0.0, 1.0);
			float b = RandRangeT<float>(0.0, 1.0);

			g1.addPerVertexColor(vec4f(r, g, b, 1.0f));
			SGBulletRigidMesh* acube = new SGBulletRigidMesh();
			acube->transform()->setTranslate(vec3f(i-3, 10.0, j-3));
			acube->setup(g1, 1.0);
			TheSceneGraph::Instance().addRigidBody(acube);
		}
	}
	*/

	//Add deformable object
	VolMesh* pVolMesh = VolMeshSamples::CreateEggShell(16, 16, 2.0f, 0.4f);
	//VolMesh* pVolMesh = VolMeshSamples::CreateTruthCube(10, 10, 10, 0.1);
	for(U32 i=0; i < pVolMesh->countNodes(); i++) {
		NODE& n = pVolMesh->nodeAt(i);
		n.pos = n.pos + vec3d(0, 2, 0);
		n.restpos = n.restpos + vec3d(0, 2, 0);
	}


	g_lpTissue = new SGBulletRigidBodyCuttableMesh(*pVolMesh, pVolMesh->countNodes());
	g_lpTissue->setFlagDrawSweepSurf(true);
	g_lpTissue->setFlagSplitMeshAfterCut(true);
	//g_lpTissue->transform()->setTranslate(vec3f(0.0f, 6.0f, 0.0f));

	TheSceneGraph::Instance().world()->addRawRigidBody(g_lpTissue->getB3RigidBody());
	TheSceneGraph::Instance().add(g_lpTissue);
	SAFE_DELETE(pVolMesh);

	//Scalpel
	g_lpAvatar = new AvatarRing(TheTexManager::Instance().get("spin"));
	g_lpAvatar->setTissue(g_lpTissue);
	g_lpAvatar->setOnCutFinishedEventHandler(finishedcut);
	TheSceneGraph::Instance().add(g_lpAvatar);
	TheGizmoManager::Instance().setFocusedNode(g_lpAvatar);
	TheGizmoManager::Instance().cmdTranslate(vec3f(0,5,0));

	//render mask
//	SGRenderMask* renderMask = new SGRenderMask(TheTexManager::Instance().get("maskalpha"));
//	renderMask->setName("rendermask");
//	TheSceneGraph::Instance().add(renderMask);

	glutMainLoop();

	return 0;
}
