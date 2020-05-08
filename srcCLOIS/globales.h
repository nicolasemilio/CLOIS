#ifndef externo
int cantidad_parametros;
#endif

#ifdef externo
extern int cantidad_parametros;
#endif

#include <vector>
#include <ostream>

struct si
{
  int seed;
  string instance;
};

struct tabu{
    int id_param; //parameter number: [0, cant_parameters]
    float valuetabu;
};

struct configuracion
{
  char nombre[30];
  float limite_minimo, limite_maximo;
  int decimales;
  int t_dominio;
};

class calibracion
{
  public:
  float aptitud_promedio; // Esto considera solo una instancia a sintonizar
  std::vector<float> parametro;
  calibracion();
//   calibracion(const calibracion &);
  ~calibracion(){};
  calibracion& operator=(const calibracion &rhs);
  int operator==(const calibracion &rhs) const;
  bool operator<(const calibracion &rhs) const;
  friend std::ostream& operator<<(std::ostream &, const calibracion &);
};

class conjunto
{ 
  public: 
  int id;
  std::vector <calibracion> cjto;
  void vaciar(void);
  void ordenar(void);
  friend std::ostream& operator<<(std::ostream &, const conjunto &);
};

class poblacion
{
  public:
  int id;
  std::vector <conjunto> cjt;
  void vaciar(void);
  friend std::ostream& operator<<(std::ostream &, const poblacion &);
};

/*******************************************************************/
#ifndef externo
//list <valores> *ruleta_parametro;
vector <si> lista_semillas_instancias;
vector< list<int> > PossibleValuesPerParameter;
vector< list<int> > ValuesThatProducedAnImprovement;
vector < vector < float > > ScorePerValue;
vector < vector < int > > NumberOfUses;
vector < int > PositionsCurrent;
vector < int > PositionsMod;
int crit_uno, crit_dos;
unsigned int evals;
float ponderaBV, ponderaNU;

int I, E=0, T=0;
poblacion Poblacion, Poblacion_temporal;
char *archivo_configuracion;
char *archivo_instancias;
//180119
char *archivo_candidatas;
//180119
char *ejecutable;
struct configuracion *configuracion_parametros;
int **valores_parametros;
conjunto poblacion, poblacion_intermedia;
conjunto OLPoolCalibrations, PoolCalibrations;
int NumSeeds;
int SEED;
int MaxM;
int maximo_decimales;
int MAX_ITER, MAX_EVAL, MAX_TIME;
int CIT;
int minimizar;
#endif

void generar_y_ordenar_matriz_valores_parametros(int, int **);

/*******************************************************************/

