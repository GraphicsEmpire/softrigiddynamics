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

using namespace PS;
using namespace PS::SG;
using namespace PS::FILESTRINGUTILS;
using namespace std;


AvatarScalpel* g_lpScalpel = NULL;

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

	}

	//Modifier
	TheSceneGraph::Instance().setModifier(glutGetModifiers());

	glutPostRedisplay();
}


void closeApp() {

}


int main(int argc, char* argv[]) {
	cout << "Cutting tets" << endl; // prints !!!Hello World!!!

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


	//Add deformable object


	//render mask
//	SGRenderMask* renderMask = new SGRenderMask(TheTexManager::Instance().get("maskalpha"));
//	renderMask->setName("rendermask");
//	TheSceneGraph::Instance().add(renderMask);

	glutMainLoop();

	return 0;
}
