#define externo
#include "includes.h"
#include "globales.h" 

calibracion::calibracion()   // Constructor
{
  aptitud_promedio = 0.00;
}

bool calibracion::operator<(const calibracion &a) const
{   
  if(a.aptitud_promedio > aptitud_promedio) return 1;
  return 0;
}

calibracion& calibracion::operator=(const calibracion &a)
{
  this->aptitud_promedio = a.aptitud_promedio;
  this->parametro = a.parametro;
  return *this;
}

int calibracion::operator==(const calibracion &a) const
{
  if( this->aptitud_promedio != a.aptitud_promedio) return 0;
  return 1;
}

std::ostream& operator<<(std::ostream &output, const calibracion &c)
{
  output << "[" << &c << "] ( ";
  for(int i=0; i<cantidad_parametros; i++)
    output << c.parametro[i] << " ";
  output << ") ["<< c.aptitud_promedio<<"]"<<endl;
  return output;
}

void conjunto::vaciar(void)
{
  int size = cjto.size();
  for(int i=0; i<size; i++){
    cjto[i].parametro.clear();
  }
  cjto.clear();
}

std::ostream& operator<<(std::ostream &output, const conjunto &co)
{
  output <<endl<< "--------------Conjunto "<<co.id+1<<" ("<<co.cjto.size()<<")-----------------------"<<endl;
  if(co.cjto.size()>0)
  {
    for (vector<calibracion>::const_iterator ca = co.cjto.begin(); ca!=co.cjto.end(); ++ca)
    {
      output<<*ca;   
    }
  }
  return output;
}

void poblacion::vaciar(void)
{
  int size = cjt.size();
  for(int i=0; i<size; i++){
    cjt[i].cjto.clear();
  }
  cjt.clear();
}

std::ostream& operator<<(std::ostream &output, const poblacion &po)
{
  output << "--------------Poblacion "<<po.id+1<<" ("<<po.cjt.size()<<")----------------------"<<endl;
  if(po.cjt.size()>0)
  {
    for (vector<conjunto>::const_iterator co = po.cjt.begin(); co!=po.cjt.end(); ++co)
    {
      output<<*co;   
    }
  }
  return output;
}
