#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define TAMANHO 9

/*
	Lista encadeada que armazenará os movimentos feitos no puzzle e ao final,
	todos serão deletados e nela serão armazenados os movimentos feitos
	que resultaram na configuração final.
*/
typedef struct nodoTabelao{int indice;int puzzlePassado[TAMANHO];
                             struct nodoTabelao *prox;} Tabelao;

Tabelao *tabelaoPuzzleI = NULL; // Inicializa tabelao que manterá os valores armazenados
Tabelao *tabelaoPuzzleF = NULL;

int puzzle[TAMANHO];

// Tabelão com as posições que podem ser movidas para o respectivo índice no array 1d do puzzle 8
// por exemplo, na posição 0, as peças das posições 1 e 3 podem tomar o lugar vazio...
//(embora o array seja 1d aqui, a lógica de jogo e representação é em 2d)
int max[9][4] = {   {1,3,-1,-1}  ,
            		    {0,2,4,-1 }  ,
            		    {1,5,-1,-1}  ,
            		    {0,4,6,-1 }  ,
            		    {1,3,5,7  }  ,
            		    {2,4,8,-1 }  ,
            		    {3,7,-1,-1}  ,
            		    {4,6,8,-1 }  ,
            		    {5,7,-1-1 }   };

// Variáveis para controle do puzzle

int finalizou=0;

int maxRodadas=0;

// Funções

void telaInicial();

void print(int *puzzleExibe);

int verificaFim();

int retornaPosVazia();

int retornaMaxEspPosVazia(int posVazia);

int retornaPosAoRedor(int posVazia,int k);

void printTabelao();

int backtrackingMagico(int posVazia);

void gravaTabelao(int *puzzle);

void adicionaNoInicioDaLista(int *puzzle);

int verificaTabelao();

void limparTodoTabelao();

int contaInversoes();

int verificaSolubilidade();

// =====================================

int main()
{
    int maxRodadasInput = -1, i=0, deveRodar=1;

	char entrada[9];
	int entradaUserPuzzle[9];
  telaInicial();
	printf("\nDigite todos os números para preencher a tabela separados por vírgula\n (de 0 à 8, sendo 0 a posição vazia) EX: 3,1,2,4,0,5,6,8,7,... \n");

	scanf(" %s", entrada);
  //Converte a entrada com vírgulas para os valores do array de jogo
	sscanf(entrada,"%d,%d,%d,%d,%d,%d,%d,%d,%d",&entradaUserPuzzle[0],&entradaUserPuzzle[1],&entradaUserPuzzle[2],&entradaUserPuzzle[3],&entradaUserPuzzle[4],&entradaUserPuzzle[5],&entradaUserPuzzle[6],&entradaUserPuzzle[7],&entradaUserPuzzle[8]);

  // aqui verifica se a entrada é inválida, se for, mata o programa
	for(;i<TAMANHO;i++){
		if(entradaUserPuzzle[i]>9 || entradaUserPuzzle[i]<0){
			deveRodar=0;
      break;
		}
	}

	if(!deveRodar){
		printf("\n Desculpe mas uma das entradas é inválida! \n Reinicie o programa. \n");
	}else{

	    memcpy(puzzle, entradaUserPuzzle, sizeof(int)*TAMANHO);


	    int posVazia = retornaPosVazia();
      printf("\n Esta é a configuração fornecida por você: \n");
		  print(puzzle);

	  if(verificaSolubilidade()){

          printf("\n Quantidade máxima de movimentos?\n");
	      scanf(" %d", &maxRodadasInput);

	      gravaTabelao(puzzle);
	      if(backtrackingMagico(posVazia)){

	        if(maxRodadas<=maxRodadasInput){
            	printTabelao();
          		printf("Total de movimentos feitos: %d\n ", maxRodadas);
                printf("\n Parabéns! O programa agora será finalizado. Até mais.\n");
      	    }else
      			printf("\n Não foi possível finalizar com esta quantidade de movimentos :/");
            
          }else
          printf("Nem terminou \n");
	  }else
		    printf("\nNão há solução para tal problema :/\n");

  }

    printf("\n Projeto desenvolvido por: Adriano Gomes da Silva\n");

    return 0;
}

// Função firula para a tela inicial
void telaInicial(){
  printf("\n============================================");
  printf("\n Olá, seja bem vindo ao resolvedor do jogo");
  printf("\n		8-puzzle!\n");
  printf("\n============================================");
}

//Função que exibe o puzzle passado como parâmetro
void print(int *puzzleExibe){
    int i,j,k=0;
    printf("\n ------------- \n");
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
            printf("| %d |",puzzleExibe[k]);
	           k++;
        }
        printf("\n ------------- ");
        printf("\n");
    }
    printf("\n");
}

//Função que encontra onde está o 0 no puzzle e retorna o índice
int retornaPosVazia(){
	int i;
	for(i=0;i<TAMANHO;i++){
		if(puzzle[i]==0){
			return i;
		}
	}
  return -1;
}

//Função que verifica se finalizou o puzzle ou não;
//Repare que a condição final é 0,1,2,3,4,5,6,7,8
int verificaFim(){
	int i=0;
	for(i=0;i<TAMANHO;i++){
		if(puzzle[i]!=i){
			return 0;
		}
	}
	return 1;
}

//Função que destrói o tabelão memorizador!
void limparTodoTabelao()
{
    Tabelao *no = tabelaoPuzzleI, *aux;
    while (no != NULL)
    {
        aux = no;
        no = no->prox;
        free (aux);
    }
    tabelaoPuzzleI = NULL;
    tabelaoPuzzleF = NULL;
}

//Função que retorna o máximo de movimentos
//possíveis para uma posição.
//Delimitado a partir do puzzle;
//Exemplo:
/*
	Se a posição vazia é a 4
	(lembre-se, a representação do
	puzzle é em um ARRAY 1d, então
	a posição 4 equivale ao centro
	de um array 2d;), os valores
	das posições 1,3,5 e 7 podem
	tomar a posição 4, ou seja,
	quatro movimentos são possíveis;
		-------------
		|	| 1	| 	|
		| 1	| 0	| 1	|
		|	| 1	|	|
		-------------

*/
int retornaMaxEspPosVazia(int posVazia){

	if(posVazia==4) return 4;
	if(posVazia%2==0){
		return 2;
	}
	return 3;
}

// Funçãozinha que retorna a
//	posição do tabuleiro que será trocada
int retornaPosAoRedor(int posVazia,int k){
	return max[posVazia][k];
}

// Função onde a magia do backtracking acontece;
int backtrackingMagico(int posVazia){

	// k é a variável de "de onde posso pegar uma peça e colocar aqui"
	// temp é uma variável nada mais que temporária
    int	k=0, temp;

	// posAoRedor é a variável de "dessa posição eu pegarei uma peça para colocar na casa vazia"
	// maxEsp é a variável de "esse é o máximo de variáveis que posso pegar para colocar aqui
  	int posAoRedor = -1,
      maxEsp = retornaMaxEspPosVazia(posVazia);

	// Verificação para sinalizar que finalizou;
    if(verificaFim() ){
      //printf("\nTerminou!\n");
      finalizou=1;	// Essa variável é alterada, para sinalizar que finalizou de verdade, pois o tabelão
					// onde os moves feitos são armazenados é destruído 1x quando finaliza, por isso desta var;
      return 1;
    }


   while(k<maxEsp){
    //Busca a posição ao redor que pode mover para a pos vazia
    posAoRedor = retornaPosAoRedor(posVazia,k);
	// Faz o move
    temp = puzzle[posVazia];
    puzzle[posVazia] = puzzle[posAoRedor];
    puzzle[posAoRedor] = temp;
	// ==========

		// Verifica se o move está presente no tabelão. Se não estiver, retorna true;
		// senão, tenta a próxima posição ao redor da posição vazia ;)
    	if(verificaTabelao()){

          // grava o move pois não existe no tabelão ainda
    	  gravaTabelao(puzzle);

		  // guarda temporariamente onde a posVazia se encontra,
		  // para caso não seja o move final do puzzle 8
          temp = posVazia;

		  // PosAoRedor possui a posição vazia, então, passa para a var correta
          posVazia= posAoRedor;

		  // faz o backtracking com a nova posição vazia
    	  if(backtrackingMagico(posVazia)){
			// se finalizou, limpa o tabelão
            if(finalizou){
              limparTodoTabelao();
              finalizou--; // decrementa para não entrar mais aqui
            }

			// qual o movimento
            maxRodadas++;

			// reverte a pos já movima para a var posVazia, para reverter o move
            posVazia = temp;

			// reverte o move
            temp = puzzle[posVazia];
            puzzle[posVazia] = puzzle[posAoRedor];
            puzzle[posAoRedor] = temp;

			// grava no tabelão (lista encadeada de puzzles) para exibir ao user
            adicionaNoInicioDaLista(puzzle);
            return 1; // rinse and repeat
          }

		  // se entrou aqui, quer dizer que não foi o move final e algo deu errado
		  // no caminho, então, reverte o move e parte para a próxima;
          posVazia = temp;

          temp = puzzle[posVazia];
          puzzle[posVazia] = puzzle[posAoRedor];
          puzzle[posAoRedor] = temp;
      }
      else{
		  // reverte move
          temp = puzzle[posVazia];
          puzzle[posVazia] = puzzle[posAoRedor];
          puzzle[posAoRedor] = temp;
      }
	  // Não deu essa posição, tenta a próxima ao redor do espaço vazio
      k++;

    }
  return 0;
}

// Função que grava um puzzle passado como parâmetro no tabelão que será
// percorrido, como base da memorização de soluções por parte do programa.
void gravaTabelao(int *puzzle){
  Tabelao *novo = (Tabelao *)malloc(sizeof(Tabelao));
  memcpy(novo->puzzlePassado, puzzle, sizeof(int)*TAMANHO); // copy maroto por memória
  novo->prox = NULL;
  if(tabelaoPuzzleI==NULL){
    tabelaoPuzzleI  = novo;
    tabelaoPuzzleI->prox = tabelaoPuzzleF;
    tabelaoPuzzleF  = novo;
  }else{
    tabelaoPuzzleF->prox = novo;
    tabelaoPuzzleF = novo;
  }
}


// Adiciona um puzzle no início do TABELÃO
void adicionaNoInicioDaLista(int *puzzle){
  Tabelao *novo = (Tabelao *)malloc(sizeof(Tabelao));
  memcpy(novo->puzzlePassado, puzzle, sizeof(int)*TAMANHO); // copy maroto por memória
  novo->prox = tabelaoPuzzleI;
  tabelaoPuzzleI = novo;

}

// Função que exibe todos os movimentos feitos, partindo do input do usuário,
// até a posição final;
void printTabelao(){
  Tabelao *tabAux = tabelaoPuzzleI;
  int i=0;
  if(!verificaFim()){
    while(tabAux!=NULL){
      printf("\n Movimento %d \n", i);
      print(tabAux->puzzlePassado);
      tabAux = tabAux->prox;
      i++;
    }
  }
	// cria outro vetor só por conveniência para a função de print
  int tabFim[TAMANHO] = {0,1,2,3,4,5,6,7,8};
  printf("\n Movimento %d \n", i);
  print(tabFim);
}

// Verifica no TABELÃO (lista encadeada de puzzles) se tal move já foi feito. Se sim, vai para o outro;
int verificaTabelao(){
  int diferenteLaco;
  int i;
  Tabelao *verifica = tabelaoPuzzleI;

  while(verifica!=NULL){

      diferenteLaco=0;
      i=0;
      while(i<TAMANHO){
        if(verifica->puzzlePassado[i] != puzzle[i]){
          diferenteLaco = 1;
          break;
        }
        i++;
      }

      if(!diferenteLaco) return 0;

      verifica = verifica->prox;

  }
  return 1;

}

// Função que conta a quantidade de inversões
// que um valor do puzzle tem de fazer até chegar
// em sua posição final;
int contaInversoes(){

	int i=0,j, inversoes=0;

	for(;i<TAMANHO;i++){
		for(j=i+1;j<TAMANHO;j++){
			if(puzzle[i]==0) break;
			if(puzzle[j]!=0 && puzzle[i]>puzzle[j]){

				inversoes++;
			}
		}
	}
	return inversoes;
}

// função que chama a função acima e retorna 0 se não é
// um somatório de inversões par. Para puzzles de tamanho
// ímpar, o resultado das inversões deve ser par;
// para puzzles de tamanho par, 'o buraco é mais embaixo';
int verificaSolubilidade(){
	if(contaInversoes()%2!=0)return 0;
	else return 1;
}
