/* Librerias utilizadas, con funcion utilizada */
#include<iostream> /* Proporciona operaciones de entrada/salida; incluye operaciones de lectura, escritura o formateo secuencial */
#include<thread>
#include<stdlib.h> /*Proporciona: atoi */
#include<string> /*Proporciona: strcpy */
#include<fstream>  /*Proporciona: fail, eof */
#include<unistd.h> 
#include <stdexcept> /* Proporciona: runtime_error */
#include <memory>  /* unique_ptr( Proporciona: Puntero inteligente) */
#include <cstdio>
#include <stdio.h> /* fgets */
#include <array>
using namespace std;

const int max_pings = 5;
const int MAX_THREAD = 5;
/* ----------------------------------- DECLARACION DE STRUCT --------------------------------------------------*/
struct ping{ 
    string ip;
    int paquetes_transmitidos;
    int paquetes_recibidos;
    int paquetes_perdidos;
    string estado;
}pings[max_pings];

/* ----------------------------- DECLARACION DE FUNCION requestPing --------------------------------------------*/
void requestPing( ping ping, string cantidad ){
    
    /* ----------------------------------- DEFINICION COMANDO -------------------------------------------------*/
    string comando;
    comando  = "ping " + ping.ip;   // El comando ping, sirve para verificar la conectivcdad de la red con la ip
    comando += " -c " + cantidad;   // La opcion -c + cantidad sirve para especificar la cantidad de requests a realizar
    comando += " -q";               // La opcion -q devuelve solo el resumen de la red

    /* ----------------------------------- DEFINICION BUFFER -------------------------------------------------*/
    array <char, 128> buffer; /*  buffer tipo <char, 128>*/
    string salida; /* Declaracion de string llamada salida */

    unique_ptr<FILE,decltype(&pclose)>
    pipe(popen(comando.c_str(), "r"), pclose);

    /* ----------------------------------- ESTADO DE PIPE -------------------------------------------------*/
    if (!pipe){
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr){
        salida += buffer.data();
    }

    /* -------------------------------- ERROR DE PIPE (PING) ----------------------------------------------*/
    if (salida == ""){
        throw runtime_error("El ping fallo");
    }

    /*--------------------------- BUSQUEDA Y ASIGNACION DE RESPUESTAS DE REQUESTS -------------------------*/
        int pos_transmitidos = salida.find(" packets transmitted") - 1;
        int pos_recibidos = salida.find(" received") - 1;
        int pos_perdidos = salida.find(" packet loss") - 2;

    /*------------------- Transformamos y guardamos los paquetes transmitidos --------------------*/
        char v_aux[1];
        salida.copy(v_aux, 1, pos_transmitidos);
        ping.paquetes_transmitidos = atoi(v_aux); 

    /*--------------------- Transformamos y guardamos los paquetes recibidos ---------------------*/
        salida.copy(v_aux, 1, pos_recibidos);
        ping.paquetes_recibidos = atoi(v_aux); 

    /*--------------------- Transformamos y guardamos los paquetes perdidos ----------------------*/
        salida.copy(v_aux, 1, pos_perdidos);
        ping.paquetes_perdidos = atoi(v_aux);


    /*--------------------- DEFINICION DE ESTADO DE IP ----------------------*/
        if (ping.paquetes_recibidos >= 1){ /* Si recibe 1 o mas paquetes, el estado es UP */
            ping.estado = "UP";
        }
        else{/* Caso contrario, el estado es DOWN */
            ping.estado = "DOWN";
        }

    /* ---------------------------------------- IMPRESION DE ESTADISTICAS DE PAQUETES --------------------------------------------------*/
        if (ping.ip.length() >= 15){
            cout<<ping.ip<<" \t" << ping.paquetes_transmitidos << " \t" << ping.paquetes_recibidos << " \t" << ping.paquetes_perdidos << " \t" << ping.estado << "\n";
        }
        else{
            cout<<ping.ip<<" \t\t" << ping.paquetes_transmitidos << " \t" << ping.paquetes_recibidos << " \t" << ping.paquetes_perdidos << " \t" << ping.estado << "\n";
        }
}

int main(int argc, char *argv[]){
    /* ----------------------------------- CONDICIONAL DE ARGUMENTOS--------------------------------------------------*/
    if (argc < 2){ /* En caso de no ingresar los argumentos necesarios, ejecutara lo siguiente */
        cout<<"Argumentos insuficientes (2)"<<endl;
        return 1;
    }
    /* ----------------------------------------- ABRIR ARCHIVO -------------------------------------------------------*/
    ifstream ipFile; /* Se declara variable [ipFile] de tipo ifstream */
    ipFile.open(argv[1]); /* Abrimos el archivo */

    /* --------------------------------------- "ERROR" ABRIR ARCHIVO -------------------------------------------------*/
    if (ipFile.fail() ){ 
        cout<<" ERROR!: No se pudo abrir el archivo "<<endl;
        exit(1);
    }
    /* ---------------------------------------- "OK" ABRIR ARCHIVO ---------------------------------------------------*/
    // Depot sera de utilidad para asignar c/u de las IP en el array de tipo struct pings[]
    string depot; 
    // Si el archivo se abre correctamente, iniciara el proceso de completado del array pings[]
    if (ipFile.is_open()){
        int count = 0;
        while (ipFile.good()){
            ipFile >> depot;
            pings[count].ip = depot;
            count++;
        }
    }
    /* ---------------------------------------- IMPRESION CABECERA --------------------------------------------------*/
    cout<<"\tIP\t\tTrans.\tRec.\tPerd.\tEstado"<<endl;
    cout<<"-------------------------------------------------------"<<endl;
    /* ----------------------------------------- MANEJO DE HILOS ----------------------------------------------------*/
    thread threads[MAX_THREAD]; /* Numero maximo de hilos. */
    string n_requests = argv[2]; /* Numero de requests que el usuario desea realizar*/

    //Inicializa el llamado de "requestPing hasta que i < MAX_THREAD, mandando la IP y el n_requests"
    for (int i = 0; i < MAX_THREAD; i++){
        threads[i] = std::thread(requestPing, pings[i], n_requests);
    }
    /* ------------------------------------- SINCRONIZACION DE HILOS ------------------------------------------------*/
    for (int i = 0; i < MAX_THREAD; i++){
        threads[i].join(); /* Bloquea el subproceso de llamada hasta que finaliza el subproceso representado por la instancia */
        // Bloquea el subproceso de llamada, hasta que finalice
    }
    cout<<""<<endl;
    return 0;
 }
