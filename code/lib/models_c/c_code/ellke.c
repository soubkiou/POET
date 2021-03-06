#include<Python.h>
#include<numpy/arrayobject.h>
#include<math.h>
#include<omp.h>

#define IND(a,i) *((double *)(a->data+i*a->strides[0]))

static PyObject *ellke(PyObject *self, PyObject *args);

static PyObject *ellke(PyObject *self, PyObject *args)
{
  PyArrayObject *k, *ek, *kk;
  int i;
  double a0,a1,a2,a3,a4,b0,b1,b2,b3,b4,m1;
  npy_intp dims[1];

  if(!PyArg_ParseTuple(args,"O", &k))
    {
      return NULL;
    }
  
  a1=0.44325141463;
  a2=0.06260601220;
  a3=0.04757383546;
  a4=0.01736506451;
  b1=0.24998368310;
  b2=0.09200180037;
  b3=0.04069697526;
  b4=0.00526449639;
  
  dims[0] = k->dimensions[0];
  ek      = (PyArrayObject *) PyArray_SimpleNew(1,dims,PyArray_DOUBLE);
  #pragma omp parallel for private(m1)
  for(i=0;i<dims[0];i++)
    {
      m1        = 1.-pow(IND(k,i),2);
      IND(ek,i) = 1.+m1*(a1+m1*(a2+m1*(a3+m1*a4))) - m1*(b1+m1*(b2+m1*(b3+m1*b4)))*log(m1);
    }
    
  a0=1.38629436112;
  a1=0.09666344259;
  a2=0.03590092383;
  a3=0.03742563713;
  a4=0.01451196212;
  b0=0.5;
  b1=0.12498593597;
  b2=0.06880248576;
  b3=0.03328355346;
  b4=0.00441787012;
  
  kk      = (PyArrayObject *) PyArray_SimpleNew(1,dims,PyArray_DOUBLE);
  #pragma omp parallel for private(m1)
  for(i=0;i<dims[0];i++)
    {
      m1        = 1.-pow(IND(k,i),2);
      IND(kk,i) = a0+m1*(a1+m1*(a2+m1*(a3+m1*a4))) - (b0+m1*(b1+m1*(b2+m1*(b3+m1*b4))))*log(m1);
    }
  return Py_BuildValue("NN",ek,kk);
}

static char ellke_doc[]="\
   Computes Hasting's polynomial approximation for the complete\n\
   elliptic integral of the first (ek) and second (kk) kind.\n\
\n\
   Parameters\n\
   ----------\n\
   k:       1D NPY ARRAY - contains values from trquad.py\n\
\n\
   Returns\n\
   -------\n\
   ek:      1D NPY ARRAY - elliptic integral of the first kind\n\
   kk:      1D NPY ARRAY - elliptic integral of the second kind\n\
\n\
   Revisions\n\
   ---------\n\
   Original version by Jason Eastman\n\
   2012-08-25   Kevin Stevenson, UChicago \n\
                kbs@uchicago.edu\n\
                Converted from Python\n\
";

static PyMethodDef ellke_methods[] = {
  {"ellke", ellke,METH_VARARGS,ellke_doc},{NULL}};

void initellke(void)
{
  Py_InitModule("ellke", ellke_methods);
  import_array();
}

