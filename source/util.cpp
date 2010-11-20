#include "util.h"

void renderImageWorldSpace(CIwSVec2 position, float angle, float scaleFactor, int textureSize, int frameNumber, int numFrames) {
	
	int left = position.x;
	int top = position.y;	
	
	static CIwSVec3 vertices[4];
	static CIwSVec2 UVs[4];
	
	//set up model space vertices
	
	int vertexDist = scaleFactor*textureSize/2;
	
	vertices[0] = CIwSVec3(-1*vertexDist, -1*vertexDist, -1);
	vertices[2] = CIwSVec3(vertexDist, -1*vertexDist, -1);
	vertices[3] = CIwSVec3(vertexDist, vertexDist, -1);
	vertices[1] = CIwSVec3(-1*vertexDist, vertexDist, -1);
	
	CIwMat modelTransform = CIwMat::g_Identity;
	modelTransform.SetRotZ(TO_RADIANS(angle));
	modelTransform.SetTrans(CIwVec3(left, -1*top, 1));
	IwGxSetModelMatrix(&modelTransform, false);
	
	int squaredSize = textureSize*textureSize;
	int offset = squaredSize/numFrames;
	
	//set up sprite UV's
	UVs[0] = CIwSVec2(frameNumber*offset, 0);
	UVs[2] = CIwSVec2((frameNumber+1)*offset, 0);
	UVs[3] = CIwSVec2((frameNumber+1)*offset, squaredSize);
	UVs[1] = CIwSVec2(frameNumber*offset, squaredSize);
	
	//render the unit in model space
	IwGxSetUVStream(UVs);
	IwGxSetColStream(NULL);
	IwGxSetVertStreamModelSpace(vertices, 4);
	IwGxDrawPrims(IW_GX_QUAD_STRIP, NULL, 4);
	IwGxFlush();
}