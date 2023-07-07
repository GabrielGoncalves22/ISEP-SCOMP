#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "shared_data_type.h"

#define ERROR_VALUE -1
#define SHARED_MEMORY_NAME "/ex03"

int main() {
	int data_size = sizeof(shared_data_type);
	int num, total = 0;

	/* Abre a zona de memória partilhada já criada */
	int fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, S_IRUSR|S_IWUSR);
	if (fd == ERROR_VALUE) {
		perror("An error occurred while trying to create and open a shared memory area!\n");
		exit(EXIT_FAILURE);
	}

    /* Se não der erro no ftruncate define o tamanho da área de memória */
	if (ftruncate(fd, data_size) == ERROR_VALUE) {
		perror("An error occurred while trying to define the size of the area!\n");
		exit(EXIT_FAILURE);
	}
	
	/* Pointer para a memória partilhada */
	shared_data_type *shared_data = (shared_data_type*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (shared_data == MAP_FAILED) {
        perror("An error occurred while trying to map a shared memory object in the process address space!\n");
        exit(EXIT_FAILURE);
	}

	/* Enquanto não pode ler, fica em espera ativa */
	while (!shared_data->can_read);

    /* Soma todos os valores contidos no array */
	printf("Array values: \n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
		num = shared_data->numbers[i];
		total += num;

		printf("%d\n", num);
    }

	printf("Array average: %.2f\n", (float) total / ARRAY_SIZE);


    /* Se não der erro desconecta o pointer da memória partilhada */
	if (munmap(shared_data, data_size) == ERROR_VALUE) {
		perror("An error occurred while trying to disconnect the shared memory area from the process address space!\n");
        exit(EXIT_FAILURE);
    }

    /* Se não der erro fecha a zona de memória partilhada */
	if (close(fd) == ERROR_VALUE) {
		perror("An error occurred while trying to close the file descriptor!\n");
        exit(EXIT_FAILURE);
    }

	/* Se não der erro remove a zona de memória partilhada */
	if (shm_unlink(SHARED_MEMORY_NAME) == ERROR_VALUE) {
		perror("An error occurred while trying to remove memory area from file system!\n");
        exit(EXIT_FAILURE);
    }

	return 0;
}