#include "ModelPool.h"
#include "../ObjLoader.h"

ModelPool::ModelPool(const char* path, char type) {
	std::vector<float> vertices, normals;
	
	LoadObj(path, &vertices, &normals, type);

	//Print(vertices.size() / 3);
	m_iNumVtx = vertices.size() / 3;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex = NULL;

	m_pPoints = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors = new vec4[m_iNumVtx];
	m_pTex = new vec2[m_iNumVtx];

	for (int i = 0; i < m_iNumVtx; i++)
	{
		m_pPoints[i] = vec4(vertices[i * 3], vertices[ i * 3 + 1 ], vertices[i * 3 + 2], 1.0f);
		m_pNormals[i] = vec3(normals[i * 3], normals[i * 3 +1], normals[i * 3 +2]);
		m_pColors[i] = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	}
	//Print(m_pNormals);
	SetShaderName("vsPerPixelLighting.glsl", "fsPerPixelLighting.glsl");

	SetMaterials(vec4(0), vec4(1.0f, 1.0, 1.0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	SetShader();
	SetShadingMode(GOURAUD_SHADING);
}

void ModelPool::Draw()
{
	DrawingSetShader();
	glDrawArrays(GL_TRIANGLES, 0, m_iNumVtx);
}

void ModelPool::DrawW()
{
	DrawingWithoutSetShader();
	glDrawArrays(GL_TRIANGLES, 0, m_iNumVtx);
}



// ���B�ҵ��� vLightPos �����O�@�ɮy�Ъ��T�w�����m
void ModelPool::Update(float dt, point4 vLightPos, color4 vLightI)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxMV3X3Final = mat3(	// �u���e���� 3X3 �x�}�����e
			m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal);
#endif
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	if (m_iMode == FLAT_SHADING) RenderWithFlatShading(vLightPos, vLightI);
	else RenderWithGouraudShading(vLightPos, vLightI);

#else // Lighting With GPU
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	for (int i = 0; i < 2; i++)
	{
		m_vLightInView[i] = m_mxView * vLightPos;		// �N Light �ഫ�����Y�y�ЦA�ǤJ

		m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * vLightI;
		m_SpecularProduct[i] = m_Material.ks * m_Material.specular * vLightI;
		m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * vLightI;
	}

	// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e


#endif

}

void ModelPool::Update(float dt, const LightSource &Lights)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxMV3X3Final = mat3(
			m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal);
#endif
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	if (m_iMode == FLAT_SHADING) RenderWithFlatShading(Lights);
	else RenderWithGouraudShading(Lights);

#else // Lighting With GPU
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	for (int i = 0; i < 2; i++)
	{
		m_vLightInView[i] = m_mxView * Lights.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ

		m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * Lights.diffuse;
		m_SpecularProduct[i] = m_Material.ks * m_Material.specular * Lights.specular;
		m_spotCosCutoff[i] = Lights.spotCosCutoff;
		m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * Lights.ambient;
	}
	// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e


#endif

}

void ModelPool::Update(const LightSource *Lights, float dt)
{
	//if (m_bViewUpdated || m_bTRSUpdated) {
	//	m_mxMVFinal = m_mxView * m_mxTRS;
	//	m_mxMV3X3Final = mat3(	// �u���e���� 3X3 �x�}�����e
	//		m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
	//		m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
	//		m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
	//	m_bViewUpdated = m_bTRSUpdated = false;
	//}



	//for (int i = 0; i < LIGHTCOUNT; i++)
	//{
	//	m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * Lights[i].ambient;
	//	lightType[i] = Lights[i].type;
	//	m_LightDir[i] = m_mxMV3X3Final * Lights[i].spotDirection;
	//	m_SpotExponent[i] = Lights[i].spotExponent;
	//	m_vLightInView[i] = m_mxView * Lights[i].position;
	//	m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * Lights[i].diffuse;
	//	m_Diffuse[i] = Lights[i].diffuse;
	//	m_SpecularProduct[i] = m_Material.ks * m_Material.specular * Lights[i].specular;
	//	m_spotCosCutoff[i] = Lights[i].spotCosCutoff;
	//	m_iLighting[i] = Lights[i].isLighting;
	//}
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}

	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		lightType[i] = Lights[i].type;
		m_vLightInView[i] = m_mxView * Lights[i].position;		// �N Light �ഫ�����Y�y�ЦA��
		m_vSpotTarget[i] = m_mxView * Lights[i].spotTarget;
		// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
		m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * Lights[i].ambient;
		m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * Lights[i].diffuse;
		m_SpecularProduct[i] = m_Material.ks * m_Material.specular * Lights[i].specular;
		m_iLighting[i] = Lights[i].isLighting;
	}

}

void ModelPool::Update(float dt)
{
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxITView = InverseTransposeMatrix(m_mxMVFinal);
		m_bViewUpdated = m_bTRSUpdated = false;
	}
}