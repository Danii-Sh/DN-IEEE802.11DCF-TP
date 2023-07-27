#include <stdio.h>
#include <math.h>


void main()
{


float p;
float t;
float Ptr,Ps;
int Tc,Ts;
int n;
char buf[3];



printf("Choose Number of Nodes; From 1 to 99 \n");

buf[0]=buf[1]=0;

fgets(buf, 3, stdin);



if(buf[1]==10)
	{n= (buf[0]-'0');
	}

else
	{n=(buf[0]-'0')*10;
	n=n+buf[1]-'0';
	}

printf("Number of Nodes are %d \n", n);



int counter=0;


float a=0;
while(counter != 1000000)
{

t=a;

p=1-(pow((1-t),(n-1)));

//t=(2/(3+(3*p*(1+(2*p)+(2*p*2*p)+(2*p*2*p*2*p)+(2*p*2*p*2*p*2*p)))));
//t=((2*(1-(2*p)))/((3*(1-(2*p)))+(3*p)(1-pow((2*p),(5)))));

t=((2)*(1-(2*p)))/((3*(1-(2*p)))+(2*p*(1-(pow((2*p),(5))))));

if(t-a<0.00001)
{if(a-t<0.00001)
{
printf("Numerical Result Found \n");
goto L;}}

if(a-t<0.00001)
{if(t-a<0.00001)
{
printf("Numerical Result Found \n");
goto L;}}


a=a+0.000001;
counter++;
}
L:
printf("Numerically Found p is %f \n", p);
printf("Numerically Found t is %f \n", t);


Ptr=1-pow((1-t),(n));
Ps=(n*t*(pow((1-t),(n-1)))) / Ptr;

float S;

Tc=10+2;   		///   Packet + DIFS
Ts=10+1+1+2;    ///   Packet + SIFS + ACK + DIFS 
S=(Ps*Ptr*1400)/((Ps*Ptr*Ts)+(Ptr*Tc*(1-Ps)));

printf("Basic mode ThroughPut is %f Bytes\n", S);



Tc=1+2;   		///   RTS + DIFS
Ts=1+1+1+1+10+1+1+2;    ///   RTS + SIFS + CTS + SIFS + Packet + SIFS + ACK + DIFS 
S=(Ps*Ptr*1800)/((Ps*Ptr*Ts)+(Ptr*Tc*(1-Ps)));

printf("RTS/CTS mode ThroughPut is %f Bytes\n", S);



}








