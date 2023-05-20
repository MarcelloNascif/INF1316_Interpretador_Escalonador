#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

#include "info.h"

//Protótipos:
void handler1(int sig);
char* concatenarStrings(const char* str1, const char* str2);
void execProcess(Process currentP);

//Globais:
int io_bound = FALSE;

int main(void){
	int shared_memory, shmid_pid;
	Process currentP;
	Process *processInfo;
	pid_t* pid;
	
	int i = 0;
	
	struct timeval init, end;
	float sec;

	// Anexar à memória compartilhada
	shared_memory = shmget(SHM_KEY, MAX_PROCESSOS * sizeof(Process), IPC_CREAT | 0666);
	processInfo = (Process *)shmat(shared_memory, 0, 0);
	if (!processInfo){
		perror("Erro ao anexar à memória compartilhada do processInfo.\n");
		exit(1);
	}

	// Anexar à memória compartilhada
	shmid_pid = shmget(SHM_KEY2, sizeof(pid_t), IPC_CREAT | 0666);
	pid = shmat(shmid_pid, 0, 0);
	if (!pid){
		perror("Erro ao anexar à memória compartilhada do pid.\n");
		exit(1);
	}

	Queue filaRR;
	Queue filaRT;
	Queue filaIO;
	initQueue(&filaRR);
	initQueue(&filaRT);
	initQueue(&filaIO);

	gettimeofday(&init, NULL);

	signal(SIGUSR1, handler1); // recebeu sinal de IO bound

	Process p;

	for (EVER)
	{

		gettimeofday(&end, NULL);
		sec = ((end.tv_sec - init.tv_sec) % 60);
		printf("\ntempo: %.1f s\n", sec);
		
		if (processInfo[i].index == i){/*Se ainda recebe processo entra aqui*/ 
			currentP = processInfo[i];

			if (currentP.policy == REAL_TIME){
				enqueue(&filaRT, currentP);
				queueSort(&filaRT);
			}

			else if (currentP.policy == ROUND_ROBIN){
				enqueue(&filaRR, currentP);
				queueSort(&filaRR);
			}

			i++;
		} 


	/*Inicia a execução dos processos*/ 
		/* Processo do Real Time */
		if ((!isEmpty(&filaRT)) && (filaRT.front->process.init == sec)) {  // Primeiro da fila entra em execução
		   
			p = filaRT.front->process;
			if (!p.started){
				execProcess(p); // Executa processo e armazena seu pid em currentP.pid
				sleep(p.duration);
				p.pid = *pid;
				p.started = TRUE;
			}
			else{
				kill(p.pid, SIGCONT);
				sleep(p.duration);
			}

			kill(p.pid, SIGSTOP);

			// printf("***************\n%s pid lido pelo escalonador: %d\n******************\n", p.name, p.pid);
			
			dequeue(&filaRT);
			enqueue(&filaRT, p);
			// displayQueue(&filaRT); //Imprime Fila de processos Real-Time

		}
		/* Processo do Round Robin */
		else if (!isEmpty(&filaRR)){
			p = filaRR.front->process;
			
			if (!p.started){
				execProcess(p); // Executa processo e armazena seu pid em currentP.pid
				sleep(p.duration);
				p.pid = *pid;
				p.started = TRUE;
			}
			else{
				kill(p.pid, SIGCONT);
				sleep(p.duration);
			}

			kill(p.pid, SIGSTOP);
			
			// printf("***************\n%s pid lido pelo escalonador: %d\n******************\n", p.name, p.pid);
			
			dequeue(&filaRR);
            /*if(io_bound == TRUE){
				puts("Sou TRUE");
                if(fork() == 0){
                    enqueue(&filaIO, p);
					printf("Processo = %s -- PID = %d -- Entrou IO\n", p.name, p.pid);
                    sleep(3);
                    dequeue(&filaIO);
                    io_bound = FALSE;
                }
            }*/
			enqueue(&filaRR, p);
			//displayQueue(&filaRR); //Imprime Fila de processos Round-Robin
		}
	}

	// libera a memória compartilhada
	shmctl(shared_memory, IPC_RMID, 0);
	shmctl(shmid_pid, IPC_RMID, 0);
	
	return 0;
}

void handler1(int sig) {
	puts("Entrei no handler");
	io_bound = TRUE;
}

char* concatenarStrings(const char* str1, const char* str2) {
	size_t tamanhoStr1 = strlen(str1);
	size_t tamanhoStr2 = strlen(str2);
	size_t tamanhoTotal = tamanhoStr1 + tamanhoStr2 + 1;

	char* resultado = (char*)malloc(tamanhoTotal);

	if (resultado == NULL) {
		perror("Erro ao alocar memória");
		exit(1);
	}

	strcpy(resultado, str1);
	strcat(resultado, str2);

	return resultado;
}

void execProcess(Process p){
	char inicioPath[] = "./programas/";
	char *path;

	path = concatenarStrings(inicioPath, p.name);

	// int pidPai = getpid();

	// char tmp_pidPai[24]; // usado apenas para converter o pid do pai para string pra poder passar como argumento (ja que todos devem ser passados como string)
    // sprintf(tmp_pidPai, "%d", pidPai);

	// printf("pidpai = %s\n\n", tmp_pidPai);
	
	char *argv[] = {NULL};
	
	if(fork() == 0){
		printf("Executando o %s\n", path);
		execvp(path, argv);
	} 
	return;
}