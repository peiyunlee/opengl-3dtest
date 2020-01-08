// Phong reflection model
#version 130

#define PointNum 4

in vec4 vPosition;	  // Vertex Position
in vec3 vNormal;    // Vertex Normal
in vec4 vVtxColor;  // Vertex Color

out vec3 fN;// ��X Normal �b���Y�y�ФU����V
out vec3 fL[PointNum];// ��X Light Direction �b���Y�y�ФU����V

out vec3 fV;// ��X View Direction �b���Y�y�ФU����V
out vec3 lD[PointNum];

uniform mat4  ModelView;   // Model View Matrix
uniform mat4  Projection;  // Projection Matrix
uniform vec4  LightInView[PointNum]; // Light's position in View Space

uniform vec4  SpotTarget[PointNum];      //Spot���ӥؼЦ�m

void main()
{
	vec4 vPosInView = ModelView * vPosition;
	// �ثe�w�g�H���Y�y�Ь���¦, �ҥH View ����m�N�O (0,0,0), �ҥH��m���I���N�O View Dir
	fV = -vPosInView.xyz;

	fN = (ModelView * vec4(vNormal, 0.0)).xyz;
	
	for(int i = 0;i < PointNum; i++){
		fL[i] = vec3(LightInView[i].xyz - vPosInView.xyz);
		lD[i]  = vec3(SpotTarget[i].xyz - LightInView[i].xyz);
	}

	gl_Position = Projection * vPosInView;
}
