#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIFO_NAME_BUFF 30
#define OG_RW_PERMISSION 0660

int id = 1;
int endTime = 0;
int generatorTime = 0;

void alarm_handler(int sig) { endTime = 1; }

typedef struct {
  char access;
  int id;
  clock_t t_parking;
  char fifoPath[256];
} vehicle;

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

  clock_t timeParking = 0;

  int r = rand() % 10 + 1;

  timeParking = r * uTime;

  nova.t_parking = timeParking;

  return nova;
};
void waitTime(clock_t elapsed) {
  int begin = clock();

  int end = clock();
  while ((end - begin) >= elapsed) {
    end = clock();
  }
}
void *vehicleThread(void *arg) {

  if (pthread_detach(pthread_self()) != 0) {
    perror("Vehicle thread: ");
    free(arg);
  }
  vehicle *nova = (vehicle *)(arg);

  sprintf(nova->fifoPath, "/tmp/fifo%d", nova->id);
  //  printf("CARRO: %d\n Time: %d\n acesso: %c\n", nova->id, nova->t_parking,
  //         nova->access);

  mkfifo(nova->fifoPath, OG_RW_PERMISSION);
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

  if (fd != -1) {
    write(fd, &nova, sizeof(nova));
  }

  close(fd);
  free(nova);
  unlink(nova->fifoPath);
  return NULL;
}

void *mainThread(void *arg) {

  int uTime = (*(int *)arg);

  // Gerar acesso --------------------------------------------
  signal(SIGALRM, alarm_handler);
  alarm(generatorTime);

  while (endTime == 0) {

    // Gerar tempo de estacionamento e número identificador
    int waitT = 0;

    pthread_t init;
    int r = rand() % 10 + 1;

    if (r <= 5) {
      waitT = 0;
    } else if (r > 5 && r <= 8) {
      waitT = uTime;
    } else if (r > 8) {
      waitT = 2 * uTime;
    }

    vehicle *new = malloc(sizeof(vehicle));
    *new = create_vehicle(uTime);
    pthread_create(&init, NULL, vehicleThread, new);

    waitTime(waitT * uTime);
  }
  return NULL;
}

int main(int argc, char const *argv[]) {

  if (argc != 3) {
    fprintf(stderr, "Wrong number of arguements.\n Usage: parque <T_GERACAO> "
                    "<U_RELOGIO> \n");
    return 1;
  }

  // Guardar variáveis ---------------------------------------------
  pthread_t init;

  srand(time(NULL));

  errno = 0;

  generatorTime = strtol(argv[1], NULL, 10);
  printf("%d\n", generatorTime);
  if (errno == ERANGE || errno == EINVAL) {
    perror("convert working time failed");
  }

  errno = 0;

  int uRelogio = strtol(argv[2], NULL, 10);

  printf("%d\n", uRelogio);
  if (errno == ERANGE || errno == EINVAL) {
    perror("convert parking space failed");
  }

  // TODO

  if (pthread_create(&init, NULL, mainThread, &uRelogio) != 0) {
    perror("mainThread: ");
    return 2;
  }
  if (pthread_join(init, NULL) != 0) {
    perror("mainThread : ");
  }
  printf("%s\n", "End Main");

  return 0;
}
