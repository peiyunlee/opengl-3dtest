#pragma once

#include "../header/Angel.h"

#include "ModelPool.h"
#include "CQuad.h"
#include "CWireSphere.h"
#include "CLineSegment.h"

class Room
{
private:
public:
	Room();
	~Room();

	int lightCount;

	bool g_bAutoRotating;

	float g_fLightR;
	float g_fLightG;
	float g_fLightB;

	float g_fElapsedTime;
	float g_fLightRadius;
	float g_fLightTheta;

	virtual void SetProjectionMatrix(mat4 mpx) = 0;
	virtual void SetViewMatrix(mat4 mvx) = 0;
	virtual void Draw() = 0;
	virtual void Update(float delta) = 0;

	LightSource *g_Light;

	CWireSphere *g_pLight;
	CLineSegment *g_LightLine;
protected:
	virtual void LightGenerator(int count)=0;
	virtual void ObjectGenerator() = 0;
};


class Room1 :public Room
{
private:
	CQuad *g_LeftWall, *g_RightWall;
	CQuad *g_FrontWall, *g_BackWall;
	CQuad *g_BottomWall, *g_TopWall;

	ModelPool *g_pCat;
	ModelPool *g_pDeer;
	ModelPool *g_pRat;
	ModelPool *g_pWolf;
public:
	Room1();
	~Room1();

	void SetProjectionMatrix(mat4 mpx);
	void SetViewMatrix(mat4 mvx);
	void Draw();
	void Update(float delta);
protected:
	void LightGenerator(int count);
	void ObjectGenerator();
	void UpdateLightPosition(float dt);
};