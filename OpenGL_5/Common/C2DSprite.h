#ifndef C2DSPRITE_H
#define C2DSPRITE_H

#include "../header/Angel.h"
#include "CShape.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define QUAD_NUM 6		// 2 faces, 2 triangles/face 

class C2DSprite : public CShape
{
public:
	C2DSprite();
	~C2DSprite();

	vec4  m_OriginalLf, m_OriginalTR;  // sprite �blocal �y�Ъ����W�P�k�U��ӳ��I�y��
	vec4  m_Lf, m_TR;  // sprite �b�@�ɮy�Ъ����W�P�k�U��ӳ��I�y��
	vec4  m_DefaultColor;
	bool  m_bPushed;

	void Update(float dt, point4 vLightPos, color4 vLightI);
	void Update(float dt, const LightSource &Lights);
	void Update(const LightSource *, float) {};
	void Update(float dt); // ���p��������ө�

	void SetTRSMatrix(mat4 &mat);
	void SetDefaultColor(vec4 vColor);
	bool getButtonStatus() { return m_bPushed; }
	bool OnTouches(const vec2 &tp);

	GLuint GetShaderHandle() { return m_uiProgram; }

	void Draw();
	void DrawW();
};




#endif