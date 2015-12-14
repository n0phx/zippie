// copyright 2015 andrean franc
#ifndef ZIPPIE_PYBINDINGS_PIEZMI_H_
#define ZIPPIE_PYBINDINGS_PIEZMI_H_
#include <Python.h>
#include "structmember.h"

#include "zippie/zmi.h"


typedef struct {
    PyObject_HEAD
    zippie::ZipMemberInfo* zip_member_info;
} PieZipMemberInfo;


static void PieZipMemberInfo_dealloc(PieZipMemberInfo* self) {
    self->zip_member_info = NULL;
    self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}


static PyMethodDef PieZipMemberInfo_methods[] = {
    {NULL}  /* Sentinel */
};


static PyTypeObject PieZipMemberInfoType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "zippie.PieZipMemberInfo",    /*tp_name*/
    sizeof(PieZipMemberInfo),     /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PieZipMemberInfo_dealloc, /*tp_dealloc*/
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
    "PieZipMemberInfo objects",   /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PieZipMemberInfo_methods,     /* tp_methods */
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
#endif  // ZIPPIE_PYBINDINGS_PIEZMI_H_
