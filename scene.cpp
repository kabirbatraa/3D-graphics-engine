#include "scene.h"

#include "V3.h"
#include "M33.h"
#include "tm.h"
#include "shadowMap.h"
#include "cubeMap.h"

Scene *scene;

using namespace std;

#include <iostream>
#include <fstream>

#include <string>


// graphics or whatever
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"



Scene::Scene() {

	cgi = 0;
	soi = 0;

	gui = new GUI();
	gui->show();

	int u0 = 16;
	int v0 = 40;
	int h = 480;
	int w = 640;
	fb = new FrameBuffer(u0, v0, w, h);
	fb->position(u0, v0);
	fb->label("SW framebuffer");
	fb->show();
	fb->SetBGR(0xFF0000FF);
	fb->redraw();

	fb3 = new FrameBuffer(u0, v0, w, h);
	fb3->position(u0+fb->w+u0, v0);
	fb3->label("3rd person");
	fb3->show();
	fb3->SetBGR(0xFFFF0000);
	fb3->redraw();
	fb3->hide();



	// hardware framebuffer
	hardwareFB = new FrameBuffer(u0, v0, w, h);
	hardwareFB->position(u0+fb->w+u0, v0);
	hardwareFB->label("HW framebuffer");
	hardwareFB->show();
	hardwareFB->SetBGR(0xFF0000FF);
	hardwareFB->redraw();

	hardwareFB->isHardwareFB = true;

	// other hardware stuff:

	needToInitializeHardware = true;
	hwClearColor = V3(0.0f, 0.0f, 0.5f);

	texturesN = 1;
	textures = new FrameBuffer[texturesN];
	textureNums = new GLuint[texturesN];
	textures[0].LoadTiff("spriteSheet.tiff");
	// textures[1] = ...
	// textures[2] = ...
	// textures[3] = 





	gui->uiw->position(u0, v0 + fb->h + v0);


	float hfov = 60.0f;
	ppc = new PPC(hfov, w, h);
	ppc3 = new PPC(hfov, w, h);
	ppc3->C = V3(-100.0f, 300.0f, 200.0f);


	/*
	tmsN = 1;
	// tmsN = 2;
	tms = new TM[tmsN];
	tms[0].LoadBin("geometry/teapot1k.bin");
	tms[0].SetCenter(V3(-50.0f, 0.0f, -100.0f));
	tms[0].Scale(0.7f);

	// tms[1].LoadBin("geometry/tree1.bin");
	// tms[1].SetCenter(V3(50.0f, 0.0f, -150.0f));
	// tms[1].Scale(15);
	*/

	// tmsN = 2;
	// tms = new TM[tmsN];
	// TM loadedTM;
	// loadedTM.LoadBin("geometry/teapot1k.bin");
	// // tms[0].SetUnshared(&loadedTM);
	// tms[0].LoadBin("geometry/teapot1k.bin");
	// // tms[0].LoadBin("geometry/teapot57k.bin");
	// tms[0].SetCenter(V3(0.0f, 0.0f, -150.0f));
	// tms[0].Scale(0.5f);

	// float yLevel = -50.0f;
	// tms[1].SetGroundPlane(V3(-70.0f, yLevel, -125.0f), V3(70.0f, yLevel, -125.0f), V3(0.0f, yLevel, -250.0f));


	// make the third person look at 0 0 0
	ppc3->PositionAndOrient(ppc3->C, V3(0, 0, 0), V3(0.0f, 1.0f, 0.0f));
	
	// ppc3->PositionAndOrient(ppc3->C, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
	// ppc->PositionAndOrient(ppc->C, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));


	// camera face forward
	// ppc->PositionAndOrient(V3(0.0f, 0.0f, 100.0f), V3(0.0f, 0.0f, -100.0f), V3(0.0f, 1.0f, 0.0f));
	// camera at z = 100

	

	tmsN = 3;
	tms = new TM[tmsN];

	// main teapot
	tms[0].LoadBin("geometry/teapot1k.bin");
	// tms[0].SetCenter(V3(0.0f, 0.0f, -150.0f));
	tms[0].SetCenter(V3(0.0f, 0.0f, 0.0f)); // teapot at 0 0 0 
	tms[0].Scale(0.5f);

	// second teapot
	tms[1].LoadBin("geometry/teapot1k.bin");
	tms[1].SetCenter(V3(0.0f, 0.0f, 0.0f)); 
	tms[1].Rotate(V3(0,0,0), V3(1,0,0), 50);  // origin, direction, theta

	tms[1].SetCenter(V3(30.0f, -10.0f, 0.0f)); 
	tms[1].Scale(0.3f);
	// tms[1].Rotate(tms[1].GetCenter(), tms[1].GetCenter() - tms[0].GetCenter(), 10);

	// also need ground plane
	tms[2].SetGroundPlane(V3(-300, -50, -300), V3(300, -50, -300), V3(0, -50, 300));

	/* this is the sprite
	tms[1].SetTexturedRectangle(50, 50); // the texture coordinates are set in dbg
	tms[1].texture = &textures[0]; // the sprite sheet tiff 
	// tms[1].textureNum
	V3 pos(0.0f, 30.0f, 15.0f);
	tms[1].SetCenter(pos);
	tms[1].Rotate(pos, V3(1.0f, 0.0f, 0.0f), -10);
	*/








	
	// i want to position and orient using the angles + distance = 100
	// rollAngle = 0.0f;
	// phiAngle = 90.0f;
	// thetaAngle = 0.0f;

	// float distance = 100.0f;
	// 		float sinphi = sinf(phiAngle * 3.141592 / 180.0f);
	// 		float cosphi = cosf(phiAngle * 3.141592 / 180.0f);
	// 		float costheta = cosf(thetaAngle * 3.141592 / 180.0f);
	// 		float sintheta = sinf(thetaAngle * 3.141592 / 180.0f);

	// 		// note that y and z are switched in spherical
	// 		V3 position(
	// 			distance * sinphi * costheta, 
	// 			distance * cosphi,
	// 			distance * sinphi * sintheta
	// 		);

	// 		// float rollAngle2 = (rollAngle+90.0f) * 3.141592 / 180.0f;
	// 		// V3 upDirection(cosf(rollAngle2), sin(rollAngle2), 0.0f); // determined by roll angle
	// 		V3 upDirection(0.0f, 1.0f, 0.0f);
	// 		ppc->PositionAndOrient(position, tms[0].GetCenter(), upDirection);


	
	// point light stuff

	ka = 0.2f; // ambient light 
	lightPos = V3(0, 50.0f, -120.0f);
	es = 30.0f;

	// hfov = 120 degrees, resolution is 512x512
	lightPPC = new PPC(120, 512, 512);
	lightPPC->PositionAndOrient(lightPos, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
	// similar to frame buffer, but just the z buffer
	lightShadowMap = new ShadowMap(512, 512);
	



	environmentMap = new CubeMap("uffizi_cross.tiff");


	

	// texture render mode = 4
	renderMode = 5;

	filledOrWireFrame = false;

	// by calling DBG here, pressing keys breaks everything idk why
	// DBG();


	cubeMapTextures = new FrameBuffer[6];
	// order must be right left top bottom back front

	// right
	cubeMapTextures[0].LoadTiff("uffizi_crossRight.tiff");

	// left
	cubeMapTextures[1].LoadTiff("uffizi_crossLeft.tiff");
	
	// top
	cubeMapTextures[2].LoadTiff("uffizi_crossTop.tiff");

	// bottom
	cubeMapTextures[3].LoadTiff("uffizi_crossBottom.tiff");

	// back 
	cubeMapTextures[4].LoadTiff("uffizi_crossBack.tiff");
	
	// front
	cubeMapTextures[5].LoadTiff("uffizi_crossFront.tiff");


	// make the camera look at zero 
	V3 position(0,0,-100);
	V3 zero(0.0f, 0.0f, 0.0f);
	V3 upDirection(0.0f, 1.0f, 0.0f);
	ppc->PositionAndOrient(position, zero, upDirection);

	imageSize = 250;
	textureSquare = TM();
	image = new FrameBuffer(0, 0, imageSize, imageSize); 

	
}






void Scene::GenerateShadowMap(PPC *lightPPC, ShadowMap *lightShadowMap) {
	lightShadowMap->SetZB(0.0f);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderToShadowMap(lightPPC, lightShadowMap);
	}
}



void Scene::Render(PPC *rppc, FrameBuffer *rfb) {
	// V3 matColor(0.0f, 1.0f, 0.0f);

	// reset the framebuffer
	rfb->SetBGR(0xFFFFFFFF);
	rfb->SetZB(0.0f);

	// iterate through every triangle mesh
	for (int tmi = 0; tmi < tmsN; tmi++) {
		// tms[tmi].RenderWireFrame(rppc, rfb);
		if (renderMode == 1 || renderMode == 2) {
			// takes triangle vertices and interpolates color
			tms[tmi].RenderFilled(rppc, rfb);
		}
		else if (renderMode == 3) {
			// tms[tmi].RenderFilledWithLight(rppc, rfb, matColor, ka, lightPos);
			// tms[tmi].RenderFilledWithLightAndSpecular(rppc, rfb, matColor, ka, lightPos, es);
			// tms[tmi].RenderFilledUsingRealColorWithLightAndSpecular(rppc, rfb, ka, lightPos, es);
			tms[tmi].RenderFilledUsingRealColorWithLightAndSpecularAndShadowMap(rppc, rfb, lightPPC, lightShadowMap, ka, lightPos, es);

		}
		// render mode 4: render with no light, but with texture
		else if(renderMode == 4) {
			tms[tmi].RenderFilledWithTextures(rppc, rfb);
		}
		else if(renderMode == 5) {
			// render .. uh idk
		}
		// default should be render filled
		else { 
			tms[tmi].RenderFilled(rppc, rfb);
		}
	}

	// light dot visualization
	// rfb->Render3DPoint(rppc, lightPos, V3(0, 1, 1), 7);

	rfb->redraw();

}



void Scene::Render() {

	// make the light ppc follow the light position and regenerate the shadow map
	// lightPPC->PositionAndOrient(lightPos, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
	// GenerateShadowMap(lightPPC, lightShadowMap);

	// first person
	// Render(ppc, fb);
	// third person
	// Render(ppc3, fb3);

	// draw the camera triangle mesh on the 3d person view	
	// TM cameraTM = ppc->GenerateTriangleMeshVisualization();
	// cameraTM.RenderWireFrame(ppc3, fb3);
	// fb3->redraw();



	// just go through tms and 
	// fb->SetBGR(0xFFFFFFFF);
	fb->RenderEnvironmentMap(environmentMap, ppc);
	fb->SetZB(0.0f);
	fb3->SetBGR(0xFFFFFFFF);
	fb3->SetZB(0.0f);

	// rendering the teapot and not looking at it just breaks everything :c
	// tms[0].RenderFilledWithTextures(ppc, fb);
	tms[0].RenderUsingCubeMap(ppc, fb, environmentMap);
	// tms[0].RenderFilled(ppc3, fb3);
	// tms[0].VisualizeVertexNormals(ppc, fb, 5.0f);

	tms[1].RenderFilledWithTextures(ppc, fb);

	// draw the camera triangle mesh on the 3d person view	
	TM cameraTM = ppc->GenerateTriangleMeshVisualization();
	cameraTM.RenderWireFrame(ppc3, fb3);

	// for (int i = 0; i < 6; i++) {
	// 	cout << i << endl;
	// 	// cout << environmentMap->cameraPointers->a << endl;
	// 	// cout << environmentMap->cameraPointers[0]->a << endl;
	// 	cameraTM = environmentMap->cameraPointers[i]->GenerateTriangleMeshVisualization();
	// 	cameraTM.RenderWireFrame(ppc3, fb3);
	// }

	fb->redraw();
	fb3->redraw();

}

void Scene::RenderHardware() {

	bool shaders = true;

	if (needToInitializeHardware) {
		// happens only once

		if (shaders) {
			cgi = new CGInterface();
			cgi->PerSessionInit();
			soi = new ShaderOneInterface();
			soi->PerSessionInit(cgi);
		}

		glEnable(GL_DEPTH_TEST); // can do this once instead of every time we render

		// lots of hardware texture setup needs to be done
		// for example, we need to load the textures here

		// create hardwrae textures
		// create texture handles
		// set parameters
		// pass textures[ti].pix pixels to hw to know the texels of each textre 



		// textureNums[0] GLuint
		// FrameBuffer textures[0] --> pix

		// TODO: create a new texture in the hardware, 
		// and give it the reference: textureNums

		// FrameBuffer texture = textures[0];
		int texwidth = textures[0].w;
		int texheight = textures[0].h;

		glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &textureNums[0]);
		// glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureNums[0]); //  this sets the active texture!
		tms[1].textureNum = textureNums[0];

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // glTexCoordPointer(3, GL_FLOAT, 0, textureCoordinates); 

		// i think these are the kinds of interpolation for scaling image up or down
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures[0].pix);

        // glVertexPointer(3, GL_FLOAT, 0, verts);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);

		// also generate a texture for the textureSquare
		glGenTextures(1, &textureSquareID);

// /*
		// i also want cubemaps to work
		// GLuint cubeMapID;
		glGenTextures(1, &cubeMapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 

		int width, height, nrChannels;
		// unsigned char *data;  
		// for(unsigned int i = 0; i < textures_faces.size(); i++)
		for(unsigned int i = 0; i < 6; i++)
		{
			// data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);

			// right now, the cube map is just 1 texture, but i want it to be 6 different textures
			// one way to do that is to use an image editing software

			// data is the framebuffer that is the texture of a side of the cube map
			// data is just cubeMapTextures[i].pix

			// for textures: (for reference)
			// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textures[0].pix);

			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
				0, GL_RGBA, cubeMapTextures[i].w, cubeMapTextures[i].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubeMapTextures[i].pix
			);
		}
		// */

		

		needToInitializeHardware = false;
	}

	// if (shaders) {
	// 	cgi->EnableProfiles();
	// 	soi->PerFrameInit();
	// }

	if (filledOrWireFrame) {
		// filled
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		
	}
	else {
		// not filled
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}
	

	// glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClearColor(hwClearColor[0], hwClearColor[1], hwClearColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	float nearz = 0.1f;
	float farz = 1000.0f;
	ppc->SetIntrinsicsHW(nearz, farz);
	ppc->SetExtrinsicsHW();

	for (int tmi = 0; tmi < tmsN; tmi++) {
		// tms[tmi].RenderFilledWithTextures(ppc, fb)

		// for specifically the teapot, use the shader
		if (tmi == 0) {
			cgi->EnableProfiles();
			soi->PerFrameInit();
			
		}

		if (tmi == 1) {
			// second teapot
			// load the texture thing
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureSquareID); //  this sets the active texture!
			textureSquare.textureNum = textureSquareID;
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			// i think these are the kinds of interpolation for scaling image up or down
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageSize, imageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pix);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
			// then render the texture image to see if it works
			// textureSquare.RenderToHardware();

			// if ((int)thetaAngle % 50 > 25) {
			// 	tms[tmi].RenderToHardware();
			// }
		}
		
		// if (tmi != 1) {
		// 	tms[tmi].RenderToHardware();
		// }
		tms[tmi].RenderToHardware();



		// for specifically the teapot, use the shader (now disable it)
		if (tmi == 0) {
			cgi->DisableProfiles();
		}
	}

	// if (shaders)
	// 	cgi->DisableProfiles();


}

void Scene::DBG() {

	{




		// // create a new camera path by writing into the CameraPath.txt file
		// int numberOfCameras = 4;

		// // set camera original
		// // should be 10 seconds long
		// V3 position(0,0,-100);
		// V3 zero(0.0f, 0.0f, 0.0f);
		// V3 upDirection(0.0f, 1.0f, 0.0f);
		// ppc->PositionAndOrient(position, zero, upDirection);
		
		
		// ofstream file("CameraPath.txt", ios::out);
		// if (file.is_open()) {
		// 	// camera looks at teapot
		// 	file << ppc->C << " " << ppc->a << " " << ppc->b << " " << ppc->c << endl;

		// 	position = V3(-100, 0, 0);
		// 	ppc->PositionAndOrient(position, zero, upDirection);
		// 	file << ppc->C << " " << ppc->a << " " << ppc->b << " " << ppc->c << endl;

		// 	position = V3(0, 0, 100);
		// 	ppc->PositionAndOrient(position, zero, upDirection);
		// 	file << ppc->C << " " << ppc->a << " " << ppc->b << " " << ppc->c << endl;
			
		// 	position = position + V3(0, 100, 0);
		// 	ppc->PositionAndOrient(position, zero, upDirection);
		// 	ppc->Rotate(position - zero, 90.0f);
		// 	file << ppc->C << " " << ppc->a << " " << ppc->b << " " << ppc->c << endl;

		// 	file.close(); //close the file object.
		// }
		// else {
		// 	cerr << "Problem writing to file" << endl; 
		// }

		// // load camera path from CameraPath.txt
		// cout << "loading camera path" << endl;
		// PPC *cameraPath = LoadCameraPathFromFile("CameraPath.txt", numberOfCameras);

		// for (int i = 0; i < numberOfCameras; i++) {
		// 	cout << i << ": " << cameraPath[i].C << "\t" << cameraPath[i].a << "\t" << cameraPath[i].b << "\t" << cameraPath[i].c << "\t" << endl;
		// }

		// // set camera to first camera: 
		// ppc->PositionAndOrient(cameraPath[0].C, cameraPath[0].GetVD(), cameraPath[0].b * -1.0f);
		// ppc->SetFocalLength(cameraPath[0].GetFocalLength());


		// // camera stuff
		// int currentCamera = 1;
		// int cameraInterpolationCounter = 0;
		// int framesBetweenCameras = 1000;

		// int i = currentCamera;
		// PPC *startCamera = new PPC(60.0f, fb->w, fb->h); 
		// startCamera->PositionAndOrient(cameraPath[i-1].C, cameraPath[i-1].C + cameraPath[i-1].GetVD(), cameraPath[i-1].b * -1.0f);
		// startCamera->SetFocalLength(cameraPath[i-1].GetFocalLength());
		// PPC *endCamera = new PPC(60.0f, fb->w, fb->h); 
		// endCamera->PositionAndOrient(cameraPath[i].C, cameraPath[i].C + cameraPath[i].GetVD(), cameraPath[i].b * -1.0f);
		// endCamera->SetFocalLength(cameraPath[i].GetFocalLength());







		// main loop initialization

		hwClearColor = V3(0.0f, 0.0f, 0.5f);

		// i want a camera at the position of the main teapot triangle mesh 
		// int imageSize = 250; // also global
		// initialize camera that takes picture
		PPC *textureCamera = new PPC(60, imageSize, imageSize); // fov, w, h
		// initialize i want a framebuffer to store the image
		// FrameBuffer *image = new FrameBuffer(0, 0, imageSize, imageSize); // this should be global
		image->show();

		// we will have load the image texture in the gpu code (renderhardware)
		// now we just need the triangle mesh.. more specifically, we want the four corners
		// but lets get those my making a rectangle basically
		// and then we want to pass those four corners into the shader, so the corners can be scene variables that get updated every frame
		// TM textureSquare = TM(); // lets make this a scene variable
		// textureSquare.SetTexturedRectangle(imageSize, imageSize); // its not upside down
		// flip the texture coordinates upside down though
		float scaleDown = 5;
		int squareSize = imageSize / scaleDown; // 250 / 10 = 25
		textureSquare.SetTexturedRectangle(squareSize, squareSize, 0, 1, 1, -1);
		


		// framerate stuff
		clock_t prevTime = clock();
		int framesPerFrameRateCalculation = 500;
		int counter2 = 0;
		phiAngle = 100.0f;



		// float theta = 10;


		// main loop
		while (true) {



			// if (currentCamera == numberOfCameras) {
			// 	// do nothing bc we are out of cameras
			// }
			// else if (cameraInterpolationCounter == framesBetweenCameras) {
			// 	currentCamera++;
			// 	cout << currentCamera << endl;
			// 	cameraInterpolationCounter = 0;
			// 	if (currentCamera != numberOfCameras) {
			// 		i = currentCamera;
			// 		// go to next camera if there are more
			// 		startCamera = new PPC(60.0f, fb->w, fb->h); 
			// 		startCamera->PositionAndOrient(cameraPath[i-1].C, cameraPath[i-1].C + cameraPath[i-1].GetVD(), cameraPath[i-1].b * -1.0f);
			// 		startCamera->SetFocalLength(cameraPath[i-1].GetFocalLength());
			// 		endCamera = new PPC(60.0f, fb->w, fb->h); 
			// 		endCamera->PositionAndOrient(cameraPath[i].C, cameraPath[i].C + cameraPath[i].GetVD(), cameraPath[i].b * -1.0f);
			// 		endCamera->SetFocalLength(cameraPath[i].GetFocalLength());
			// 	}
				
			// }
			// else {
			// 	// set the camera usingthe interpolation counter
			// 	int frame = cameraInterpolationCounter;
			// 	int N = framesBetweenCameras;
			// 	float interpolationFraction = (float) frame / (float) N;
			// 	ppc = startCamera->CalculateIntermediateLinearlyInterpolatedCamera(endCamera, interpolationFraction);

			// 	// hardwareFB->redraw(); 
			// 	// Render();
			// 	// Fl::check();


			// 	cameraInterpolationCounter++;
			// }

















			counter2++;
			if (counter2 == framesPerFrameRateCalculation) {
				counter2 = 0;

				clock_t currentTime = clock();

				double fps = (1.0 / (currentTime - prevTime) * 1000) * framesPerFrameRateCalculation;

				string s = "HW framebuffer; Framerate: " + to_string(fps);
				// string s = "HW framebuffer " + to_string(currentTime) + " " + to_string(prevTime);
				hardwareFB->label(s.c_str());

				prevTime = currentTime;
			}


			// make the second teapot spin around
			// thetaAngle += 0.8f;
			thetaAngle += 1.0f;
			// phiAngle = 110.0f;
			// phiAngle += 1;

			float distance = 50.0f;
			float sinphi = sinf(phiAngle * 3.141592 / 180.0f);
			float cosphi = cosf(phiAngle * 3.141592 / 180.0f);
			float costheta = cosf(thetaAngle * 3.141592 / 180.0f);
			float sintheta = sinf(thetaAngle * 3.141592 / 180.0f);
			// note that y and z are switched in spherical
			V3 position(
				distance * sinphi * costheta, 
				distance * cosphi,
				distance * sinphi * sintheta
			);
			tms[1].SetCenter(position);


			// take a picture of the second teapot using the camera 
			// image->SetBGR(V3(1, 0, 1).GetColor()); 
			image->SetBGR(0x00FFFFFF); 
			image->SetZB(0.0f);
			// orient the picture taking camera 
			textureCamera->PositionAndOrient(tms[0].GetCenter(), tms[1].GetCenter(), V3(0, 1, 0)); // camera pos at teapot, look at second teapot, up is up
			// render the second teapot onto the framebuffer
			tms[1].RenderFilled(textureCamera, image);
			image->redraw();

			// put this image onto the square
			textureSquare.texture = image;
			// now in render hardware, we have the texture and image is global so we can use it to render



			/*
			// we need to rotate and position the rectangle correctly
			// get view direction
			V3 viewDirection = textureCamera->GetVD();
			float x = viewDirection[0];
			float y = viewDirection[1];
			float z = viewDirection[2];

			// this is the rotation around y axis
			float theta1 = atan(z / x);
			theta1 = theta1 * 180 / 3.141592f + 90;
			if (x < 0) {
				theta1 += 180;
			}
			textureSquare.Rotate(V3(0,0,0), V3(0,-1,0), theta1);
			
			// this is the rotation look up and down
			float l = sqrtf(x * x + z * z);
			V3 hyp = viewDirection;
			hyp[1] = 0;
			hyp = hyp.normalized();
			float theta2 = atan(y / l);
			theta2 = theta2 * 180 / 3.141592f;
			V3 rotationAxis = hyp ^ V3(0, 1, 0);
			textureSquare.Rotate(V3(0,0,0), rotationAxis, theta2);
			*/



			
			// float theta = thirdAxis.length();
			// theta = theta * 180 / 3.141592;
			// textureSquare.Rotate(V3(0,0,0), perpendicularAxis, -theta);

			// translate to 
			// textureSquare.SetCenter(viewDirection * textureCamera->GetFocalLength() / scaleDown);
			// now the texture square is in the correct place, so set the rectangle coordinates
			// R0 = textureSquare.verts[0];
			// R1 = textureSquare.verts[1];
			// R2 = textureSquare.verts[2];
			// R3 = textureSquare.verts[3];
			

			// what if, instead of generating a square and rotating it, we unprojected the four corners of the framebuffer
			// textureCamera->Unproject(V3(u, v, 1/w), unprojectedPoint)
			// float distance = 
			float oneOverW = 1.0f / distance;
			// float oneOverW = 50.0f;
			V3 center = position;
			// textureCamera->Unproject(V3(0, 0, oneOverW), center);
			V3 rightShift = textureCamera->a * squareSize / 2;
			V3 downShift = textureCamera->b * squareSize / 2;
			R0 = center - rightShift - downShift;
			R1 = center - rightShift + downShift;
			R2 = center + rightShift + downShift;
			R3 = center + rightShift - downShift;

			// textureCamera->Unproject(V3(0, 0, oneOverW), R0);
			// textureCamera->Unproject(V3(0, imageSize, oneOverW), R1);
			// textureCamera->Unproject(V3(imageSize, imageSize, oneOverW), R2);
			// textureCamera->Unproject(V3(imageSize, 0, oneOverW), R3);

			// textureCamera->Unproject(V3(0, 0, oneOverW), R0);
			// textureCamera->Unproject(V3(0, 1, oneOverW), R1);
			// textureCamera->Unproject(V3(1, 1, oneOverW), R2);
			// textureCamera->Unproject(V3(1, 0, oneOverW), R3);

			textureSquare.verts[0] = R0;
			textureSquare.verts[1] = R1;
			textureSquare.verts[2] = R2;
			textureSquare.verts[3] = R3;

			// order we want
			// top left
			// bottom left
			// bottom right
			// top right

			// cout << "0 " << R0 << endl;
			// cout << "1 " << R1 << endl;
			// cout << "2 " << R2 << endl;
			// cout << "3 " << R3 << endl;
			



			// render the cpu based scene
			// Render();

			// render the GPU based scene
			hardwareFB->redraw(); 
			Fl::check();
			// textureSquare.Rotate(V3(0,0,0), V3(0, 1, 0), -theta2);
			// textureSquare.Rotate(V3(0,0,0), V3(-1, 0, 0), -theta1);
			// textureSquare = copy;

			// textureSquare.Rotate(V3(0,0,0), thirdAxis, -theta);
			// textureSquare.Rotate(V3(0,0,0), perpendicularAxis, theta);
			// textureSquare.Rotate(V3(0,0,0), V3(0,1,0), theta1);




			// textureSquare.Rotate(V3(0,0,0), rotationAxis, -theta2);
			// textureSquare.Rotate(V3(0,0,0), V3(0,-1,0), -theta1);
			
		}
		return;
	}





	{
		// create AABB using teapot and test if u can get min and max
		// AABB aabb(tms[0].SetAABB());
		// cerr << aabb.minv << endl;
		// cerr << aabb.maxv << endl;


		// try to transition from color to black and white
		// int fN = 10000;
		// for (int fi = 0; fi < fN; fi++) {
		// 	// Render();
		// 	hardwareFB->redraw();
		// 	Fl::check();
		// 	bwFraction = (float)fi / (float)(fN - 1);
		// }
		// cout << "bwFraction: " << bwFraction << endl;


		// create a new camera path by writing into the CameraPath.txt file
		int numberOfCameras = 4;

		// set camera original
		// should be 10 seconds long
		V3 position(0,0,-100);
		V3 zero(0.0f, 0.0f, 0.0f);
		V3 upDirection(0.0f, 1.0f, 0.0f);
		ppc->PositionAndOrient(position, zero, upDirection);
		
		
		ofstream file("CameraPath.txt", ios::out);
		if (file.is_open()) {
			// camera looks at teapot
			file << ppc->C << " " << ppc->a << " " << ppc->b << " " << ppc->c << endl;

			position = V3(-100, 0, 0);
			ppc->PositionAndOrient(position, zero, upDirection);
			file << ppc->C << " " << ppc->a << " " << ppc->b << " " << ppc->c << endl;

			position = V3(0, 0, 100);
			ppc->PositionAndOrient(position, zero, upDirection);
			file << ppc->C << " " << ppc->a << " " << ppc->b << " " << ppc->c << endl;
			
			position = position + V3(0, 100, 0);
			ppc->PositionAndOrient(position, zero, upDirection);
			ppc->Rotate(position - zero, 90.0f);
			file << ppc->C << " " << ppc->a << " " << ppc->b << " " << ppc->c << endl;

			file.close(); //close the file object.
		}
		else {
			cerr << "Problem writing to file" << endl; 
		}



		// /*

		// load camera path from CameraPath.txt
		cout << "loading camera path" << endl;
		PPC *cameraPath = LoadCameraPathFromFile("CameraPath.txt", numberOfCameras);

		for (int i = 0; i < numberOfCameras; i++) {
			cout << i << ": " << cameraPath[i].C << "\t" << cameraPath[i].a << "\t" << cameraPath[i].b << "\t" << cameraPath[i].c << "\t" << endl;
		}

		// set camera to first camera: 
		ppc->PositionAndOrient(cameraPath[0].C, cameraPath[0].GetVD(), cameraPath[0].b * -1.0f);
		ppc->SetFocalLength(cameraPath[0].GetFocalLength());
		// for (int i = 1; i < numberOfCameras; i++) {  // is the current camera (interpolate between previous and current)

		// 	PPC *startCamera = new PPC(60.0f, fb->w, fb->h); 
		// 	startCamera->PositionAndOrient(cameraPath[i-1].C, cameraPath[i-1].C + cameraPath[i-1].GetVD(), cameraPath[i-1].b * -1.0f);
		// 	startCamera->SetFocalLength(cameraPath[i-1].GetFocalLength());
		// 	PPC *endCamera = new PPC(60.0f, fb->w, fb->h); 
		// 	endCamera->PositionAndOrient(cameraPath[i].C, cameraPath[i].C + cameraPath[i].GetVD(), cameraPath[i].b * -1.0f);
		// 	endCamera->SetFocalLength(cameraPath[i].GetFocalLength());
		// 	// PPC *endCamera = &cameraPath[i];

		// 	int N = 1000;
		// 	for (int frame = 0; frame <= N; frame++) {

		// 		float interpolationFraction = (float) frame / (float) N;
		// 		ppc = startCamera->CalculateIntermediateLinearlyInterpolatedCamera(endCamera, interpolationFraction);

		// 		hardwareFB->redraw(); 
		// 		// Render();
		// 		Fl::check();
		// 	}
		// }


		// */

		

		// sprite stuff
		int spriteFrame = 0;

		int framesPerFreezeFrame = 100;
		int counter = 0;

		hwClearColor = V3(0.0f, 0.0f, 0.5f);

		// framerate stuff
		clock_t prevTime = clock();
		int framesPerFrameRateCalculation = 500;
		int counter2 = 0;


		// camera stuff
		int currentCamera = 1;
		int cameraInterpolationCounter = 0;
		int framesBetweenCameras = 5000;

		int i = currentCamera;
		PPC *startCamera = new PPC(60.0f, fb->w, fb->h); 
		startCamera->PositionAndOrient(cameraPath[i-1].C, cameraPath[i-1].C + cameraPath[i-1].GetVD(), cameraPath[i-1].b * -1.0f);
		startCamera->SetFocalLength(cameraPath[i-1].GetFocalLength());
		PPC *endCamera = new PPC(60.0f, fb->w, fb->h); 
		endCamera->PositionAndOrient(cameraPath[i].C, cameraPath[i].C + cameraPath[i].GetVD(), cameraPath[i].b * -1.0f);
		endCamera->SetFocalLength(cameraPath[i].GetFocalLength());


		while (true) {


			// go through the sprite frames
			// float sixth = 1.0f / 6.0f;
			// tms[1].AdjustTexturedRectangle(spriteFrame * sixth, 0.0f, sixth, 1.0f);

			// counter++;
			// if (counter == framesPerFreezeFrame) {
			// 	counter = 0;
			// 	spriteFrame++;
			// }

			// spriteFrame = spriteFrame % 6;


			// clock_t currentTime = clock();
			// clock_t deltaTime = currentTime - prevTime;
			// currentFramerate = 1.0 / (deltaTime) * 1000;
			// currentFramerate = deltaTime;

			counter2++;
			if (counter2 == framesPerFrameRateCalculation) {
				counter2 = 0;

				clock_t currentTime = clock();

				double fps = (1.0 / (currentTime - prevTime) * 1000) * framesPerFrameRateCalculation;

				string s = "HW framebuffer; Framerate: " + to_string(fps);
				// string s = "HW framebuffer " + to_string(currentTime) + " " + to_string(prevTime);
				hardwareFB->label(s.c_str());

				prevTime = currentTime;
			}


			if (currentCamera == numberOfCameras) {
				// do nothing bc we are out of cameras
			}
			else if (cameraInterpolationCounter == framesBetweenCameras) {
				currentCamera++;
				cout << currentCamera << endl;
				cameraInterpolationCounter = 0;
				if (currentCamera != numberOfCameras) {
					i = currentCamera;
					// go to next camera if there are more
					startCamera = new PPC(60.0f, fb->w, fb->h); 
					startCamera->PositionAndOrient(cameraPath[i-1].C, cameraPath[i-1].C + cameraPath[i-1].GetVD(), cameraPath[i-1].b * -1.0f);
					startCamera->SetFocalLength(cameraPath[i-1].GetFocalLength());
					endCamera = new PPC(60.0f, fb->w, fb->h); 
					endCamera->PositionAndOrient(cameraPath[i].C, cameraPath[i].C + cameraPath[i].GetVD(), cameraPath[i].b * -1.0f);
					endCamera->SetFocalLength(cameraPath[i].GetFocalLength());
				}
				
			}
			else {
				// set the camera usingthe interpolation counter
				int frame = cameraInterpolationCounter;
				int N = framesBetweenCameras;
				float interpolationFraction = (float) frame / (float) N;
				ppc = startCamera->CalculateIntermediateLinearlyInterpolatedCamera(endCamera, interpolationFraction);

				// hardwareFB->redraw(); 
				// Render();
				// Fl::check();


				cameraInterpolationCounter++;
			}

			// render the cpu based scene
			// Render();
			// render the GPU based scene
			hardwareFB->redraw(); 
			Fl::check();

			

			/*
			// update the camera position
			// need to replace this code with camera pathing
			
			// rollAngle = 0.0f;
			// phiAngle += 15.0f;
			// thetaAngle += 0.01f;
			// thetaAngle += 15.1f;


			float distance = 100.0f;
			float sinphi = sinf(phiAngle * 3.141592 / 180.0f);
			float cosphi = cosf(phiAngle * 3.141592 / 180.0f);
			float costheta = cosf(thetaAngle * 3.141592 / 180.0f);
			float sintheta = sinf(thetaAngle * 3.141592 / 180.0f);

			// note that y and z are switched in spherical
			V3 position(
				distance * sinphi * costheta, 
				distance * cosphi,
				distance * sinphi * sintheta
			);

			// float rollAngle2 = (rollAngle+90.0f) * 3.141592 / 180.0f;
			// V3 upDirection(cosf(rollAngle2), sin(rollAngle2), 0.0f); // determined by roll angle
			V3 zero(0.0f, 0.0f, 0.0f);
			V3 upDirection(0.0f, 1.0f, 0.0f);
			ppc->PositionAndOrient(position, zero, upDirection);

			ppc->Rotate((ppc->C).normalized(), rollAngle);

			*/


		}
		return;
	}



	{
		int frames = 10000;
		for (int fi = 0; fi < frames; fi++) {
			fi--;
			hwClearColor = V3(0.0f, 0.0f, (float)fi / (float)(frames - 1));



			Render();

			// this will make the framebuffer call draw which calls Scene::RenderHardware() above
			hardwareFB->redraw(); 
			Fl::check();

			// rollAngle = 0.0f;
			// phiAngle += 15.0f;
			thetaAngle += 0.1f;
			// thetaAngle += 15.1f;


			float distance = 100.0f;
			float sinphi = sinf(phiAngle * 3.141592 / 180.0f);
			float cosphi = cosf(phiAngle * 3.141592 / 180.0f);
			float costheta = cosf(thetaAngle * 3.141592 / 180.0f);
			float sintheta = sinf(thetaAngle * 3.141592 / 180.0f);

			// note that y and z are switched in spherical
			V3 position(
				distance * sinphi * costheta, 
				distance * cosphi,
				distance * sinphi * sintheta
			);

			// float rollAngle2 = (rollAngle+90.0f) * 3.141592 / 180.0f;
			// V3 upDirection(cosf(rollAngle2), sin(rollAngle2), 0.0f); // determined by roll angle
			V3 zero(0.0f, 0.0f, 0.0f);
			V3 upDirection(0.0f, 1.0f, 0.0f);
			ppc->PositionAndOrient(position, zero, upDirection);

			ppc->Rotate((ppc->C).normalized(), rollAngle);


		}
		return;
	}




	{
		while (true) {
			// i want to position and orient using the angles + distance = 100
			// rollAngle = 0.0f;
			// phiAngle += 15.0f;
			// thetaAngle += 15.0f;

			float distance = 100.0f;
			float sinphi = sinf(phiAngle * 3.141592 / 180.0f);
			float cosphi = cosf(phiAngle * 3.141592 / 180.0f);
			float costheta = cosf(thetaAngle * 3.141592 / 180.0f);
			float sintheta = sinf(thetaAngle * 3.141592 / 180.0f);

			// note that y and z are switched in spherical
			V3 position(
				distance * sinphi * costheta, 
				distance * cosphi,
				distance * sinphi * sintheta
			);

			// float rollAngle2 = (rollAngle+90.0f) * 3.141592 / 180.0f;
			// V3 upDirection(cosf(rollAngle2), sin(rollAngle2), 0.0f); // determined by roll angle
			V3 zero(0.0f, 0.0f, 0.0f);
			V3 upDirection(0.0f, 1.0f, 0.0f);
			ppc->PositionAndOrient(position, zero, upDirection);

			ppc->Rotate((ppc->C).normalized(), rollAngle);


			// float distance = 100.0f;
			// float sinphi = sinf(phiAngle * 3.141592 / 180.0f);
			// float cosphi = cosf(phiAngle * 3.141592 / 180.0f);
			// float costheta = cosf(thetaAngle * 3.141592 / 180.0f);
			// float sintheta = sinf(thetaAngle * 3.141592 / 180.0f);

			// V3 position(
			// 	distance * sinphi * costheta, 
			// 	distance * sinphi * sintheta, 
			// 	distance * cosphi
			// );

			// V3 zero(0.0f, 0.0f, 0.0f);

			// V3 upDirection(0.0f, 1.0f, 0.0f);
			// ppc->PositionAndOrient(position, zero, upDirection);

			// V3 directionCameraToTeapot = (ppc->C).normalized() * -1;
			// ppc->Rotate(directionCameraToTeapot, rollAngle);

			Render();
			Fl::check();
		}
		return;
	}

	{


		// tms[0].Rotate(V3(0.0f, 0.0f, -300.0f), V3(0.0f, 1.0f, 0.0f), 105.0f);
		
		float rotateDegrees = -15.0f;
		// for (int fi = 0; fi < 450; fi++) {

		int spriteFrame = 0;

		while (true) {
			Render();
			Fl::check();
			
			float sixth = 1.0f / 6.0f;
			tms[0].AdjustTexturedRectangle(spriteFrame * sixth, 0.0f, sixth, 1.0f);
			spriteFrame++;
			spriteFrame = spriteFrame % 6;



			if (paused) {
				continue;
			}
			// tms[0].Rotate(tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f), 1.0f);
			for (int i = 0; i < tmsN; i++) {
				tms[i].Rotate(V3(0.0f, 0.0f, -300.0f), V3(0.0f, 1.0f, 0.0f), rotateDegrees);
			}
		}


		Render();
		return;
	}


	{
		while (true) {
			Render();
			Fl::check();
		}
		return;
	}

	

	{
		while (true) {
			V3 matColor(1.0f, 0.0f, 0.0f);
			// float ka = 0.03f; // ambient light 

			TM tm;
			tm.SetAsCopy(&(tms[0]));
			// modify tms[0]

			if (renderMode == 1) {
				// do nothing to triangle mesh, just use given colors
			}
			else if (renderMode == 2) {
				// tms[0].PointLight(matColor, ka, lightPos); // modifies color of vertices based on light
				tms[0].PointLightWithSpecular(matColor, ka, lightPos, es, ppc->C); // modifies color of vertices based on light
				
			}
			else {
				// calculate color in rendering step (per pixel)
			}



			Render();
			Fl::check();

			// as we decrease the b value (increase magnitude)
			// the image gets smaller (squished)
			// cout << "ppc3 b" << ppc3->b << endl;
			// ppc3->b[1] = ppc3->b[1] - 0.01;

			// fix tms[0]
			tms[0].SetAsCopy(&(tm));
		}
		return;
	}

	{
		fb->SetBGR(0xFF000000);
		fb->SetZB(0.0f);
		float width = fb->w;
		float height = fb->h;

		V3 p0(10, 10, 0);
		V3 p1(width - 10, 10, 0);
		V3 p2(width / 2, height - 10, 0);

		V3 c0(1, 0, 0);
		V3 c1(0, 1, 0);
		V3 c2(0, 0, 1);
		// fb->DrawTriangle(p0, p1, p2, 0xFFAA8800);
		fb->DrawTriangleInterpolatedColor(p0, p1, p2, c0, c1, c2);

		// while (true) {
		// 	Render();
		// 	Fl::check();
		// }

		fb->redraw();

		return;

	}








	{
		TM tm;
		tm.SetAsCopy(&(tms[0]));
		tms[0].SetUnshared(&tm);
		for (int fi = 0; fi < 1000; fi++) {
			Render();
			Fl::check();
			tms[0].Explode(0.1f);
			V3 center = tms[0].GetCenter();
			// tms[0].InflateFromCenter(0.1f, center);
		}
		return;
	}

	{
		Render();
		float vlength = 10.0f;
		tms[0].VisualizeVertexNormals(ppc, fb, vlength);
		tms[0].VisualizeVertexNormals(ppc3, fb3, vlength);
		return;
	}

	{
		V3 matColor(1.0f, 0.0f, 0.0f);
		float ka = 0.03f;
		tms[0].Light(matColor, ka, ppc->GetVD());
		Render();
		return;
	}

	// reset camera
	ppc->PositionAndOrient(V3(0,0,0), V3(0,0,-1), V3(0,1,0));

	while (true) {
		Render();
		Fl::check();
	}
	return;

}



// move camera
void Scene::Forward() {
	ppc->Translate(ppc->GetVD() * 3.0f);
	// revolve up
	// phiAngle -= deltaAngleOnButtonClick;
}
void Scene::Backward() {
	ppc->Translate(ppc->GetVD() * -3.0f);
	// revolve down
	// phiAngle += deltaAngleOnButtonClick;
}

// move teapot
void Scene::Leftward() {
	// tms[0].Translate(V3(1.0f, 0.0f, 0.0f) * -3.0f);
	// revolve left
	// thetaAngle += deltaAngleOnButtonClick;
}
void Scene::Rightward() {
	// tms[0].Translate(V3(1.0f, 0.0f, 0.0f) * 3.0f);
	// revolve right
	// thetaAngle -= deltaAngleOnButtonClick;
}

// rotate camera
void Scene::LookUp() {
	ppc->Rotate(ppc->a, 5);
	// ppc->Rotate(V3(-1,0,0), 5);
}
void Scene::LookDown() {
	ppc->Rotate(ppc->a, -5);
	// ppc->Rotate(V3(-1,0,0), -5);
}
void Scene::LookLeft() {
	// ppc->Rotate(ppc->b, -5);
	ppc->Rotate(V3(0,-1,0), -5);
	// roll left
	// rollAngle -= deltaAngleOnButtonClick;
}
void Scene::LookRight() {
	// ppc->Rotate(ppc->b, 5);
	ppc->Rotate(V3(0,-1,0), 5);
	// roll right
	// rollAngle += deltaAngleOnButtonClick;
}


// pass in the number of keyframes to load as a parameter
// file should contain 4 vector values per line in the order (C, a, b, c)
// returns a pointer to an array of camera paths of length = keyFrames
PPC *Scene::LoadCameraPathFromFile(char *fileName, int keyFrames) {

	PPC *cameraPath = new PPC[keyFrames];

	ifstream file(fileName, ios::in);
	if (file.is_open()) {

		for (int i = 0; i < keyFrames; i++) {
			cameraPath[i] = PPC();
			file >> cameraPath[i].C >> cameraPath[i].a >> cameraPath[i].b >> cameraPath[i].c;
		}

		file.close(); //close the file object.
		return cameraPath;
	}
	else {
		cerr << "Problem with opening file: " << fileName << endl; 
	}
}


void Scene::NewButton() {

	cout << "button pressed" << endl;

	// flip the mirror tiling of the wall texture
	tms[1].texture->mirrorTiling = !tms[1].texture->mirrorTiling;

	

	// return;
	// cerr << "Play Button pressed" << endl;
	// // load camera path from CameraPath.txt
	// cout << "loading camera path" << endl;
	// PPC *cameraPath = LoadCameraPathFromFile("CameraPath.txt", 4);

	// for (int i = 0; i < 4; i++) {
	// 	cout << i << ": " << cameraPath[i].C << "\t" << cameraPath[i].a << "\t" << cameraPath[i].b << "\t" << cameraPath[i].c << "\t" << endl;
	// }

	// // go through camera path (interpolate through the cameras, 100 frames per interpolation)
	// // set camera to first camera: 
	// ppc->PositionAndOrient(cameraPath[0].C, cameraPath[0].GetVD(), cameraPath[0].b * -1.0f);
	// ppc->SetFocalLength(cameraPath[0].GetFocalLength());
	// int frameNumber = 1;
	// for (int i = 1; i < 4; i++) {

	// 				// PPC *startCamera = new PPC(60.0f, fb->w, fb->h);
	// 				// startCamera->Translate(V3(-20, 0, 0));
	// 				// startCamera->Rotate(V3(1, 0, 0), 10);

	// 				// PPC *endCamera = new PPC(60.0f, fb->w, fb->h);
	// 				// endCamera->Rotate(V3(1, 0, 0), -10);

	// 	PPC *startCamera = new PPC(60.0f, fb->w, fb->h); 
	// 	startCamera->PositionAndOrient(cameraPath[i-1].C, cameraPath[i-1].C + cameraPath[i-1].GetVD(), cameraPath[i-1].b * -1.0f);
	// 	startCamera->SetFocalLength(cameraPath[i-1].GetFocalLength());
	// 	PPC *endCamera = new PPC(60.0f, fb->w, fb->h); 
	// 	endCamera->PositionAndOrient(cameraPath[i].C, cameraPath[i].C + cameraPath[i].GetVD(), cameraPath[i].b * -1.0f);
	// 	endCamera->SetFocalLength(cameraPath[i].GetFocalLength());
	// 	// PPC *endCamera = &cameraPath[i];

	// 	int N = 100;
	// 	for (int frame = 0; frame <= N; frame++) {

	// 		float interpolationFraction = (float) frame / (float) N;
	// 		ppc = startCamera->CalculateIntermediateLinearlyInterpolatedCamera(endCamera, interpolationFraction);


	// 		string temp = "folder/file";
	// 		temp = temp + to_string(frameNumber);
	// 		temp = temp + ".tiff";
	// 		char arr[20];
	// 		strcpy_s(arr, temp.c_str());

	// 		// char arr[] = "folder/filennn.tiff";
	// 		// arr[11] = '0' + frameNumber;
	// 		// arr[12] = '0' + frameNumber;
	// 		// arr[13] = '0' + frameNumber;
	// 		// if (frameNumber == 1)
	// 			fb->SaveAsTiff(arr);
	// 		frameNumber++;
	// 		Render();
	// 		Fl::check();
	// 	}
	// }
}

void Scene::TranslateLight(float x, float y, float z) {
	cout << "translate called " << x << y << z << endl;
	V3 translationVector(x*10, y*10, z*10);
	lightPos = lightPos + translationVector;
}

// increments of 0.1
void Scene::ChangeAmbientLight(float change) {
	cout << "ChangeAmbientLight called " << change << endl;
	ka += change;
	if (ka < 0) ka = 0;
	if (ka > 1) ka = 1;
}


void Scene::SetRenderMode(int rm) {
	cout << "SetRenderMode called " << rm << endl;
	renderMode = rm;
}

void Scene::MultiplyES(float multiplier) {
	es *= multiplier;
	// either 0.5 or 2
}

void Scene::ToggleBilinear() {
	cout << "bilinear" << endl;
	for (int i = 0; i < tmsN; i++) {
		if (tms[i].texture)
			tms[i].texture->bilinearInterpolation = !tms[i].texture->bilinearInterpolation;

	}
}


void Scene::Pause() {
	cout << "pause" << endl;
	paused = !paused;
}

void Scene::idk() {
	filledOrWireFrame = !filledOrWireFrame;
}