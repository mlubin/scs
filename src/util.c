#include "util.h"
/* return milli-seconds */
#if (defined _WIN32 || _WIN64 || defined _WINDLL)
void tic(timer* t)
{
	QueryPerformanceFrequency(&t->freq);
	QueryPerformanceCounter(&t->tic);
}

scs_float tocq(timer* t)
{
	QueryPerformanceCounter(&t->toc);
	return (1e3 * (t->toc.QuadPart - t->tic.QuadPart) / (scs_float) t->freq.QuadPart);
}
#elif (defined __APPLE__)
void tic(timer* t) {
	/* read current clock cycles */
	t->tic = mach_absolute_time();
}

scs_float tocq(timer* t) {

	uint64_t duration; /* elapsed time in clock cycles*/

	t->toc = mach_absolute_time();
	duration = t->toc - t->tic;

	/*conversion from clock cycles to nanoseconds*/
	mach_timebase_info(&(t->tinfo));
	duration *= t->tinfo.numer;
	duration /= t->tinfo.denom;

	return (scs_float) duration / 1e6;
}
#else
void tic(timer* t)
{
	clock_gettime(CLOCK_MONOTONIC, &t->tic);
}

scs_float tocq(timer* t)
{
	struct timespec temp;

	clock_gettime(CLOCK_MONOTONIC, &t->toc);

	if ((t->toc.tv_nsec - t->tic.tv_nsec)<0) {
		temp.tv_sec = t->toc.tv_sec - t->tic.tv_sec-1;
		temp.tv_nsec = 1e9+t->toc.tv_nsec - t->tic.tv_nsec;
	} else {
		temp.tv_sec = t->toc.tv_sec - t->tic.tv_sec;
		temp.tv_nsec = t->toc.tv_nsec - t->tic.tv_nsec;
	}
	return (scs_float)temp.tv_sec * 1e3 + (scs_float)temp.tv_nsec / 1e6;
}
#endif

scs_float toc(timer * t) {
	scs_float time = tocq(t);
	scs_printf("time: %8.4f milli-seconds.\n", time);
	return time;
}

scs_float strtoc(char * str, timer * t) {
	scs_float time = tocq(t);
	scs_printf("%s - time: %8.4f milli-seconds.\n", str, time);
	return time;
}

void printConeData(Cone * k) {
	scs_int i;
	scs_printf("num zeros = %i\n", (int) k->f);
	scs_printf("num LP = %i\n", (int) k->l);
	scs_printf("num SOCs = %i\n", (int) k->qsize);
	scs_printf("soc array:\n");
	for (i = 0; i < k->qsize; i++) {
		scs_printf("%i\n", (int) k->q[i]);
	}
	scs_printf("num SDCs = %i\n", (int) k->ssize);
	scs_printf("sdc array:\n");
	for (i = 0; i < k->ssize; i++) {
		scs_printf("%i\n", (int) k->s[i]);
	}
	scs_printf("num ep = %i\n", (int) k->ep);
	scs_printf("num ed = %i\n", (int) k->ed);
}

void printWork(Data * d, Work * w) {
	scs_int i, l = d->n + d->m;
	scs_printf("\n u_t is \n");
	for (i = 0; i < l; i++) {
		scs_printf("%f\n", w->u_t[i]);
	}
	scs_printf("\n u is \n");
	for (i = 0; i < l; i++) {
		scs_printf("%f\n", w->u[i]);
	}
	scs_printf("\n v is \n");
	for (i = 0; i < l; i++) {
		scs_printf("%f\n", w->v[i]);
	}
}

void printData(Data * d) {
	scs_printf("m = %i\n", (int) d->m);
	scs_printf("n = %i\n", (int) d->n);

	scs_printf("max_iters = %i\n", (int) d->max_iters);
	scs_printf("verbose = %i\n", (int) d->verbose);
	scs_printf("normalize = %i\n", (int) d->normalize);
	scs_printf("warmStart = %i\n", (int) d->warm_start);
	scs_printf("eps = %4f\n", d->eps);
	scs_printf("alpha = %4f\n", d->alpha);
	scs_printf("rhoX = %4f\n", d->rho_x);
	scs_printf("cg_rate = %4f\n", d->cg_rate);
	scs_printf("scale = %4f\n", d->scale);
}

void printArray(scs_float * arr, scs_int n, char * name) {
	scs_int i, j, k = 0;
	scs_int numOnOneLine = 10;
	scs_printf("\n");
	for (i = 0; i < n / numOnOneLine; ++i) {
		for (j = 0; j < numOnOneLine; ++j) {
			scs_printf("%s[%li] = %4f, ", name, (long) k, arr[k]);
			k++;
		}
		scs_printf("\n");
	}
	for (j = k; j < n; ++j) {
		scs_printf("%s[%li] = %4f, ", name, (long) j, arr[j]);
	}
	scs_printf("\n");
}

void freeData(Data * d, Cone * k) {
    if (d) {
        if (d->b)
            scs_free(d->b);
        if (d->c)
            scs_free(d->c);
        if (d->A) {
            freeAMatrix(d->A);
            scs_free(d->A);
        }
        scs_free(d);
    }
    if (k) {
        if (k->q)
            scs_free(k->q);
        if (k->s)
            scs_free(k->s);
        scs_free(k);
    }
    d = NULL;
    k = NULL;
}

void freeSol(Sol *sol) {
    if (sol) {
        if (sol->x) {
            scs_free(sol->x);
            sol->x = NULL;
        }
        if (sol->y) {
            scs_free(sol->y);
            sol->y = NULL;
        }
        if (sol->s) {
            scs_free(sol->s);
            sol->s = NULL;
        }
        scs_free(sol);
    }
    sol = NULL;
}
