#include "Room.h"

Room1::Room1() {
	g_bAutoRotating = false;

	g_fElapsedTime = 0;
	g_fLightRadius = 5;
	g_fLightTheta = 0;

	g_fLightR = 0.5f;
	g_fLightG = 0.5f;
	g_fLightB = 0.5f;

	LightGenerator(4);
	ObjectGenerator();
}

Room1::~Room1() {
	if (g_pCat != NULL) delete g_pCat;
	if (g_pDeer != NULL) delete g_pDeer;
	if (g_pWolf != NULL) delete g_pWolf;
	if (g_pRat != NULL) delete g_pRat;

	if (g_LeftWall != NULL) delete g_LeftWall;
	if (g_RightWall != NULL) delete g_RightWall;
	if (g_FrontWall != NULL) delete g_FrontWall;
	if (g_BackWall != NULL) delete g_BackWall;
	if (g_TopWall != NULL) delete g_TopWall;
	if (g_BottomWall != NULL) delete g_BottomWall;
}

void Room1::LightGenerator(int count) {

	mat4 mxT;
	lightCount = 4;
	g_Light = new LightSource[lightCount];
	g_pLight = new CWireSphere[lightCount];
	g_LightLine = new CLineSegment[lightCount];

	g_Light[0] =
	{
		0,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(g_fLightRadius, g_fLightRadius + 6.0, 0.0f, 1.0f),   // position
		vec3(0.0f, 0.0f, 0.0f),	  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),  //spotDirection
		1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
		30.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.0f,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1,
	};
	g_Light[1] =
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(1, 0, 0.0, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(-2.0f, 5.0f, -2.0f, 1.0f),   // position
		vec3(-6.0f, 3.0f, -6.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
		60.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1,
	};
	g_Light[2] =
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(0, 1.0, 0, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(4.0f, 3.0f, -4.0f, 1.0f),   // position
		vec3(10.0f, -1.0f, -10.0f),  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),  //spotDirection
		1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1,
	};
	g_Light[3] =
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(0, 0, 1.0, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(-2.0f, 5.0f, 2.0f, 1.0f),   // position
		vec3(-10.0f, -1.0f, 10.0f),  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),	  //spotDirection
		1.0f,	// spotExponent(parameter e); cos^(e)(phi) 
		60.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1,
	};

	//代表光的物體
	for (int i = 0; i < lightCount; i++)
	{
		// 設定 代表 Light 的 WireSphere
		g_pLight[i].SetDefault(0.25f, 6, 3);
		g_pLight[i].SetShader();
		mxT = Translate(g_Light[i].position);
		g_pLight[i].SetTRSMatrix(mxT);
		g_pLight[i].SetColor(g_Light[i].diffuse);
		//g_pLight[i]->SetLightingDisable();
	}


	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		g_Light[i].UpdateDirection();
		g_LightLine[i].SetDefault(g_Light[i].position, g_Light[i].spotTarget, vec4(1, 0, 0, 1));
		g_LightLine[i].SetShader();
	}
}

void Room1::ObjectGenerator() {
	mat4 mxT;
	vec4 vT;

	vT.x = 0.0f; vT.y = 0.0f; vT.z = 0;
	mxT = Translate(vT);
	g_BottomWall = new CQuad;
	g_BottomWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_BottomWall->SetColor(vec4(0.6f));
	g_BottomWall->SetTRSMatrix(mxT*Scale(20.0f, 1, 20.0f));
	g_BottomWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_BottomWall->SetShadingMode(GOURAUD_SHADING);
	g_BottomWall->SetShader();

	vT.x = 0.0f; vT.y = 20.0f; vT.z = 0;
	mxT = Translate(vT);
	g_TopWall = new CQuad;
	g_TopWall->SetColor(vec4(0.6f));
	g_TopWall->SetTRSMatrix(mxT*RotateZ(180.0f)*Scale(20.0f, 1, 20.0f));
	g_TopWall->SetMaterials(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1), vec4(0.5f, 0.5f, 0.5f, 1.0f));
	g_TopWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_TopWall->SetShadingMode(GOURAUD_SHADING);
	g_TopWall->SetShader();

	vT.x = -10.0f; vT.y = 10.0f; vT.z = 0;
	mxT = Translate(vT);
	g_LeftWall = new CQuad;
	g_LeftWall->SetColor(vec4(0.6f));
	g_LeftWall->SetTRSMatrix(mxT*RotateZ(-90.0f)*Scale(20.0f, 1, 20.0f));
	g_LeftWall->SetMaterials(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1), vec4(0.5f, 0.5f, 0.5f, 1.0f));
	g_LeftWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_LeftWall->SetShadingMode(GOURAUD_SHADING);
	g_LeftWall->SetShader();

	vT.x = 10.0f; vT.y = 10.0f; vT.z = 0;
	mxT = Translate(vT);
	g_RightWall = new CQuad;
	g_RightWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_RightWall->SetColor(vec4(0.6f));
	g_RightWall->SetTRSMatrix(mxT*RotateZ(90.0f)*Scale(20.0f, 1, 20.0f));
	g_RightWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_RightWall->SetShadingMode(GOURAUD_SHADING);
	g_RightWall->SetShader();

	vT.x = 0.0f; vT.y = 10.0f; vT.z = 10.0f;
	mxT = Translate(vT);
	g_FrontWall = new CQuad;
	g_FrontWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_FrontWall->SetColor(vec4(0.6f));
	g_FrontWall->SetTRSMatrix(mxT*RotateX(-90.0f)*Scale(20.0f, 1, 20.0f));
	g_FrontWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_FrontWall->SetShadingMode(GOURAUD_SHADING);
	g_FrontWall->SetShader();

	vT.x = 0.0f; vT.y = 10.0f; vT.z = -10.0f;
	mxT = Translate(vT);
	g_BackWall = new CQuad;
	g_BackWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_BackWall->SetColor(vec4(0.6f));
	g_BackWall->SetTRSMatrix(mxT*RotateX(90.0f)*Scale(20.0f, 1, 20.0f));
	g_BackWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_BackWall->SetShadingMode(GOURAUD_SHADING);
	g_BackWall->SetShader();

	vT.x = 7.0; vT.y = 0.5; vT.z = -6.0;
	mxT = Translate(vT);
	g_pCat = new ModelPool("Model/cat.obj", Type_3DMax);
	g_pCat->SetTRSMatrix(mxT*RotateY(225.0f)*Scale(0.004f, 0.004f, 0.004f));
	g_pCat->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.5, 0.5, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pCat->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);


	vT.x = -6.0; vT.y = 0.5; vT.z = -6.0;
	mxT = Translate(vT);
	g_pDeer = new ModelPool("Model/deer.obj", Type_3DMax);
	g_pDeer->SetTRSMatrix(mxT*RotateY(315.0f)*Scale(0.004f, 0.004f, 0.004f));
	g_pDeer->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.5, 0.5, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pDeer->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	vT.x = -6.0; vT.y = 0.5; vT.z = 6.0;
	mxT = Translate(vT);
	g_pWolf = new ModelPool("Model/wolf.obj", Type_3DMax);
	g_pWolf->SetTRSMatrix(mxT*RotateY(45.0f)*Scale(0.007f, 0.007f, 0.007f));
	g_pWolf->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.5, 0.5, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pWolf->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	vT.x = 0.0; vT.y = 0.5; vT.z = -0.0;
	mxT = Translate(vT);
	g_pRat = new ModelPool("Model/rat.obj", Type_3DMax);
	g_pRat->SetTRSMatrix(mxT*RotateY(45.0f)*Scale(0.015f, 0.015f, 0.015f));
	g_pRat->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.5, 0.5, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pRat->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

}

void Room1::SetProjectionMatrix(mat4 mpx) {
	g_pCat->SetProjectionMatrix(mpx);
	g_pDeer->SetProjectionMatrix(mpx);
	g_pWolf->SetProjectionMatrix(mpx);
	g_pRat->SetProjectionMatrix(mpx);

	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		g_pLight[i].SetProjectionMatrix(mpx);
		g_LightLine[i].SetProjectionMatrix(mpx);
	}

	g_BottomWall->SetProjectionMatrix(mpx);
	g_TopWall->SetProjectionMatrix(mpx);
	g_LeftWall->SetProjectionMatrix(mpx);
	g_RightWall->SetProjectionMatrix(mpx);
	g_FrontWall->SetProjectionMatrix(mpx);
	g_BackWall->SetProjectionMatrix(mpx);
}

void Room1::Draw() {

	g_pCat->Draw();
	g_pDeer->Draw();
	g_pWolf->Draw();
	g_pRat->Draw();

	g_BottomWall->Draw();
	g_LeftWall->Draw();
	g_RightWall->Draw();
	g_FrontWall->Draw();
	g_BackWall->Draw();
	g_TopWall->Draw();


	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		g_pLight[i].Draw();
		g_LightLine[i].Draw();
	}
}

void Room1::UpdateLightPosition(float dt)
{
	mat4 mxT;
	// 每秒繞 Y 軸轉 90 度
	g_fElapsedTime += dt;
	g_fLightTheta = g_fElapsedTime*(float)M_PI_2;
	if (g_fLightTheta >= (float)M_PI*2.0f) {
		g_fLightTheta -= (float)M_PI*2.0f;
		g_fElapsedTime -= 4.0f;
	}

	g_Light[0].position.x = g_fLightRadius * cosf(g_fLightTheta);
	g_Light[0].position.z = g_fLightRadius * sinf(g_fLightTheta);
	mxT = Translate(g_Light[0].position);
	g_pLight[0].SetTRSMatrix(mxT);
	g_LightLine[0].UpdatePosition(g_Light[0].position, g_Light[0].spotTarget);

}

void Room1::SetViewMatrix(mat4 mvx) {
	g_pCat->SetViewMatrix(mvx);
	g_pDeer->SetViewMatrix(mvx);
	g_pWolf->SetViewMatrix(mvx);
	g_pRat->SetViewMatrix(mvx);

	g_BottomWall->SetViewMatrix(mvx);
	g_TopWall->SetViewMatrix(mvx);
	g_LeftWall->SetViewMatrix(mvx);
	g_RightWall->SetViewMatrix(mvx);
	g_FrontWall->SetViewMatrix(mvx);
	g_BackWall->SetViewMatrix(mvx);
	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		g_pLight[i].SetViewMatrix(mvx);
		g_LightLine[i].SetViewMatrix(mvx);

	}
}

void Room1::Update(float delta) {

	if (g_bAutoRotating) { // Part 2 : 重新計算 Light 的位置
		UpdateLightPosition(delta);
	}

	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		g_pLight[i].Update(delta);
		g_LightLine[i].UpdatePosition(g_Light[i].position, g_Light[i].spotTarget);
	}

	// 如果需要重新計算時，在這邊計算每一個物件的顏色
	g_pCat->Update(g_Light, delta);
	g_pDeer->Update(g_Light, delta);
	g_pWolf->Update(g_Light, delta);
	g_pRat->Update(g_Light, delta);

	g_BottomWall->Update(g_Light, delta);
	g_TopWall->Update(g_Light, delta);
	g_LeftWall->Update(g_Light, delta);
	g_RightWall->Update(g_Light, delta);
	g_FrontWall->Update(g_Light, delta);
	g_BackWall->Update(g_Light, delta);
}