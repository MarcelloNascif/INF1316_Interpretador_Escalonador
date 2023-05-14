#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "info.h"

Queue* create_queue (){
    Queue* f = (Queue*)malloc(sizeof(Queue));
    f->ini=f->fim = NULL;
    f->qtd = 0;
    return f;
}

Process * create_process(char * nome, int pid){
	Process* p = (Process*)malloc(sizeof(Process));
    	p->nome = nome;
    	p->pid = pid;
	p->status = PRONTO;
    	p->tempoEmExecucao = 0;
    	p->tempoEmEspera = 0;
	p->enviouSinalIO = 0;
    	p->prox=NULL;

	return p;
}

Process * copy_process(Process *p){
	Process *novo;
	novo = create_process(p->nome, p->pid);
	novo->tempoEmExecucao = p->tempoEmExecucao;
	novo->tempoEmEspera = p->tempoEmEspera;
	novo->enviouSinalIO = p->enviouSinalIO;
	return novo;
}
 
void insert_queue (Queue* f, Process *p){
    p->prox = NULL;
    if (f->fim != NULL)
        f->fim->prox = p;
    else
        f->ini=p;
    f->fim = p;
    f->qtd++;
}
 
Process * remove_queue (Queue* f){
    Process* t;
    if(f->ini==NULL)
        return NULL;
    t = f->ini;
    f->ini = t->prox;
    if (f->ini == NULL)
        f->fim = NULL;
    f->qtd--;
    return t;
}

void remove_process(Queue *f, Process *p){
	Process *aux, *ant;

	if(f->qtd == 0){
		printf("queue vazia!\n");
		return;
	}
	if(f->ini->pid == p->pid){
		f->ini = f->ini->prox;
		(f->qtd)--;
		if(f->qtd == 0) f->ini = f->fim = NULL;
		return;		
	}
	aux = f->ini->prox;
	ant = f->ini;
	while(aux != NULL){
		if(aux->pid == p->pid){
			ant->prox = aux->prox;
			if(aux->pid == f->fim->pid){
				f->fim = ant;
			}
			(f->qtd)--;
			if(f->qtd == 0) f->ini = f->fim = NULL;
			return;
		}
		ant = aux;
		aux = aux->prox;

	}	
	printf("Não encontrou o Processo %s na queue!\n", p->nome);
	return;
}

void print_queue(Queue *f, char *nomeQueue){
	Process *aux;
	
	printf("\nProcesso na queue de %s:\n", nomeQueue);
	if(f == NULL){
		printf("queue não foi criada!\n");
		return;
	}
	if(f->qtd == 0){
		printf("   queue vazia!\n\n");
		return;
	}
	aux = f->ini;
	while(aux!=NULL){
		printf("  Nome: %s\n",aux->nome);
		aux = aux->prox;
	}
	printf("\n");
	return;
}

void free_process(Process *p){
	free(p);
}

void free_queue (Queue* f){
    Process* q = f->ini;
    while(q!=NULL){
    	Process* t = q->prox;
    	free(q);
    	q = t;
    }
    free(f);
}

int empty_queue (Queue * f){
    return (f->ini == NULL);
}