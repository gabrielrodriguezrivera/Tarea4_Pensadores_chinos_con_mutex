#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <iomanip>  //  libreria de manipulacion de decimales
#include <unistd.h>
#include <term.h>
#include <sstream>
#include <pthread.h>
#include <ncurses.h>


#define thread_num 6

using namespace std;

pthread_mutex_t m[thread_num];

int palillo[thread_num];

struct Chino {
    int id;
    bool comer;
    int *left;
    int *right;
};

void *brete(void* arg){
    
    struct Chino *data = (struct Chino *) arg;
    int valor;
    int sub = data->id;
    int L = sub-1;
    if(L < 0){
        L = L + thread_num;
    }
    int R = sub;
    bool locked = 0;
    while(true){
        valor = rand() % 100;   //  funcion random entre 0 y 1
        if (valor < 55 && data->comer){
           
            data->comer = false;
            *data->left = 0;
            *data->right = 0;
        
            pthread_mutex_unlock(m + L);  //  Desbloquea el objeto
            pthread_mutex_unlock(m + R);  //  Desbloquea el objeto
        }
        else{
            pthread_mutex_trylock(m + L);    //  Se bloquea el objeto
            pthread_mutex_trylock(m + R);    //  Se bloquea el objeto
    
            *data->left = 1;
            *data->right = 1;
            data->comer = true;

        }
        usleep(2000000);
    }
    pthread_exit(NULL); 
}

const char* doubleToStr(double value){
    std::stringstream ss ;
    ss << value;
    const char* str = ss.str().c_str();
    return str;
}

int main()
{   
    int N = thread_num;
    
    pthread_t filosofo[N];
    struct Chino argArray[N];

 

    //  shm_open abre un objeto de memoria compartida:
    //  int shm_open(const char *name, int oflag, mode_t mode);
    //  O_CREAT     si el objeto de memoria compartida existe, ésta bandera no tiene efecto. Sino, se crea el objeto de memoria compartido
    //  O_RDWR      abre para leer o crear accesos
    //  O_TRUNC     si el objeto de memoria compartida existe y lo abrió el modo O_RDWR, el objeto será truncado a longitud cero
    //  Si el proceso es exitoso, la función debe retornar un entero no-negativo representando el descriptor de archivo no utilizado con el número más bajo
    //  De lo contrario, retorna un -1 y fija "errno" para indicar un error
    //int fd = shm_open("/foo", O_CREAT | O_TRUNC | O_RDWR, 0600);

    //  ftruncate trunca un archivo a un largo especificado:
    //  int ftruncate(int fd, off_t length);
    //  fd es el archivo
    //  el tamaño del archivo será el tamaño de la estructura
    //ftruncate(fd, sizeof(struct shared));

    //  mmap es una función que mapea páginas de memoria
    //  void *mmap(void *addr, size_t len, int prot, int flags, int files, off_t off);
    //  PROT_READ   los datos pueden ser leídos
    //  PROT_WRITE  los datos pueden ser escritos
    //  MAP_SHARED  los cambios son compartidos
    //  Deberá retornar la dirección en la cual el mapeo fue colocado. De otra manera, retornará un error
    //struct shared *p = mmap(0, sizeof(struct shared),
    //    PROT_READ, PROT_WRITE, MAP_SHARED, fd, 0);

    //  El puntero "p" accede al valor de palillos
    //p->palillos = 0;

    //  Asegurarse que puede ser compartido a través de procesos
    //pthread_mutexattr_t shared;         //  objeto de atributos mutex
    //pthread_mutexattr_init(&shared);    //  inicializa la dirección del objeto de atributos mutex
    //pthread_mutexattr_setpshared(&shared, PTHREAD_PROCESS_SHARED);  //  la función establecerá el atributo de proceso compartido en un objeto de atributos

    pthread_mutex_init(m, NULL);   //  inicializa el mutex referenciado con los atributos que posee "&shared"
    
    int i;
    
    for (i = 0; i < 6; i++) {
        palillo[i] = 0;
        if(i == 0){
            argArray[i].left = &palillo[N-1];
            argArray[i].right = &palillo[i];
        }
        else{
            argArray[i].left = &palillo[i-1];
            argArray[i].right = &palillo[i];
        }
        argArray[i].comer = false;
        argArray[i].id = i;
        int ret = pthread_create(&filosofo[i], NULL, &brete, (void*) &argArray[i]);    //  Crea un hilo y pasa por parametro el valor de la variable "pies"
        if (ret != 0){
            printf("Error: pthread_create() failed\n");
            exit(EXIT_FAILURE);  
        }
        
        

    //munmap(p, sizeof(struct shared*));  //  "Desmapea" (elimina) el archivo de memoria para liberar recursos
    //shm_unlink("/foo"); //  Elimina el objeto compartido de memoria
    }

    char captura;
    bool ciclo=true;
    char *tecla;

    WINDOW *w;
    initscr ();
    raw();
    start_color();
    init_pair(1,COLOR_GREEN, COLOR_BLACK);
    init_pair(2,COLOR_RED, COLOR_BLACK);
    init_pair(3,COLOR_CYAN, COLOR_BLACK);
    init_pair(4,COLOR_WHITE,COLOR_BLACK);
    noecho();

    int x0 = 40;
    int y0 = 12;

    int a[N], b[N];
    for (i=0; i<N; i++){
        a[i]=1;
        b[i]=4;
    }


    while(ciclo==true){


        
        captura=getch();
        nodelay(stdscr, true);
        cbreak();

        if(captura==83 || captura==115){
            ciclo=false;
            tecla=&captura;
        }

        for (i=0; i<N; i++){
            
            if(argArray[i].comer){
                b[i] = 3;
            }
            else{
                b[i] = 4;
            }
            if(palillo[i] == 1){
                a[i] = 2;
            }
            else{
                a[i] = 1;
            }

        }
        
        move(y0-3*2, x0-3*2);
        attron(COLOR_PAIR(b[0]));
        printw("Chino 1");
        attron(COLOR_PAIR(b[0]));

        move(y0-2*2, x0 + 3*2);
        attron(COLOR_PAIR(a[0]));
        printw("%c",'|');
        ////printw("%d", *argArray[0].left);
        attroff(COLOR_PAIR(a[0]));

        move(y0-1*2, x0 + 4*2);
        attron(COLOR_PAIR(b[1]));
        printw("Chino 2");
        attron(COLOR_PAIR(b[1]));

        move(y0-0*2, x0 + 5*2);
        attron(COLOR_PAIR(a[1]));
        printw("%c",'|');
        ////printw("%d", *argArray[1].left);
        attroff(COLOR_PAIR(a[1]));

        move(y0+1*2, x0 + 4*2);
        attron(COLOR_PAIR(b[2]));
        printw("Chino 3");
        attron(COLOR_PAIR(b[2]));

        move(y0+2*2, x0 + 3*2);
        attron(COLOR_PAIR(a[2]));
        printw("%c",'|');
        //printw("%d", *argArray[2].left);
        attroff(COLOR_PAIR(a[2]));

        move(y0+3*2, x0-3*2);
        attron(COLOR_PAIR(b[3]));
        printw("Chino 4");
        attron(COLOR_PAIR(b[3]));

        move(y0+2*2, x0 - 6*2);
        attron(COLOR_PAIR(a[3]));
        printw("%c",'|');
        attroff(COLOR_PAIR(a[3]));

        move(y0+1*2, x0 - 10*2);
        attron(COLOR_PAIR(b[4]));
        printw("Chino 5");
        attron(COLOR_PAIR(b[4]));

        move(y0-0*2, x0 - 8*2);
        attron(COLOR_PAIR(a[4]));
        printw("%c",'|');
        attroff(COLOR_PAIR(a[4]));

        move(y0-1*2, x0 - 10*2);
        attron(COLOR_PAIR(b[5]));
        printw("Chino 6");
        attron(COLOR_PAIR(b[5]));    

        move(y0-2*2, x0 - 6*2);
        attron(COLOR_PAIR(a[5]));
        printw("%c",'|');
        attroff(COLOR_PAIR(a[5]));

        move(0,0);
        refresh();  
    }

    endwin ();
    
    pthread_exit(NULL);
}