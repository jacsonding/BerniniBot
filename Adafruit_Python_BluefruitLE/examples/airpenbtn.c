#include <Python.h>
#include <stdint.h>

static PyObject* airpenbtn(PyObject* self, PyObject *args)
{

  char * p_data;
    int len;

    int16_t x;
    int16_t y;
    int16_t z;

    uint8_t data_buff[6] = {0};


  if (!PyArg_ParseTuple(args, "s#", &p_data, &len)) {
    return NULL;
  }

  memcpy(data_buff, p_data, 6);

  memcpy(&x, data_buff, 2);
  memcpy(&y, data_buff+2, 2);
  memcpy(&z, data_buff+4, 2);

  return Py_BuildValue("iii", x,y,z);

}

static char airpenbtn_docs[] =
    "helloworld( ): Any message you want to put here!!\n";

static PyMethodDef airpenbtn_funcs[] = {
    {"airpenbtn", (PyCFunction)airpenbtn, 
     METH_VARARGS, airpenbtn_docs},
    {NULL}
};

void initairpenbtn(void)
{
    Py_InitModule3("airpenbtn", airpenbtn_funcs,
                   "Extension module example!");
}