#include "includes.h"
#include "globales.h"
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <numeric>


bool debug=true;
bool debug2=false;
vector <int> not_evaluated;
vector <float> ValuesToOrder;
vector <int> indexToOrder;
vector <int> indexOrdered;
vector < vector <float> > RankPerValue;
vector <float> valueszeroF;
vector <int> valueszeroI;
int num_usos_tot;
vector <float> denom_roulette; //Almacena suma de valores para ruleta, por parámetro
vector <calibracion> Pool_Final;
vector <tabu> tabulist;
vector <int> StatsNumValsTabu;
int tl_size;


using namespace std;


int int_rand (int a, int b)
{ 
  //drand48() devuelve valores en el intervalo [0.0, 1.0)
  int retorno = 0;
  if (a < b)
  {
    retorno = (int) ((b - a) * drand48 ());
    retorno = retorno + a;
  }
  else
  {
    retorno = (int) ((a - b) * drand48 ());
    retorno = retorno + b;
  }
  return retorno;
}


float float_rand(float a, float b) {
    float retorno = 0;

    if (a < b) {
        retorno = (float) ((b - a) * drand48());
        retorno = retorno + a;
    } else {
        retorno = (float) ((a - b) * drand48());
        retorno = retorno + b;
    }

    return retorno;
}


void salir(void)
{ 
  FILE *archivo;
  char *archivo_parametros="CLOIS_OUTPUT.params";
  if(debug) printf("Archivo_parametros: %s\n", archivo_parametros);
  archivo = fopen (archivo_parametros, "w");

  for(int i = 0; i < Pool_Final.size();i++){
      fprintf(archivo,"%s ", archivo_instancias);

      for(int p=0; p<cantidad_parametros; p++){
          fprintf(archivo," %f ", Pool_Final[i].parametro[p]);
      }
      fprintf(archivo, "AVG(Q): %f ", Pool_Final[i].aptitud_promedio);
      fprintf(archivo,"\n");

  }
  fclose(archivo);
  exit(0);
  return;
}

void agregar_semilla_instancia(vector <si> *lista)
{
  si si_temp;
  ifstream infile;
  infile.open(archivo_instancias); // open file 
  int sel=int_rand(0, CIT);
  int c=0;
  if(infile){
    string s="";
    while(infile){
      getline(infile,s);
      if(sel==c)
      {
        si_temp.instance=s;
        si_temp.seed=int_rand(0,10000);
        lista_semillas_instancias.push_back(si_temp);
        infile.close(); // file close
        return;
      }
      c++;
    }
  }
  else{
    cout<<"Archivo de instancias no encontrado!";
    infile.close();
  }
  return;
}

void mostrar_lista_semillas_instancias(void){
  for (vector<si>::const_iterator l=lista_semillas_instancias.begin(); l!=lista_semillas_instancias.end(); ++l){
    int sem=l->seed;
    const char *ins = l->instance.c_str();
    cout<<"S:"<<sem<<" I:"<<ins<<endl;
  }
  //getchar();
  return; 
}

void leer_archivo_configuracion(void){
  FILE *archivo;
  int response;
  archivo=fopen(archivo_configuracion, "r");
  response=fscanf(archivo, "%d", &cantidad_parametros);
  cout<<cantidad_parametros<<endl;

  if(cantidad_parametros < 0){
    printf("Cantidad de parametros invalida");
    fclose(archivo);
    exit(1);
  }
  
  configuracion_parametros=new( struct configuracion [cantidad_parametros] );
  
  for(int i=0; i< cantidad_parametros; i++){
    response=fscanf(archivo, "%s", configuracion_parametros[i].nombre);
    printf("nombre leido: %s\n", configuracion_parametros[i].nombre);
    response=fscanf(archivo, "%f", &configuracion_parametros[i].limite_minimo);
    printf("lim_min: %f\n", configuracion_parametros[i].limite_minimo);
    response=fscanf(archivo, "%f", &configuracion_parametros[i].limite_maximo);
    printf("lim_max: %f\n", configuracion_parametros[i].limite_maximo);
    response=fscanf(archivo, "%d", &configuracion_parametros[i].decimales);
    printf("decimales: %d\n", configuracion_parametros[i].decimales);
  }
  
  response=fscanf(archivo, "%d", &maximo_decimales);
  fclose(archivo);
  if(debug) printf("leer_archivo_configuracion done!\n");
  return;
}


void calcular_aptitud_semilla_instancia(calibracion *cal_temp, const char* instancia, int semilla, int s)
{
  float f_total=0.00;
  int response;

  ifstream resultados;
  FILE * sh;
  
  char archivo_resultados[1000];
  char archivo_sh[1000];
  strcpy (archivo_resultados, "resultados.res");
  strcpy (archivo_sh, "ejecutar_");
  strcat (archivo_sh, archivo_instancias);
  strcat (archivo_sh, ".sh");

  char comando[1024];
  char comando2[1024];
  float aptitud;

  for(int i=0; i< cantidad_parametros; i++)
    cout<<cal_temp->parametro[i]<<" ";
  cout<<endl;

  fflush(stdout);
  sprintf(comando, "rm -rf %s", archivo_resultados);
  response=system(comando);

  sprintf(comando, "bash %s %s %s %d", ejecutable, instancia, archivo_resultados, semilla);
  for(int i=0; i<cantidad_parametros; i++)
  {
    if(configuracion_parametros[i].decimales>1)
      sprintf(comando2," -%s %f", configuracion_parametros[i].nombre, cal_temp->parametro[i]);
    else
      sprintf(comando2," -%s %d", configuracion_parametros[i].nombre, cal_temp->parametro[i]);
    strcat(comando, comando2);
  }

  printf("%s\n", comando);
  response=system(comando);
  resultados.open(archivo_resultados);
  resultados>>aptitud;
  resultados.close();
    
  if(MAX_TIME!=0) //Cuando MAX_TIME==0, no se considera tiempo limite
    T+=aptitud;

  cal_temp->aptitud_promedio = (cal_temp->aptitud_promedio*(s) + aptitud)/((s)+1);
  //cal_temp->c_instancias_semillas++;
  printf("Aptitud Total: %2f (%d)\n", (cal_temp->aptitud_promedio), (s));
  
  E++;
  cout<<"-->Van "<< E <<" evaluaciones ("<< T <<" segundos)"<<endl;
  if((MAX_EVAL!=0) && (E>MAX_EVAL)) // Cuando MAX_EVAL==0, no se considera numero de evaluaciones limite
  {
    cout<<"MAX_EVAL!: "<<E<<endl;
    //salir();
  }

  if((MAX_TIME!=0) && (T>MAX_TIME)) //Cuando MAX_TIME==0, no se considera tiempo limite
  {
    cout<<"MAX_TIME!: "<<T<<endl;
    //salir();
  }
  return;
}

void calcular_aptitud_calibracion(calibracion *cal_temp)
{
  
  for(int i=0; i<NumSeeds; i++)
  {
    int is=int_rand(0,lista_semillas_instancias.size());
    //cout<<"is: "<<is<<endl;
    int sem=lista_semillas_instancias[is].seed;
    const char *ins = lista_semillas_instancias[is].instance.c_str();
    calcular_aptitud_semilla_instancia(cal_temp, ins, sem, i);
  }
  return;
}

//Es mejor a que b?
bool mejor(calibracion a, calibracion b)
{ 
  if(minimizar==1)
      if((a.aptitud_promedio) < (b.aptitud_promedio)) return true;
      else return false;
  else
      if((a.aptitud_promedio) > (b.aptitud_promedio)) return true;
      else return false;
}

//180119
bool existe_archivo(const char *fileName){
    ifstream infile(fileName);
    return infile.good();
}

int leer_archivo_candidatas(conjunto *co, int tamano){
  
  int candidatas=0;
  ifstream in(archivo_candidatas);
  string s;
  while(getline(in, s)) 
    if (!(s.compare(" ") == 0))
        candidatas++;
    
  cout<<"Hay "<< candidatas << " soluciones candidatas en el archivo." << endl;
  if(candidatas > tamano){
      cout<<"Se usaran sólo las "<< tamano << " primeras." << endl;
      candidatas = tamano;
  }
  in.close();
  
  FILE *archivo;
  int response;
  archivo=fopen(archivo_candidatas, "r");
  
  configuracion c_temp;
  
  for(int j=0; j<candidatas; j++){
    for(int i=0; i< cantidad_parametros; i++){
      if(configuracion_parametros[i].decimales == 1){ // Entero o categorico
          int v;
          response=fscanf(archivo, "%d", &v);
          if((v > configuracion_parametros[i].limite_maximo) || (v < configuracion_parametros[i].limite_minimo)) {
            cout << "Valor: " << v << " fuera del rango de valores especificado para: " << configuracion_parametros[i].nombre << endl;
            salir();
          }
          else
            co->cjto[j].parametro[i] = v;
      }
      else{ //Real
          float v;
          response=fscanf(archivo, "%f", &v);
          if(((int)(v * configuracion_parametros[i].decimales) > configuracion_parametros[i].limite_maximo) || ((int)(v * configuracion_parametros[i].decimales) < configuracion_parametros[i].limite_minimo)) {
            cout << "Valor: " << v << " fuera del rango de valores especificado para: " << configuracion_parametros[i].nombre << endl;
            salir();
          }
          co->cjto[j].parametro[i] = (int)(v * maximo_decimales);
          
      }
    }
  }
  fclose(archivo);
  if(debug) printf("leer_archivo_candidatas done!\n");
  return candidatas;
}


//Ver si es necesario mantenerlo
int calcular_M(void){
  int Maximo = 0;
  for(int i=0; i<cantidad_parametros; i++){
    if((configuracion_parametros[i].limite_maximo - configuracion_parametros[i].limite_minimo) > Maximo)
      Maximo = configuracion_parametros[i].limite_maximo - configuracion_parametros[i].limite_minimo;
  }
  //Version 130520 Tamaño de poblacion maximo
  if (Maximo > MaxM)
    Maximo = MaxM;
  //Version 140104 Tamaño de poblacion minimo
  if(Maximo < 10)
    Maximo = 10; 
  printf("calcular_M done!\n");
  return Maximo;
}

int contar_instancias_training(char * archivo){
  int lineas=0;
  string s;
  ifstream infile;
  infile.open(archivo); // open file 
  while(infile){
    getline(infile,s);
    //cout<<"Largo linea: "<<s.length()<<endl;
    //getchar();
    if(s.length()==0) return lineas;
    else lineas++;
  }
  return lineas;
}


template<typename T>
void pop_front(std::vector<T> &v)
{
    if (v.size() > 0) {
        v.erase(v.begin());
    }
}

template<typename T>
void initializeTabuList(std::vector<T> &v){
    tabu val;
    val.id_param = -1;
    val.valuetabu = -1;
    tabulist.clear();
    for(unsigned int i = 0; i < tl_size; i ++){
        v.push_back(val);
        //cout << v[i].valuetabu << " - " << v[i].id_param << endl;
    }

}
template<typename T>
void updateTabuList(std::vector<T> &v, int idpar, float vtabu){

    tabu val;
    val.id_param=idpar;
    val.valuetabu=vtabu;

    //Sacamos elemento Front
    pop_front(v);
    //Incluimos elemento Back
    v.push_back(val);
}

template<typename T>
void printTabuList(std::vector<T> &v){
    cout << "*******TABULIST+++++++++++++++++++++" << endl;
    for(unsigned int i = 0; i < v.size(); i++){
        cout << "record: " << i << " - param: " << tabulist[i].id_param << " - val: " << tabulist[i].valuetabu << endl;
    }
}

template<typename T>
bool CheckTabu(std::vector<T> &v, int idpar,float vtabu){

    bool flag = false;

    for(unsigned int i = 0; ((i < v.size()) && !flag) ; i++){

        //cout <<  v[i].id_param << " v.s. " << idpar << endl;
        if(v[i].id_param == idpar ){
            //cout << v[i].valuetabu << " v.s. " << vtabu << endl;
            if(v[i].valuetabu == vtabu){
                cout << "Param: " << idpar << " - Val:" << vtabu << " ES TABU" << endl;
                flag = true;
            }
        }
    }

    return flag;
}


void GenerateInitialSolution(calibracion *cur){

    unsigned int p;
    float minimumvalue = 0.1;
    float rnd;

    cur->parametro.clear();
    cout << "NUEVA CONFIG INICIAL" << endl;
    for(p = 0; p < cantidad_parametros; p++){
        //Caso valor con decimales
        if(configuracion_parametros[p].decimales > 1){
            rnd = float_rand(configuracion_parametros[p].limite_minimo, configuracion_parametros[p].limite_maximo);
            rnd = floorf(rnd * 10) / 10;
            //El rand puede dar menor que el valor minimo permitido por simpletuner (0.1)
            if(configuracion_parametros[p].limite_minimo < minimumvalue){
                while(rnd < minimumvalue){
                    rnd = float_rand(configuracion_parametros[p].limite_minimo, configuracion_parametros[p].limite_maximo);
                    rnd = floorf(rnd * 10) / 10;
                }
            }
        }
        else{
            //Caso valores Enteros
            rnd = int_rand(configuracion_parametros[p].limite_minimo, configuracion_parametros[p].limite_maximo);
        }
        cout << configuracion_parametros[p].nombre << " - " << rnd << endl;

        cur->parametro.push_back(rnd);
	//cout << "TAMANO PARAMETRO -->" << cur->parametro.size() << endl;
	//getchar();
    }



    calcular_aptitud_calibracion(&*cur);

}

void ManageBoundariesOfParameters() {

    int lim_min, lim_max, dec;
    dec = 0;
    float prop = 0;
    float minimumvalue = 0.1;
    float propalto = 0;
    float rnd = 0;
    for (int p = 0; p < cantidad_parametros; p++) {
        if(configuracion_parametros[p].decimales > 1){
            configuracion_parametros[p].limite_minimo = configuracion_parametros[p].limite_minimo/configuracion_parametros[p].decimales;
            configuracion_parametros[p].limite_maximo = configuracion_parametros[p].limite_maximo/configuracion_parametros[p].decimales;
           // cout << configuracion_parametros[p].nombre << configuracion_parametros[p].limite_minimo << " - " << configuracion_parametros[p].limite_maximo << endl;
        }

        //For Stats TabuList
        StatsNumValsTabu.push_back(0);
    }

}

void StatsTabuList(int par){
    StatsNumValsTabu[par]++;
}

void PrintStatsTabuList(){

    cout << "*******************PRINT STATS TABU LIST*********************" << endl;
    for(unsigned int i = 0; i < StatsNumValsTabu.size(); i++){
        cout << "PARAM:" << i << " - NumTimesIncludedInTabu: " << StatsNumValsTabu[i] << endl;
    }
    cout << "*************************************************************" << endl;
}


void SimpleTuner(){

    calibracion cur,mod,best_restart;
    int countbests,tries,num_max_tries;
    int par_mod;
    bool flag = true;
    float val_mod,rnd,minimumvalue;
    num_max_tries = 5;
    minimumvalue = 0.1;
    bool stagnation_flag = true;
    countbests = 0;
    tries = 0;

    ManageBoundariesOfParameters();

    while(E < MAX_EVAL){

        GenerateInitialSolution(&cur);
        initializeTabuList(tabulist);
        printTabuList(tabulist);

        while(stagnation_flag){

            mod = cur;
            //Qué parametro voy a cambiar - rnd chosen
            par_mod = int_rand(0, cantidad_parametros);
            //Valor aleatorio a asignar
            if(configuracion_parametros[par_mod].decimales > 1){

                //El rand puede dar menor que el valor minimo permitido por simpletuner (0.1)
                while(flag){
                    rnd = float_rand(configuracion_parametros[par_mod].limite_minimo, configuracion_parametros[par_mod].limite_maximo);
                    rnd = floorf(rnd * 10) / 10;
                    if(configuracion_parametros[par_mod].limite_minimo < minimumvalue){
                        while(rnd < minimumvalue){
                            rnd = float_rand(configuracion_parametros[par_mod].limite_minimo, configuracion_parametros[par_mod].limite_maximo);
                            rnd = floorf(rnd * 10) / 10;
                        }
                    }

                    if(!CheckTabu(tabulist,par_mod,rnd)){
                        //Si entra no es tabu
                        flag = false;
                    }
                }

                flag = true;

            }
            else{
                //Caso valores Enteros
                rnd = int_rand(configuracion_parametros[par_mod].limite_minimo, configuracion_parametros[par_mod].limite_maximo);
                while(CheckTabu(tabulist,par_mod,rnd)){ //no es tabu -> ret false
                    rnd = int_rand(configuracion_parametros[par_mod].limite_minimo, configuracion_parametros[par_mod].limite_maximo);
                }
            }

            mod.parametro[par_mod] = rnd;
            calcular_aptitud_calibracion(&mod);
            switch(minimizar){
                case 0: //max
                    break;
                case 1:
                    if(cur.aptitud_promedio >= mod.aptitud_promedio){
                        cout << "ME CAMBIE A UNA MEJOR CONFIG DESDE " << cur.aptitud_promedio << " A " << mod.aptitud_promedio << endl;
                        cur = mod;
                        tries = 0;
                    }
                    else{
                        updateTabuList(tabulist,par_mod,rnd);
                        printTabuList(tabulist);
                        StatsTabuList(par_mod);
                        tries++;
                    }
                    break;
                default:
                    break;
            }
            if(tries > num_max_tries){
                stagnation_flag = false;
                cout << "SE ACABO EL RESTART" << endl;
            }
            if(E > MAX_EVAL) stagnation_flag = false;

        }

        if(countbests == 0){
            best_restart = cur;
            Pool_Final.push_back(cur);
        } else{
            switch(minimizar){
                case 0: //max
                    break;
                case 1:
                    if(best_restart.aptitud_promedio >= cur.aptitud_promedio){
                        Pool_Final.push_back(cur);
                        best_restart = cur;
                        tries = 0;
                    }
                    break;
                default:
                    break;
            }
        }

        PrintStatsTabuList();
        countbests++;
        stagnation_flag = true;
        tries = 0;
    }

}



int main (int argc, char *argv[])
{
  ejecutable=argv[1];
  archivo_configuracion=argv[2];
  archivo_instancias=argv[3];
  CIT=contar_instancias_training(archivo_instancias);
  if(debug2) cout<<"Contador Instancias Training: "<<CIT<<endl;
  SEED=atoi(argv[4]);
  NumSeeds=atoi(argv[5]);
  MaxM=atoi(argv[6]);  

  MAX_EVAL=atoi(argv[7]);
  //MAX_ITER=MAX_EVAL;
  MAX_TIME=atoi(argv[8]);
  minimizar=atoi(argv[9]);
  tl_size=atoi(argv[10]);


  cout<<"------------------------------------------------------------------------"<<endl;
  cout<<"---------------------------- INICIALIZACION ----------------------------"<<endl;
  cout<<"------------------------------------------------------------------------"<<endl;
  
  leer_archivo_configuracion();
  srand48(SEED);

  cout<<"------------------------------------------------------------------------"<<endl;
  cout<<"-------------------- CREACION CONJUNTO INICIAL -------------------------"<<endl;
  cout<<"------------------------------------------------------------------------"<<endl;

  //Ver si es necesaria esta funcion
  int M=calcular_M();
  poblacion.id=0;
  
  int Max_IS;
  if(MAX_EVAL!=0) Max_IS=MAX_EVAL;
  if(MAX_TIME!=0) Max_IS=MAX_TIME*10;
  MAX_ITER=Max_IS;

  for(int i=0; i<Max_IS; i++)
  {
    if(debug2) cout<<"Agregando semilla/instancia: "<<i<<endl;
    agregar_semilla_instancia(&lista_semillas_instancias);
  }

  SimpleTuner();

  cout << "NO ME HE CAIDO" << endl;
  for(int i = 0; i < Pool_Final.size(); i++){
      for(int j = 0; j < cantidad_parametros; j++){
          cout << Pool_Final[i].parametro[j] << " - ";
      }

      cout << " Q: " << Pool_Final[i].aptitud_promedio << endl;

  }



  //crear_conjunto_inicial(&poblacion, M);


  cout<<"Max Iteraciones!"<<I<<endl;
  salir();
  return 0;
}
