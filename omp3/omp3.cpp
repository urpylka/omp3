#include "stdafx.h"
#include <iostream>
#include "mpi.h"
#include <omp.h>

int mas[] = { 10, 100, 200, 400, 800, 1000, 1500 };
const int n = 1500;
double u[n][n];
double eps = 0.001;
void init(int dim)
{
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			if (i == 0 || i == dim - 1) u[i][j] = 10;
			else if (j == 0 || j == dim - 1) u[i][j] = (double)i * 2 / dim;
			else u[i][j] = 0;
		}
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	bool b;
	double t1, t2;
	double buffer = 0;
	int k = 0;
	double d;
	int m = 0;
	printf("Linear: \n");
	for (int l = 0; l < 7; l++)
	{
		k = 0;
		b = true;
		init(mas[l]);
		t1 = omp_get_wtime();
		while (b)
		{
			m = 0;
			for (int i = 1; i < mas[l] - 1; i++)
			{
				for (int j = 1; j < mas[l] - 1; j++)
				{
					buffer = u[i][j];
					u[i][j] = (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]) / 4;
					if (u[i][j] < 1)
						d = abs(u[i][j] - buffer);
					else  d = abs((u[i][j] - buffer) / u[i][j]);
					if (d < eps) m++;
				}
			}
			if (m >= (mas[l] - 2)*(mas[l] - 2)) b = false;
			k++;
		}
		t2 = omp_get_wtime();
		printf("	%d\n", mas[l]);
		printf("Time = %f\n", t2 - t1);
		printf("k = %d\n", k);
	}

	printf("\nParallel OpenMP: \n");
	b = true;
	bool b1 = true;
	bool b2 = false;
	buffer = 0;
	m = 0;
	int m1 = 0, m2 = 0;
	for (int l = 0; l < 7; l++)
	{
		k = 0;
		init(mas[l]);
		t1 = omp_get_wtime();
#pragma omp parallel sections default(shared) private(buffer, m1, m2, d) num_threads(2)
		{
#pragma omp section
		{
			b = true;
			while (b)
			{
				m1 = 0;
				if (b1)
				{
					b1 = false;
					for (int i = 1; i < mas[l] / 2; i++) {
						for (int j = 1; j < mas[l] - 1; j++)
						{
							buffer = u[i][j];
							u[i][j] = (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]) / 4;
							if (u[i][j] < 1)
								d = abs(u[i][j] - buffer);
							else  d = abs((u[i][j] - buffer) / u[i][j]);
							if (d < eps) m1++;
						}
					}
					m = m1;
					b2 = true;
				}
			}
		}
#pragma omp section
		{
			b = true;
			while (b)
			{
				if (b2)
				{
					b2 = false;
					m2 = m;
					for (int i = mas[l] / 2; i < mas[l] - 1; i++)
					{
						for (int j = 1; j < mas[l] - 1; j++)
						{
							buffer = u[i][j];
							u[i][j] = (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]) / 4;
							if (u[i][j] < 1)
								d = abs(u[i][j] - buffer);
							else  d = abs((u[i][j] - buffer) / u[i][j]);
							if (d < eps) m2++;
						}
						if (i == mas[l] / 2) b1 = true;
					}
					if (m2 >= (mas[l] - 2)*(mas[l] - 2)) b = false;
					k++;
				}
			}
		}
		}
		t2 = omp_get_wtime();
		printf("	%d\n", mas[l]);
		printf("Time = %f\n", t2 - t1);
		printf("k = %d\n", k);

	}
	getchar();
	system("pause");
	return 0;
}