#include <Python.h>

static PyObject* py_add(PyObject* self, PyObject* args)
{
    int a, b;

    (void)self;
    if (!PyArg_ParseTuple(args, "ii", &a, &b)) {
        return NULL;
    }
    return Py_BuildValue("i", a + b);
}

// Table des méthodes exposées à Python
static PyMethodDef PythonMethods[] = {
    {"add", py_add, METH_VARARGS, "Addition of two numbers"},
    {NULL, NULL, 0, NULL}  // Fin du tableau
};

// Définition du module
static struct PyModuleDef pythonmodule = {
    PyModuleDef_HEAD_INIT,
    "pythonlib",
    NULL,
    -1,
    PythonMethods,
    NULL,
    NULL, NULL, NULL
};

// Initialisation du module
PyMODINIT_FUNC PyInit_pythonlib(void) {
    return PyModule_Create(&pythonmodule);
}
