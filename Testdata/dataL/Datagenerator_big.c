#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

//using namespace std;

#define MIN_NUM_AREA 500
#define HYPER_RATIO 90
#define MIN_AREA_SIZE 5000
#define MAX_AREA_SIZE 10000

#define NUM_PROCESS 100

int start[1100],run[1100];
char str[110];
char ch;
int tmp;

int main()
{
    srand(20130303);
    freopen("sche_traces_big.txt","w",stdout);
    int i, j;
    for(i=1;i<=NUM_PROCESS;i++)
    {
        start[i]=(int)((rand()%2999+1)*1.0/23*100);
        run[i]=(int)((rand()%999+1)*1.0/379*100);
    }
    for(i=1;i<=NUM_PROCESS-1;i++)
        for(j=i+1;j<=NUM_PROCESS;j++)
        {
            if(start[i]>start[j]) 
            {
                tmp=start[i];
                start[i]=start[j];
                start[j]=tmp;
                tmp=run[i];
                run[i]=run[j];
                run[j]=tmp;
            }
        }
    for(i=1;i<=NUM_PROCESS;i++)
    {
        printf("P%d\t%.2lf\t%.2lf\t%d\n",i,start[i]*1.0/100,run[i]*1.0/100,rand()%10000);
    }
    fclose(stdout);
    
    srand(time(NULL)); 	   
    int num_mem;
    for(i=1;i<=NUM_PROCESS;i++)
    {
        sprintf(str,"P%d.mem",i);
        freopen(str,"w",stdout);
	
	num_mem = run[i]*1000;
	int num_area, area_size;
	area_size = MIN_AREA_SIZE + rand()%(MAX_AREA_SIZE - MIN_AREA_SIZE);
	num_area = num_mem / area_size;// while ((num_area=rand()%1000) < MIN_NUM_AREA);

	for (j = 0; j < num_area; ++j) {
	    int base_add = rand()%50;
	    int size = (rand()%49) + 1;
	    int k;
	    for (k = j*area_size; k < (j+1)*area_size; ++k) {
		int tmp = rand()%100;
		if (tmp < HYPER_RATIO) printf("0x%x\n", base_add+rand()%size);     //in the focusing address
		else printf("0x%x\n", rand()%100);
	    }
	}
	int base_add = rand()%50;
	int size = (rand()%49) + 1;
	int k;
	for (k = num_area*area_size; k < num_mem; ++k) {
	    int tmp = rand()%100;
	    if (tmp < HYPER_RATIO) printf("0x%x\n", base_add+rand()%size);     //in the focusing address
	    else printf("0x%x\n", rand()%100);
	}
	/*
        for(int j=1;j<=run[i]*1000;j++)
        {
            printf("0x%x\n",rand()%100);
        }
	*/
        fclose(stdout);
    }
    return 0;
}

