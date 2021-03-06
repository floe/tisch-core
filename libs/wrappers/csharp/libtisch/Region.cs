/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class Region : vectorVector {
  private HandleRef swigCPtr;

  internal Region(IntPtr cPtr, bool cMemoryOwn) : base(libtischPINVOKE.RegionUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(Region obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~Region() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          libtischPINVOKE.delete_Region(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public Region(uint _flags) : this(libtischPINVOKE.new_Region__SWIG_0(_flags), true) {
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
  }

  public Region() : this(libtischPINVOKE.new_Region__SWIG_1(), true) {
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
  }

  public int contains(Vector v) {
    int ret = libtischPINVOKE.Region_contains(swigCPtr, Vector.getCPtr(v));
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public uint flags() {
    uint ret = libtischPINVOKE.Region_flags__SWIG_0(swigCPtr);
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void flags(uint _flags) {
    libtischPINVOKE.Region_flags__SWIG_1(swigCPtr, _flags);
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
  }

  public vectorGesture gestures {
    set {
      libtischPINVOKE.Region_gestures_set(swigCPtr, vectorGesture.getCPtr(value));
      if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      IntPtr cPtr = libtischPINVOKE.Region_gestures_get(swigCPtr);
      vectorGesture ret = (cPtr == IntPtr.Zero) ? null : new vectorGesture(cPtr, false);
      if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

}
