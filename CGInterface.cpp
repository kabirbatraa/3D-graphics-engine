
// #define GEOM_SHADER

#include "CGInterface.h"
#include "v3.h"
#include "scene.h"

#include <iostream>

using namespace std;

CGInterface::CGInterface() {};

void CGInterface::PerSessionInit() {

  glEnable(GL_DEPTH_TEST);

  CGprofile latestVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
#ifdef GEOM_SHADER
  CGprofile latestGeometryProfile = cgGLGetLatestProfile(CG_GL_GEOMETRY);
#endif
  CGprofile latestPixelProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

#ifdef GEOM_SHADER
  if (latestGeometryProfile == CG_PROFILE_UNKNOWN) {
	  cerr << "ERROR: geometry profile is not available" << endl;
    exit(0);
  }

  cgGLSetOptimalOptions(latestGeometryProfile);
  CGerror Error = cgGetError();
  if (Error) {
	  cerr << "CG ERROR: " << cgGetErrorString(Error) << endl;
  }

  cout << "Info: Latest GP Profile Supported: " << cgGetProfileString(latestGeometryProfile) << endl;

  geometryCGprofile = latestGeometryProfile;
#endif

  cout << "Info: Latest VP Profile Supported: " << cgGetProfileString(latestVertexProfile) << endl;
  cout << "Info: Latest FP Profile Supported: " << cgGetProfileString(latestPixelProfile) << endl;

  vertexCGprofile = latestVertexProfile;
  pixelCGprofile = latestPixelProfile;
  cgContext = cgCreateContext();


}

bool ShaderOneInterface::PerSessionInit(CGInterface *cgi) {

#ifdef GEOM_SHADER
  geometryProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->geometryCGprofile, "GeometryMain", NULL);
  if (geometryProgram == NULL)  {
    CGerror Error = cgGetError();
    cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }
#endif

  vertexProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->vertexCGprofile, "VertexMain", NULL);
  if (vertexProgram == NULL) {
    CGerror Error = cgGetError();
    cerr << "Shader One Vertex Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }

  fragmentProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->pixelCGprofile, "FragmentMain", NULL);
  if (fragmentProgram == NULL)  {
    CGerror Error = cgGetError();
    cerr << "Shader One Fragment Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }

	// load programs
#ifdef GEOM_SHADER
	cgGLLoadProgram(geometryProgram);
#endif
	cgGLLoadProgram(vertexProgram);
	cgGLLoadProgram(fragmentProgram);

	// build some parameters by name such that we can set them later...
	vertexModelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj");
	vertexMorphRadius = cgGetNamedParameter(vertexProgram, "morphRadius");
	vertexMorphFraction = cgGetNamedParameter(vertexProgram, "morphFraction");
	vertexMorphCenter = cgGetNamedParameter(vertexProgram, "morphCenter");
#ifdef GEOM_SHADER
  geometryModelViewProj = cgGetNamedParameter(geometryProgram, "modelViewProj" );
#endif
  fragmentBWFraction = cgGetNamedParameter(fragmentProgram, "bwFraction");
  fragmentEye = cgGetNamedParameter(fragmentProgram, "eye");
  R0 = cgGetNamedParameter(fragmentProgram, "R0");
  R1 = cgGetNamedParameter(fragmentProgram, "R1");
  R2 = cgGetNamedParameter(fragmentProgram, "R2");
  R3 = cgGetNamedParameter(fragmentProgram, "R3");

  return true;

}

void ShaderOneInterface::PerFrameInit() {

	// set intrinsics
	scene->ppc->SetIntrinsicsHW(0.1f, 1000.0f);
	// set extrinsics
	scene->ppc->SetExtrinsicsHW();
	//set parameters

	cgGLSetStateMatrixParameter(vertexModelViewProj, 
		CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

#ifdef GEOM_SHADER
  cgGLSetStateMatrixParameter(
    geometryModelViewProj, 
	  CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
#endif

  cgSetParameter1f(vertexMorphFraction, scene->bwFraction);
  float radius = 20.0f;
  cgSetParameter1f(vertexMorphRadius, radius);
  V3 center = scene->tms[0].GetCenter();
  cgSetParameter3fv(vertexMorphCenter, (const float*)&center);
  cgSetParameter1f(fragmentBWFraction, scene->bwFraction);
  cgSetParameter3fv(fragmentEye, (float*)&(scene->ppc->C));

  // swap 03 and 12? if image is backward basically

  cgSetParameter3fv(R0, (float*)&(scene->R0));
  cgSetParameter3fv(R1, (float*)&(scene->R1));
  cgSetParameter3fv(R2, (float*)&(scene->R2));
  cgSetParameter3fv(R3, (float*)&(scene->R3));

  // default order
  // top right
  // bottom right
  // bottom left
  // top left

  // order we want
  // top left
	// bottom left
	// bottom right
	// top right

  // cgSetParameter3fv(R0, (float*)&(scene->R3));
  // cgSetParameter3fv(R1, (float*)&(scene->R2));
  // cgSetParameter3fv(R2, (float*)&(scene->R1));
  // cgSetParameter3fv(R3, (float*)&(scene->R0));

  BindPrograms();

}

void ShaderOneInterface::BindPrograms() {

#ifdef GEOM_SHADER
  cgGLBindProgram(geometryProgram);
#endif
  cgGLBindProgram(vertexProgram);
  cgGLBindProgram(fragmentProgram);

}

void CGInterface::DisableProfiles() {

  cgGLDisableProfile(vertexCGprofile);
#ifdef GEOM_SHADER
  cgGLDisableProfile(geometryCGprofile);
#endif
  cgGLDisableProfile(pixelCGprofile);

}

void CGInterface::EnableProfiles() {

  cgGLEnableProfile(vertexCGprofile);
#ifdef GEOM_SHADER
  cgGLEnableProfile(geometryCGprofile);
#endif
  cgGLEnableProfile(pixelCGprofile);

}

