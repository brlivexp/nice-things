stock IsPlayerBetweenTwoPoints(playerid, Float:nx, Float:ny, Float: nz, Float:px, Float:py, Float:pz, Float: offset = 1.5) {

	//if(floatround(nz) != floatround(pz))
	//      return  false;

	static Float: playerPos[3];

	if(!GetPlayerPos(playerid, playerPos[0],playerPos[1],playerPos[2])) return false;

	static Float: diffX;

	if(nx > px) {
		diffX = nx - px;
	}
	else {
		diffX = px - nx;
	}

	new Float: diffY;

	if(ny > py) {
		diffY = ny - py;
	}
	else {
		diffY = py - ny;
	}

	static Float: diffXY;

	if(diffX > diffY) {
		diffXY = (diffX / diffY) * offset ;
	}
	else {
		diffXY = (diffY / diffX) * offset ;
	}

	if((nx < px)){
		while(nx < px) {
			nx += (diffXY);
			if(ny > py) {
				ny -= offset  ;
			}
			else {
				ny += offset  ;
			}

			MapAndreas_FindZ_For2DCoord(nx, ny, nz);

			if(floatsqroot(((playerPos[0]-nx)*(playerPos[0]-nx))+((playerPos[1]-ny)*(playerPos[1]-ny))+((playerPos[2]-nz)*(playerPos[2]-nz))) < 1.0) {
				return true;
			}
			pz = nz;
		}
	}
	else {
		while(nx > px) {
			nx -= (diffXY);
			if(ny > py) {
				ny -= offset ;
			}
			else {
				ny += offset ;
			}
			MapAndreas_FindZ_For2DCoord(nx, ny, nz);

			if(floatsqroot(((playerPos[0]-nx)*(playerPos[0]-nx))+((playerPos[1]-ny)*(playerPos[1]-ny))+((playerPos[2]-nz)*(playerPos[2]-nz))) < 1.0) {
				return true;
			}

			pz = nz;
		}
	}

	return false;
}

IsPlayerBetweenTwoPoints(playerid, Float:x1,Float:y1, Float:x2, Float:y2, Float: maxDiff = 5.0) {

	static Float: x0, Float: y0, Float: z0;

	if(!GetPlayerPos(playerid, x0, y0, z0)) {
		return false;
	}

	if(x1 > x2) {
	    if(x0 > x1 || x0 < x2) return false;
	}
	else {
	    if(x0 < x1 || x0 > x2) return false;
	}


	if(y1 > y2) {
	    if(y0 > y1 || y0 < y2) return false;
	}
	else {
	    if(y0 < y1 || y0 > y2) return false;
	}

 	return floatabs(atan2(y2-y1,x2-x1) - atan2(y2-y0,x2-x0)) < maxDiff;
}
