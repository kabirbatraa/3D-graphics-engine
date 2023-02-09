#pragma once

#include <GL/glew.h>
#include <Cg/cgGL.h>
#include <Cg/cg.h>

// two classes defining the interface between the CPU and GPU

// models part of the CPU-GPU interface that is independent of specific shaders
class CGInterface {

public:
	CGprofile   vertexCGprofile; // vertex shader profile of GPU
	CGprofile   geometryCGprofile; // geometry shader profile of GPU
	CGprofile   pixelCGprofile; // pixel shader profile of GPU
	CGcontext  cgContext;
	void PerSessionInit(); // per session initialization
	CGInterface(); // constructor
	void EnableProfiles(); // enable GPU rendering
	void DisableProfiles(); // disable GPU rendering
};


// models the part of the CPU-GPU interface for a specific shader
//        here there is a single shader "ShaderOne"
//  a shader consists of a vertex, a geometry, and a pixel (fragment) shader
// fragment == pixel; shader == program; e.g. pixel shader, pixel program, fragment shader, fragment program, vertex shader, etc.
class ShaderOneInterface {

  CGprogram geometryProgram; // the geometry shader to be used for the "ShaderOne"
  CGprogram vertexProgram;
  CGprogram fragmentProgram;
  // uniform parameters, i.e parameters that have the same value for all geometry rendered

  // commented so they are not repeated
  // CGparameter vertexModelViewProj; // a matrix combining projection and modelview matrices
  // CGparameter geometryModelViewProj; // geometry shader
  // CGparameter fragmentBWFraction;

  // in 11/11
  CGparameter vertexModelViewProj; // a matrix combining projection and modelview matrices
  CGparameter vertexMorphRadius;
  CGparameter vertexMorphCenter;
  CGparameter vertexMorphFraction;
  CGparameter geometryModelViewProj; // geometry shader
  CGparameter fragmentBWFraction;
  CGparameter fragmentEye; // i think this is the eye vector of the camera

  CGparameter R0, R1, R2, R3; // corners








  

public:
  ShaderOneInterface() {};
  bool PerSessionInit(CGInterface *cgi); // per session initialization
  void BindPrograms(); // enable geometryProgram, vertexProgram, fragmentProgram
  void PerFrameInit(); // set uniform parameter values, etc.

};
