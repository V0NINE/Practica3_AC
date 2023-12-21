#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define NN 512000000
#define MAX_INT ((int)((unsigned int)(-1)>>1))

int valors[NN+1];
int valors2[NN+1];

void qs(int *val, int ne)
{
    int i,f,j;
    int pivot,vtmp,vfi;
    pivot = val[0];
    i = 1;
    f = ne-1;
    vtmp = val[i];

    while (i <= f)
    {
        if (vtmp < pivot) 
        {
            val[i-1] = vtmp;
            i ++;
            vtmp = val[i];
        }
        else {
            vfi = val[f];
            val[f] = vtmp;
            f --;
            vtmp = vfi;
        }
    }

    val[i-1] = pivot;

    if (f>1) qs(val,f);
    if (i < ne-1) qs(&val[i],ne-f-1);
}

void merge2(int* val, int n,int *vo)
{
    int vtmp;
    int i,j,posi,posj;
    posi = 0;
    posj = (n/2);
    
    for (i=0;i<n;i++)
        if (((posi < n/2) && (val[posi] <= val[posj])) || (posj >= n))
            vo[i] = val[posi++];
        else if (posj < n)
            vo[i] = val[posj++];
}

int main(int nargs,char* args[])
{
    int ndades,i,m,parts;
    int *vin,*vout,*vtmp;
    long long sum=0;

    assert(nargs == 3);

    ndades = atoi(args[1]);
    assert(ndades <= NN);

    parts = atoi(args[2]);

    if (parts < 2) assert("Han d'haver dues parts com a minim" == 0);
    if (ndades % parts) assert("N ha de ser divisible per parts" == 0);

    for(i=0;i<ndades;i++) valors[i]=rand()%MAX_INT;

    // quicksort original, per fer PROVES
    // qs(valors,ndades);
    // vin=valors;
    // Quicksort a parts, Cada part l'hauria de fer un thread !!
    for (i=0;i<parts;i++)
    {
        // printf("de %d a %d\n",i*ndades/parts,(i+1)*(ndades/parts));
        qs(&valors[i*(ndades/parts)],ndades/parts);
    }

    // Merge de dos vectors. 1 de cada 2 threads uneix els dos vectors
    // A cada volta nomes treballen la meitat dels threads fins arribar
    // al thread principal
    vin = valors;
    vout = valors2;
    for (m = 2*(ndades/parts); m <= ndades; m *= 2)
    {
        for (i = 0; i < ndades; i += m)
            merge2(&vin[i],m,&vout[i]);

        vtmp=vin;
        vin=vout;
        vout=vtmp;
    }

    /* EXEMPLE ASSIGNACIO FEINA a 4 THREADS
    qs(&valors[0],ndades/4); //per a TH0
    qs(&valors[ndades/4],ndades/4); //per a TH1
    qs(&valors[ndades/2],ndades/4); //per a TH2
    qs(&valors[3*ndades/4],ndades/4); //per a TH3
    // sincro barrera
    merge2(&valors[0],ndades/2,&valors2[0]); //per a TH0
    merge2(&valors[ndades/2],ndades/2,&valors2[ndades/2]); //per a TH2

    /sincro barrera

    merge2(valors2,N,valors); //per a TH0
    vin=valors;
    */
    for (i=1;i<ndades;i++) assert(vin[i-1]<=vin[i]);

    for (i=0;i<ndades;i+=100)
        sum += vin[i];
    
    printf("validacio %lld \n",sum);
}