/**************************************************************
***************************************************************
*** 
***   cmd-clusters.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains commands for calculating cluster statistics.
***
***************************************************************
**************************************************************/

void cmd_clusters()
{
	// Boundary conditions must be either free in all directions or
	// periodic in all directions.
	if (	!(parameters.periodic_bc_x==0 && parameters.periodic_bc_y==0 && parameters.periodic_bc_z==0) &&
		!(parameters.periodic_bc_x==1 && parameters.periodic_bc_y==1 && parameters.periodic_bc_z==1))
	{
		printf("Boundary conditions must be either free in all directions or periodic in all directions.\n");
		return;
	}
	
	int nxtimesnytimesnz=parameters.nx*parameters.ny*parameters.nz;
	
	int i, j, k;

	int number_of_clusters=0, size_of_cluster;
	
	int *lattice=(int*)malloc (sizeof(int)*nxtimesnytimesnz);
	
	struct cluster_stats *cluster_list=(struct cluster_stats*)malloc (sizeof(struct cluster_stats)*nxtimesnytimesnz);
	
	short int *cluster_spins=(short int*)malloc (sizeof(short int)*nxtimesnytimesnz);
	
	struct cluster_topology cluster_topology;
	
	// Label the clusters.
	clusters (&parameters, lattice, parameters.spins);
	
	// Get cluster statistics.
	cluster_stats (&parameters, cluster_list, lattice, parameters.spins);
	
	printf("cluster statistics:\n\n");
	
	// There must be at least one cluster.
	i=0;
	do
	{
		// Get the spins that are in the cluster.
		save_cluster_spins (&parameters, cluster_spins, i, cluster_list, lattice);
		
		// Get the topology of the cluster.
		if (parameters.periodic_bc_x==0 && parameters.periodic_bc_y==0 && parameters.periodic_bc_z==0)
		{
			topology_free_bc (&parameters, &cluster_topology, cluster_spins);
		}
		else if (parameters.periodic_bc_x==1 && parameters.periodic_bc_y==1 && parameters.periodic_bc_z==1)
		{
			topology_periodic_bc (&parameters, &cluster_topology, cluster_spins);
		}
		
		printf ("  cluster number:            %d\n", i);
		printf ("  sign:                      %d\n", (cluster_list+i)->sign);
		printf ("  number of spins:           %d\n", (cluster_list+i)->number_of_spins);
		printf ("  vertices / edges / faces:  %d / %d / %d\n", cluster_topology.vertices, cluster_topology.edges, cluster_topology.faces);
		printf ("  Euler characteristic:      %d\n", cluster_topology.euler_characteristic);
		printf ("\n");
	}
	while (i<nxtimesnytimesnz-1 && (cluster_list+(++i))->number_of_spins>0);

	return;
}

