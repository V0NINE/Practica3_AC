#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#define NN 512000000
#define MAX_INT ((int)((unsigned int)(-1) >> 1))

int valors[NN + 1];
int valors2[NN + 1];

typedef struct
{
    int *val;
    int ne;
} qs_args;

typedef struct
{
    int *val; 
    int n;
    int *vo;
} merge_args;

void qs(int *val, int ne);
void merge2(int *val, int n, int *vo);
void *qs_aux(void *args);
void *merge_aux(void *args);

int main(int nargs, char *args[])
{
    int ndades, i, m, parts;
    int *vin, *vout, *vtmp;
    long long sum = 0;

    assert(nargs == 3);
    ndades = atoi(args[1]);

    assert(ndades <= NN);
    parts = atoi(args[2]);

    if (parts < 2)
        assert("Han d'haver dues parts com a minim" == 0);
    if (ndades % parts)
        assert("N ha de ser divisible per parts" == 0);

    pthread_t threads[parts];
    qs_args qargs[parts];
    merge_args margs[parts];

    for (i = 0; i < ndades; i++)
        valors[i] = rand() % MAX_INT;
    
    for (i = 0; i < parts; i++)
    {
        qargs[i].val = &valors[i * (ndades / parts)];
        qargs[i].ne = ndades / parts;
        pthread_create(&threads[i], NULL, qs_aux, (void *)&qargs[i]);        
    }

    // 0 1 2 3 4 5 6 7 8

    // 0 2 4 6

    // 0 4      

    for (i = 1; i < parts/2; i *= 2)
    {
        for (int n = 0; n < parts; n++)
            pthread_join(threads[n], NULL);

        for (int j = 0; j < parts; j += 2 * i)
        {
            margs[j].val = &valors[j * (ndades / parts)];

            if (i == 1)
                margs[j].n = 2 * (ndades / parts);
            else
                margs[j].n = 2 * i * (ndades / parts);

            margs[j].vo = &valors2[j * (ndades / parts)];
            pthread_create(&threads[j], NULL, merge_aux, (void *)&margs[j]);   
        }
    }

    for (int n = 0; n < parts; n += parts/2)
        pthread_join(threads[n], NULL);

    // printf("---VALORS ORDENATS ABANS DEL MERGE---\n");
    // for (i = 0; i < ndades; i++)
    //     printf("%d\n", valors[i]);

    merge2(valors2,ndades,valors); //per a TH0
    vin=valors;

    //printf("---VALORS ORDENATS---\n");
    //for (i = 0; i < ndades; i++)
    //    printf("%d\n", vin[i]);

    for (i = 1; i < ndades; i++)
        assert(vin[i - 1] <= vin[i]);
    
    for (i = 0; i < ndades; i += 100)
        sum += vin[i];

    printf("validacio %lld \n", sum);
}

void *qs_aux(void *args)    
{
    qs_args *qargs = (qs_args *)args;
    qs(qargs->val, qargs->ne);
}

void *merge_aux(void *args)
{
    merge_args *margs = (merge_args *)args;
    merge2(margs->val, margs->n, margs->vo);
}

void qs(int *val, int ne)
{
    int i, f, j;
    int pivot, vtmp, vfi;
    pivot = val[0];
    i = 1;
    f = ne - 1;
    vtmp = val[i];
    while (i <= f)
    {
        if (vtmp < pivot)
        {
            val[i - 1] = vtmp;
            i++;
            vtmp = val[i];
        }
        else
        {
            vfi = val[f];
            val[f] = vtmp;
            f--;
            vtmp = vfi;
        }
    }
    val[i - 1] = pivot;
    if (f > 1)
        qs(val, f);
    if (i < ne - 1)
        qs(&val[i], ne - f - 1);
}

void merge2(int *val, int n, int *vo)
{
    int vtmp;
    int i, j, posi, posj;
    posi = 0;
    posj = (n / 2);

    for (i = 0; i < n; i++)
        if (((posi < n / 2) && (val[posi] <= val[posj])) || (posj >= n))
            vo[i] = val[posi++];
        else if (posj < n)
            vo[i] = val[posj++];
}

