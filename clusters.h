/**************************************************************
***************************************************************
*** 
***   clusters.h
***
***   Jason Olejarz
***   2010-08-18
***
***   This is the header file for cluster.c.
***
***************************************************************
**************************************************************/

struct cluster_stats
{
	int label;
	int number_of_spins;
	short int sign;
};

struct cluster_topology
{
	int vertices;
	int edges;
	int faces;
	int euler_characteristic;
};

void clusters (struct evolve_parameters *parameters, int *lattice, short int *spins);
void cluster_stats (struct evolve_parameters *parameters, struct cluster_stats *cluster_list, int *lattice, short int *spins);
void save_cluster_spins (struct evolve_parameters *parameters, short int *cluster_spins, int cluster_number, struct cluster_stats *cluster_list, int *lattice);
void topology_free_bc (struct evolve_parameters *parameters, struct cluster_topology *cluster_topology, short int *cluster_spins);
void topology_periodic_bc (struct evolve_parameters *parameters, struct cluster_topology *cluster_topology, short int *cluster_spins);

