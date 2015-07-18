MapAndreasFlatGround(Float:x, Float:y, Float:z, Float: size = 2.0) { //by ipsBruno

	static Float: xx[7], Float:yy[7];

	//

	xx[0] = x;
	yy[0] = y;

	xx[1] = x+size;
	yy[1] = y;

	xx[2] = x+size;
	yy[2] = y+size;

	xx[3] = x;
	yy[3] = y+size;

	//

	xx[4] = x-size;
	yy[4] = y;

	xx[5] = x-size;
	yy[5] = y-size;

	xx[6] = x;
	yy[6] = y-size;


	return
		DiagonalRoute(x, y, z, xx[1],yy[1],z, .maxSteps = 15.0) &&
		DiagonalRoute(x, y, z, xx[2],yy[2],z, .maxSteps = 15.0) &&
		DiagonalRoute(x, y, z, xx[3],yy[3],z, .maxSteps = 15.0) &&
		DiagonalRoute(x, y, z, xx[4],yy[4],z, .maxSteps = 15.0) &&
		DiagonalRoute(x, y, z, xx[5],yy[5],z, .maxSteps = 15.0) &&
		DiagonalRoute(x, y, z, xx[6],yy[6],z, .maxSteps = 15.0)
	;
}
