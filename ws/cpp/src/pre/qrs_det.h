#ifndef _qrs_det_
#define _qrs_det_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define M 5

float * lowPass(float *sign0, int nsamp);
float * highPass(double *sign0, int nsamp);
int * QRS(float *lowPass, int nsamp);
int * getR(int *QRS, int sample_number_tested);
int * getRR(int *rr, int sample_number_tested);


//fungsi pembantu qrs process manggil program
/*
char  qrs_proc(double *signIn, int sample_number_tested, int *rr_out, int *rpeak_out){
	int i;
    char resx = 0;
    printf("pas masuk dalam \n");
	for (i = 0; i < 4; i++)
	{
		printf("%d\t%d\n", &rr_out[i],&rpeak_out[i]);
	}



	float *hP = highPass(signIn, sample_number_tested);
	int lenHigh = sizeof(hP)/sizeof(hP[0]);

	// for (i = 0; i < sample_number_tested; i++)
	// {
	// 	printf("\n sinyal %f\n" , signIn[i]);
	// }

	float *lP = lowPass(hP, sample_number_tested);
	int lenLow = sizeof(lP)/sizeof(lP[0]);

	// for (i = 0; i < sample_number_tested; i++)
	// {
	// 	printf("lowpass %f\n" , lP[i]);
	// }

	int *qrs = QRS(lP, sample_number_tested);
	int lenqrs = sizeof(qrs)/sizeof(qrs[0]);
	//printf("%d\n", lenqrs);

	// for (i = 0; i < sample_number_tested; i++)
	// {
	// 	if (qrs[i] == 1)
	// 	{
	// 		printf("r terdeteksi di index: %d\n", i);
	// 	}
	// }

	rr_out = getR(qrs, sample_number_tested);
	int lenrr = sizeof(rr_out)/sizeof(rr_out[0]);

	rpeak_out = getRpeak(rr_out, sample_number_tested);
	int lenrpeak = sizeof(rpeak_out)/sizeof(rpeak_out[0]);


    printf("dalam\n");
	for (i = 0; i < 100; i++)
	{
		if (rr_out[i]!=0)
		{
			printf("%d\t%d\n", rr_out[i],rpeak_out[i]);
		}
	}


	return resx;
}
*/





/*
*	lowpass for qrs detection
*/
float * lowPass(float *sign0, int nsamp) {
	float * lowPass = (float *) malloc(nsamp * sizeof(float));
	int i,j;

	int wide = 7;
	for (i = 0; i < nsamp; i++)
	{
		float sum = 0;
		if (i+wide < nsamp)
		{
			for (j = i; j < i+wide; j++)
			{
				float current = sign0[j] * sign0[j];
				sum += current;
			}
		}
		else if (i+wide >= nsamp)
		{
			int over = i+wide - nsamp;
			for (j = i; j < nsamp; j++)
			{
				float current = sign0[j] * sign0[j];
				sum += current;
			}
			for (j = 0; j < over; j++)
			{
				float current = sign0[j] * sign0[j];
				sum += current;
			}
		}

		lowPass[i] = sum;
	}

	return lowPass;
}


/*
*	highpass for qrs detection
*/
float * highPass(double *sign0, int nsamp) {
	float * highPass = (float *) malloc(nsamp * sizeof(float));
	float constant = (float) 1/M;

	int i,j;

	for (i = 0; i < nsamp; i++)
	{
		float y1 = 0;
		float y2 = 0;
		int y2_index = i-((M+1)/2);

		if (y2_index < 0)
		{
			y2_index = nsamp + y2_index;
		}
		y2 = (float) sign0[y2_index];

		float y1_sum = 0;
		for (j = i; j > i-M; j--)
		{
			int x_index = i - (i-j);
			if (x_index < 0)
			{
				x_index = nsamp + x_index;
			}
			y1_sum += sign0[x_index];
		}

		y1 = constant * y1_sum;
		highPass[i] = y2 - y1;
	}

	return highPass;
}

/*
*	QRS detection function
*/
int * QRS(float *lowPass, int nsamp) {
	int * QRS = (int *) malloc(nsamp * sizeof(int));
	int i,j;
	//int len = sizeof(lowPass)/sizeof(lowPass[0]);
	//printf("jumlah lowPass: %d\n", len);
	bool added;

	double treshold = 0;

	for (i = 0; i < 200; i++)
	{
		if (lowPass[i] > treshold)
		{
			treshold = lowPass[i];
		}
	}

	int frame = 100;

	for (i = 0; i < nsamp; i+=frame)
	{
		float max = 0;
		int index = 0;
		if (i+frame > nsamp)
		{
			index = nsamp;
		}
		else {
			index = i + frame;
		}

		for (j = i; j < index; j++)
		{
			if (lowPass[j] > max)
			{
				max = lowPass[j];
			}
		}

		added = false;
		for (j = i; j < index; j++)
		{
			if (lowPass[j] > treshold && !added)
			{
				QRS[j] = 1;
				added = true;
			}
			else {
				QRS[j] = 0;
			}
		}

		double gama = 0.15;
		double alpha = 0.05;

		treshold = alpha * gama * max + (1 - alpha) * treshold;

	}

	return QRS;
}

//get array r-detect
int * getR(int *QRS, int sample_number_tested){
	int * rr = (int *) malloc(sample_number_tested * sizeof(int));
	int i;
	int j = 1;

	rr[0] = 0;
	for (i = 0; i < sample_number_tested; i++)
	{
		if(QRS[i] == 1){
			rr[j] = i;
			j++;
		}
	}
	return rr;
}

//get array r-peak
int * getRR(int *rr, int sample_number_tested){
	int * rpeak = (int *) malloc(sample_number_tested * sizeof(int));
	int i,j;

	for (i = 0; i < sample_number_tested; i++)
	{
		if (rr[i+1]==0)
		{
			rpeak[i] = sample_number_tested - rr[i];
		}
		else
			rpeak[i] = rr[i+1] - rr[i];
	}

	return rpeak;
}


#ifdef __cplusplus
}
#endif

#endif
