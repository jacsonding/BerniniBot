#include <Python.h>
#include <stdint.h>

static PyObject* airpen(PyObject* self, PyObject *args)
{

  char * p_data;
    int len;

    int16_t x;
    int16_t y;
    int16_t z;

    uint8_t btn_id;
    uint8_t state;

    uint8_t data_buff[6] = {0};
    uint8_t btn_state_buff;


  if (!PyArg_ParseTuple(args, "s#", &p_data, &len)) {
    return NULL;
  }

  if(len == 6)
  {
      memcpy(data_buff, p_data, 6);

      memcpy(&x, data_buff, 2);
      memcpy(&y, data_buff+2, 2);
      memcpy(&z, data_buff+4, 2);

      return Py_BuildValue("iii", x,y,z);
  }
  else if (len == 1)
  {
      memcpy(&btn_state_buff, p_data, 1);

      btn_id = btn_state_buff & 0x7F;
      state = (btn_state_buff >> 7);

      return Py_BuildValue("ii", btn_id, state);
  }
  else 
  {
      return Py_BuildValue("i", 0);
  }



    // return Py_BuildValue("s", "Hello, Python extensions!!");
}

static char airpen_docs[] =
    "helloworld( ): Any message you want to put here!!\n";

static PyMethodDef airpen_funcs[] = {
    {"airpen", (PyCFunction)airpen, 
     METH_VARARGS, airpen_docs},
    {NULL}
};

void initairpen(void)
{
    Py_InitModule3("airpen", airpen_funcs,
                   "Extension module example!");
}