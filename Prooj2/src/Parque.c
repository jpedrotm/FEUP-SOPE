#include "util.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_NAME_SIZE 15
#define OG_RW_PERMISSION 0660

int endTime = 0;
long int parkingSpace = 0;
int closedPark = 0;

/*
criar o seu FIFO próprio (identificado por “fifo?”, onde '?' será ou  ou E,
 ou S ou O);
receber pedidos de acesso através do seu FIFO; cada pedido inclui os seguintes
dados:
-->porta de entrada;
-->tempo de estacionamento (em clock ticks);
-->número identificador único da viatura (inteiro);
-->nome do FIFO privado para a comunicação com o thread“viatura” do programa
Gerador.
 --> criar     um     thread “arrumador”  para  cada  pedido  de  entrada
recebido e  passar-lhe  a  informação  correspondente a esse pedido;
 estar  atento  a  uma  condição  de  terminação  (correspondendo  à  passagem
do T_ABERTURA  do  Parque) e, nessa altura, ler todos os pedidos pendentes no
seu FIFO e fechá-lo para que potenciais novos  clientes  de  estacionamento
sejam notificados  do  encerramento  do  Parque;  encaminhar  os  últimos
pedidos a correspondentes thread“arrumador”.
*/
void *controlador(void *arg) {

  printf("Thread %s \n", (char *)arg);

  char *fifoPath = malloc(sizeof(char) * FIFO_NAME_SIZE);
  sprintf(fifoPath, "/tmp/fifo%c", (*(char *)arg));

  int desFifo;

  if (mkfifo(fifoPath, OG_RW_PERMISSION) != 0) {
    perror("FIFO CONTROLER: ");
    return NULL;
  }
  // TODO REVER OPEN DO FIFO
  if ((desFifo = open(fifoPath, O_RDONLY | O_NONBLOCK)) == -1) {
    perror("FIFO OPEN FAILED: ");
    unlink(fifoPath);
    return NULL;
  }

  while (!endTime) {

    vehicle nova;

    if (read(desFifo, &nova, sizeof(nova)) > 0) {

      printf("CARRO: %d\n Time: %d\n acesso: %c\n path:  \n\n", nova.id,
             nova.t_parking, nova.access);
    } // else
    //  break;
  }

  unlink(fifoPath); // TODO CHECK UNLINK
  free(fifoPath);
  return NULL;
}
int main(int argc, char const *argv[]) {

  if (argc != 3) {
    printf("Wrong number of arguements.\n Usage: parque <N_lugares> "
           "<T_abertura> \n");
    return 1;
  }

  errno = 0;
  double worktime = strtol(argv[1], NULL, 10);
  if (errno == ERANGE || errno == EINVAL) {
    perror("convert working time failed");
  }

  errno = 0;
  parkingSpace = strtol(argv[2], NULL, 10);
  if (errno == ERANGE || errno == EINVAL) {
    perror("convert parking space failed");
  }

  // TODO

  pthread_t N, S, E, O;

  clock_t begin = clock();
  double elapsed = 0;
  // \DUVIDA Ao "matar" os threads exit() vs pthread_cancel TODO
  if (pthread_create(&N, NULL, controlador, "N") != 0) {
    perror("Thread N: ");
    pthread_exit(0);
  }
  if (pthread_create(&S, NULL, controlador, "S") != 0) {
    perror("Thread S: ");
    pthread_cancel(N);
    pthread_exit(0);
  }
  if (pthread_create(&E, NULL, controlador, "E") != 0) {
    perror("Thread E: ");
    pthread_cancel(N);
    pthread_cancel(S);
    pthread_exit(0);
  }
  if (pthread_create(&O, NULL, controlador, "O") != 0) {
    perror("Thread O: ");
    pthread_cancel(N);
    pthread_cancel(S);
    pthread_cancel(E);
    pthread_exit(0);
  }

  do {
    clock_t end = clock();
    elapsed = (double)((end - begin) / CLOCKS_PER_SEC);
    // printf("%d", (int)elapsed);
  } while (elapsed < worktime);
  endTime = 1;
  printf("work %d", (int)worktime);
  // End Time
  if (pthread_join(N, NULL) != 0) {
    perror("threadN : ");
  }
  if (pthread_join(S, NULL) != 0) {
    perror("threadS : ");
  }
  if (pthread_join(E, NULL) != 0) {
    perror("threadE : ");
  }
  if (pthread_join(O, NULL) != 0) {
    perror("threadO : ");
  }
  printf("%d\n", endTime);
  printf("%d\n", (int)elapsed);
  printf("%s\n", "End Main");

  pthread_exit(0);
}
