#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>

const char SEMNAME[] = "/sync3";

static sem_t *sem1;

typedef struct {
  char access;
  int id;
  clock_t t_parking;
  //  char fifoPath[256]; // TODO
} vehicle;

sem_t *initSem(const char *semName) {
  sem_t *sem = NULL;
  int exits = 0;
  if ((sem = sem_open(semName, O_CREAT | O_EXCL, S_IWUSR | S_IRUSR, 1)) ==
      SEM_FAILED) {

    if (errno == EEXIST) {
      exits = 1;
    } else {
      perror("Creating semaphore");
      return SEM_FAILED;
    }
  }

  if (exits) {
    if ((sem = sem_open(semName, 0)) == SEM_FAILED) {
      perror("Opening semaphore");
      return SEM_FAILED;
    }
  }

  printf("%d\n", (int)sem);
  return sem;
}

int closeSem(sem_t *sem, const char *semName) {
  if (sem_close(sem) == -1) {
    perror("Close Semaphore: ");
    return -1;
  }
  if (sem_unlink(semName) == -1) {
    return -1;
  }
  return 0;
}

void waitTime(clock_t elapsed) {
  clock_t begin = clock();
  clock_t end = clock();

  while ((end - begin) < elapsed) {
    end = clock();
  }
}
