/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package libtisch;

public class BlobID extends FeatureInt {
  private long swigCPtr;

  protected BlobID(long cPtr, boolean cMemoryOwn) {
    super(libtischJNI.SWIGBlobIDUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(BlobID obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libtischJNI.delete_BlobID(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public BlobID(int tf) {
    this(libtischJNI.new_BlobID__SWIG_0(tf), true);
  }

  public BlobID() {
    this(libtischJNI.new_BlobID__SWIG_1(), true);
  }

  public void load(InputState state) {
    libtischJNI.BlobID_load(swigCPtr, this, InputState.getCPtr(state), state);
  }

  public int next() {
    return libtischJNI.BlobID_next(swigCPtr, this);
  }

  public String name() {
    return libtischJNI.BlobID_name(swigCPtr, this);
  }

  public static BlobID dynamic_cast(FeatureBase base) {
    long cPtr = libtischJNI.BlobID_dynamic_cast(FeatureBase.getCPtr(base), base);
    return (cPtr == 0) ? null : new BlobID(cPtr, false);
  }

}
