#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int id = 1;
int endTime = 0;
int generatorTime = 0;

void alarm_handler(int sig) { endTime = 1; }

struct vehicle_t {
  char access;
  int id;
  int t_parking;
};

void waitTime(clock_t elapsed) {
  int begin = clock();

  int end = clock();
  while ((begin - end) >= elapsed) {
    end = clock();
  }
}
void *vehicle(void *arg) { return NULL; }

void *mainThread(void *arg) {

  // Gerar acesso --------------------------------------------
  signal(SIGALRM, alarm_handler);
  alarm(generatorTime);

  struct vehicle_t nova;

  while (1) {
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

    // Gerar tempo de estacionamento e número identificador
    int waitT = 0;
    if (endTime == 0) {
      int timeParking = 0;
      int r = rand() % 10 + 1;
      timeParking = r * (*(int *)arg);
      nova.t_parking = timeParking;

      if (r <= 5) {
        waitT = 0;
      } else if (r > 5 && r <= 8) {
        waitT = (*(int *)arg);
      } else if (r > 8) {
        waitT = 2 * (*(int *)arg);
      }
      waitTime(waitT * (*(int *)arg));
    } else
      break;

    printf("CARRO: %d\n", nova.id);
    printf("Time: %d\n", nova.t_parking);
    printf("waitTime: %d\n", waitT);
    printf("acesso: %d\n", nova.access);
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

  if (errno == ERANGE || errno == EINVAL) {
    perror("convert working time failed");
  }

  errno = 0;

  int uRelogio = strtol(argv[2], NULL, 10);

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
