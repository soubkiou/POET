#include<Python.h>
#include<numpy/arrayobject.h>
#include<math.h>
#include<omp.h>

#define IND(a,i) *((double *)(a->data+i*a->strides[0]))

static PyObject *rotation(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *rotation(PyObject *self, PyObject *args, PyObject *keywds)
{
  PyObject *etc;
  PyArrayObject *x,*y,*params,*airmass,*rot;
  double a,b,offset;
  int i;
  npy_intp dims[1];

  //  etc = PyList_New(0);

  static char *kwlist[] = {"params","x","etc",NULL};

  if(!PyArg_ParseTupleAndKeywords(args,keywds,"OO|O",kwlist,&params,&x,&etc))
    {
      return NULL;
    }

  a         = IND(params,0);
  b         = IND(params,1);
  offset    = IND(params,2);
  
  airmass   = (PyArrayObject *) PyList_GetItem(x,0); //IND(x,0);
  rot       = (PyArrayObject *) PyList_GetItem(x,1); //IND(x,1);

  dims[0] = rot->dimensions[0];

  y = (PyArrayObject *) PyArray_SimpleNew(1,dims,PyArray_DOUBLE);
  #pragma omp parallel for
  for(i=0;i<dims[0];i++)
    {
      IND(y,i) = 1 + a*IND(airmass,i) + b*cos(M_PI/180.*(IND(rot,i)+offset));
    }
  return PyArray_Return(y);
}

static char rotation_doc[] = "\
    This function creates a model that fits variations due to instrument rotation.\n\
\n\
    Parameters\n\
    ----------\n\
    a:	        Flux offset\n\
    b:	        Airmass multiplier\n\
    offset:	    Rotation offset in degrees\n\
    airmass:	Array of airmass points\n\
    rotation:	Array of airmass points\n\
\n\
    Returns\n\
    -------\n\
    This function returns an array of y values...\n\
\n\
    Revisions\n\
    ---------\n\
    2012-06-19	Kevin Stevenson, UChicago \n\
                kbs@uchicago.edu\n\
                Original version\n\
    2012-08-24  Kevin Stevenson\n\
                Converted to C\n\
\n\
";

static PyMethodDef rotation_methods[] = {
  {"rotation",(PyCFunction)rotation,METH_VARARGS|METH_KEYWORDS,rotation_doc},{NULL}};

void initrotation(void)
{
  Py_InitModule("rotation",rotation_methods);
  import_array();
}
