#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double f1(double x) { return x * x * x; }
double f2(double x) { return sin(20 * x) * x; }
typedef enum { RECT_LEFT, RECT_MID, RECT_RIGHT } RectMethod;

/**
 * @brief Calculates an approximated value of an integral
 * @param a left end of the integral
 * @param b right end of the integral
 * @param n number of rectangles used (more = better result but slower)
 * @param f function used in the integral
 */
double rectangleMethod(double a, double b, int n, double (*f)(double),
                       RectMethod method) {
  FILE *fp = NULL;
  switch (method) {
  case RECT_LEFT:
    fp = fopen("rectangles_l.csv", "w");
    break;
  case RECT_MID:
    fp = fopen("rectangles_m.csv", "w");
    break;
  case RECT_RIGHT:
    fp = fopen("rectangles_r.csv", "w");
    break;
  }
  fprintf(fp, "x,height,width\n");
  double width = (b - a) / n, sum = 0, x_i;
  for (int i = 0; i < n; i++) {
    switch (method) {
    case RECT_LEFT:
      x_i = a + i * width;
      break;
    case RECT_MID:
      x_i = a + (i + 0.5) * width;
      break;
    case RECT_RIGHT:
      x_i = a + (i + 1) * width;
      break;
    }
    double height = (*f)(x_i);
    fprintf(fp, "%f,%f,%f\n", a + i * width, height, width);
    sum += width * height;
  }
  fclose(fp);
  return sum;
}
/**
 * @brief Calculates an approximated value of an integral
 * @param a left end of the integral
 * @param b right end of the integral
 * @param n number of trapezoids used (more = better result but slower)
 * @param f function used in the integral
 */
double trapezoidalMethod(double a, double b, int n, double (*f)(double)) {
  double width = (b - a) / n, sum = (*f)(a) + (*f)(b);
  FILE *fp = fopen("trapezoids.csv", "w");
  fprintf(fp, "x,y\n");
  for (int i = 1; i < n; i++) {
    double x = a + (i * width);
    double y = (*f)(x);
    fprintf(fp, "%f,%f\n", x, y);
    sum += 2 * y;
  }
  fclose(fp);
  return sum * width / 2;
}
/**
 * @brief Calculates an approximated value of an integral
 * @param a left end of the integral
 * @param b right end of the integral
 * @param n number of samples (more = better result but slower)
 * @param f function used in the integral
 */
double monteCarlo(double a, double b, int n, double (*f)(double)) {
  FILE *fp = fopen("monte_carlo_viz.csv", "w");
  fprintf(fp, "x,y,is_under\n");

  double y_min = 0;
  double y_max = 0;
  int search_steps = 1000;
  for (int i = 0; i <= search_steps; i++) {
    double val = f(a + (double)i * (b - a) / search_steps);
    if (val > y_max)
      y_max = val;
    if (val < y_min)
      y_min = val;
  }

  y_max *= 1.1;
  y_min *= 1.1;

  int positive_hits = 0;
  int negative_hits = 0;

  for (int i = 0; i < n; i++) {
    double x = a + ((double)rand() / RAND_MAX) * (b - a);
    double y = y_min + ((double)rand() / RAND_MAX) * (y_max - y_min);
    double fx = f(x);

    int status = 0;

    if (fx > 0 && y > 0 && y <= fx) {
      positive_hits++;
      status = 1;
    } else if (fx < 0 && y < 0 && y >= fx) {
      negative_hits++;
      status = -1;
    }

    fprintf(fp, "%f,%f,%d\n", x, y, status);
  }

  fclose(fp);

  double box_area = (b - a) * (y_max - y_min);
  return box_area * ((double)(positive_hits - negative_hits) / n);
}
void benchmarkTime() {
  int n_values[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
  int num_tests = 7;
  int repetitions = 100;

  printf("n_values,time_seconds\n");

  for (int i = 0; i < num_tests; i++) {
    int n = n_values[i];
    int reps = 100; 
    if (n >= 100000)
      reps = 10;
    if (n >= 10000000)
      reps = 3;

    clock_t start = clock();
    for (int r = 0; r < reps; r++) {
      monteCarlo(0, M_PI, n, f2);
    }
    clock_t end = clock();

    double avg_time = ((double)(end - start) / CLOCKS_PER_SEC) / reps;

    printf("%d, %f\n", n, avg_time);
  }
}

int main() {
  // double lb = 0, ub = M_PI;
  // int subreg = 150;
  // int samp = 5000;
  //  int samples[]={10,100, 1000, 10000, 100000};
  //
  //  int len=sizeof(samples)/sizeof(int);
  //  for(int i=0;i<len;i++){
  //       //printf("%f,", fabs(-0.157079-rectangleMethod(lb, ub, samples[i],
  //       f2, RECT_MID)));
  //    //printf("%f,", fabs(-0.157079-trapezoidalMethod(lb, ub, samples[i],
  //    f2))); double sum=0; for(int j=0;j<10;j++){
  //        sum=fabs(-0.157079-monteCarlo(lb, ub, samples[i], f2));
  //    }
  //    printf("%f,",sum/10);
  //  }
  // printf("l %f\n", rectangleMethod(lb, ub, subreg, f2, RECT_LEFT));
  // printf("s %f\n", rectangleMethod(lb, ub, subreg, f2, RECT_MID));
  // printf("p %f\n", rectangleMethod(lb, ub, subreg, f2, RECT_RIGHT));
  // printf("t %f\n", trapezoidalMethod(lb, ub, subreg, f2));
  // printf("mc %f\n", monteCarlo(lb, ub, samp, f2));
  monteCarlo(0, M_PI, 1000, f2);
  benchmarkTime();
  return 0;
}
