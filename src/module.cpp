#include <e32std.h>
#include <newfileserviceclient.h>
#include <aiwservicehandler.h>
#include <aiwcommon.hrh>
#include <newfileservice.hrh>
#include <aiwdialdatatypes.h>

#include <Python.h>
#include <symbian_python_ext_util.h>
#include "local_epoc_py_utils.h"

//const TUid KUidCamcorder1 = {0x101ffa86}; // in N95

static PyObject* TakePhotoL(TUid aUid)
{
  PyObject* photoFile;

  // this code borrows heavily from an S60 example
  CDesCArray* selectedFiles = new (ELeave) CDesCArrayFlat(4);
  CleanupStack::PushL(selectedFiles);
  CNewFileServiceClient* nfsc = NewFileServiceFactory::NewClientL();
  CleanupStack::PushL(nfsc);            
  CAiwGenericParamList* params = CAiwGenericParamList::NewLC();

  // Note that when running in a "headless" application, this appears
  // to yield a KERN-EXEC 3. Presumably because someone is assuming
  // that some GUI application related handle is non-NULL.
  // 
  // If running this when a GUI application already has an active
  // "server application" running, we get a KErrInUse leave, so I
  // guess each application can only have at most one subordinate
  // application at any one time.
  TBool fileCreated;
  TInt error;
  Py_BEGIN_ALLOW_THREADS;
  TRAP(error, fileCreated = nfsc->NewFileL(aUid,
					   *selectedFiles,
					   params, 
					   ENewFileServiceImage,
					   ETrue));
  Py_END_ALLOW_THREADS;
  if (error)
    User::Leave(error);

  // Do not know if multiple files could be created by Camera, but
  // we are content with the first one.
  if (fileCreated && (selectedFiles->Count() >= 1)) {
    TPtrC fileName = selectedFiles->MdcaPoint(0);
    photoFile = Py_BuildValue("u#", fileName.Ptr(), fileName.Length());
  } else {
    photoFile = Py_None;
    Py_INCREF(Py_None);
  }

  CleanupStack::PopAndDestroy(3); // params, nfsc, selectedFiles
  return photoFile;
}

static PyObject *_fn__pynewfile__take_photo(PyObject *aPyMod, PyObject *aPyArgs)
{
  int intUid;
  if (!PyArg_ParseTuple(aPyArgs, "i", &intUid))
    {
      return NULL;
    }
  TUid uid;
  uid.iUid = intUid;

  PyObject* ret = NULL;
  TRAPD(error, ret = TakePhotoL(uid));
  if ((error != KErrNone)) {
    return SPyErr_SetFromSymbianOSErr(error);
  }
  return ret;
}

static PyMethodDef const _ft__pynewfile[] = {{"take_photo", reinterpret_cast<PyCFunction>(_fn__pynewfile__take_photo), METH_VARARGS, NULL}, {NULL}};

EXPORT_C void initpynewfile()
{
  PyObject *pyMod = Py_InitModule("pynewfile", const_cast<PyMethodDef *>((&_ft__pynewfile[0])));
  if ((pyMod == NULL)) {
    return;
  }
}

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
{
  return KErrNone;
}
#endif
