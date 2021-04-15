/*
 * coincidences.cc: Codigo para encontrar coincidencias en los datos
 * provenientes de las placas centelladoras de Escaramujo.
 * 
 * Héctor Pérez
 * Marzo 2021
 * 
 * Modificaciones: Subadra Echeverria
 * Abril 2021
 *
 * Basado en el código de Federico Izraelevich (2015) 
 * para procesar datos de Escaramujo.
 * 
 */

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <cstring>


#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TF1.h"



/* Este programa toma como argumento el nombre del archivo .root donde se
 * encuentran los datos provenientes de Escaramujo */
int main(int argc,char *argv[]) {

  if( argc < 2 )
  {
    std::cout << "Error: No input file\n";
    std::exit(1);
  }

  TFile *DataFile = new TFile( argv[1] );
  TTree *dataTree = 0;

  std::cout << argv[1] << " open..\n";

  /* Se asocia un arbol para leer los datos del archivo */

  DataFile->GetObject("dataTree", dataTree);


  std::cout << "data tree found..\n";

  dataTree->SetBranchStatus("*",0);
  dataTree->SetBranchStatus("TSDate",1);
  dataTree->SetBranchStatus("TSTime",1);
  dataTree->SetBranchStatus("TSNs",1);
  dataTree->SetBranchStatus("RE0",1);
  dataTree->SetBranchStatus("FE0",1);
  dataTree->SetBranchStatus("RE1",1);
  dataTree->SetBranchStatus("FE1",1);
  dataTree->SetBranchStatus("RE2",1);
  dataTree->SetBranchStatus("FE2",1);
  dataTree->SetBranchStatus("RE3",1);
  dataTree->SetBranchStatus("FE3",1);


  std::cout << "brances activated..\n";
  
  /* Variables para cargar los datos del archivo
   *
   * TSDateData: Cadena de caracteres que aloja la fecha de captura de datos */
	char TSDateData[7];
  /* TSTimeData: Cadena de caracteres que aloja la hora de captura de datos
   * con presición de segundos */
	char TSTimeData[7];
  /* TSNsData: Entero que indica los nanosegundos transcurridos desde 
   * TSTimeData hasta el inicio del evento capturado */
	uint32_t TSNsData;

  /* RE<ch>: Vectores que contienen los tiempos en los cuales el voltaje 
   * de la señal de los pulsos excedieron el umbral predefinido para el canal ch.
   * Los tiempos son los nanosegundos que transcurrieron desde TSNsData.
   * 
   * FE<ch>: Vectores que contienen los tiempos en los cuales el voltaje
   * de la señal de los pulsos dejaron de exceder el umbral predefinido 
   * para el canal ch. Tambien están medidos en nanosegundos desde
   * TSNsData. */
  std::vector<float> *RE0 = 0;
  std::vector<float> *FE0 = 0;
  std::vector<float> *RE1 = 0;
  std::vector<float> *FE1 = 0;
  std::vector<float> *RE2 = 0;
  std::vector<float> *FE2 = 0;
  std::vector<float> *RE3 = 0;
  std::vector<float> *FE3 = 0;


  dataTree->SetBranchAddress("TSDate",TSDateData);
  dataTree->SetBranchAddress("TSTime",TSTimeData);
  dataTree->SetBranchAddress("TSNs",&TSNsData);
  dataTree->SetBranchAddress("RE0",&RE0);
  dataTree->SetBranchAddress("FE0",&FE0);  
  dataTree->SetBranchAddress("RE1",&RE1);
  dataTree->SetBranchAddress("FE1",&FE1);  
  dataTree->SetBranchAddress("RE2",&RE2);
  dataTree->SetBranchAddress("FE2",&FE2);  
  dataTree->SetBranchAddress("RE3",&RE3);
  dataTree->SetBranchAddress("FE3",&FE3);  
  
  
  std::cout << "brances asociated..\n";
      
  /* Variables para determinar las coincidencias */

  /* kllCutCoin: límite inferior de coincidencias. No se toman en 
   * cuenta pulsos cuya separación sea menor que este valor temporal
   * en nanosegundos. */
  int kllCutCoin = 2;
  /* kulCutCoin: límite superior de coincidencias. Si los pulsos
   * están separados mas de este tiempo, no se consideran coincidencias.
   * (valor en nanosegundos) */
  int kulCutCoin = 4;
  
  /* Variables para conteo de coincidencias
   *
   * El canal 0 (ch0) se asocia con la placa A
   * El canal 1 (ch1) se asocia con la placa B
   * El canal 2 (ch2) se asocia con la placa C
   * el canal 3 (ch3) no esta físicamente conectado a ninguna placa
   * 
   * Las variables *_Tcounter llevan el conteo total de coincidencias */
  int AnC_Tcounter = 0;
  int AnB_Tcounter = 0;
  int BnC_Tcounter = 0;
  int AnBnC_Tcounter = 0;
  /* Las variables *_counter llevan el conteo de coincidencias por evento */
  int AnC_counter = 0;
  int AnB_counter = 0;
  int BnC_counter = 0;
  int AnBnC_counter = 0;
  /* nic_counter lleva el conteo de eventos que no contienen coincidencias */
  int nic_counter = 0;
  /* La variable boleana coincidence sirve para marcar los eventos que contienen
   * coincidencias */
  bool coincidence = false;

  /* Se obtiene el número de eventos en el archivo .root */
  int n_events = (int) dataTree->GetEntries();

  std::cout << n_events << " events found\n";

  /*contador de pulsos despues de delta t en el canal 1 (placa B)*/
  

  /*intervalo de tiempo propuesto luego de las coincidencias en las placas A Y C*/
  int Dt=500; /*ns*/

  /*abriendo un archivo para guardar el histograma*/
  TFile *aFile= new TFile("h7.root","RECREATE");
  /*Se crea el histograma*/ 
  TH1D *aHist = new TH1D("h1"," Vida  Media del Muon (Escaramujo) ",45,0,20000);
  
  /* Se inicia el ciclo para leer los eventos del archivo */
  for(int i=0; i<n_events; i++)
  {
    
    /* Se cargan los datos del i-ésimo evento*/
    dataTree->GetEntry(i);

    /* Iniacialización de las variables contadoras por evento */
    AnB_counter = 0;
    AnC_counter = 0;
    BnC_counter = 0;
    AnBnC_counter = 0;
    
    coincidence = false;

    /* Coincidencias entre las placas A y C (canales 0 y 2) 
     * 
     * Primero se dertemina que existan pulsos en los canales 
     * correspondientes. Para ello se verifica que el tamaño de
     * los vectores asociados a los canales 0 y 2 sea mayor a 0. */
    if((RE0->size() > 0) && (RE2->size() > 0)) // 0 y 2
    {
       /* Si existen pulsos en estos canales, se mide la diferencia de 
       * tiempo en el primer pulso que llega a cada canal. Luego se
       * verifica que esta diferencia de tiempo sea mayor o igual a
       * kllCutCoin y menor que kulCutCoin */
      if( ( fabs(RE0->at(0)-RE2->at(0)) >= kllCutCoin ) & ( fabs(RE0->at(0)-RE2->at(0)) <= kulCutCoin ) )
      {
        /* Si se cumple esta condición entonces se cuenta la coincidencia
         * entre estas placas */
        AnC_counter++;
        coincidence = true;
        
        /*si el tiempo entre el pulso en la placa B y la coincidencia entre las placas A y C
        es mayor o igual al intervalo de tiempo delta t, entonces se llena el histograma
        con la diferencia entre tiempos*/
        if( (RE1->size() > 0) && (fabs(RE0->at(0)-RE1->at(0))>=Dt) )
        {
          /*se llena el histograma con la diferencia entre tiempos*/
          aHist->Fill(fabs(RE0->at(0)-RE1->at(0)));
          
        }
          
      }
    }


          
    /* Se repite el procedimiento para las demas placas */
    
    if((RE0->size() > 0) && (RE1->size() > 0)) // 0 y 1
    {
      if(( fabs(RE0->at(0)-RE1->at(0)) >= kllCutCoin ) &  ( fabs(RE0->at(0)-RE1->at(0)) <= kulCutCoin ))
      {
        AnB_counter++;
        coincidence = true;
      }
    }
    
    if((RE1->size() > 0) && (RE2->size() > 0)) // 1 y 2
    {
      if(( fabs(RE1->at(0)-RE2->at(0)) >= kllCutCoin ) &  ( fabs(RE1->at(0)-RE2->at(0)) <= kulCutCoin ))
      {
        BnC_counter++;
        coincidence = true;
      }
    }

    if((RE0->size() > 0) && (RE1->size() > 0) && (RE2->size() > 0)) // 1, 2 y 3
    {
      if(( fabs(RE0->at(0)-RE1->at(0)) >= kllCutCoin ) &  ( fabs(RE0->at(0)-RE1->at(0)) <= kulCutCoin ))
      {
        if(( fabs(RE0->at(0)-RE2->at(0)) >= kllCutCoin ) &  ( fabs(RE0->at(0)-RE2->at(0)) <= kulCutCoin ))
        {
          AnBnC_counter++;
        }
      }
    }

    /* Si no se encontró que se cumplieran las condiciones de coincidencia,
     * también se cuenta. */
    if(!coincidence)
    {
      nic_counter++;
    }

    /* Se actualizan los contadores totales */
    AnB_Tcounter += AnB_counter;
    AnC_Tcounter += AnC_counter;
    BnC_Tcounter += BnC_counter;
    AnBnC_Tcounter += AnBnC_counter;
    
  }

  /* Se muestran los resultados */
  std::cout<< "Total coincidendes in channels 0 y 1: " << AnB_Tcounter << std::endl;
  std::cout<< "Total coincidendes in channels 0 y 2: " << AnC_Tcounter << std::endl;
  std::cout<< "Total coincidendes in channels 1 y 2: " << BnC_Tcounter << std::endl;
  std::cout<< "Total coincidendes in channels 0,1,2: " << AnBnC_Tcounter << std::endl;
  std::cout<< "Not in coincidence: " << nic_counter << std::endl;
  
  aHist->Write(); /* se almacena el histograma y  se cierra el archivo*/
  aFile->Close();
  
  DataFile->Close();
  return 0;
}

  
