#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>

#define ROWS 3
#define COLS 50
#define SIZE (ROWS * COLS)

void display(int floorNo, int *floor, int freeSlots, int time) {
    printf("|   %d    |", floorNo);
    for (int i = 0; i < COLS; i++) {
        printf("%3d", floor[i]);
    }
	printf("|   %2d     |   %3d  |\n", freeSlots, time);
	printf("|--------+------------------------------------------------------------------------------------------------------------------------------------------------------|----------|--------|\n");
}

int main(int argc, char *argv[]) {
    int numtasks, rank, i, tag = 1, dims[2] = {ROWS, COLS}, periods[2] = {0, 0}, reorder = 0, coords[2];
    int slot = 0;

    MPI_Comm cartcomm;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks == SIZE) {
        MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cartcomm);
        MPI_Comm_rank(cartcomm, &rank);
        MPI_Cart_coords(cartcomm, rank, 2, coords);

		int time = 0;
        srand(rank);
		if(rank == 0) {
			printf("+--------+------------------------------------------------------------------------------------------------------------------------------------------------------+----------+--------+\n");

			printf("|Floor   |");
			for(i = 1; i <= COLS; i++)
				printf("%3d", i);
			printf("|Free Slots|  Time  |\n");
			printf("|--------+------------------------------------------------------------------------------------------------------------------------------------------------------|----------|--------|\n");
        }

		while (time < 999) {

            slot = rand() % 2;

            if (coords[1] == 0) {
                int *floor = (int *)malloc(COLS * sizeof(int));
                floor[0] = slot;

				int freeSlots = floor[0];

                for (i = 1; i < COLS; i++) {
                    MPI_Recv(&floor[i], 1, MPI_INT, coords[0] * 50 + i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                	freeSlots += floor[i];
				}


				if(time % 15 == 0) {

					if(coords[0] == 0) {
						printf("|        |                                                                                                                                                      |          |        |\n");
						printf("|--------+------------------------------------------------------------------------------------------------------------------------------------------------------|----------|--------|\n");
					}

	                display(coords[0], floor, freeSlots, time);

				}
                free(floor);
            } else {
                MPI_Send(&slot, 1, MPI_INT, coords[0] * 50, tag, MPI_COMM_WORLD);
            }

			time += 5;
            sleep(1);
        }
    } else {
        printf("Must specify %d tasks. Terminating.\n", SIZE);
    }

    MPI_Finalize();
    return 0;
}

