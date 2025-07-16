/**************************************************************
***************************************************************
*** 
***   plot.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for plot.c.
***
***************************************************************
**************************************************************/

void active_plot(short int *spins, int nx, int ny, int nz, short int periodic_x, short int periodic_y, short int periodic_z, int steps, short int spin, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot);
void animate(short int *spins, int *flips, double *energy, int steps_begin, int steps, int interval, int nx, int ny, int nz, short int spin, double delay, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot);
void animate_real_time(short int *spins, int *flips, double *energy, double *time, int steps_begin, int steps, double time_between_frames, int nx, int ny, int nz, short int spin, double delay, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot);
void animate_step(short int *spins, int *flips, double *energy, int steps_begin, int steps, int nx, int ny, int nz, short int spin, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot);
void animate_real_time_step(short int *spins, int *flips, double *energy, double *time, int steps_begin, int steps, int nx, int ny, int nz, short int spin, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot);
void blinkers_plot(short int *regions, int nx, int ny, int nz, char gnuplot_data_filename[], FILE *gnuplot_data, FILE *gnuplot_plot);
void state_plot_gnuplot(short int *spins, int nx, int ny, int nz, int steps, short int spin, char gnuplot_data_filename[], FILE *gnuplot_data, FILE *gnuplot_plot);
void state_plot_matlab(short int *spins, int nx, int ny, int nz, int steps, short int spin, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot);
void xy_plot(double *x, double *y, int size, char gnuplot_data_filename[], FILE *gnuplot_data, FILE *gnuplot_plot);

