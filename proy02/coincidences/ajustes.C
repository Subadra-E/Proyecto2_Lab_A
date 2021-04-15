/*
 * ajustes.C: Muestra como llenar un histograma
 * con números aleatorios generados a partir
 * de una función definida y luego hacer un ajuste
 * con esta misma función.
 * 
 * Ejemplo para el curso introductorio de ROOT.
 * Laboratorio Avanzado
 * 
 * Héctor Pérez, febrero 2021.
 *
 * Modificaciones: Subadra Echeverria, Marzo 2021.
 * 
 */

/************Librerías*****************/
#include "TROOT.h"
#include "TH1D.h"
#include "TF1.h"

/*******************************CAMBIOS*********************************************/
   
   /*********Funcion exponencial para la distribución de probabilidad***********/
   /***************de los pulsos por medio de constructores de ROOT************/
void MiFit() 
{
   

    /* se define la función A*e^(-t/T)+C con  el rango que va desde 0 a 20000 nanosegundos*/
    /*que es aproximadamente el rango que se observan en los histogramas*/
    TF1 *expn = new TF1("exp","[0]*TMath::Exp(-x/[1])+[2]",0,20000); 
    /*******se nombran los parámetros a utilizar en la función******/
    expn->SetParNames("A","T","C"); 
    /************Se dan valores iniciales a los parámetros**********/
    expn->SetParameters(1450,2195,5);
    
    /****ahora se procede a leer el archivo que almacena la suma de los 6 histogramas para hacer el ajuste*****/
    /*****el histograma rresultante se llama histot.root*****/
    TFile *Arreglo = new TFile("histot.root");
    /********Se utiliza el método "Get" para cargar el contenido del histograma*****/
    /**************este histograma se guardó con el nombre "h1"*********/
    TH1D *MiArreglo=(TH1D*)Arreglo->Get("h1");
    
    /***************Se crea un Canvas que mostrará la salida en
    pantalla del histograma con el ajuste realizado
    el tamaño es de 8000 x 600 pixeles **************/
    
    TCanvas *Lienzo = new TCanvas("L1","Ajuste",800,600);
    /******se utiliza el método "Fit" para realizar el ajuste llamado "expn"******/
    MiArreglo->Fit("expn");


    /* Ajuste de la función a los datos en el histograma */
  
    TFitResultPtr resultados = MiArreglo->Fit("exp","QS");
    /**********Se imprimen los resultados **********/
    resultados->Print("V");
    /************Valor de Chi cuadrado************/
    std::cout<< "Chi2/NDF: " << resultados->Chi2()/resultados->Ndf() << std::endl;
    /**************Se imprimen los valores de los errores de los parámetros ****/
    std::cout<< "A: " << resultados->Parameter(0) << std::endl;
    std::cout<< "error en A: " << resultados->ParError(0) << std::endl;
    std::cout<< "T: " << resultados->Parameter(1) << std::endl;
    std::cout<< "error en T: " << resultados->ParError(1) << std::endl;
    std::cout<< "C: " << resultados->Parameter(2) << std::endl;
    std::cout<< "error en C: " << resultados->ParError(2) << std::endl;
    }