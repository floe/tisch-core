/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class MultiBlobRotation : Rotation {
  private HandleRef swigCPtr;

  internal MultiBlobRotation(IntPtr cPtr, bool cMemoryOwn) : base(libtischPINVOKE.MultiBlobRotationUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(MultiBlobRotation obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~MultiBlobRotation() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          libtischPINVOKE.delete_MultiBlobRotation(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public MultiBlobRotation(uint tf) : this(libtischPINVOKE.new_MultiBlobRotation__SWIG_0(tf), true) {
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
  }

  public MultiBlobRotation() : this(libtischPINVOKE.new_MultiBlobRotation__SWIG_1(), true) {
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
  }

  public override FeatureBase clone() {
    IntPtr cPtr = libtischPINVOKE.MultiBlobRotation_clone(swigCPtr);
    MultiBlobRotation ret = (cPtr == IntPtr.Zero) ? null : new MultiBlobRotation(cPtr, false);
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override void load(InputState state) {
    libtischPINVOKE.MultiBlobRotation_load(swigCPtr, InputState.getCPtr(state));
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
  }

  public override string name() {
    string ret = libtischPINVOKE.MultiBlobRotation_name(swigCPtr);
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public new static MultiBlobRotation dynamic_cast(FeatureBase arg0) {
    IntPtr cPtr = libtischPINVOKE.MultiBlobRotation_dynamic_cast(FeatureBase.getCPtr(arg0));
    MultiBlobRotation ret = (cPtr == IntPtr.Zero) ? null : new MultiBlobRotation(cPtr, false);
    if (libtischPINVOKE.SWIGPendingException.Pending) throw libtischPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}
