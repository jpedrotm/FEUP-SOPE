#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_NAME_SIZE 6
#define OG_RW_PERMISSION 0660

int endTime = 0;
long int parkingSpace = 0;
int closedPark = 0;

void alarm_handler(int sig) { endTime = 1; }
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

  char fifoName[FIFO_NAME_SIZE] = {'f', 'i', 'f', 'o', ((char *)arg)[0], '\0'};
  int desFifo;

  if (mkfifo(fifoName, OG_RW_PERMISSION) != 0) {
    perror("FIFO CONTROLER: ");
    return NULL;
  }
  // TODO REVER OPEN DO FIFO
  if ((desFifo = open(fifoName, O_RDONLY | O_NONBLOCK)) == -1) {
    perror("FIFO OPEN FAILED: ");
    unlink(fifoName);
  }

  unlink(fifoName); // TODO CHECK UNLINK
  return NULL;
}

/*
->inicializa  as  variáveis  globais  necessárias,  incluindo  o  temporizador
geral  que  controla  o  tempo  de
  abertura do Parque
->cria os 4 threads “controlador”, um para cada acesso, e aguarda que eles
terminem;
->efetua e publica estatísticas globais.
*/
void *threadPricipal(void *arg) {

  pthread_t N, S, E, O;
  signal(SIGALRM, alarm_handler);
  alarm((*(long int *)arg));
  // DUVIDA Ao "matar" os threads exit() vs pthread_cancel TODO
  if (pthread_create(&N, NULL, controlador, "N") != 0) {
    perror("Thread N: ");
    return NULL;
  }
  if (pthread_create(&S, NULL, controlador, "S") != 0) {
    perror("Thread S: ");
    pthread_cancel(N);
    return NULL;
  }
  if (pthread_create(&E, NULL, controlador, "E") != 0) {
    perror("Thread E: ");
    pthread_cancel(N);
    pthread_cancel(S);
    return NULL;
  }
  if (pthread_create(&O, NULL, controlador, "O") != 0) {
    perror("Thread O: ");
    pthread_cancel(N);
    pthread_cancel(S);
    pthread_cancel(E);
    return NULL;
  }
  // End Time
  while (1) {
    if (endTime == 1) {
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
      break;
    }
  }
  return NULL;
}

int main(int argc, char const *argv[]) {

  if (argc != 3) {
    printf("Wrong number of arguements.\n Usage: parque <N_lugares> "
           "<T_abertura> \n");
    return 1;
  }

  pthread_t init;

  errno = 0;

  long int worktime = strtol(argv[2], NULL, 10);

  if (errno == ERANGE || errno == EINVAL) {
    perror("convert working time failed");
  }

  errno = 0;

  parkingSpace = strtol(argv[1], NULL, 10);

  if (errno == ERANGE || errno == EINVAL) {
    perror("convert parking space failed");
  }

  // TODO

  printf("%ld %ld \n", worktime, parkingSpace);

  if (pthread_create(&init, NULL, threadPricipal, &worktime) != 0) {
    perror("threadPrincipal: ");
    return -1;
  }
  if (pthread_join(init, NULL) != 0) {
    perror("threadPrincipal : ");
  }
  printf("%s\n", "End Main");

  return 0;
}
