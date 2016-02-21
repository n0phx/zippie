// copyright 2015 andrean franc
#include <Python.h>
#include "structmember.h"

#include "zippie/logging.h"
#include "pybindings/piezmfo.h"
#include "pybindings/piezmi.h"
#include "pybindings/piezipfile.h"


static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};


#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC initzippie(void) {
    PyObject* mod;
    logging::set_level(logging::ERROR);

    if ((PyType_Ready(&PieZipMemberInfoType) < 0) ||
            (PyType_Ready(&PieZipMemberFileObjectType) < 0) ||
            (PyType_Ready(&PieZipFileType) < 0))
        return;

    mod = Py_InitModule3("zippie",
                         module_methods,
                         "ZipFile extension");

    if (mod == NULL)
        return;

    char exc_name[] = "zippie.BadZipFile";
    BadZipFile = PyErr_NewException(exc_name, NULL, NULL);
    Py_INCREF(BadZipFile);
    PyModule_AddObject(mod, "BadZipFile", BadZipFile);
    Py_INCREF(&PieZipMemberInfoType);
    PyModule_AddObject(mod,
                       "PieZipMemberInfo",
                       reinterpret_cast<PyObject*>(&PieZipMemberInfoType));
    Py_INCREF(&PieZipMemberFileObjectType);
    PyModule_AddObject(mod,
                       "PieZipMemberFileObject",
                       reinterpret_cast<PyObject*>(&PieZipMemberFileObjectType));
    Py_INCREF(&PieZipFileType);
    PyModule_AddObject(mod,
                       "PieZipFile",
                       reinterpret_cast<PyObject*>(&PieZipFileType));
}
