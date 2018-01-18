#include<Python.h>
#include<numpy/arrayobject.h>
#include<math.h>
#include<omp.h>

#define IND(a,i) *((double *)(a->data+i*a->strides[0]))

static PyObject *log4qramp(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *log4qramp(PyObject *self, PyObject *args, PyObject *keywds)
{
  PyObject *etc;
  PyArrayObject *x,*y,*rampparams;
  double x0,a,b,c,d,e,f,g,x1;
  int i;
  npy_intp dims[1];

  static char *kwlist[] = {"rampparams","x","etc",NULL};

  //etc = PyList_New(0);

  if(!PyArg_ParseTupleAndKeywords(args,keywds,"OO|O",kwlist,&rampparams,&x,&etc))
    {
      return NULL;
    }
  
  x0 = IND(rampparams,0);
  a  = IND(rampparams,1);
  b  = IND(rampparams,2);
  c  = IND(rampparams,3);
  d  = IND(rampparams,4);
  e  = IND(rampparams,5);
  f  = IND(rampparams,6);
  g  = IND(rampparams,7);
  x1 = IND(rampparams,8);

  dims[0] = x->dimensions[0];

  y = (PyArrayObject *) PyArray_SimpleNew(1,dims,PyArray_DOUBLE);
  #pragma omp parallel for 
  for(i=0;i<dims[0];i++)
    {
      if(IND(x,i)<=x0)
        {
          IND(y,i) = 1;
        }
      else
	{
	  IND(y,i) = a*pow(log(IND(x,i)-x0),4)+b*pow(log(IND(x,i)-x0),3) \
	    +c*pow(log(IND(x,i)-x0),2)+d*log(IND(x,i)-x0)+e*pow((IND(x,i)-x1),2)\
	    +f*(IND(x,i)-x1)+g;
	}
    }
  return PyArray_Return(y);
}

static char log4qramp_doc[]="\
  This function creates a model that fits a ramp using quartic-log + quadratic polynomial.\n\
\n\
  Parameters\n\
  ----------\n\
    x0: phase offset for log\n\
    a:	log(x)^4 term\n\
    b:	log(x)^3 term\n\
    c:	log(x)^2 term\n\
    d:	log(x) term\n\
    e:	quadratic term\n\
    f:	linear term\n\
    g:  constant term\n\
    x1: phase offset for polynomial\n\
    x:	Array of time/phase points\n\
\n\
  Returns\n\
  -------\n\
    This function returns the flux values for the ramp models\n\
\n\
  Revisions\n\
  ---------\n\
  2009-11-28	Kevin Stevenson, UCF  	\n\
			kevin218@knights.ucf.edu\n\
		Original version\n\
  2011-01-05    Nate Lust, UCF\n\
                natelust at linux dot com\n\
                Converted to c extention function\n\
";

static PyMethodDef log4qramp_methods[] = {
  {"log4qramp",(PyCFunction)log4qramp,METH_VARARGS|METH_KEYWORDS,log4qramp_doc},{NULL}};

void initlog4qramp(void)
{
  Py_InitModule("log4qramp",log4qramp_methods);
  import_array();
}
