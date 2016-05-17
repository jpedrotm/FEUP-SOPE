#include "util.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIFO_NAME_BUFF 30
#define OG_RW_PERMISSION 0660

int id = 1;

vehicle create_vehicle(int uTime) {
  vehicle nova;
  nova.id = id;
  id++;

  int option = rand() % 4;
  char access = 'N';

  switch (option) {
  case 0:
    access = 'N';
    break;
  case 1:
    access = 'S';
    break;
  case 2:
    access = 'O';
    break;
  case 3:
    access = 'E';
    break;
  default:
    break;
  }

  nova.access = access;

  int r = rand() % 10 + 1;
  nova.t_parking = r * uTime;

  return nova;
};
void waitTime(clock_t elapsed) {
  clock_t begin = clock();
  clock_t end = clock();

  while ((end - begin) < elapsed) {
    end = clock();
  }
}
void *vehicleThread(void *arg) {

  if (pthread_detach(pthread_self()) != 0) {
    perror("Vehicle thread: ");
    free(arg);
  }
  vehicle *nova = (vehicle *)(arg);
  char a[200];
  // sprintf(nova->fifoPat, "/tmp/fifo%d", nova->id);
  sprintf(a, "/tmp/fifo%d", nova->id);
  printf("CARRO: %d\n Time: %d\n acesso: %c\n", nova->id, (int)nova->t_parking,
         nova->access);

  mkfifo(a, OG_RW_PERMISSION);
  int fd;
  // TODO FALTA OPEN VEICULO FIFO

  switch (nova->access) {
  case 'N':
    if ((fd = open("/tmp/fifoN", O_WRONLY)) == -1) {
      perror("Vehicle thread Cfifo: ");
    }
    break;
  case 'E':
    if ((fd = open("/tmp/fifoE", O_WRONLY)) == -1) {
      perror("Vehicle thread Cfifo: ");
    }
    break;
  case 'S':
    if ((fd = open("/tmp/fifoS", O_WRONLY)) == -1) {
      perror("Vehicle thread Cfifo: ");
    }
    break;
  case 'O':
    if ((fd = open("/tmp/fifoO", O_WRONLY)) == -1) {
      perror("Vehicle thread Cfifo: ");
    }
    break;
  }
  //  printf("Depois do switch \n");
  if (fd != -1) {
    write(fd, nova, sizeof(*nova));
  }

  close(fd);
  free(nova);
  // unlink(nova->fifoPath);
  unlink(a);
  return NULL;
}
int main(int argc, char const *argv[]) {

  if (argc != 3) {
    fprintf(stderr, "Wrong number of arguements.\n Usage: parque <T_GERACAO> "
                    "<U_RELOGIO> \n");
    return 1;
  }

  // Guardar variáveis ---------------------------------------------
  srand(time(NULL));

  errno = 0;

  double generatorTime = strtol(argv[1], NULL, 10);
  if (errno == ERANGE || errno == EINVAL) {
    perror("convert working time failed");
  }

  errno = 0;

  int uRelogio = strtol(argv[2], NULL, 10);

  if (errno == ERANGE || errno == EINVAL) {
    perror("convert parking space failed");
  }

  // TODO

  // Gerar acesso --------------------------------------------
  clock_t begin = clock();
  clock_t end = clock();
  double elapsed = 0;
  while (elapsed < generatorTime) {

    // Gerar tempo de estacionamento e número identificador

    int r = rand() % 10 + 1;

    int waitT = 0;
    if (r > 5 && r <= 8) {
      waitT = uRelogio;
    } else if (r > 8) {
      waitT = 2 * uRelogio;
    }
    pthread_t init;
    vehicle *new = malloc(sizeof(vehicle));
    *new = create_vehicle(uRelogio);
    pthread_create(&init, NULL, vehicleThread, new); // TODO

    waitTime(waitT);

    end = clock();
    elapsed = (double)(end - begin) / CLOCKS_PER_SEC;
  }
  printf("%ld\n", elapsed);
  printf("%s\n", "End Main");

  exit(0);
}
