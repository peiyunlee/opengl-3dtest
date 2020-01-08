#include "Room.h"

Room::Room() {
	g_bAutoRotating = false;

	g_fElapsedTime = 0;
	g_fLightRadius = 5;
	g_fLightTheta = 0;
}

Room::~Room() {
	if (g_Light != NULL) delete[] g_Light;
	if (g_pLight != NULL) delete[] g_pLight;
	if (g_LightLine != NULL) delete[] g_LightLine;
}