#define fft2(a,b) {float2 tmp = a; a+=b; b=tmp-b;} //+ und - Berechnung der FFT

//Multiplikation einer Imaginären Zahl duchführen
inline float2 cmultexp(float2 x,float phi)
{      
	float si,cs;
	float ReTeil;
	float ImTeil;
    si=sincos(phi,&cs);
	ReTeil = x.x*cs - x.y*si;
	ImTeil = x.y*cs + x.x*si;
    return (float2) (ReTeil,ImTeil);
}


// Phi berechen, das ist abhängig von dem Block und von der Position
float2 mulw(float2 x, int relativ, const int step)
{
	float phi = 0;
	phi = (((float)-2 * M_PI_F) * relativ ) / step;
	return cmultexp(x,phi);
}


// Position Berechnen, welcher Block gemeint ist. Von oben an gezählt.
// Und noch mal die Relative Position in dem Block berechnen.
int getIndex(int id, int step, int *relativ)
{
	*relativ = id & (step-1);
	int abs = (id - *relativ) * 2;
	return abs;
} 

__kernel
void fft(unsigned int elements, __global float2 * A, __global float2 * B)
{
    //Position der Daten eigentlich der Work-Item weil 1 Dimensional
    const idx =  get_global_id(0); 

	int this;
	int next;
	int step;
	int absolut;
	int relativ;

	this = ((idx) * 2); //Sprungweite immer 2
	next = this +1;

	B[this] = A[this];
	B[next] = A[next];
	
	fft2(B[this],B[next]); //Berechung von + und -
    barrier(CLK_GLOBAL_MEM_FENCE);
    
    //block immer verdoppeln => Sprungweite immer um 2 erhöht
	for(step = 2; (step * 2) <= (elements); step *= 2)
	{
        //Bestimmung der x-Werte
		absolut = getIndex(idx, step, &relativ);
		this = absolut+relativ;
		next = this + step;

        //Berechnung des W-Multiplikators
		B[next] = mulw(B[next],relativ,(step*2));

		barrier(CLK_GLOBAL_MEM_FENCE);
        //Berechung
		fft2(B[this],B[next]);
        //Warte bis alle Work-Items mit der Berechung fertig sind
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
};
