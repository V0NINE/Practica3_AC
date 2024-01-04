#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#define NN 512000000  
#define MAX_INT ((int)((unsigned int)(-1)>>1))

int valors[NN+1];
int valors2[NN+1];

typedef struct {
    int *val;
    int ne;
}qs_args;

void *qs(void *arg)
{
    qs_args *args = (qs_args *)arg;

    int i,f,j;
    int pivot,vtmp,vfi;
    pivot = args->val[0];
    i = 1;
    f = args->ne-1;
    vtmp = args->val[i];

    while (i <= f)
    {
        if (vtmp < pivot) 
        {
            args->val[i-1] = vtmp;
            i ++;
            vtmp = args->val[i];
        }
        else {
            vfi = args->val[f];
            args->val[f] = vtmp;
            f --;
            vtmp = vfi;
        }
    }

    args->val[i-1] = pivot;

    if (f > 1)      //if (f>1) qs(val,f);
    {
        qs_args args_qs;
        args_qs.val = args->val;
        args_qs.ne = f;
        qs(&args_qs);
    }
    if(i < args->ne-1)     //if (i < ne-1) qs(&val[i],ne-f-1);
    {
        qs_args args_qs;
        args_qs.val = &args->val[i];
        args_qs.ne = args->ne-f-1;
        qs(&args_qs);
    }
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

    pthread_t threads[parts];
    qs_args args_qs[parts];
    
    if (parts < 2) assert("Han d'haver dues parts com a minim" == 0);
    if (ndades % parts) assert("N ha de ser divisible per parts" == 0);

    for(i=0;i<ndades;i++) valors[i]=rand()%MAX_INT;

    for(i = 0; i < parts; i++)
    {
        args_qs[i].val = &valors[i*(ndades/parts)];     // primera direcció de memòria del vector per cada thread
        args_qs[i].ne = ndades/parts;                  // nombre d'elements que ha de tractar cada thread

        pthread_create(&threads[i], NULL, qs, (void *)&args[i]);
    }

    // quicksort original, per fer PROVES
    // qs(valors,ndades);
    // vin=valors;
    // Quicksort a parts, Cada part l'hauria de fer un thread !!
   /* for (i=0;i<parts;i++)
    {
        // printf("de %d a %d\n",i*ndades/parts,(i+1)*(ndades/parts));
        qs(&valors[i*(ndades/parts)],ndades/parts);
    }*/

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
    for (i=1;i<ndades;i++) assert(vin[i-1]<=vin[i]); // comprova que la llista estigui ordenada

    for (i=0;i<ndades;i+=100)
        sum += vin[i];
    
    printf("validacio %lld \n",sum);
}