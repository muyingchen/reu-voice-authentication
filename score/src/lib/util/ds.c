#include <stdio.h>
/*  this the direct_search algorithm from Robert Hook and T. A. Reeves
    "Direct Search" Solution of Numerical and Statistical Problems
    (Journal ACM 1961 (p212-229)

    The search uses an input vector to calculate a value from a function
    S and then modifies the vector to minimize the function.  Input
    parameters are:

        phi:	the current base point
        K:	The number of coordinate points
        DELTA:	The current step size
        delta:	The "minimum" step size	
        rho:	The reduction factor for the step size (rho < 1)
        S:	The function used for the minimization

    OTHER VARIABLES:

        theta:	the previous base point
        psi:	the base point resulting from the current move
        Spsi:	The functional value of S(psi)
        Sphi:	The functional value of S(phi)
        SS:	?

    Last change date: Nov 27 1990
    cleaned up slightly, verbose option removed summer 1992.
*/
#define DS_C_VERSION "V1.0"
#include <util/utillib.h>

float SS, Spsi, Sphi, theta, *DELTA, *delta, (*S)(int *), rho;
int phi[30], K, k;
int *psi, DELTA_change;

void direct_search(int *IN_psi, int IN_K, float *IN_DELTA, float IN_rho, float *IN_delta, float (*IN_S) ( int *))
{
    psi = IN_psi;
    K = IN_K;
    DELTA = IN_DELTA;
    rho = IN_rho;
    S = IN_S;
    delta = IN_delta;

    Spsi = (*S)(psi);
L1: SS = Spsi;

    for (k=0; k<K; k++)
        phi[k] = psi[k];

    for (k=0; k<K; k++){
        phi[k] += DELTA[k];
        Sphi = (*S)(phi);
        if (Sphi < SS)
            SS = Sphi;
        else{
            phi[k] -= 2 * (int)DELTA[k];
            Sphi = S(phi);
            if (Sphi < SS)
                SS = Sphi;
            else
                phi[k] += DELTA[k];
        } 
    }

    if (SS < Spsi){
        do {
            for (k=0; k<K; k++){
                theta = psi[k];
                psi[k] = phi[k];
                phi[k] = 2*phi[k] - theta;
            }
            Spsi = SS;
            SS = Sphi = (*S)(phi);

	    for (k=0; k<K; k++){
	      phi[k] += DELTA[k];
	      Sphi = (*S)(phi);
	      if (Sphi < SS)
		SS = Sphi;
	      else{
		phi[k] -= 2 * (int)DELTA[k];
		Sphi = S(phi);
		if (Sphi < SS)
		  SS = Sphi;
		else
		  phi[k] += DELTA[k];
	      } 
	    }

        } while (SS < Spsi);
        goto L1;
    }
    DELTA_change=0;
    for (k=0; k<K; k++)
        if (DELTA[k] >= delta[k]){
            DELTA[k] = rho * DELTA[k];
            DELTA_change=1;
        }
    if (DELTA_change==1) goto L1;
}







