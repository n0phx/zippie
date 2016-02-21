// copyright 2015 andrean franc
#ifndef ZIPPIE_PYBINDINGS_PIEZIPFILE_H_
#define ZIPPIE_PYBINDINGS_PIEZIPFILE_H_
#include <Python.h>
#include "structmember.h"

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "pybindings/piezmfo.h"
#include "pybindings/piezmi.h"
#include "zippie/zippie.h"


typedef std::map<std::string, PyObject*> pyobj_map;
typedef struct {
    PyObject_HEAD
    zippie::ZipFile* zip_file;
    pyobj_map zip_member_infos;
} PieZipFile;


static void PieZipFile_dealloc(PieZipFile* self) {
    for (pyobj_map::iterator it = self->zip_member_infos.begin();
            it != self->zip_member_infos.end();
            ++it) {
        Py_DECREF(it->second);
    }
    if (self->zip_file)
        delete self->zip_file;
    self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}


static PyObject* PieZipFile_new(PyTypeObject* type, PyObject* args,
                               PyObject* kwargs) {
    PieZipFile* self = reinterpret_cast<PieZipFile*>(type->tp_alloc(type, 0));
    self->zip_member_infos = pyobj_map();
    return reinterpret_cast<PyObject*>(self);
}


static int PieZipFile_init(PieZipFile* self, PyObject* args, PyObject* kwargs) {
    const char* filename;

    if (!PyArg_ParseTuple(args, "s", &filename))
        return -1;

    try {
        self->zip_file = new zippie::ZipFile(filename);
    } catch (zippie::utils::bad_zip_file& e) {
        PyErr_SetString(BadZipFile, e.what());
        return -1;
    }
    return 0;
}


static zippie::ZipMemberInfo* get_zmi(PieZipFile* self,
                                      const std::string& filename) {
    try {
        return &(self->zip_file->getinfo(filename));
    } catch (std::out_of_range& e) {
        PyErr_SetString(PyExc_KeyError, "Archive member not found.");
        return NULL;
    } catch (std::runtime_error& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return NULL;
    }
}


static PyObject* create_piezmi(PieZipFile* self, const std::string& filename) {
    PieZipMemberInfo* piezmi = PyObject_New(PieZipMemberInfo,
                                            &PieZipMemberInfoType);
    if (!piezmi)
        return NULL;

    zippie::ZipMemberInfo* zmi = get_zmi(self, filename);
    if (!zmi) {
        Py_DECREF(piezmi);
        return NULL;
    }
    piezmi->zip_member_info = zmi;
    // save ``PieZipMemberInfo`` in a map so subsequent accesses will get to use
    // the same object
    PyObject* obj = reinterpret_cast<PyObject*>(piezmi);
    Py_INCREF(obj);
    self->zip_member_infos.insert(std::make_pair(filename, obj));
    return obj;
}


static PyObject* get_piezmi(PieZipFile* self, const std::string& filename) {
    PyObject* piezmi;
    try {
        piezmi = self->zip_member_infos.at(filename);
    } catch (std::out_of_range& e) {
        piezmi = create_piezmi(self, filename);
    }
    return piezmi;
}


static PyObject* PieZipFile_getinfo(PieZipFile* self, PyObject* args) {
    const char* filename;

    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    return get_piezmi(self, std::string(filename));
}


static PyObject* PieZipFile_namelist(PieZipFile* self) {
    std::vector<std::string> namelist;
    try {
        namelist = self->zip_file->namelist();
    } catch (std::runtime_error& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return NULL;
    }

    PyObject* pylist = PyList_New(namelist.size());
    if (!pylist)
        return NULL;

    for (auto it = namelist.begin(); it != namelist.end(); ++it) {
        PyObject* item = PyString_FromString(it->c_str());
        PyList_SET_ITEM(pylist, it - namelist.begin(), item);
    }
    return pylist;
}


static PyObject* PieZipFile_open(PieZipFile* self, PyObject* args) {
    const char* filename;

    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    PieZipMemberFileObject* piezmfo = PyObject_New(PieZipMemberFileObject,
                                                   &PieZipMemberFileObjectType);
    if (!piezmfo)
        return NULL;

    std::string fname(filename);
    zippie::ZipMemberInfo* zmi = get_zmi(self, fname);
    if (!zmi) {
        Py_DECREF(piezmfo);
        return NULL;
    }
    try {
        piezmfo->source = self->zip_file->open(fname);
    } catch (std::runtime_error& e) {
        Py_DECREF(piezmfo);
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return NULL;
    }
    piezmfo->size = zmi->file_data_size();
    return reinterpret_cast<PyObject*>(piezmfo);
}


static PyObject* extract(PieZipFile* self,
                         const std::string& filename,
                         const std::string& dest) {
    try {
        self->zip_file->extract(filename, dest);
    } catch (std::runtime_error& e) {
        std::string msg("File cannot be extracted to the chosen path. "
                        "Reason: " + std::string(e.what()));
        PyErr_SetString(PyExc_RuntimeError, msg.c_str());
        return NULL;
    } catch (zippie::utils::bad_zip_file& e) {
        PyErr_SetString(BadZipFile, e.what());
        return NULL;
    }
    Py_RETURN_NONE;
}


static PyObject* PieZipFile_extract(PieZipFile* self, PyObject* args) {
    const char* filename;
    const char* dest;

    if (!PyArg_ParseTuple(args, "ss", &filename, &dest))
        return NULL;

    return extract(self, std::string(filename), std::string(dest));
}


static PyObject* PieZipFile_extractall(PieZipFile* self, PyObject* args) {
    const char* dest;

    if (!PyArg_ParseTuple(args, "s", &dest))
        return NULL;

    std::string path(dest);
    std::vector<std::string> namelist;
    try {
        namelist = self->zip_file->namelist();
    } catch (std::runtime_error& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return NULL;
    }

    for (auto it = namelist.begin(); it != namelist.end(); ++it) {
        if (!extract(self, *it, path))
            return NULL;
    }
    Py_RETURN_NONE;
}


static PyObject* PieZipFile_context_enter(PieZipFile* self, PyObject* args) {
    return reinterpret_cast<PyObject*>(self);
}


static PyObject* PieZipFile_context_exit(PieZipFile* self, PyObject* args) {
    self->zip_file->close();
    Py_RETURN_NONE;
}


static PyMethodDef PieZipFile_methods[] = {
    {"getinfo", (PyCFunction)PieZipFile_getinfo, METH_VARARGS,
     "Return a PieZipMemberInfo object for the requested archive member."},
    {"open", (PyCFunction)PieZipFile_open, METH_VARARGS,
     "Return a PieZipMemberFileObject for the requested archive member."},
    {"extract", (PyCFunction)PieZipFile_extract, METH_VARARGS,
     "Extract archive member to the specified path."},
    {"extractall", (PyCFunction)PieZipFile_extractall, METH_VARARGS,
     "Extract all archive members to the specified path."},
    {"namelist", (PyCFunction)PieZipFile_namelist, METH_NOARGS,
     "Return list of all archive member names."},
    {"__enter__", (PyCFunction)PieZipFile_context_enter, METH_VARARGS,
     "Enables 'with' statement"},
    {"__exit__", (PyCFunction)PieZipFile_context_exit, METH_VARARGS,
     "Enables 'with' statement"},
    {NULL}  /* Sentinel */
};


static PyTypeObject PieZipFileType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "zippie.PieZipFile",        /*tp_name*/
    sizeof(PieZipFile),         /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PieZipFile_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PieZipFile objects",       /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PieZipFile_methods,         /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PieZipFile_init,  /* tp_init */
    0,                         /* tp_alloc */
    PieZipFile_new,             /* tp_new */
};
#endif  // ZIPPIE_PYBINDINGS_PIEZIPFILE_H_
