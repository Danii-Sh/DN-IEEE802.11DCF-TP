#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


void main()
{
float p,t;
float Ptr,Ps;

unsigned long SystemClock=0;
int NodeNumber=0;											/// Overall System State Counters
        ///  |->  {backoff Counter , Window Size , Succesive Collisions ,DIFS ,transmit Remaining Time ,  Collision Counter , Transmit Counter}
int Nodes[100][7];
          //
	  //Inseret User Chosen Node Numbers to n
	  //
int ChannelOccupy[2];  ///  [0] : yes or no     [1] Node Occupying
int TXattempt[2]; /// [0]: yes or no     [1]Node
int n;
char buf[3];

int debug=0;

int Ccounter=0;;

printf("Choose Number of Nodes; 2,4,6,8,10 \n");

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


srand(time(NULL));

/// Parameter Initialize

while(NodeNumber!=n)
{
Nodes[NodeNumber][6]=0;
Nodes[NodeNumber][5]=0;
Nodes[NodeNumber][4]=0;
Nodes[NodeNumber][3]=0;
Nodes[NodeNumber][2]=0;
Nodes[NodeNumber][0]=0;
Nodes[NodeNumber][0]=0;
NodeNumber++;
//
}
ChannelOccupy[0]=0;
ChannelOccupy[1]=-1;
TXattempt[0]=0;
TXattempt[1]=-1;

NodeNumber=0;
//



clock_t u;
    u = clock();









while(SystemClock != 4000000)

{
while(NodeNumber != n)
{

if(NodeNumber==ChannelOccupy[1])
{
if(Nodes[NodeNumber][4]==0)
{
ChannelOccupy[0]=0;
ChannelOccupy[1]=-1;

double h=(pow((2),(Nodes[NodeNumber][2])));
int t;
t=h;

Nodes[NodeNumber][0]=(rand()%(t));


Nodes[NodeNumber][2]=0;
Nodes[NodeNumber][6]++;
}
else
{Nodes[NodeNumber][4]--;}
}

else
{
if(ChannelOccupy[0]==0)
{
if(Nodes[NodeNumber][0]==0)
{if(Nodes[NodeNumber][3]==1){
TXattempt[0]++;
TXattempt[1]=NodeNumber;
Nodes[NodeNumber][3]=0;
Nodes[NodeNumber][4]=15;
}else{
Nodes[NodeNumber][3]=1;}}
else
{

Nodes[NodeNumber][0]--;

}}}
NodeNumber++;
}

if (TXattempt[0]>1)
{


TXattempt[1]=-1;
TXattempt[0]=0;
NodeNumber=0;
while(NodeNumber!=n)
{

if(Nodes[NodeNumber][0]==0)
{if(Nodes[NodeNumber][3]==0){
Nodes[NodeNumber][3]=1;
if(Nodes[NodeNumber][4]==15)
{Nodes[NodeNumber][4]==0;
if(Nodes[NodeNumber][2]!=6)
{Nodes[NodeNumber][2]++;}
double h=(pow((2),(Nodes[NodeNumber][2])));
int t;
t=h;
Nodes[NodeNumber][0]=(rand()%(t));

Nodes[NodeNumber][5]++;
}}}
NodeNumber++;
//}
}Ccounter++;
ChannelOccupy[0]=0;
ChannelOccupy[1]=-1;
}
if(TXattempt[0]==1)
{
ChannelOccupy[0]=1;
ChannelOccupy[1]=TXattempt[1];
}

TXattempt[0]=0;
TXattempt[1]=-1;
//
NodeNumber=0;
SystemClock++;
}














u = clock() - u;
double time_taken = ((double)u)/CLOCKS_PER_SEC; // in seconds
printf("Simulation Of IEEE 802.11 DCF took %f seconds to run \n", time_taken);

long double TXnum=0;
while(NodeNumber!=n)
{
TXnum=(Nodes[NodeNumber][6])+TXnum;
NodeNumber++;
}

printf(" %.0Lf all TX times \n", TXnum);


printf(" %d Collisions \n", Ccounter);

float S=(4000000-TXnum-Ccounter)/4000000;

printf("Simulation Throughput is %f \n", S);


}



