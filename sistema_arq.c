#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <windows.h>
#include <locale.h>
#include <dirent.h>
#include <pthread.h>
#include <math.h>
#include <locale.h>


#define B_POR_INODE   	5
#define N_INODE       	10
#define B_LIVRES   		N_INODE
#define B_DADOS     	N_INODE
#define ARQUIVO			0
#define DIRETORIO		1


typedef struct i_node{
	
	int id; 				// numero do inode (tamanho de 4 bytes)
	int id_pai; 			// numero do inode pai, dentro de quem (tamanho de 4 bytes)
	int tipo;				// 0- arquivo, 1- diretorio (tamanho de 4 bytes)
	char nome[12]; 			// nome do arq (tamanho de 12 bytes)
	int size; 				// tamanho em blocos do arquivo (tamanho de 4 bytes)
	int bloco[B_POR_INODE]; // vetor para armazenar posição dos blocos  (tamanho de 4 bytes * B_POR_INODE)
	
}i_node;

void CriarArquivo(FILE *nome_arquivo, int id_inode_pai);
void CriarDiretorio(FILE *nome_arquivo, int id_inode_pai);
void CriarDiretorioRaiz(FILE *nome_arquivo);
void AlocarMemoria(FILE *nome_arquivo, int *MemoriaAlocada);
int InodeLivre(FILE *nome_arquivo);
int ProcuraInode(FILE *nome_arquivo, int id_inode);
int LiberarBlocoLivre(FILE *nome_arquivo, int id);
int GravaBlocoDados(FILE *nome_arquivo, int id, char dado);
int MostrarInode(FILE *nome_arquivo, int id_inode);
char MostraBlocoDados(FILE *nome_arquivo, int id);
int MostraDiretorio(FILE *nome_arquivo, int id_inode);
void ExcluirArquivo(FILE *nome_arquivo, int id_inode);
void ExcluirDiretorio(FILE *nome_arquivo, int id_inode);


int main(int argc, char *argv[]){

	setlocale(LC_ALL,"Portuguese");
	
	FILE *nome_arquivo; 
	char file[] = "sa.bin";
	int id_inode_pai=1;
	char b[20];
	char c[2];
	int tamBuff;
	unsigned int a = 1;
	
	if(nome_arquivo = fopen(file,"w+b"))
	{
		truncate(file,(sizeof(i_node)*N_INODE)+(2*N_INODE));
		fseek(nome_arquivo, 0, SEEK_END);
		printf("================================================================================\n");
		printf(">>\t\t     BLOCOS LIVRES: %2d \tBYTES\n", N_INODE);
		printf(">>\t\t     BLOCOS INODES: %2d \tBYTES\n", sizeof(i_node)*N_INODE);
		printf(">>\t\t     BLOCOS DADOS : %2d \tBYTES\n", N_INODE);
		printf(">>\t\t     FILE   TOTAL : %2d \tBYTES\n", ftell(nome_arquivo));
		printf("================================================================================");
		
		
		do{
			
			memset(&b,0x00,sizeof(b));
			printf("\n\t\t   | CODE |             MENU        |  #  |\n\t\t   | cdirr|  Criar      Dir. Raiz   |  #  |\n\t\t   | cdir |  Criar      Diretório   |  #  |\n\t\t   | rdir |  Remover    Diretório   |  #  |\n\t\t   | mdir |  Mostrar    Diretório   |  #  |\n\t\t   | cfile|  Criar      Arquivo     |  #  |\n\t\t   | rfile|  Remover    Arquivo     |  #  |\n\t\t   | mfile|  Mostrar    Arquivo     |  #  |\n");
			printf(">>\n\t\t ESCOLHA UMA OPCAO:\n"); 
			printf(">> ");
			scanf("%s",&b);
			
			tamBuff= strlen(b);
			
			if (tamBuff < 0) // erro na recepção de mensagem
			{
				printf("Error: Buffer de Entrada.\n");

			}else
			if (strcmp(b,"cdirr") == 0)
			{
				
				CriarDiretorioRaiz(nome_arquivo);
				
			}else		
			if (strcmp(b,"cdir") == 0)
			{
				
				CriarDiretorio(nome_arquivo,id_inode_pai);
				
			}else	
			if (strcmp(b,"rdir") == 0)
			{
				int remove;
				printf(">> [ID] Qual Diretório deseja excluir?\n");
				scanf("%i",&remove);
				while(remove < 0 || remove > N_INODE){
					printf(">> ID INVÁLIDO! \n");
					printf(">> [ID] Qual Diretório deseja excluir? ");
					scanf("%i",&remove);
				}
				ExcluirDiretorio(nome_arquivo,remove); // Para excluir um diretorio é necessário apenas passar o id do seu inode
				
			}else
			if (strcmp(b,"mdir") == 0)
			{
				for(int c=1;c<N_INODE;c++){
					MostraDiretorio(nome_arquivo,c); // Para mostrar um diretorio é necessario apenas passar o id do seu inode
				}
				
			}else
			
			if (strcmp(b,"cfile") == 0)
			{
				CriarArquivo(nome_arquivo,id_inode_pai); // Arquivos devem ter o id_inode do seu pai, ou seja, id do inode do diretorio onde serão criados
				
			}else
			
			if (strcmp(b,"rfile") == 0)
			{
				int remove;
				printf(">> [ID] Qual Arquivo deseja excluir? ");
				scanf("%i",&remove);
				while(remove < 0 || remove > N_INODE){
					printf(">> ID INVÁLIDO! \n");
					printf(">> [ID] Qual Diretório deseja excluir? ");
					scanf("%i",&remove);
				}
				ExcluirArquivo(nome_arquivo,remove); // Para excluir um arquivo é necessário apenas passar o id do seu inode
				
			}else
			
			if (strcmp(b,"mfile") == 0)
			{
				int mostra;
				printf(">> [ID] Qual Arquivo deseja listar? ");
				scanf("%i",&mostra);
				while(mostra < 0 || mostra > N_INODE){
					printf(">> ID INVÁLIDO! \n");
					printf(">> [ID] Qual Arquivo deseja listar? ");
					scanf("%i",&mostra);
				}
					for(int c = mostra; c < (mostra+5);c++){
						printf("%c", MostraBlocoDados(nome_arquivo,c));
					}
			
			}
			else{
				printf("\n> Cliente digitou comando inválido.\n");
			}
			
			printf("\n");	
			printf(">>\t\t DESEJA CONTINUAR AS OPERACOES NO ARQUIVO? \n");
			printf(">>\t\t    [1] - SIM    [0] - NAO \n");
			scanf("%u", &a);
			
			while(a != 1 && a!= 0){
				printf(">>\t\t VALOR INVÁLIDO, DIGITE NOVAMENTE: \n");
				printf(">>\t\t     [1] - SIM    [0] - NAO  \n");
				scanf("%u", &a);
			}
		
		}while(a == 1);	
			
	}
	
	fclose(nome_arquivo);
}


// indica os blocos livre que estão usados ou livres
void AlocarMemoria(FILE *nome_arquivo, int *MemoriaAlocada)
{
	char cont=0;
	int c=0;
	fseek(nome_arquivo, 0, SEEK_SET);
	
	MemoriaAlocada[0]=cont;
	
	for (c=0;c<B_LIVRES;c++)
	{
		char livre=0;
		fread(&livre,sizeof(char),1,nome_arquivo);
		
		if (livre == 0x00)
		{
			//printf("BLOCO LIVRE %i VAZIO\n",c);
			fseek(nome_arquivo, -sizeof(char), SEEK_CUR);
			fprintf(nome_arquivo,"%c",0xFF);
			cont++;
			MemoriaAlocada[0]=cont;
			MemoriaAlocada[cont]=c;
			
			if (cont==B_POR_INODE) break;
		}
		//else
			//printf("BLOCO LIVRE OCUPADO\n");
		
	}
}


// retorna um inode livre
int InodeLivre(FILE *nome_arquivo)
{
	char cont=0;
	int c=0;
	int id_inode=0;
	fseek(nome_arquivo, B_LIVRES, SEEK_SET);
	
	for (c=0;c<N_INODE;c++)
	{
		fread(&id_inode,sizeof(int),1,nome_arquivo);
		cont++;
		
		if (id_inode == 0x00)
		{
			fseek(nome_arquivo, -sizeof(int), SEEK_CUR);
			fprintf(nome_arquivo,"%c",cont);
			return cont;
		}
		
		if (c<N_INODE-1)
			fseek(nome_arquivo, sizeof(i_node)-sizeof(int), SEEK_CUR);
		
		
	}
}


// Procura um inode e posiciona o ponteiro no inicio dele
int ProcuraInode(FILE *nome_arquivo, int id_inode)
{
	char cont=0;
	int c=0;
	int id_inode_atual=0;
	fseek(nome_arquivo, B_LIVRES, SEEK_SET);
	
	for (c=0;c<N_INODE;c++)
	{
		fread(&id_inode_atual,sizeof(int),1,nome_arquivo);
		cont++;
		
		if (id_inode_atual == id_inode)
		{
			fseek(nome_arquivo, -sizeof(int), SEEK_CUR);
			return 0;
		}
		
		if (c<N_INODE-1)
			fseek(nome_arquivo, sizeof(i_node)-sizeof(int), SEEK_CUR);
		
		
	}
	
	return -1;
}


// libera bloco na região de blocos livres
int LiberarBlocoLivre(FILE *nome_arquivo, int id)
{
	
	if (id > B_LIVRES) return -1;
	
	fseek(nome_arquivo, id, SEEK_SET);
	
	fprintf(nome_arquivo,"%c",0x00);
	
	return 0;	
}


// grava dado na região blocos de dados
int GravaBlocoDados(FILE *nome_arquivo, int id, char dado)
{
	
	if (id > B_DADOS) return -1;
	
	fseek(nome_arquivo, B_LIVRES+(N_INODE*sizeof(i_node))+id, SEEK_SET);
	
	fprintf(nome_arquivo,"%c",dado);
	
	return 0;	
}


// mostra dado no bloco de dados
char MostraBlocoDados(FILE *nome_arquivo, int id)
{
	
	if (id > B_DADOS) return -1;
	
	char dado;
	
	fseek(nome_arquivo, B_LIVRES+(N_INODE*sizeof(i_node))+id, SEEK_SET);
	
	fread(&dado,sizeof(char),1,nome_arquivo);
	
	//printf("%c",dado);
	
	return dado;
	
}


// mostra todo o conteudo do diretorio
int MostraDiretorio(FILE *nome_arquivo, int id_inode)
{
	
	if (id_inode == -1) return -1;
	
	i_node inode;
	char dado;
	
	ProcuraInode(nome_arquivo,id_inode);
	
	fread(&inode,sizeof(i_node),1,nome_arquivo);
	
	if (inode.tipo == ARQUIVO)
		printf("\t %s \t\tID:  %i \t SIZE:  %i  Bytes   TIPO:  %i\n", inode.nome, inode.id, inode.size, inode.tipo);
	
	if (inode.tipo == DIRETORIO)
		printf("\t %s \t\tID:  %i \t SIZE:  %i  Bytes   TIPO:  %i\n", inode.nome, inode.id, inode.size, inode.tipo);
	
	return 0;	
}


// mostrar os dados do inode, conteudo (se tipo = ARQUIVO) ou os arquivos (se tipo = DIRETORIO)
int MostrarInode(FILE *nome_arquivo, int id_inode)
{
	char cont=0;
	int c=0;
	int id_inode_atual=0;
	i_node inode;
	
	fseek(nome_arquivo, B_LIVRES, SEEK_SET);
	
	for (c=0;c<N_INODE;c++)
	{
		fread(&id_inode_atual,sizeof(int),1,nome_arquivo);
		
		if (id_inode_atual == id_inode)
		{
			fseek(nome_arquivo, -sizeof(int), SEEK_CUR);
			fread(&inode,sizeof(i_node),1,nome_arquivo);
			//printf("id:%i idPai:%i tipo:%i nome:%s size:%i B0:%i B1:%i B2:%i B3:%i B4:%i\n",inode.id,inode.id_pai,inode.tipo,inode.nome,inode.size,inode.bloco[0],inode.bloco[1],inode.bloco[2],inode.bloco[3],inode.bloco[4]);
			if (inode.tipo == ARQUIVO)
			{
				printf("CONTEUDO: ");
				for (cont=0;cont<inode.size;cont++)
					MostraBlocoDados(nome_arquivo,inode.bloco[cont]);
				printf("\n");
			}
			
			if (inode.tipo == DIRETORIO)
			{
				printf("CONTEUDO: \n");
				for (cont=0;cont<B_POR_INODE;cont++)
					MostraDiretorio(nome_arquivo,inode.bloco[cont]);
				printf("\n");
			}
			
			return 0;
		}
		
		if (c<N_INODE-1)
			fseek(nome_arquivo, sizeof(i_node)-sizeof(int), SEEK_CUR);
		
		
	}
	
	return -1;
}

// Criar novo arquivo
void CriarArquivo(FILE *nome_arquivo, int id_inode_pai)
{
	int c=0;
	char buffer[50];
	int MemoriaAlocada[1+B_POR_INODE];
	i_node inode, inodePai;
	
	ProcuraInode(nome_arquivo,id_inode_pai);
	
	fread(&inodePai,sizeof(i_node),1,nome_arquivo);
	
	if (inodePai.size >= B_POR_INODE) printf("ESPAÇO INSUFICIENTE NESTE DIRETORIO.\n");
	else{
	
		AlocarMemoria(nome_arquivo,MemoriaAlocada);
		
		if (MemoriaAlocada[0] == 0) printf("MEMORIA CHEIA.\n");
		else{

			int id_inode = InodeLivre(nome_arquivo);
			
			for (c=0;c<B_POR_INODE;c++)
			{
				if (inodePai.bloco[c] == -1)
				{
					inodePai.bloco[c] = id_inode;
					inodePai.size++;
					break;
				}
			}
			
			ProcuraInode(nome_arquivo,id_inode_pai);
			fwrite(&inodePai,sizeof(i_node),1,nome_arquivo);
			
			memset(&buffer,0x00,sizeof(buffer));
			memset(&inode, 0x00, sizeof(inode));
			//printf("%i\n",id_inode);
			
			printf("NOME DO ARQUIVO: ");
			//gets(buffer);
			
			scanf("%15s",&buffer);
			strcat(buffer,"\0");
			
			inode.id = id_inode;
			inode.id_pai = id_inode_pai;
			inode.tipo = ARQUIVO;
			strcpy(inode.nome,buffer);
			
			// gets(buffer);
			//memset(&buffer,0x00,sizeof(buffer));
			printf("CONTEUDO: ");
			gets(buffer);
			scanf("%5s",&buffer);
			strcat(buffer,"\0");
			
			//printf("%i\n",strlen(buffer));
			
			inode.size = MemoriaAlocada[0] < strlen(buffer) ? MemoriaAlocada[0] : strlen(buffer);
			
			for (c=0;c<B_POR_INODE;c++){
				inode.bloco[c] = -1;
			}
			
			for(c=0; c<MemoriaAlocada[0]; c++)
			{
				if (buffer[c] != '\0')
				{
					inode.bloco[c] = MemoriaAlocada[c+1];
					GravaBlocoDados(nome_arquivo,MemoriaAlocada[c+1],buffer[c]);
				}else LiberarBlocoLivre(nome_arquivo,MemoriaAlocada[c+1]);
			}

			ProcuraInode(nome_arquivo,id_inode);
			fwrite(&inode,sizeof(i_node),1,nome_arquivo);
			
		} 
	}	
}


// excluir arquivo existente pelo id
void ExcluirArquivo(FILE *nome_arquivo, int id_inode)
{
	char cont=0;
	i_node inode, inodePai;

	ProcuraInode(nome_arquivo,id_inode);
	
	fread(&inode,sizeof(i_node),1,nome_arquivo);
	
	for (cont=0;cont<B_POR_INODE;cont++)
	{
		if (inode.bloco[cont] != -1) {
			LiberarBlocoLivre(nome_arquivo,inode.bloco[cont]);
		}
	}
	
	ProcuraInode(nome_arquivo,inode.id_pai);
	
	fread(&inodePai,sizeof(i_node),1,nome_arquivo);
	
	for (cont=0;cont<B_POR_INODE;cont++)
	{
		if (inodePai.bloco[cont] == id_inode)
		{
			inodePai.bloco[cont] = -1;
			inodePai.size--;
			break;
		}
	}
	
	ProcuraInode(nome_arquivo,inode.id_pai);
	
	fwrite(&inodePai,sizeof(i_node),1,nome_arquivo);
	
	ProcuraInode(nome_arquivo,id_inode);
	
	memset(&inode,0x00,sizeof(inode));
	
	fwrite(&inode,sizeof(i_node),1,nome_arquivo);

}

// Criar diretorio Raiz
void CriarDiretorioRaiz(FILE *nome_arquivo)
{
	//printf("> %s\n",b);
	int c=0;
	char buffer[50];
	i_node inode;
	int var = -1;
	
	int id_inode = InodeLivre(nome_arquivo);
	
	memset(&buffer,0x00,sizeof(buffer));
	memset(&inode.nome,0x00,sizeof(inode.nome));
	
	for(c=0;c<B_POR_INODE;c++){
		memset(&inode.bloco[c],0x00,sizeof(inode.bloco[c]));
	}
	
	//printf("%i\n",id_inode);
	
	printf("\nNOME DO DIRETORIO RAIZ: ");
	scanf("%5s",&buffer);
	
	inode.id = id_inode;
	inode.id_pai = id_inode;
	inode.tipo = DIRETORIO;
	strcpy(inode.nome,buffer);
	inode.size = 0;
	
	
	for (c=0;c<B_POR_INODE;c++){
		inode.bloco[c] = -1;
	}
	
	ProcuraInode(nome_arquivo,id_inode);
	fwrite(&inode,sizeof(i_node),1,nome_arquivo);
}

// Criar novo diretorio
void CriarDiretorio(FILE *nome_arquivo, int id_inode_pai)
{
	int c=0;
	char buffer[50];
	i_node inode, inodePai;
	
	ProcuraInode(nome_arquivo,id_inode_pai);
	
	fread(&inodePai,sizeof(i_node),1,nome_arquivo);
	
	//printf("\nsize inodePai: %i",inodePai.size);
	
	if (inodePai.size >= B_POR_INODE) printf("ESPAÇO INSUFICIENTE NESTE DIRETORIO.\n");
	else{

		int id_inode = InodeLivre(nome_arquivo);
		
		for (c=0;c<B_POR_INODE;c++)
			{
				if (inodePai.bloco[c] == -1)
				{
					inodePai.bloco[c] = id_inode;
					inodePai.size++;
					break;
				}
			}
			
		ProcuraInode(nome_arquivo,id_inode_pai);
		fwrite(&inodePai,sizeof(i_node),1,nome_arquivo);
		
		memset(&buffer,0x00,sizeof(buffer));
		memset(&inode.nome, 0x00,sizeof(inode.nome));
		//printf("%i\n",id_inode);
		
		printf("\nNOME DO DIRETORIO: ");
		scanf("%5s",&buffer);
		
		strcat(buffer,"\0");
		
		inode.id = id_inode;
		inode.id_pai = id_inode_pai;
		inode.tipo = DIRETORIO;
		
		//printf("inode Nome Diretorio %s",inode.nome);
		
		strcpy(inode.nome,buffer);
		
		inode.size = 0;
		
		for (c=0;c<B_POR_INODE;c++){
			inode.bloco[c] = -1;
		}
		
		ProcuraInode(nome_arquivo,id_inode);
		fwrite(&inode,sizeof(i_node),1,nome_arquivo);
			
	}	
}

// excluir Diretorio
void ExcluirDiretorio(FILE *nome_arquivo, int id_inode)
{
	char cont=0;
	i_node inode, inodePai;

	ProcuraInode(nome_arquivo,id_inode);
	
	fread(&inode,sizeof(i_node),1,nome_arquivo);
	
	if (inode.size > 0) printf("ESTE DIRETORIO NÃO ESTA VAZIO.\n");
	else{
	
		ProcuraInode(nome_arquivo,inode.id_pai);
		
		fread(&inodePai,sizeof(i_node),1,nome_arquivo);
		
		for (cont=0;cont<B_POR_INODE;cont++)
		{
			if (inodePai.bloco[cont] == id_inode)
			{
				inodePai.bloco[cont] = -1;
				inodePai.size--;
				break;
			}
		}
		
		ProcuraInode(nome_arquivo,inode.id_pai);
		
		fwrite(&inodePai,sizeof(i_node),1,nome_arquivo);
		
		ProcuraInode(nome_arquivo,id_inode);
		
		memset(&inode,0x00,sizeof(inode));
		
		fwrite(&inode,sizeof(i_node),1,nome_arquivo);
	}

}