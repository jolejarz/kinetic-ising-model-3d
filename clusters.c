/**************************************************************
***************************************************************
*** 
***   clusters.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains routines for determining cluster statistics.
***
***************************************************************
**************************************************************/

#include "includes.h"

void clusters (struct evolve_parameters *parameters, int *lattice, short int *spins)
{
	int nx=parameters->nx, ny=parameters->ny, nz=parameters->nz;
	int nxtimesny=nx*ny;
	int nxtimesnytimesnz=nxtimesny*nz;
	
	int x, y, z;
	
	int xminus, yminus, zminus;
	
	int current_spin, test_spin;
	
	int test_spin_1, test_spin_2, test_spin_3;
	
	int i, j, k, n;
	
	int *find_spin, *find_cluster;
	
	int cluster_current, cluster_test;
	
	// Initialize 'lattice'.
	for (i=0; i<nxtimesnytimesnz; i++)
	{
		*(lattice+i)=i;
	}

	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				// Determinine the coordinates of the neighboring spins.
				// If the boundaries are free along a direction and the flipped spin is at a boundary
				// where there is no neighboring spin, the coordinates of the missing neighbor are set
				// to the flipped spin's coordinates. This signals that there are free boundaries,
				// and special processing is done.
				if (x>0) xminus=x-1; else xminus=parameters->periodic_bc_x==1 ? nx-1 : x;
				if (y>0) yminus=y-1; else yminus=parameters->periodic_bc_y==1 ? ny-1 : y;
				if (z>0) zminus=z-1; else zminus=parameters->periodic_bc_z==1 ? nz-1 : z;

				current_spin=x+nx*y+nxtimesny*z;

				// Compare the selected spin with its three neighboring spins at
				// (xminus, y, z), (x, yminus, z), and (x, y, zminus).
				//
				// If two spins are equal and they belong to the same cluster,
				// there is nothing to be done.
				//
				// If two spins are equal and neither already belongs to a cluster,
				// make a new cluster for both of them.
				//
				// If two spins are equal and only the test spin belongs to a cluster,
				// add the current spin to the test spin's cluster.
				//
				// If two spins are equal and only the current spin belongs to a cluster,
				// add the test spin to the current spin's cluster.
				//
				// If two spins are equal and they belong to different clusters,
				// make all spins in the test spin's cluster point to the current spin's
				// cluster.
				//
				for (i=0; i<3; i++)
				{
					switch (i)
					{
						case 0: {test_spin=xminus+nx*y+nxtimesny*z; break;}
						case 1: {test_spin=x+nx*yminus+nxtimesny*z; break;}
						case 2: {test_spin=x+nx*y+nxtimesny*zminus; break;}
					}
					
					// If there is no test spin due to free boundaries, do not do any processing.
					if (current_spin==test_spin) continue;
					
					if (*(spins+current_spin)==*(spins+test_spin))
					{
						// The current spin and test spin are equal.

						// Find the clusters of the two spins.
						for (n=0; n<2; n++)
						{
							switch (n)
							{
								case 0: {find_spin=&current_spin; find_cluster=&cluster_current; break;}
								case 1: {find_spin=&test_spin; find_cluster=&cluster_test; break;}
							}
							
							// Find the cluster of the spin.
							*find_cluster=*find_spin;
							while (*find_cluster!=*(lattice+*find_cluster)) *find_cluster=*(lattice+*find_cluster);

							// Compress the path from test_spin to j.
							j=*find_spin;
							while (j!=*(lattice+j))
							{
								k=*(lattice+j);
								*(lattice+j)=*find_cluster;
								j=k;
							}
						}

						// Make the current cluster point to the test cluster.
						*(lattice+cluster_current)=cluster_test;
					}
				}
			}
		}
	}
	
	// All clusters have been labeled.

	return;
}

// 'clusters' must be called before calling 'cluster_stats'.
void cluster_stats (struct evolve_parameters *parameters, struct cluster_stats *cluster_list, int *lattice, short int *spins)
{
	int i, j, k, number_of_clusters=0, size_of_cluster, nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
	
	// Initialize 'cluster_list' to all zeros.
	memset (cluster_list, 0, sizeof(struct cluster_stats)*nxtimesnytimesnz);
	
	// Determine which spins point to themselves.
	// The locations of these spins are the cluster labels.
	for (i=0; i<nxtimesnytimesnz; i++)
	{
		if (*(lattice+i)==i)
		{
			// Determine how many spins are in the cluster.
			size_of_cluster=0;
			for (j=0; j<nxtimesnytimesnz; j++)
			{
				k=j;
				while (k!=*(lattice+k)) k=*(lattice+k);
				if (k==i) size_of_cluster++;
			}
			
			// Save the cluster's stats.
			(cluster_list+number_of_clusters)->label=i;
			(cluster_list+number_of_clusters)->number_of_spins=size_of_cluster;
			(cluster_list+number_of_clusters)->sign=*(spins+i);
			
			// Increment the number of clusters.
			number_of_clusters++;
		}
	}
	
	return;
}

// 'cluster_stats' must be called before calling 'save_cluster_spins'.
void save_cluster_spins (struct evolve_parameters *parameters, short int *cluster_spins, int cluster_number, struct cluster_stats *cluster_list, int *lattice)
{
	// All spins in cluster 'cluster_number' are written to 'cluster_spins'.

	int nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
	
	int i, j, k, size_of_cluster=0;
	
	// Zero all elements of 'cluster_spins'.
	memset (cluster_spins, 0, sizeof(short int)*nxtimesnytimesnz);
	
	// Get the label for this cluster.
	int label=(cluster_list+cluster_number)->label;

	// Determine which spins are in the cluster.
	for (j=0; j<nxtimesnytimesnz; j++)
	{
		k=j;
		while (k!=*(lattice+k)) k=*(lattice+k);
		
		// If spin 'j' is in the cluster, then set it to '1'.
		if (k==label) *(cluster_spins+j)=1;
	}

	return;
}

// 'save_cluster_spins' must be called before calling 'topology_free_bc'.
void topology_free_bc (struct evolve_parameters *parameters, struct cluster_topology *cluster_topology, short int *cluster_spins)
{
	int nx=parameters->nx, ny=parameters->ny, nz=parameters->nz;
	int nxtimesny=nx*parameters->ny;
	int nxtimesnytimesnz=nxtimesny*parameters->nz;
	
	int x, y, z, xminus, yminus, zminus;
	
	int i;
	
	short int *vertex_counted=(short int*)malloc (sizeof(short int)*(nx+1)*(ny+1)*(nz+1));

	short int *edge_counted_x=(short int*)malloc (sizeof(short int)*nx*(ny+1)*(nz+1));
	short int *edge_counted_y=(short int*)malloc (sizeof(short int)*(nx+1)*ny*(nz+1));
	short int *edge_counted_z=(short int*)malloc (sizeof(short int)*(nx+1)*(ny+1)*nz);

	short int *face_counted_x=(short int*)malloc (sizeof(short int)*(nx+1)*ny*nz);
	short int *face_counted_y=(short int*)malloc (sizeof(short int)*nx*(ny+1)*nz);
	short int *face_counted_z=(short int*)malloc (sizeof(short int)*nx*ny*(nz+1));

	short int *edge_counted_array_1;
	short int *edge_counted_array_2;
	short int *edge_counted_array_3;
	short int *edge_counted_array_4;
	short int *face_counted_array;

	int vertex_counted_1, vertex_counted_2, vertex_counted_3, vertex_counted_4;
	int edge_counted_1, edge_counted_2, edge_counted_3, edge_counted_4;
	int face_counted_1;
	
	short int update;
	
	// Initialize the arrays.
	memset (vertex_counted, 0, sizeof(short int)*(nx+1)*(ny+1)*(nz+1));
	memset (edge_counted_x, 0, sizeof(short int)*nx*(ny+1)*(nz+1));
	memset (edge_counted_y, 0, sizeof(short int)*(nx+1)*ny*(nz+1));
	memset (edge_counted_z, 0, sizeof(short int)*(nx+1)*(ny+1)*nz);
	memset (face_counted_x, 0, sizeof(short int)*(nx+1)*ny*nz);
	memset (face_counted_y, 0, sizeof(short int)*nx*(ny+1)*nz);
	memset (face_counted_z, 0, sizeof(short int)*nx*ny*(nz+1));
	
	// Initialize the 'cluster_topology' structure.
	cluster_topology->vertices=0;
	cluster_topology->edges=0;
	cluster_topology->faces=0;

	// Loop through the lattice.
	// Spins that belong to the cluster have been set to '1'. All other spins have been set to '0'.
	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				// Determinine the coordinates of the neighboring spins.
				// If the flipped spin is at a boundary, the coordinates of the missing neighbor
				// are set to the flipped spin's coordinates. This signals that there are free
				// boundaries, and special processing is done.
				xminus= x>0 ? x-1 : x;
				yminus= y>0 ? y-1 : y;
				zminus= z>0 ? z-1 : z;

				for (i=0; i<6; i++)
				{
					// Reset 'update'.
					update=0;
					
					switch (i)
					{
						case 0:
						{
							if (x==nx-1 && *(cluster_spins+x+nx*y+nxtimesny*z)==1)
							{
								vertex_counted_1=nx+(nx+1)*y+(nx+1)*(ny+1)*z;
								vertex_counted_2=nx+(nx+1)*(y+1)+(nx+1)*(ny+1)*z;
								vertex_counted_3=nx+(nx+1)*y+(nx+1)*(ny+1)*(z+1);
								vertex_counted_4=nx+(nx+1)*(y+1)+(nx+1)*(ny+1)*(z+1);
								
								edge_counted_array_1=edge_counted_y;
								edge_counted_array_2=edge_counted_y;
								edge_counted_array_3=edge_counted_z;
								edge_counted_array_4=edge_counted_z;
								
								edge_counted_1=nx+(nx+1)*y+(nx+1)*ny*z;
								edge_counted_2=nx+(nx+1)*y+(nx+1)*ny*(z+1);
								edge_counted_3=nx+(nx+1)*y+(nx+1)*(ny+1)*z;
								edge_counted_4=nx+(nx+1)*(y+1)+(nx+1)*(ny+1)*z;
								
								face_counted_array=face_counted_x;
								
								face_counted_1=nx+(nx+1)*y+(nx+1)*ny*z;
								
								update=1;
							}
							break;
						}
						case 1:
						{
							if (y==ny-1 && *(cluster_spins+x+nx*y+nxtimesny*z)==1)
							{
								vertex_counted_1=x+(nx+1)*ny+(nx+1)*(ny+1)*z;
								vertex_counted_2=x+(nx+1)*ny+(nx+1)*(ny+1)*(z+1);
								vertex_counted_3=x+1+(nx+1)*ny+(nx+1)*(ny+1)*z;
								vertex_counted_4=x+1+(nx+1)*ny+(nx+1)*(ny+1)*(z+1);
								
								edge_counted_array_1=edge_counted_z;
								edge_counted_array_2=edge_counted_z;
								edge_counted_array_3=edge_counted_x;
								edge_counted_array_4=edge_counted_x;
								
								edge_counted_1=x+(nx+1)*ny+(nx+1)*(ny+1)*z;
								edge_counted_2=x+1+(nx+1)*ny+(nx+1)*(ny+1)*z;
								edge_counted_3=x+nx*ny+nx*(ny+1)*z;
								edge_counted_4=x+nx*ny+nx*(ny+1)*(z+1);
								
								face_counted_array=face_counted_y;
								
								face_counted_1=x+nxtimesny+nx*(ny+1)*z;
								
								update=1;
							}
							break;
						}
						case 2:
						{
							if (z==nz-1 && *(cluster_spins+x+nx*y+nxtimesny*z)==1)
							{
								vertex_counted_1=x+(nx+1)*y+(nx+1)*(ny+1)*nz;
								vertex_counted_2=x+1+(nx+1)*y+(nx+1)*(ny+1)*nz;
								vertex_counted_3=x+(nx+1)*(y+1)+(nx+1)*(ny+1)*nz;
								vertex_counted_4=x+1+(nx+1)*(y+1)+(nx+1)*(ny+1)*nz;
								
								edge_counted_array_1=edge_counted_x;
								edge_counted_array_2=edge_counted_x;
								edge_counted_array_3=edge_counted_y;
								edge_counted_array_4=edge_counted_y;
								
								edge_counted_1=x+nx*y+nx*(ny+1)*nz;
								edge_counted_2=x+nx*(y+1)+nx*(ny+1)*nz;
								edge_counted_3=x+(nx+1)*y+(nx+1)*ny*nz;
								edge_counted_4=x+1+(nx+1)*y+(nx+1)*ny*nz;
								
								face_counted_array=face_counted_z;
								
								face_counted_1=x+nx*y+nxtimesnytimesnz;
								
								update=1;
							}
							break;
						}
						case 3:
						{
							if ((x==xminus && *(cluster_spins+x+nx*y+nxtimesny*z)==1) || *(cluster_spins+xminus+nx*y+nxtimesny*z)!=*(cluster_spins+x+nx*y+nxtimesny*z))
							{
								vertex_counted_1=x+(nx+1)*y+(nx+1)*(ny+1)*z;
								vertex_counted_2=x+(nx+1)*(y+1)+(nx+1)*(ny+1)*z;
								vertex_counted_3=x+(nx+1)*y+(nx+1)*(ny+1)*(z+1);
								vertex_counted_4=x+(nx+1)*(y+1)+(nx+1)*(ny+1)*(z+1);
								
								edge_counted_array_1=edge_counted_y;
								edge_counted_array_2=edge_counted_y;
								edge_counted_array_3=edge_counted_z;
								edge_counted_array_4=edge_counted_z;
								
								edge_counted_1=x+(nx+1)*y+(nx+1)*ny*z;
								edge_counted_2=x+(nx+1)*y+(nx+1)*ny*(z+1);
								edge_counted_3=x+(nx+1)*y+(nx+1)*(ny+1)*z;
								edge_counted_4=x+(nx+1)*(y+1)+(nx+1)*(ny+1)*z;
								
								face_counted_array=face_counted_x;
								
								face_counted_1=x+(nx+1)*y+(nx+1)*ny*z;
								
								update=1;
							}
							break;
						}
						case 4:
						{
							if ((y==yminus && *(cluster_spins+x+nx*y+nxtimesny*z)==1) || *(cluster_spins+x+nx*yminus+nxtimesny*z)!=*(cluster_spins+x+nx*y+nxtimesny*z))
							{
								vertex_counted_1=x+(nx+1)*y+(nx+1)*(ny+1)*z;
								vertex_counted_2=x+(nx+1)*y+(nx+1)*(ny+1)*(z+1);
								vertex_counted_3=x+1+(nx+1)*y+(nx+1)*(ny+1)*z;
								vertex_counted_4=x+1+(nx+1)*y+(nx+1)*(ny+1)*(z+1);
								
								edge_counted_array_1=edge_counted_z;
								edge_counted_array_2=edge_counted_z;
								edge_counted_array_3=edge_counted_x;
								edge_counted_array_4=edge_counted_x;
								
								edge_counted_1=x+(nx+1)*y+(nx+1)*(ny+1)*z;
								edge_counted_2=x+1+(nx+1)*y+(nx+1)*(ny+1)*z;
								edge_counted_3=x+nx*y+nx*(ny+1)*z;
								edge_counted_4=x+nx*y+nx*(ny+1)*(z+1);
								
								face_counted_array=face_counted_y;
								
								face_counted_1=x+nx*y+nx*(ny+1)*z;
								
								update=1;
							}
							break;
						}
						case 5:
						{
							if ((z==zminus && *(cluster_spins+x+nx*y+nxtimesny*z)==1) || *(cluster_spins+x+nx*y+nxtimesny*zminus)!=*(cluster_spins+x+nx*y+nxtimesny*z))
							{
								vertex_counted_1=x+(nx+1)*y+(nx+1)*(ny+1)*z;
								vertex_counted_2=x+1+(nx+1)*y+(nx+1)*(ny+1)*z;
								vertex_counted_3=x+(nx+1)*(y+1)+(nx+1)*(ny+1)*z;
								vertex_counted_4=x+1+(nx+1)*(y+1)+(nx+1)*(ny+1)*z;
								
								edge_counted_array_1=edge_counted_x;
								edge_counted_array_2=edge_counted_x;
								edge_counted_array_3=edge_counted_y;
								edge_counted_array_4=edge_counted_y;
								
								edge_counted_1=x+nx*y+nx*(ny+1)*z;
								edge_counted_2=x+nx*(y+1)+nx*(ny+1)*z;
								edge_counted_3=x+(nx+1)*y+(nx+1)*ny*z;
								edge_counted_4=x+1+(nx+1)*y+(nx+1)*ny*z;
								
								face_counted_array=face_counted_z;
								
								face_counted_1=x+nx*y+nxtimesny*z;
								
								update=1;
							}
							break;
						}
					}

					// Check if 'update' is set to '1'.
					if (update==1)
					{
						// Update the numbers of vertices.
						if (*(vertex_counted+vertex_counted_1)==0)
						{
							(cluster_topology->vertices)++;
							(*(vertex_counted+vertex_counted_1))++;
						}
						if (*(vertex_counted+vertex_counted_2)==0)
						{
							(cluster_topology->vertices)++;
							(*(vertex_counted+vertex_counted_2))++;
						}
						if (*(vertex_counted+vertex_counted_3)==0)
						{
							(cluster_topology->vertices)++;
							(*(vertex_counted+vertex_counted_3))++;
						}
						if (*(vertex_counted+vertex_counted_4)==0)
						{
							(cluster_topology->vertices)++;
							(*(vertex_counted+vertex_counted_4))++;
						}
						
						// Update the numbers of edges.
						if (*(edge_counted_array_1+edge_counted_1)==0)
						{
							(cluster_topology->edges)++;
							(*(edge_counted_array_1+edge_counted_1))++;
						}
						if (*(edge_counted_array_2+edge_counted_2)==0)
						{
							(cluster_topology->edges)++;
							(*(edge_counted_array_2+edge_counted_2))++;
						}
						if (*(edge_counted_array_3+edge_counted_3)==0)
						{
							(cluster_topology->edges)++;
							(*(edge_counted_array_3+edge_counted_3))++;
						}
						if (*(edge_counted_array_4+edge_counted_4)==0)
						{
							(cluster_topology->edges)++;
							(*(edge_counted_array_4+edge_counted_4))++;
						}
						
						// Update the number of faces.
						if (*(face_counted_array+face_counted_1)==0)
						{
							(cluster_topology->faces)++;
							(*(face_counted_array+face_counted_1))++;
						}
					}
				}
			}
		}
	}

	// Get the Euler characteristic for the cluster.
	//
	// Euler characteristic = # of vertices - # of edges + # of faces
	//
	cluster_topology->euler_characteristic=
		cluster_topology->vertices
		-cluster_topology->edges
		+cluster_topology->faces;

	// Free memory.
	free (face_counted_z);
	free (face_counted_y);
	free (face_counted_x);
	free (edge_counted_z);
	free (edge_counted_y);
	free (edge_counted_x);
	free (vertex_counted);

	return;
}

// 'save_cluster_spins' must be called before calling 'topology_periodic_bc'.
void topology_periodic_bc (struct evolve_parameters *parameters, struct cluster_topology *cluster_topology, short int *cluster_spins)
{
	int nx=parameters->nx, ny=parameters->ny, nz=parameters->nz;
	int nxtimesny=nx*parameters->ny;
	int nxtimesnytimesnz=nxtimesny*parameters->nz;
	
	int x, y, z, xminus, yminus, zminus, xplus, yplus, zplus;
	
	int i;
	
	short int *vertex_counted=(short int*)malloc (sizeof(short int)*nx*ny*nz);

	short int *edge_counted_x=(short int*)malloc (sizeof(short int)*nx*ny*nz);
	short int *edge_counted_y=(short int*)malloc (sizeof(short int)*nx*ny*nz);
	short int *edge_counted_z=(short int*)malloc (sizeof(short int)*nx*ny*nz);

	short int *edge_counted_array_1;
	short int *edge_counted_array_2;
	short int *edge_counted_array_3;
	short int *edge_counted_array_4;

	int vertex_counted_1, vertex_counted_2, vertex_counted_3, vertex_counted_4;
	int edge_counted_1, edge_counted_2, edge_counted_3, edge_counted_4;
	
	short int update;
	
	// Initialize the arrays.
	memset (vertex_counted, 0, sizeof(short int)*nx*ny*nz);
	memset (edge_counted_x, 0, sizeof(short int)*nx*ny*nz);
	memset (edge_counted_y, 0, sizeof(short int)*nx*ny*nz);
	memset (edge_counted_z, 0, sizeof(short int)*nx*ny*nz);
	
	// Initialize the 'cluster_topology' structure.
	cluster_topology->vertices=0;
	cluster_topology->edges=0;
	cluster_topology->faces=0;

	// Loop through the lattice.
	// Spins that belong to the cluster have been set to '1'. All other spins have been set to '0'.
	for (z=0; z<nz; z++)
	{
		for (y=0; y<ny; y++)
		{
			for (x=0; x<nx; x++)
			{
				// Determinine the coordinates of the neighboring spins.
				
				xminus= x>0 ? x-1 : nx-1;
				yminus= y>0 ? y-1 : ny-1;
				zminus= z>0 ? z-1 : nz-1;
				
				xplus= x<nx-1 ? x+1 : 0;
				yplus= y<ny-1 ? y+1 : 0;
				zplus= z<nz-1 ? z+1 : 0;
				
				for (i=0; i<3; i++)
				{
					// Reset 'update'.
					update=0;
					
					switch (i)
					{
						case 0:
						{
							if (*(cluster_spins+xminus+nx*y+nxtimesny*z)!=*(cluster_spins+x+nx*y+nxtimesny*z))
							{
								vertex_counted_1=x+nx*y+nx*ny*z;
								vertex_counted_2=x+nx*yplus+nx*ny*z;
								vertex_counted_3=x+nx*y+nx*ny*zplus;
								vertex_counted_4=x+nx*yplus+nx*ny*zplus;
								
								edge_counted_array_1=edge_counted_y;
								edge_counted_array_2=edge_counted_y;
								edge_counted_array_3=edge_counted_z;
								edge_counted_array_4=edge_counted_z;
								
								edge_counted_1=x+nx*y+nx*ny*z;
								edge_counted_2=x+nx*y+nx*ny*zplus;
								edge_counted_3=x+nx*y+nx*ny*z;
								edge_counted_4=x+nx*yplus+nx*ny*z;
								
								update=1;
							}
							break;
						}
						case 1:
						{
							if (*(cluster_spins+x+nx*yminus+nxtimesny*z)!=*(cluster_spins+x+nx*y+nxtimesny*z))
							{
								vertex_counted_1=x+nx*y+nx*ny*z;
								vertex_counted_2=x+nx*y+nx*ny*zplus;
								vertex_counted_3=xplus+nx*y+nx*ny*z;
								vertex_counted_4=xplus+nx*y+nx*ny*zplus;
								
								edge_counted_array_1=edge_counted_z;
								edge_counted_array_2=edge_counted_z;
								edge_counted_array_3=edge_counted_x;
								edge_counted_array_4=edge_counted_x;
								
								edge_counted_1=x+nx*y+nx*ny*z;
								edge_counted_2=xplus+nx*y+nx*ny*z;
								edge_counted_3=x+nx*y+nx*ny*z;
								edge_counted_4=x+nx*y+nx*ny*zplus;
								
								update=1;
							}
							break;
						}
						case 2:
						{
							if (*(cluster_spins+x+nx*y+nxtimesny*zminus)!=*(cluster_spins+x+nx*y+nxtimesny*z))
							{
								vertex_counted_1=x+nx*y+nx*ny*z;
								vertex_counted_2=xplus+nx*y+nx*ny*z;
								vertex_counted_3=x+nx*yplus+nx*ny*z;
								vertex_counted_4=xplus+nx*yplus+nx*ny*z;
								
								edge_counted_array_1=edge_counted_x;
								edge_counted_array_2=edge_counted_x;
								edge_counted_array_3=edge_counted_y;
								edge_counted_array_4=edge_counted_y;
								
								edge_counted_1=x+nx*y+nx*ny*z;
								edge_counted_2=x+nx*yplus+nx*ny*z;
								edge_counted_3=x+nx*y+nx*ny*z;
								edge_counted_4=xplus+nx*y+nx*ny*z;
								
								update=1;
							}
							break;
						}
					}

					// Check if 'update' is set to '1'.
					if (update==1)
					{
						// Update the numbers of vertices.
						if (*(vertex_counted+vertex_counted_1)==0)
						{
							(cluster_topology->vertices)++;
							(*(vertex_counted+vertex_counted_1))++;
						}
						if (*(vertex_counted+vertex_counted_2)==0)
						{
							(cluster_topology->vertices)++;
							(*(vertex_counted+vertex_counted_2))++;
						}
						if (*(vertex_counted+vertex_counted_3)==0)
						{
							(cluster_topology->vertices)++;
							(*(vertex_counted+vertex_counted_3))++;
						}
						if (*(vertex_counted+vertex_counted_4)==0)
						{
							(cluster_topology->vertices)++;
							(*(vertex_counted+vertex_counted_4))++;
						}
						
						// Update the numbers of edges.
						if (*(edge_counted_array_1+edge_counted_1)==0)
						{
							(cluster_topology->edges)++;
							(*(edge_counted_array_1+edge_counted_1))++;
						}
						if (*(edge_counted_array_2+edge_counted_2)==0)
						{
							(cluster_topology->edges)++;
							(*(edge_counted_array_2+edge_counted_2))++;
						}
						if (*(edge_counted_array_3+edge_counted_3)==0)
						{
							(cluster_topology->edges)++;
							(*(edge_counted_array_3+edge_counted_3))++;
						}
						if (*(edge_counted_array_4+edge_counted_4)==0)
						{
							(cluster_topology->edges)++;
							(*(edge_counted_array_4+edge_counted_4))++;
						}
						
						// Update the number of faces.
						(cluster_topology->faces)++;
					}
				}
			}
		}
	}

	// Get the Euler characteristic for the cluster.
	//
	// Euler characteristic = # of vertices - # of edges + # of faces
	//
	cluster_topology->euler_characteristic=
		cluster_topology->vertices
		-cluster_topology->edges
		+cluster_topology->faces;

	// Free memory.
	free (edge_counted_z);
	free (edge_counted_y);
	free (edge_counted_x);
	free (vertex_counted);

	return;
}

