#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#define NN 512000000  
#define MAX_INT ((int)((unsigned int)(-1)>>1))

int valors[NN+1];
int valors2[NN+1];
pthread_t threads[parts];
int ndades, parts;

typedef struct {
    int *val;
    int ne;
    int tid;
}qs_args;

typedef struct {
    int *vin;
    int n;
    int *vout;
    int tid;
}join_args;
printf("branca main");
void join(qs_args *args)
{	
    if (args->tid % 2 == 0 && args->tid + 1 < parts)
    {	
    	 pthread_join(threads[args->tid + 1], NULL);
	 parts--;

	 pthread_create(&threads[args->tid], merge2, &valors[args->tid*(ndades/parts)], 2*ndades/parts, &valors2[args->tid*(ndades/parts)],NULL); 
    }


    if (args->tid % 2 == 1) 
	    pthread_exit(NULL);

	    
}



void *qs(void *arg)
{
    qs_args *args = (qs_args *)arg;

    int i,f,j;
    int pivot,vtmp,vfi;
    pivot = args->val[0];
    i = 1;
    f = args->ne-1;
    vtmp = args->val[i];

    //printf("Hola qs, visita del thread %d, el meu primer element es: %d\n", args->tid, args->val[0]);

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

    join(args);
 
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
    join();
}

int main(int nargs,char* args[])
{
    int i,m,t;
    int *vin,*vout,*vtmp;
    long long sum=0;

    assert(nargs == 3);

    ndades = atoi(args[1]);
    assert(ndades <= NN);

    parts = atoi(args[2]);

    qs_args args_qs[parts];
    
    if (parts < 2) assert("Han d'haver dues parts com a minim" == 0);
    if (ndades % parts) assert("N ha de ser divisible per parts" == 0);

    for(i=0;i<ndades;i++) valors[i]=rand()%MAX_INT;
	
    // printf("-----VALORS ORIGINALS-----\n");
    // for(i=0;i<ndades;i++)
	// printf("%d\n", valors[i]);

    for(i = 0; i < parts; i++)
    {
        args_qs[i].val = &valors[i*(ndades/parts)];     // primera direcció de memòria del vector per cada thread
        args_qs[i].ne = ndades/parts;			// nombre d'elements que ha de tractar cada thread
	    args_qs[i].tid = i;
	    //printf("thread %d primer valor: %d\n", args_qs[i].tid, args_qs[i].val[0]);
        pthread_create(&threads[i], NULL, qs, (void *)&args_qs[i]);
    }
    
    // Sincronització i merge dos a dos
    for (int step = 1; step < parts; step *= 2) 
    {

        for (i = 0; i < parts; i += 2 * step) 
        {
            if (i + step < parts) 
            {
                // Fusiona les dos parts: [i, i+step)
                pthread_join(threads[i + step], NULL);
		merge2(&valors[i * ndades/parts], 2 * ndades/parts, &valors2[i * ndades/parts]);
            }
        }

        // Intercanvia vin y vout per a la següent ronda de merge
        vtmp = vin;
        vin = vout;
        vout = vtmp;
    }

    merge2(valors2,ndades,valors); //per a TH0
    vin=valors;
    
    // printf("-----VALORS ORDENATS-----\n");
    // for(i=0;i<ndades;i++)
	// printf("%d\n", valors[i]);

    for (i=1;i<ndades;i++) assert(vin[i-1]<=vin[i]); // comprova que la llista estigui ordenada

    for (i=0;i<ndades;i+=100)
        sum += vin[i];
    
    printf("validacio %lld \n",sum);
}
