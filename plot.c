/**************************************************************
***************************************************************
*** 
***   plot.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains all graphics functions for ising3d.
***
***************************************************************
**************************************************************/

#include "includes.h"

void active_plot(short int *spins, int nx, int ny, int nz, short int periodic_x, short int periodic_y, short int periodic_z, int steps, short int spin, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot)
{
	int num, s1, s2, s3, s4, s5, s6;
	int x, y, z;
	int nxtimesny=nx*ny;
	
	fprintf(matlab_data, "%d\n%d\n%d\n%d\n\n", nx, ny, nz, steps);
	
	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				if (x==0)
				{
					if (periodic_x==1) s1=*(spins+(nx-1)+nx*y+nxtimesny*z);
					else s1=0;
				}
				else s1=*(spins+(x-1)+nx*y+nxtimesny*z);
				if (x==nx-1)
				{
					if (periodic_x==1) s2=*(spins+nx*y+nxtimesny*z);
					else s2=0;
				}
				else s2=*(spins+(x+1)+nx*y+nxtimesny*z);
				
				if (y==0)
				{
					if (periodic_y==1) s3=*(spins+x+nx*(ny-1)+nxtimesny*z);
					else s3=0;
				}
				else s3=*(spins+x+nx*(y-1)+nxtimesny*z);
				if (y==ny-1)
				{
					if (periodic_y==1) s4=*(spins+x+nxtimesny*z);
					else s4=0;
				}
				else s4=*(spins+x+nx*(y+1)+nxtimesny*z);

				if (z==0)
				{
					if (periodic_z==1) s5=*(spins+x+nx*y+nxtimesny*(nz-1));
					else s5=0;
				}
				else s5=*(spins+x+nx*y+nxtimesny*(z-1));
				if (z==nz-1)
				{
					if (periodic_z==1) s6=*(spins+x+nx*y);
					else s6=0;
				}
				else s6=*(spins+x+nx*y+nxtimesny*(z+1));

				num=s1+s2+s3+s4+s5+s6;
				
				// If the energy of the spin is zero, it is active.
				if (num==0 && *(spins+x+nx*y+nxtimesny*z)==spin) fprintf(matlab_data, "1\n");
				else fprintf(matlab_data, "0\n");
			}
		}
	}

	// First the up/down spins are plotted in Matlab, then the active spins are plotted on the same graph.
		
	fprintf(matlab_plot, "plotdata=fopen('%s', 'r');\n", matlab_data_filename);
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "nx=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "ny=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "nz=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for z=0:nz-1\n");
	fprintf(matlab_plot, "	for y=0:ny-1\n");
	fprintf(matlab_plot, "		for x=0:nx-1\n");
	fprintf(matlab_plot, "			if fscanf(plotdata, '%%d', 1)==1\n");
	fprintf(matlab_plot, "				delete(hx(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				delete(hx(x+2, y+1, z+1));\n");
	fprintf(matlab_plot, "				delete(hy(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				delete(hy(x+1, y+2, z+1));\n");
	fprintf(matlab_plot, "				delete(hz(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				delete(hz(x+1, y+1, z+2));\n");
	fprintf(matlab_plot, "				hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'w');\n");
	fprintf(matlab_plot, "				hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'w');\n");
	fprintf(matlab_plot, "				hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'w');\n");
	fprintf(matlab_plot, "				hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'w');\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'w');\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'w');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	
	return;
}

/*-------------------------------------------------------------

	animate

	Parameter			Type
	-------------------------------------------------------
	spins				short int*
	flips				int*
	energy				double*
	steps_begin			int
	steps				int
	interval			int
	nx				int
	ny				int
	nz				int
	spin				short int
	delay				double
	matlab_data_filename		char*
	matlab_data			FILE*
	matlab_plot			FILE*

	Writes a Matlab data file and a Matlab script used for animating the evolution of
	the lattice.

-------------------------------------------------------------*/
void animate(short int *spins, int *flips, double *energy, int steps_begin, int steps, int interval, int nx, int ny, int nz, short int spin, double delay, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot)
{
	int i;
	int x, y, z;
	
	fprintf(matlab_data, "%d\n%d\n%d\n%d\n%d\n%hd\n", nx, ny, nz, steps_begin, steps, spin);
	
	for (i=0; i<nx*ny*nz; i++)
	{
		fprintf(matlab_data, "%d\n", *(spins+i));
	}
	
	fprintf(matlab_data, "%d\n", (int)(*energy));
	
	for (i=0; i<steps; i++)
	{
		z=(int)(*(flips+2*i)/nx/ny);
		y=(int)((*(flips+2*i)-z*nx*ny)/nx);
		x=(int)(*(flips+2*i)-z*nx*ny-y*nx);

		fprintf(matlab_data, "%d %d %d\n", x, y, z);
		fprintf(matlab_data, "%d\n", *(flips+2*i+1));
		fprintf(matlab_data, "%d\n", (int)(*(energy+i+1)));
	}
	
	fprintf(matlab_plot, "hold on\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "plotdata=fopen('%s', 'r');\n", matlab_data_filename);
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "nx=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "ny=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "nz=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps_begin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "spin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "s=zeros(nx,ny,nz);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for z=1:nz\n");
	fprintf(matlab_plot, "	for y=1:ny\n");
	fprintf(matlab_plot, "		for x=1:nx\n");
	fprintf(matlab_plot, "			s(x,y,z)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "energy=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), steps=', num2str(steps_begin), ', energy=', num2str(energy)])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for i=1:steps\n");
	fprintf(matlab_plot, "	x=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	y=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	z=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	flip=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	energy=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "	if flip==1\n");
	fprintf(matlab_plot, "		s(x+1,y+1,z+1)=-s(x+1,y+1,z+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "		if s(x+1,y+1,z+1)==spin\n");
	fprintf(matlab_plot, "			if (x>0 && s(x,y+1,z+1)==-spin) || x==0\n");
	fprintf(matlab_plot, "				hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (x<nx-1 && s(x+2,y+1,z+1)==-spin) || x==nx-1\n");
	fprintf(matlab_plot, "				hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (y>0 && s(x+1,y,z+1)==-spin) || y==0\n");
	fprintf(matlab_plot, "				hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (y<ny-1 && s(x+1,y+2,z+1)==-spin) || y==ny-1\n");
	fprintf(matlab_plot, "				hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (z>0 && s(x+1,y+1,z)==-spin) || z==0\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (z<nz-1 && s(x+1,y+1,z+2)==-spin) || z==nz-1\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		else\n");
	fprintf(matlab_plot, "			if (x>0 && s(x,y+1,z+1)==-spin) || x==0\n");
	fprintf(matlab_plot, "				delete(hx(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				hx(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (x<nx-1 && s(x+2,y+1,z+1)==-spin) || x==nx-1\n");
	fprintf(matlab_plot, "				delete(hx(x+2, y+1, z+1));\n");
	fprintf(matlab_plot, "				hx(x+2, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (y>0 && s(x+1,y,z+1)==-spin) || y==0\n");
	fprintf(matlab_plot, "				delete(hy(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				hy(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (y<ny-1 && s(x+1,y+2,z+1)==-spin) || y==ny-1\n");
	fprintf(matlab_plot, "				delete(hy(x+1, y+2, z+1));\n");
	fprintf(matlab_plot, "				hy(x+1, y+2, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (z>0 && s(x+1,y+1,z)==-spin) || z==0\n");
	fprintf(matlab_plot, "				delete(hz(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (z<nz-1 && s(x+1,y+1,z+2)==-spin) || z==nz-1\n");
	fprintf(matlab_plot, "				delete(hz(x+1, y+1, z+2));\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+2)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "	if mod(i,%d)==0\n", interval);
	fprintf(matlab_plot, "		title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), steps=', num2str(steps_begin+i), ', energy=', num2str(energy)])\n");
	fprintf(matlab_plot, "		pause(%lf)\n", delay);
	fprintf(matlab_plot, "		drawnow\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "fclose(plotdata);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "hold off\n");
	
	return;
}

void animate_real_time(short int *spins, int *flips, double *energy, double *time, int steps_begin, int steps, double time_between_frames, int nx, int ny, int nz, short int spin, double delay, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot)
{
	int i;
	int x, y, z;
	
	fprintf(matlab_data, "%d\n%d\n%d\n%d\n%d\n%hd\n", nx, ny, nz, steps_begin, steps, spin);
	
	for (i=0; i<nx*ny*nz; i++)
	{
		fprintf(matlab_data, "%d\n", *(spins+i));
	}
	
	fprintf(matlab_data, "%d\n", (int)(*energy));
	
	for (i=0; i<steps; i++)
	{
		z=(int)(*(flips+2*i)/nx/ny);
		y=(int)((*(flips+2*i)-z*nx*ny)/nx);
		x=(int)(*(flips+2*i)-z*nx*ny-y*nx);

		fprintf(matlab_data, "%d %d %d\n", x, y, z);
		fprintf(matlab_data, "%d\n", *(flips+2*i+1));
		fprintf(matlab_data, "%d\n", (int)(*(energy+i+1)));
		fprintf(matlab_data, "%f\n", (float)(*(time+i+1)));
	}
	
	fprintf(matlab_plot, "hold on\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "plotdata=fopen('%s', 'r');\n", matlab_data_filename);
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "nx=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "ny=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "nz=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps_begin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "spin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "s=zeros(nx,ny,nz);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for z=1:nz\n");
	fprintf(matlab_plot, "	for y=1:ny\n");
	fprintf(matlab_plot, "		for x=1:nx\n");
	fprintf(matlab_plot, "			s(x,y,z)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "energy=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), steps=', num2str(steps_begin), ', energy=', num2str(energy)])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "last_time=0;\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for i=1:steps\n");
	fprintf(matlab_plot, "	x=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	y=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	z=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	flip=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	energy=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	time=fscanf(plotdata, '%%f', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "	if flip==1\n");
	fprintf(matlab_plot, "		s(x+1,y+1,z+1)=-s(x+1,y+1,z+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "		if s(x+1,y+1,z+1)==spin\n");
	fprintf(matlab_plot, "			if (x>0 && s(x,y+1,z+1)==-spin) || x==0\n");
	fprintf(matlab_plot, "				hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (x<nx-1 && s(x+2,y+1,z+1)==-spin) || x==nx-1\n");
	fprintf(matlab_plot, "				hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (y>0 && s(x+1,y,z+1)==-spin) || y==0\n");
	fprintf(matlab_plot, "				hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (y<ny-1 && s(x+1,y+2,z+1)==-spin) || y==ny-1\n");
	fprintf(matlab_plot, "				hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (z>0 && s(x+1,y+1,z)==-spin) || z==0\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (z<nz-1 && s(x+1,y+1,z+2)==-spin) || z==nz-1\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		else\n");
	fprintf(matlab_plot, "			if (x>0 && s(x,y+1,z+1)==-spin) || x==0\n");
	fprintf(matlab_plot, "				delete(hx(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				hx(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (x<nx-1 && s(x+2,y+1,z+1)==-spin) || x==nx-1\n");
	fprintf(matlab_plot, "				delete(hx(x+2, y+1, z+1));\n");
	fprintf(matlab_plot, "				hx(x+2, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (y>0 && s(x+1,y,z+1)==-spin) || y==0\n");
	fprintf(matlab_plot, "				delete(hy(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				hy(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (y<ny-1 && s(x+1,y+2,z+1)==-spin) || y==ny-1\n");
	fprintf(matlab_plot, "				delete(hy(x+1, y+2, z+1));\n");
	fprintf(matlab_plot, "				hy(x+1, y+2, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (z>0 && s(x+1,y+1,z)==-spin) || z==0\n");
	fprintf(matlab_plot, "				delete(hz(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "			if (z<nz-1 && s(x+1,y+1,z+2)==-spin) || z==nz-1\n");
	fprintf(matlab_plot, "				delete(hz(x+1, y+1, z+2));\n");
	fprintf(matlab_plot, "				hz(x+1, y+1, z+2)=0;\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "	if time-last_time>=%lf || i==steps\n",time_between_frames);
	fprintf(matlab_plot, "		title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), energy/spin=', num2str(energy/nx/ny/nz), ', steps=', num2str(steps_begin+i), ', time=', num2str(time)])\n");
	fprintf(matlab_plot, "		pause(%lf)\n", delay);
	fprintf(matlab_plot, "		drawnow\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "		last_time=time;\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "fclose(plotdata);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "hold off\n");
	
	return;
}

/*-------------------------------------------------------------

	animate_step

	Parameter			Type
	-------------------------------------------------------
	spins				short int*
	flips				int*
	energy				double*
	steps_begin			int
	steps				int
	nx				int
	ny				int
	nz				int
	spin				short int
	matlab_data_filename		char*
	matlab_data			FILE*
	matlab_plot			FILE*

	Writes a Matlab data file and a Matlab script used for stepping through the
	animation one spin flip at a time.

-------------------------------------------------------------*/
void animate_step(short int *spins, int *flips, double *energy, int steps_begin, int steps, int nx, int ny, int nz, short int spin, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot)
{
	int i;
	int x, y, z;
	
	fprintf(matlab_data, "%d\n%d\n%d\n%d\n%d\n%hd\n", nx, ny, nz, steps_begin, steps, spin);
	
	for (i=0; i<nx*ny*nz; i++)
	{
		fprintf(matlab_data, "%d\n", *(spins+i));
	}
	
	fprintf(matlab_data, "%d\n", (int)(*energy));
	
	for (i=0; i<steps; i++)
	{
		z=(int)(*(flips+2*i)/nx/ny);
		y=(int)((*(flips+2*i)-z*nx*ny)/nx);
		x=(int)(*(flips+2*i)-z*nx*ny-y*nx);

		fprintf(matlab_data, "%d %d %d\n", x, y, z);
		fprintf(matlab_data, "%d\n", *(flips+2*i+1));
		fprintf(matlab_data, "%d\n", (int)(*(energy+i+1)));
	}

	fprintf(matlab_plot, "function evolve_animate_step\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "global nx ny nz steps_begin steps spin old_step s hx hy hz x_array y_array z_array flip_array energy_array\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "plotdata=fopen('%s', 'r');\n", matlab_data_filename);
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "nx=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "ny=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "nz=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps_begin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "spin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "hx=zeros(nx+1, ny, nz);\n");
	fprintf(matlab_plot, "hy=zeros(nx, ny+1, nz);\n");
	fprintf(matlab_plot, "hz=zeros(nx, ny, nz+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "s=zeros(nx,ny,nz);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "figure\n");
	fprintf(matlab_plot, "axis([0 nx 0 ny 0 nz], 'equal')\n");
	fprintf(matlab_plot, "set(gca,'Units','centimeters')\n");
	fprintf(matlab_plot, "position=get(gcf,'Position');\n");
	fprintf(matlab_plot, "position(4)=500;\n");
	fprintf(matlab_plot, "set(gcf,'Position',position)\n");
	fprintf(matlab_plot, "xlabel('x')\n");
	fprintf(matlab_plot, "ylabel('y')\n");
	fprintf(matlab_plot, "zlabel('z')\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), steps (min, current, max)=(', num2str(steps_begin), ', ', num2str(steps_begin), ', ', num2str(steps_begin+steps), ')'])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "slider=uicontrol('Style','slider','Callback',@slider_callback,'Min',steps_begin,'Max',steps_begin+steps,'SliderStep',[1/steps 1],'Value',steps_begin,'Position',[64 400 384 20]);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for z=0:nz-1\n");
	fprintf(matlab_plot, "	for y=0:ny-1\n");
	fprintf(matlab_plot, "		for x=0:nx-1\n");
	fprintf(matlab_plot, "			s(x+1,y+1,z+1)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "			if s(x+1,y+1,z+1)==spin\n");
	fprintf(matlab_plot, "				if hx(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hx(x+2, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+2, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+2)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "x_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "y_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "z_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "flip_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "energy_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "old_step=steps_begin;\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for i=1:steps\n");
	fprintf(matlab_plot, "	x_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	y_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	z_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	flip_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	energy_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "function slider_callback(h, eventdata)\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "global nx ny nz steps_begin steps spin old_step s hx hy hz x_array y_array z_array flip_array energy_array\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "set(h,'Value',cast(get(h,'Value'),'int32'))\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), steps (min, current, max)=(', num2str(steps_begin), ', ', num2str(get(h,'Value')), ', ', num2str(steps_begin+steps), ')'])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "if get(h,'Value')~=old_step\n");
	fprintf(matlab_plot, "	if get(h,'Value')<old_step\n");
	fprintf(matlab_plot, "		loop_start=get(h,'Value')+1;\n");
	fprintf(matlab_plot, "		loop_end=old_step;\n");
	fprintf(matlab_plot, "	elseif get(h,'Value')>old_step\n");
	fprintf(matlab_plot, "		loop_start=old_step+1;\n");
	fprintf(matlab_plot, "		loop_end=get(h,'Value');\n");	
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "	for i=loop_start:loop_end\n");
	fprintf(matlab_plot, "		x=x_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		y=y_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		z=z_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		flip=flip_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		energy=energy_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		if flip==1\n");
	fprintf(matlab_plot, "			s(x+1,y+1,z+1)=-s(x+1,y+1,z+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "			if s(x+1,y+1,z+1)==spin\n");
	fprintf(matlab_plot, "				if (x>0 && s(x,y+1,z+1)==-spin) || x==0\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (x<nx-1 && s(x+2,y+1,z+1)==-spin) || x==nx-1\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (y>0 && s(x+1,y,z+1)==-spin) || y==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (y<ny-1 && s(x+1,y+2,z+1)==-spin) || y==ny-1\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (z>0 && s(x+1,y+1,z)==-spin) || z==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (z<nz-1 && s(x+1,y+1,z+2)==-spin) || z==nz-1\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			else\n");
	fprintf(matlab_plot, "				if (x>0 && s(x,y+1,z+1)==-spin) || x==0\n");
	fprintf(matlab_plot, "					delete(hx(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (x<nx-1 && s(x+2,y+1,z+1)==-spin) || x==nx-1\n");
	fprintf(matlab_plot, "					delete(hx(x+2, y+1, z+1));\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (y>0 && s(x+1,y,z+1)==-spin) || y==0\n");
	fprintf(matlab_plot, "					delete(hy(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (y<ny-1 && s(x+1,y+2,z+1)==-spin) || y==ny-1\n");
	fprintf(matlab_plot, "					delete(hy(x+1, y+2, z+1));\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (z>0 && s(x+1,y+1,z)==-spin) || z==0\n");
	fprintf(matlab_plot, "					delete(hz(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (z<nz-1 && s(x+1,y+1,z+2)==-spin) || z==nz-1\n");
	fprintf(matlab_plot, "					delete(hz(x+1, y+1, z+2));\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "old_step=get(h,'Value');\n");
	
	return;
}

void animate_real_time_step(short int *spins, int *flips, double *energy, double *time, int steps_begin, int steps, int nx, int ny, int nz, short int spin, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot)
{
	int i;
	int x, y, z;
	
	fprintf(matlab_data, "%d\n%d\n%d\n%d\n%d\n%hd\n", nx, ny, nz, steps_begin, steps, spin);
	
	for (i=0; i<nx*ny*nz; i++)
	{
		fprintf(matlab_data, "%d\n", *(spins+i));
	}
	
	fprintf(matlab_data, "%d\n", (int)(*energy));
	
	for (i=0; i<steps; i++)
	{
		z=(int)(*(flips+2*i)/nx/ny);
		y=(int)((*(flips+2*i)-z*nx*ny)/nx);
		x=(int)(*(flips+2*i)-z*nx*ny-y*nx);

		fprintf(matlab_data, "%d %d %d\n", x, y, z);
		fprintf(matlab_data, "%d\n", *(flips+2*i+1));
		fprintf(matlab_data, "%d\n", (int)(*(energy+i+1)));
	}

	fprintf(matlab_plot, "function evolve_animate_step\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "global nx ny nz steps_begin steps spin old_step s hx hy hz x_array y_array z_array flip_array energy_array\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "plotdata=fopen('%s', 'r');\n", matlab_data_filename);
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "nx=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "ny=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "nz=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps_begin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "spin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "hx=zeros(nx+1, ny, nz);\n");
	fprintf(matlab_plot, "hy=zeros(nx, ny+1, nz);\n");
	fprintf(matlab_plot, "hz=zeros(nx, ny, nz+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "s=zeros(nx,ny,nz);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "figure\n");
	fprintf(matlab_plot, "axis([0 nx 0 ny 0 nz], 'equal')\n");
	fprintf(matlab_plot, "set(gca,'Units','centimeters')\n");
	fprintf(matlab_plot, "position=get(gcf,'Position');\n");
	fprintf(matlab_plot, "position(4)=500;\n");
	fprintf(matlab_plot, "set(gcf,'Position',position)\n");
	fprintf(matlab_plot, "xlabel('x')\n");
	fprintf(matlab_plot, "ylabel('y')\n");
	fprintf(matlab_plot, "zlabel('z')\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), steps (min, current, max)=(', num2str(steps_begin), ', ', num2str(steps_begin), ', ', num2str(steps_begin+steps), ')'])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "slider=uicontrol('Style','slider','Callback',@slider_callback,'Min',steps_begin,'Max',steps_begin+steps,'SliderStep',[1/steps 1],'Value',steps_begin,'Position',[64 400 384 20]);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for z=0:nz-1\n");
	fprintf(matlab_plot, "	for y=0:ny-1\n");
	fprintf(matlab_plot, "		for x=0:nx-1\n");
	fprintf(matlab_plot, "			s(x+1,y+1,z+1)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "			if s(x+1,y+1,z+1)==spin\n");
	fprintf(matlab_plot, "				if hx(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hx(x+2, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+2, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+2)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "x_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "y_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "z_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "flip_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "energy_array=zeros(steps,1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "old_step=steps_begin;\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for i=1:steps\n");
	fprintf(matlab_plot, "	x_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	y_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	z_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	flip_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "	energy_array(i)=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "function slider_callback(h, eventdata)\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "global nx ny nz steps_begin steps spin old_step s hx hy hz x_array y_array z_array flip_array energy_array\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "set(h,'Value',cast(get(h,'Value'),'int32'))\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), steps (min, current, max)=(', num2str(steps_begin), ', ', num2str(get(h,'Value')), ', ', num2str(steps_begin+steps), ')'])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "if get(h,'Value')~=old_step\n");
	fprintf(matlab_plot, "	if get(h,'Value')<old_step\n");
	fprintf(matlab_plot, "		loop_start=get(h,'Value')+1;\n");
	fprintf(matlab_plot, "		loop_end=old_step;\n");
	fprintf(matlab_plot, "	elseif get(h,'Value')>old_step\n");
	fprintf(matlab_plot, "		loop_start=old_step+1;\n");
	fprintf(matlab_plot, "		loop_end=get(h,'Value');\n");	
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "	for i=loop_start:loop_end\n");
	fprintf(matlab_plot, "		x=x_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		y=y_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		z=z_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		flip=flip_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		energy=energy_array(i-steps_begin);\n");
	fprintf(matlab_plot, "		if flip==1\n");
	fprintf(matlab_plot, "			s(x+1,y+1,z+1)=-s(x+1,y+1,z+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "			if s(x+1,y+1,z+1)==spin\n");
	fprintf(matlab_plot, "				if (x>0 && s(x,y+1,z+1)==-spin) || x==0\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (x<nx-1 && s(x+2,y+1,z+1)==-spin) || x==nx-1\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (y>0 && s(x+1,y,z+1)==-spin) || y==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (y<ny-1 && s(x+1,y+2,z+1)==-spin) || y==ny-1\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (z>0 && s(x+1,y+1,z)==-spin) || z==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (z<nz-1 && s(x+1,y+1,z+2)==-spin) || z==nz-1\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			else\n");
	fprintf(matlab_plot, "				if (x>0 && s(x,y+1,z+1)==-spin) || x==0\n");
	fprintf(matlab_plot, "					delete(hx(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (x<nx-1 && s(x+2,y+1,z+1)==-spin) || x==nx-1\n");
	fprintf(matlab_plot, "					delete(hx(x+2, y+1, z+1));\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (y>0 && s(x+1,y,z+1)==-spin) || y==0\n");
	fprintf(matlab_plot, "					delete(hy(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (y<ny-1 && s(x+1,y+2,z+1)==-spin) || y==ny-1\n");
	fprintf(matlab_plot, "					delete(hy(x+1, y+2, z+1));\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (z>0 && s(x+1,y+1,z)==-spin) || z==0\n");
	fprintf(matlab_plot, "					delete(hz(x+1, y+1, z+1));\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if (z<nz-1 && s(x+1,y+1,z+2)==-spin) || z==nz-1\n");
	fprintf(matlab_plot, "					delete(hz(x+1, y+1, z+2));\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=0;\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "old_step=get(h,'Value');\n");
	
	return;
}

/*-------------------------------------------------------------

	blinkers_plot

	Parameter			Type
	-------------------------------------------------------
	regions				short int*
	nx				int
	ny				int
	nz				int
	gnuplot_data_filename		char*
	gnuplot_data			FILE*
	gnuplot_plot			FILE*

	Plots the blinker regions.

-------------------------------------------------------------*/
void blinkers_plot(short int *regions, int nx, int ny, int nz, char gnuplot_data_filename[], FILE *gnuplot_data, FILE *gnuplot_plot)
{
	int x, y, z;
	int nxtimesny=nx*ny;
	
	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				if (*(regions+x+nx*y+nxtimesny*z)==1)
				{
					fprintf(gnuplot_data,"%d %d %d 0\n",0+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 0\n\n",0+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 0\n",0+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 0\n\n\n",0+x,1+y,1+z);

					fprintf(gnuplot_data,"%d %d %d 0\n",1+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 0\n\n",1+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 0\n",1+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 0\n\n\n",1+x,1+y,1+z);

					fprintf(gnuplot_data,"%d %d %d 6\n",0+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 6\n\n",1+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 6\n",0+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 6\n\n\n",1+x,0+y,1+z);

					fprintf(gnuplot_data,"%d %d %d 6\n",0+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 6\n\n",1+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 6\n",0+x,1+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 6\n\n\n",1+x,1+y,1+z);

					fprintf(gnuplot_data,"%d %d %d 10\n",0+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 10\n\n",0+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 10\n",1+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 10\n\n\n",1+x,1+y,0+z);

					fprintf(gnuplot_data,"%d %d %d 10\n",0+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 10\n\n",0+x,1+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 10\n",1+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 10\n\n\n",1+x,1+y,1+z);
				}
			}
		}
	}

	fprintf(gnuplot_plot, "set pm3d depthorder\n");
	fprintf(gnuplot_plot, "set palette rgbformulae 33,13,10\n");
	fprintf(gnuplot_plot, "set cbrange [0:10]\n");
	fprintf(gnuplot_plot, "set size 0.7,1.0\n");
	fprintf(gnuplot_plot, "unset colorbox\n");
	fprintf(gnuplot_plot, "unset key\n");
	fprintf(gnuplot_plot, "set xlabel 'x'\n");
	fprintf(gnuplot_plot, "set ylabel 'y'\n");
	fprintf(gnuplot_plot, "set zlabel 'z'\n");
	if (nx>=ny && nx>=nz)
	{
		fprintf(gnuplot_plot, "set xrange [0:%d]\n", nx);
		fprintf(gnuplot_plot, "set yrange [0:%d]\n", nx);
		fprintf(gnuplot_plot, "set zrange [0:%d]\n", nx);
	}
	else if (ny>=nz && ny>=nx)
	{
		fprintf(gnuplot_plot, "set xrange [0:%d]\n", ny);
		fprintf(gnuplot_plot, "set yrange [0:%d]\n", ny);
		fprintf(gnuplot_plot, "set zrange [0:%d]\n", ny);
	}
	else
	{
		fprintf(gnuplot_plot, "set xrange [0:%d]\n", nz);
		fprintf(gnuplot_plot, "set yrange [0:%d]\n", nz);
		fprintf(gnuplot_plot, "set zrange [0:%d]\n", nz);
	}
	fprintf(gnuplot_plot, "set ticslevel 0\n");
	fprintf(gnuplot_plot, "splot '%s' with pm3d\n", gnuplot_data_filename);

	return;
}

void blinkers_plot_matlab(short int *regions, int nx, int ny, int nz, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot)
{
	int x, y, z;
	int nxtimesny=nx*ny;
	
	fprintf(matlab_data, "%d\n%d\n%d\n", nx, ny, nz);
	
	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				fprintf(matlab_data, "%d\n", *(regions+x+nx*y+nxtimesny*z));
			}
		}
	}
	
	fprintf(matlab_plot, "plotdata=fopen('%s', 'r');\n", matlab_data_filename);
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "nx=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "ny=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "nz=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "hx=zeros(nx+1, ny, nz);\n");
	fprintf(matlab_plot, "hy=zeros(nx, ny+1, nz);\n");
	fprintf(matlab_plot, "hz=zeros(nx, ny, nz+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "figure\n");
	fprintf(matlab_plot, "axis([0 nx 0 ny 0 nz], 'equal')\n");
	fprintf(matlab_plot, "xlabel('x')\n");
	fprintf(matlab_plot, "ylabel('y')\n");
	fprintf(matlab_plot, "zlabel('z')\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), ')'])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for z=0:nz-1\n");
	fprintf(matlab_plot, "	for y=0:ny-1\n");
	fprintf(matlab_plot, "		for x=0:nx-1\n");
	fprintf(matlab_plot, "			if fscanf(plotdata, '%%d', 1)==1\n");
	fprintf(matlab_plot, "				if hx(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hx(x+2, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+2, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+2)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	
	return;
}

void cluster_spins_plot(short int *cluster_spins, int nx, int ny, int nz, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot)
{
	int x, y, z;
	int nxtimesny=nx*ny;
	
	fprintf(matlab_data, "%d\n%d\n%d\n", nx, ny, nz);
	
	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				fprintf(matlab_data, "%d\n", *(cluster_spins+x+nx*y+nxtimesny*z));
			}
		}
	}
	
	fprintf(matlab_plot, "plotdata=fopen('%s', 'r');\n", matlab_data_filename);
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "nx=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "ny=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "nz=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "hx=zeros(nx+1, ny, nz);\n");
	fprintf(matlab_plot, "hy=zeros(nx, ny+1, nz);\n");
	fprintf(matlab_plot, "hz=zeros(nx, ny, nz+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "figure\n");
	fprintf(matlab_plot, "axis([0 nx 0 ny 0 nz], 'equal')\n");
	fprintf(matlab_plot, "xlabel('x')\n");
	fprintf(matlab_plot, "ylabel('y')\n");
	fprintf(matlab_plot, "zlabel('z')\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), ')'])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for z=0:nz-1\n");
	fprintf(matlab_plot, "	for y=0:ny-1\n");
	fprintf(matlab_plot, "		for x=0:nx-1\n");
	fprintf(matlab_plot, "			if fscanf(plotdata, '%%d', 1)==1\n");
	fprintf(matlab_plot, "				if hx(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hx(x+2, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+2, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+2)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	
	return;
}

/*-------------------------------------------------------------

	state_plot_gnuplot

	Parameter			Type
	-------------------------------------------------------
	spins				short int*
	nx				int
	ny				int
	nz				int
	steps				int
	spin				short int
	gnuplot_data_filename		char*
	gnuplot_data			FILE*
	gnuplot_plot			FILE*

	Plots the lattice in 3D using gnuplot.

-------------------------------------------------------------*/
void state_plot_gnuplot(short int *spins, int nx, int ny, int nz, int steps, short int spin, char gnuplot_data_filename[], FILE *gnuplot_data, FILE *gnuplot_plot)
{
	int x, y, z;
	int nxtimesny=nx*ny;
	
	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				if (*(spins+x+nx*y+nxtimesny*z)==spin)
				{
					fprintf(gnuplot_data,"%d %d %d 0\n",0+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 0\n\n",0+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 0\n",0+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 0\n\n\n",0+x,1+y,1+z);

					fprintf(gnuplot_data,"%d %d %d 0\n",1+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 0\n\n",1+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 0\n",1+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 0\n\n\n",1+x,1+y,1+z);

					fprintf(gnuplot_data,"%d %d %d 6\n",0+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 6\n\n",1+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 6\n",0+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 6\n\n\n",1+x,0+y,1+z);

					fprintf(gnuplot_data,"%d %d %d 6\n",0+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 6\n\n",1+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 6\n",0+x,1+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 6\n\n\n",1+x,1+y,1+z);

					fprintf(gnuplot_data,"%d %d %d 10\n",0+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 10\n\n",0+x,1+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 10\n",1+x,0+y,0+z);
					fprintf(gnuplot_data,"%d %d %d 10\n\n\n",1+x,1+y,0+z);

					fprintf(gnuplot_data,"%d %d %d 10\n",0+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 10\n\n",0+x,1+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 10\n",1+x,0+y,1+z);
					fprintf(gnuplot_data,"%d %d %d 10\n\n\n",1+x,1+y,1+z);
				}
			}
		}
	}

	fprintf(gnuplot_plot, "set pm3d depthorder\n");
	fprintf(gnuplot_plot, "set palette rgbformulae 33,13,10\n");
	fprintf(gnuplot_plot, "set cbrange [0:10]\n");
	fprintf(gnuplot_plot, "set size 0.7,1.0\n");
	fprintf(gnuplot_plot, "unset colorbox\n");
	fprintf(gnuplot_plot, "unset key\n");
	fprintf(gnuplot_plot, "set xlabel 'x'\n");
	fprintf(gnuplot_plot, "set ylabel 'y'\n");
	fprintf(gnuplot_plot, "set zlabel 'z'\n");
	if (nx>=ny && nx>=nz)
	{
		fprintf(gnuplot_plot, "set xrange [0:%d]\n", nx);
		fprintf(gnuplot_plot, "set yrange [0:%d]\n", nx);
		fprintf(gnuplot_plot, "set zrange [0:%d]\n", nx);
	}
	else if (ny>=nz && ny>=nx)
	{
		fprintf(gnuplot_plot, "set xrange [0:%d]\n", ny);
		fprintf(gnuplot_plot, "set yrange [0:%d]\n", ny);
		fprintf(gnuplot_plot, "set zrange [0:%d]\n", ny);
	}
	else
	{
		fprintf(gnuplot_plot, "set xrange [0:%d]\n", nz);
		fprintf(gnuplot_plot, "set yrange [0:%d]\n", nz);
		fprintf(gnuplot_plot, "set zrange [0:%d]\n", nz);
	}
	fprintf(gnuplot_plot, "set ticslevel 0\n");
	fprintf(gnuplot_plot, "set title 'lattice=(%d, %d, %d), steps=%d\n", nx, ny, nz, steps);
	fprintf(gnuplot_plot, "splot '%s' with pm3d\n", gnuplot_data_filename);

	return;
}

/*-------------------------------------------------------------

	state_plot_matlab

	Parameter			Type
	-------------------------------------------------------
	spins				short int*
	nx				int
	ny				int
	nz				int
	steps				int
	spin				short int
	matlab_data_filename		char*
	matlab_data			FILE*
	matlab_plot			FILE*

	Plots the lattice in 3D using Matlab.

-------------------------------------------------------------*/
void state_plot_matlab(short int *spins, int nx, int ny, int nz, int steps, short int spin, char matlab_data_filename[], FILE *matlab_data, FILE *matlab_plot)
{
	int x, y, z;
	int nxtimesny=nx*ny;
	
	fprintf(matlab_data, "%d\n%d\n%d\n%d\n%hd\n", nx, ny, nz, steps, spin);
	
	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				fprintf(matlab_data, "%d\n", *(spins+x+nx*y+nxtimesny*z));
			}
		}
	}
	
	fprintf(matlab_plot, "plotdata=fopen('%s', 'r');\n", matlab_data_filename);
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "nx=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "ny=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "nz=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "steps=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "spin=fscanf(plotdata, '%%d', 1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "hx=zeros(nx+1, ny, nz);\n");
	fprintf(matlab_plot, "hy=zeros(nx, ny+1, nz);\n");
	fprintf(matlab_plot, "hz=zeros(nx, ny, nz+1);\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "figure\n");
	fprintf(matlab_plot, "axis([0 nx 0 ny 0 nz], 'equal')\n");
	fprintf(matlab_plot, "xlabel('x')\n");
	fprintf(matlab_plot, "ylabel('y')\n");
	fprintf(matlab_plot, "zlabel('z')\n");
	fprintf(matlab_plot, "title(['lattice=(', num2str(nx), ', ', num2str(ny), ', ', num2str(nz), '), steps=', num2str(steps)])\n");
	fprintf(matlab_plot, "\n");
	fprintf(matlab_plot, "for z=0:nz-1\n");
	fprintf(matlab_plot, "	for y=0:ny-1\n");
	fprintf(matlab_plot, "		for x=0:nx-1\n");
	fprintf(matlab_plot, "			if fscanf(plotdata, '%%d', 1)==spin\n");
	fprintf(matlab_plot, "				if hx(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+1, y+1, z+1)=patch([x x x x], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hx(x+2, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hx(x+2, y+1, z+1)=patch([x+1 x+1 x+1 x+1], [y y+1 y+1 y], [z z z+1 z+1], 'r');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+1, z+1)=patch([x x x+1 x+1], [y y y y], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hy(x+1, y+2, z+1)==0\n");
	fprintf(matlab_plot, "					hy(x+1, y+2, z+1)=patch([x x x+1 x+1], [y+1 y+1 y+1 y+1], [z z+1 z+1 z], 'g');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+1)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+1)=patch([x x+1 x+1 x], [y y y+1 y+1], [z z z z], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "				if hz(x+1, y+1, z+2)==0\n");
	fprintf(matlab_plot, "					hz(x+1, y+1, z+2)=patch([x x+1 x+1 x], [y y y+1 y+1], [z+1 z+1 z+1 z+1], 'b');\n");
	fprintf(matlab_plot, "				end\n");
	fprintf(matlab_plot, "			end\n");
	fprintf(matlab_plot, "		end\n");
	fprintf(matlab_plot, "	end\n");
	fprintf(matlab_plot, "end\n");
	
	return;
}

void xy_plot(double *x, double *y, int size, char gnuplot_data_filename[], FILE *gnuplot_data, FILE *gnuplot_plot)
{
	int i;

	for (i=0; i<size; i++)
	{
		fprintf(gnuplot_data, "%lf %lf\n", *(x+i), *(y+i));
	}

	fprintf(gnuplot_plot, "plot '%s'\n", gnuplot_data_filename);
	
	return;
}

