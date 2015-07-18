// =============================================================================

Float:AngleBetweenPoints(Float:XA, Float:YA,Float:XB, Float:YB)
{
    new Float: angulo = atan2(YA - YB, XA - XB);

	if(angulo < 000.0) angulo += 360.0;
	if(angulo > 360.0) angulo -= 360.0;
		
	return angulo;
}

// =============================================================================

GetPosFromView(playerid, Float:distance, &Float:x, &Float:y, &Float:z)
{
  static Float:cx,Float:cy,Float:cz,Float:fx,Float:fy,Float:fz;
  GetPlayerCameraPos(playerid, cx, cy, cz);
  GetPlayerCameraFrontVector(playerid, fx, fy, fz);
  x = fx * distance + cx;
  y = fy * distance + cy;
  z = fz * distance + cz;
}

// =============================================================================


#if !defined MATH_PI
#define MATH_PI 3.14159265
#endif

#if !defined FULL_ANGLE
#define FULL_ANGLE 360.0
#endif

#if !defined PIT
#define PIT 7.5
#endif


public OnPlayerUpdate(playerid) {


	static
		Float:X_OBJETO,
		Float:Y_OBJETO,
		Float:Z_OBJETO;
		

	static
		Float:xPos,
		Float:yPos,
		Float:zPos,
		Float:distancia[2];

	GetPlayerPos(playerid, xPos, yPos, zPos);
	GetVehiclePos(carcriado, X_OBJETO, Y_OBJETO, Z_OBJETO);

	distancia[0] = GetDistance(X_OBJETO, Y_OBJETO, Z_OBJETO, xPos, yPos, zPos);

	if(distancia[0] < 100.0) {

		GetPosFromView (playerid, distancia[0], xPos, yPos,zPos);
		
        distancia[1] = GetDistance(X_OBJETO, Y_OBJETO, Z_OBJETO, xPos, yPos, zPos);

		if(distancia[0] < distancia[1]) return false;


			new Float: angulo[2];
			new Float: raio; // preciso arrumar o raio e dar um jeito de relacionar ele com a distancia do jogador
			
			// distancia[0] entre player e carro
			// distancia[1] entre lugar onde ele olha e carro

			
            angulo[1] = AngleBetweenPoints( xPos, yPos, X_OBJETO, Y_OBJETO) ;

			// angulo[1] angulo em que ele olha e carro
			// angulo[0] angulo em radianos em que ele olha e carro
			// raio é o raio que indica a distancia entre o centro da tela e o carro. Quando maior o raio mais distante o carro está do centro da tela
			
			
			if(angulo[1] > 90.0) { // aqui FUNCIONA perfeitamente, detecta se o jogador olha para direita do carro ou para esquerda
			    SendClientMessage(playerid, -1, "Direita do Carro");
			}
			else {
			    SendClientMessage(playerid, -1, "Esquerda do Carro");
			}

			angulo[0] = (angulo[1] * MATH_PI) / 180.0; // converter para radianos

			new Float: px = 320 + floatcos(angulo[0] , radian) * (distancia[0]*distancia[1]) ; // pixel x
			new Float: py = 240 + floatsin(angulo[0] , radian) * (distancia[0]*distancia[1]) ; // pixel y

				new
				str[128]
			;

			format( str, 128, "ANGULO: %f metros | RADIANOS: %f angulo | DISTANCIA DE VISÃO: %f",  angulo[1], angulo[0] , distancia[1]);
			SendClientMessage(playerid, -1, str);

	}

	return true;
}



GetDistance(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2)
{
    return floatround(floatsqroot(floatpower(floatabs(floatsub(x2,x1)), 2) + floatpower(floatabs(floatsub(y2,y1)),2) + floatpower(floatabs(floatsub(z2,z1)) ,2)));
}