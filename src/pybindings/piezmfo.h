// copyright 2015 andrean franc
#ifndef ZIPPIE_PYBINDINGS_PIEZMFO_H_
#define ZIPPIE_PYBINDINGS_PIEZMFO_H_
#include <Python.h>
#include "structmember.h"

#include <ios>
#include <memory>

#include "zippie/utils.h"
#include "zippie/streams/crc32stream.h"


typedef struct {
    PyObject_HEAD
    std::unique_ptr<std::istream> source;
    std::streamsize size;
} PieZipMemberFileObject;


static PyObject* BadZipFile;


static void PieZipMemberFileObject_dealloc(PieZipMemberFileObject* self) {
    self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}


static PyObject* PieZipMemberFileObject_read(PieZipMemberFileObject* self,
                                             PyObject* args) {
    int n = -1;
    if (!PyArg_ParseTuple(args, "|i", &n))
        return NULL;

    zippie::utils::byte_vec buffer;
    std::streamsize bytes_read = 0;
    try {
        bytes_read = zippie::utils::read_into(self->source.get(),
                                              &buffer,
                                              n >= 0 ? n : self->size);
    } catch (zippie::streams::checksum_error& e) {
        PyErr_SetString(BadZipFile, e.what());
        return NULL;
    }
    if (bytes_read == 0)
        return Py_BuildValue("s", "");

    return Py_BuildValue("s#", &buffer[0], bytes_read);
}


static PyObject* PieZipMemberFileObject_seek(PieZipMemberFileObject* self,
                                             PyObject* args) {
    int offset = 0;
    int from_what = 0;
    if (!PyArg_ParseTuple(args, "i|i", &offset, &from_what))
        return NULL;

    // In Python, there are no error bits, so in case they are set, clear them,
    // otherwise it won't be possible to use the file stream object
    self->source->clear();
    // Python defaults to 0 which is seeking from beginning
    std::ios_base::seekdir direction = std::ios_base::beg;
    if (from_what == 1)
        direction = std::ios_base::cur;
    else if (from_what == 2)
        direction = std::ios_base::end;

    self->source->seekg(offset, direction);
    Py_RETURN_NONE;
}


static PyObject* PieZipMemberFileObject_tell(PieZipMemberFileObject* self) {
    std::streampos position = self->source->tellg();
    return Py_BuildValue("i", position);
}


static PyMethodDef PieZipMemberFileObject_methods[] = {
    {"read", (PyCFunction)PieZipMemberFileObject_read, METH_VARARGS,
     "Return all or the specified number of bytes from archive member."},
    {"seek", (PyCFunction)PieZipMemberFileObject_seek, METH_VARARGS,
     "Seek to the specified position in archive member."},
    {"tell", (PyCFunction)PieZipMemberFileObject_tell, METH_NOARGS,
     "Return the current position of file pointer in archive member."},
    {NULL}  /* Sentinel */
};


static PyTypeObject PieZipMemberFileObjectType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "zippie.PieZipMemberFileObject",    /*tp_name*/
    sizeof(PieZipMemberFileObject),     /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PieZipMemberFileObject_dealloc, /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "PieZipMemberFileObject objects",   /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PieZipMemberFileObject_methods,     /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
};
#endif  // ZIPPIE_PYBINDINGS_PIEZMFO_H_
