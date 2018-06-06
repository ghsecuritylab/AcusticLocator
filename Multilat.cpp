#include "Multilat.h"


Dht11 sensor(D2);

Multilat::Multilat()
{
	
}

float Multilat::calculateSpeedOfSound(void){
    sensor.read();
    int a = sensor.getCelsius();
	printf("Temp = %i �C\n", a);
    return 331.45 * sqrt((float(a) + 273.15)/273.15);
}

Pos Multilat::GetPosition(Signal t)
{
	float A[DIMENSIONS][DIMENSIONS];
	float b[DIMENSIONS];
	unsigned i = 0;
	unsigned c = 0;
	float v = calculateSpeedOfSound();	//SPEED_OF_SOUND; //	m/s
	printf("v = %i m/s\n", (int) v);
	unsigned line = 0;
	
	
	float min = 10000;
	for(unsigned i = 0; i < ADC_LENGTH; i++) //find most negative delay
		if(t[i] < min)
		{
			min = t[i];
			c = i;
		}
	printf("c = %i\n", c);
	for(unsigned i = 0; i < ADC_LENGTH; i++) //offset times to only have positive delays
		t[i] += min;
	
	
	for(unsigned i = 0; i < ADC_LENGTH; i++)
		t[i] *= SAMPLE_TIME;
		
	printf("min = %dE-6\n", int(min *1000000));
	i = c == 0 ? 1 : 0; //first but not equal to c
	for(unsigned j = 1; j < 5; j++)
	{
		if(j == c || j == i) continue;
		
		for(unsigned k = 0; k < DIMENSIONS; k++)
		{
			A[line][k] = 2*(v*(t[j]-t[c])*(p[k][i]-p[k][c]) - 
						 v*(t[i]-t[c])*(p[k][j]-p[k][c]));
		
		}
		b[line]	= v*(t[i]-t[c])*(v*v*(t[j]-t[c])*(t[j]-t[c])-
									(p[0][j]*p[0][j]+
									p[1][j]*p[1][j]+
									p[2][j]*p[2][j])) +
					(v*(t[i]-t[c])-v*(t[j]-t[c]))*
									(p[0][c]*p[0][c]+
									p[1][c]*p[1][c]+
									p[2][c]*p[2][c]) +
					v*(t[j]-t[c])*
						
									((p[0][i]*p[0][i]+
									p[1][i]*p[1][i]+
									p[2][i]*p[2][i]) -
					v*v*(t[i]-t[c])*(t[i]-t[c]));
										
										  
		line++;
	}
	
	for(unsigned i = 0; i < DIMENSIONS; i++)
		for(unsigned j = 0; j < DIMENSIONS; j++)
			printf("A[%d][%d] = %dE-6\n", i, j, int(A[i][j]*1000000));

	for(unsigned j = 0; j < DIMENSIONS; j++)
		printf("b[%d] = %dE-6\n", j, int(b[j]*1000000));
		
	float det = A[0][0] * (A[1][1] * A[2][2] - A[2][1] * A[1][2]) -
	 A[0][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0]) +
	 A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);

	float invdet = 1 / det;

	printf("det = %i\n", int(det));
	float Ainv[DIMENSIONS][DIMENSIONS]; // inverse of matrix A
	Ainv[0][0] = (A[1][1] * A[2][2] - A[2][1] * A[1][2]) * invdet;
	Ainv[0][1] = (A[0][2] * A[2][1] - A[0][1] * A[2][2]) * invdet;
	Ainv[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) * invdet;
	Ainv[1][0] = (A[1][2] * A[2][0] - A[1][0] * A[2][2]) * invdet;
	Ainv[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]) * invdet;
	Ainv[1][2] = (A[1][0] * A[0][2] - A[0][0] * A[1][2]) * invdet;
	Ainv[2][0] = (A[1][0] * A[2][1] - A[2][0] * A[1][1]) * invdet;
	Ainv[2][1] = (A[2][0] * A[0][1] - A[0][0] * A[2][1]) * invdet;
	Ainv[2][2] = (A[0][0] * A[1][1] - A[1][0] * A[0][1]) * invdet;
	
	for(unsigned i = 0; i < DIMENSIONS; i++)
		for(unsigned j = 0; j < DIMENSIONS; j++)
			printf("Ainv[%d][%d] = %d\n", i, j, int(Ainv[i][j]));
	Pos X;
	
	//X = Ainv*B;
	for(unsigned i = 0; i < DIMENSIONS; i++)
	{
		X[i] = Ainv[i][0]*b[0] + Ainv[i][1]*b[1] + Ainv[i][2]*b[2];
	}
		
	for(unsigned j = 0; j < DIMENSIONS; j++)
		printf("X[%d] = %d mm\n", j, int(1000*X[j]));

	return X;
}

Multilat::~Multilat()
{

}