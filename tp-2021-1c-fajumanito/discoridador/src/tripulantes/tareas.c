#include "tareas.h"

void generar_Oxigeno(int cantidadAGenerar){
    FILE *fptr;
    fptr = fopen("ARCHIVO", "r+");  //Cargar PATH para archivo

    if(fptr == NULL){       // Si no existe creo el archivo y lo escribo
        fptr = fopen("Oxigeno.ims", "w"); 
        fprintf(fptr, "%d", cantidadAGenerar);
    }else{                  // Si existe sumo los valores nuevo mas el existente.
        int temp;
        fscanf(fptr,"%d", &temp);
        cantidadAGenerar = cantidadAGenerar + temp;
        fprintf(fptr, "%d", cantidadAGenerar);
    }
    fclose(fptr);
}

void consumir_Oxigeno(int cantidadAConsumir){
    FILE *fptr; 
    fptr = fopen("ARCHIVO", "r+");  //Cargar PATH para archivo

    if(fptr == NULL){ 
        printf("No hay oxigeno disponible en la nave");
    }else{ 
        int temp;
        fscanf(fptr,"%d", &temp);
        if(cantidadAConsumir > temp){
            fprintf(fptr, "%d", 0);
            printf("No hay mas oxigeno disponible");
        }else{
            cantidadAConsumir = cantidadAConsumir - temp;   //considero que la cantidad de oxigeno se puede
            fprintf(fptr, "%d", cantidadAConsumir);         //sumar y guardo eso en el archivo
        }
        
    }
    fclose(fptr);
}

void generar_Comida(int cantidadAGenerar){
    FILE *fptr;
    fptr = fopen("ARCHIVO", "r+");  //Cargar PATH para archivo

    if(fptr == NULL){
        fptr = fopen("Comida.ims", "w"); 
        fprintf(fptr, "%d", cantidadAGenerar);
    }else{                  
        int temp;
        fscanf(fptr,"%d", &temp);
        cantidadAGenerar = cantidadAGenerar + temp;
        fprintf(fptr, "%d", cantidadAGenerar);
    }
    fclose(fptr);
}

void consumir_Comida(int cantidadAConsumir){
    FILE *fptr; 
    fptr = fopen("ARCHIVO", "r+");  //Cargar PATH para archivo

    if(fptr == NULL){ 
        printf("No hay comida disponible en la nave");
    }else{ 
        int temp;
        fscanf(fptr,"%d", &temp);
        if(cantidadAConsumir > temp){
            fprintf(fptr, "%d", 0);
            printf("No hay mas comida disponible");
        }else{
            cantidadAConsumir = cantidadAConsumir - temp;
            fprintf(fptr, "%d", cantidadAConsumir);
        }
        
    }
    fclose(fptr);
}

void generar_Basura(int cantidadAGenerar){
    FILE *fptr;
    fptr = fopen("ARCHIVO", "r+");  //Cargar PATH para archivo

    if(fptr == NULL){
        fptr = fopen("Basura.ims", "w"); 
        fprintf(fptr, "%d", cantidadAGenerar);
    }else{                  
        int temp;
        fscanf(fptr,"%d", &temp);
        cantidadAGenerar = cantidadAGenerar + temp;
        fprintf(fptr, "%d", cantidadAGenerar);
    }
    fclose(fptr);
}

void tirar_basura(){
    FILE *fptr; 
    fptr = fopen("ARCHIVO", "r+");  //Cargar PATH para archivo

    if(fptr == NULL){ 
        printf("No hay basura en la nave");
    }else{ 
        remove("Basura.ims"); //elimina labasura
    }
    fclose(fptr);
}

void funcionSolucionCantBloques(){
    
}

void funcionSolucionBitMap(){

}

void funcionSolucionSize(){

}

void funcionSolucionBlock_Count(){

}

void funcionSolucionBlocks(){

}
