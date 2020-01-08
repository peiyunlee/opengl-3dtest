//#define NPR
//#define SILHOUETTE
#version 130


#define PointNum 4


in vec3 fN;
in vec3 fL[PointNum];

in vec3 fV;
in vec3 lD[PointNum];

// 以下為新增的部分
uniform vec4  AmbientProduct[PointNum];  // light's ambient  與 Object's ambient  與 ka 的乘積

uniform vec4  DiffuseProduct[PointNum];  // light's diffuse  與 Object's diffuse  與 kd 的乘積

uniform vec4  SpecularProduct[PointNum]; // light's specular 與 Object's specular 與 ks 的乘積

uniform float spotCosCutoff[PointNum];
uniform int lightType[PointNum];
uniform float fShininess;
uniform int   iLighting[PointNum];
uniform vec4  vObjectColor;    // 代表物件的單一顏色


void main()
{
	// 先宣告 diffuse 與 specular
	vec4 ambient = vec4(0.0,0.0,0.0,1.0);
    vec4 diffuse = vec4(0.0,0.0,0.0,1.0);
	vec4 specular = vec4(0.0,0.0,0.0,1.0);
	vec3 vN;
	float fLdotLDir= 0.0f;
	vec3 lDN[PointNum];
	vec3 vL[PointNum];
	float fLdotN = 0.0f;
	vec3 vV = vec3(0.0,0.0,0.0);
	vec3 vRefL = vec3(0.0,0.0,0.0);
	float RdotV= 0.0f;

	vec4 result = vec4(0.0,0.0,0.0,1.0);
	//vec4 fLightI = vec4(0.0,0.0,0.0,1.0);
	//vec3 vLight; // 用 vec3 來宣告是為了節省計算, 如果要讓程式寫起來更方便，可改用 vec4 來宣告
	

	// 單位化傳入的 Normal Dir
	vN = normalize(fN);
	vV = normalize(fV);

	for(int i = 0 ;i < PointNum;i++){
		
		if( iLighting[i] != 1 ) {
			//gl_FragColor = vObjectColor;
			result += vec4(0.0,0.0,0.0,1.0);
		}
		else {	
			// 1. 計算 Ambient color : Ia = AmbientProduct = Ka * Material.ambient * La = 
			ambient = AmbientProduct[i]; // m_sMaterial.ka * m_sMaterial.ambient * vLightI;
			
			// 2. 單位化傳入的 Light Dir
			vL[i] = normalize(fL[i]); // normalize light vector

			// 5. 計算 L dot N
			fLdotN = vL[i].x*vN.x + vL[i].y*vN.y + vL[i].z*vN.z;

			if(lightType[i] == 0){	//環境

				if( fLdotN >= spotCosCutoff[i] ){
					#ifndef NPR
						// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
						diffuse = fLdotN * DiffuseProduct[i];
					#else
						if( fLdotN >= 0.85 ) diffuse = 1.0 * DiffuseProduct[i];
						else if( fLdotN >= 0.55 ) diffuse = 0.55 * DiffuseProduct[i]; 
						else diffuse = 0.35 * DiffuseProduct[i];
					#endif


						//計算 Light 的 反射角 vRefL = 2 * fLdotN * vN - L
						// 同時利用 normalize 轉成單位向量
						vRefL = normalize(2.0f * (fLdotN) * vN - vL[i]);

						//   計算  vReflectedL dot View
						RdotV = vRefL.x*vV.x + vRefL.y*vV.y + vRefL.z*vV.z;
				
						#ifndef NPR
							// Specular Color : Is = Ks * Material.specular * Ls * (R dot V)^Shininess;
							if( RdotV > 0 ) specular = SpecularProduct[i] * pow(RdotV, fShininess);
						#else
							specular = vec4(0.0,0.0,0.0,1.0); 
						#endif
				}
				else{
					diffuse =  fLdotN * DiffuseProduct[i]; 
				}
			}
			else if(lightType[i] == 1){
				lDN[i] = normalize(lD[i]);
				fLdotLDir = -(vL[i].x*lDN[i].x + vL[i].y*lDN[i].y + vL[i].z*lDN[i].z);
				
				if( fLdotLDir >=  0.8 ) { // 該點被光源照到才需要計算

						fLdotN = vL[i].x*vN.x + vL[i].y*vN.y + vL[i].z*vN.z;
					#ifndef NPR
							// Diffuse Color : Id = Kd * Material.diffuse * Ld * (L dot N)
							diffuse = fLdotN * DiffuseProduct[i] ; 
					#else
							if( fLdotN >= 0.85 ) diffuse = 1.0 * DiffuseProduct[i] * powf(fLdotLDir, 1.0f);
							else if( fLdotN >= 0.55 ) diffuse = 0.55 * DiffuseProduct[i] * powf(fLdotLDir, 1.0f);
							else diffuse = 0.35 * DiffuseProduct[i] * powf(fLdotLDir, 1.0f);
					#endif

							//計算 Light 的 反射角 vRefL = 2 * fLdotN * vN - L
							// 同時利用 normalize 轉成單位向量
							vRefL = normalize(2.0f * (fLdotN) * vN - vL[i]);

							//   計算  vReflectedL dot View
							RdotV = vRefL.x*vV.x + vRefL.y*vV.y + vRefL.z*vV.z;

					#ifndef NPR
							// Specular Color : Is = Ks * Material.specular * Ls * (R dot V)^Shininess;
							if( RdotV > 0 ) specular = SpecularProduct[i] * pow(RdotV, fShininess);
					#else
							specular = vec4(0.0,0.0,0.0,1.0); 
					#endif
				}
			}
			result += ambient + diffuse + specular;
		}
	}
	gl_FragColor = result;
	gl_FragColor.w = 1.0;	// 設定 alpha 為 1.0
	// gl_FragColor = vec4((ambient + diffuse + specular).xyz, 1.0);
	
	#ifdef SILHOUETTE
		vec4 edgeColor = vec4(1.0, 0.0, 0.0, 1.0);
		if( abs(dot(normalize(fN), normalize(fV))) < 0.2)  gl_FragColor = edgeColor;
	#endif
}
