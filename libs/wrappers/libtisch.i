/* Arbitrary module name */

%module (directors="1", allprotected="1") libtisch

/* exception handling - doesn't seem to work? */
%include "exception.i"

%exception {
  try {
    $action
  } catch (std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

/* Includes that will be added to the generated
xxx_wrap.c/xxx_wrap.cpp wrapper file. They will not be interpreted by
SWIG */

%{
#if _MSC_VER
  #undef  TISCH_SHARED
  #define TISCH_SHARED __declspec( dllimport )
#endif
#include <Vector.h>
#include <BasicBlob.h>
#include <SmartPtr.h>
#include <Feature.h>
#include <Factory.h>
#include <Region.h>
#include <features/BlobCount.h>
#include <features/BlobDim.h>
#include <features/BlobID.h>
#include <features/BlobPos.h>
#include <features/BlobGroup.h>
#include <features/Motion.h>
#include <features/Rotation.h>
#include <features/MultiBlobRotation.h>
#include <features/Scale.h>
#include <features/MultiBlobScale.h>
#include <Window.h>
#include <Mouse.h>
#include <Widget.h>
#include <Button.h>
#include <Label.h>
#include <Tile.h>
#include <Slider.h>
#include <Dial.h>
#include <Container.h>
#include <MasterContainer.h>
#include <GLUTWindow.h>
#include <Textbox.h>
#include <Checkbox.h>
%}


%include "stl.i"
%include "std_except.i"
%include "typemaps.i"
%include "std_string.i"
%include "std_deque.i"
%include "std_vector.i"
%include "std_map.i"

%rename(write_to)  operator<<;
%rename(read_from) operator>>;
%rename(assign)    operator=;

/* C/C++ declarations to be wrapped.  The paths are relative to the
working directory of swig, which is the directory where the invoking
SConscript file resides.  Attention: The order of includes matters!
Declarations must come before their usage, otherwise, SWIG will
generate generic type wrapper classes of style SWITTYPE_p_<xxx>. */

%include ../../libs/tools/Vector.h
%template(Vector) _Vector<double>;
%template(IntVector) _Vector<int>;
%template(vectorVector) std::vector<Vector>;

namespace osc {
  typedef int int32;
}

%include ../../libs/tools/BasicBlob.h
%template(dequeBasicBlob) std::deque<BasicBlob>;

%include ../../libs/tools/SmartPtr.h


%template(mapIntBlobHistory) std::map<int,BlobHistory>;
%template(vectorBlobState) std::vector<BlobState>;
%include ../../gestured/InputState.h

/* very obscure snippet to make pointer containers play with Python */
#ifdef SWIGPYTHON
%traits_swigtype(FeatureBase);
%fragment(SWIG_Traits_frag(FeatureBase));
#endif

/* C#: correct method modifiers for simulated multiple inheritance */
#ifdef SWIGCSHARP
%csmethodmodifiers Window::update "public new";
#endif

/* enable directors (two-way cross-language polymorphism) for FeatureBase */
%feature("director") FeatureBase;

%include ../../gestured/Feature.h


%include ../../gestured/Factory.h
%template(mapStringFeatureBaseP) std::map<std::string,FeatureBase*>;
%template(smartPtrFeatureBase) SmartPtr<FeatureBase>;
%template(vectorSmartPtrFeatureBase) std::vector< SmartPtr< FeatureBase > >;

%include ../../gestured/Gesture.h
%template(vectorGesture) std::vector<Gesture>;

%include ../../gestured/Region.h

%include ../../gestured/Matcher.h


%template(FeatureInt) Feature<int>;
%extend Feature<int> {
  static Feature<int>* dynamic_cast(FeatureBase* base) {
    return dynamic_cast<Feature<int>*>(base);
  }
};

%template(vectorInt) std::vector<int>;
%include ../../gestured/features/BlobCount.h
%extend BlobCount {
  static BlobCount* dynamic_cast(FeatureBase* base) {
    return dynamic_cast<BlobCount*>(base);
  }
};

%template(FeatureDimensions) Feature<Dimensions>;
%extend Feature<Dimensions> {
  static Feature<Dimensions>* dynamic_cast(FeatureBase* base) {
    return dynamic_cast<Feature<Dimensions>*>(base);
  }
};

%template(vectorDimensions) std::vector<Dimensions>;
%include ../../gestured/features/BlobDim.h
%extend BlobDim {
    static BlobDim* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<BlobDim*>(base);
    }
};

%include ../../gestured/features/BlobID.h
%extend BlobID {
    static BlobID* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<BlobID*>(base);
    }
};

%template(FeatureVector) Feature<Vector>;
%extend Feature<Vector> {
  static Feature<Vector>* dynamic_cast(FeatureBase* base) {
    return dynamic_cast<Feature<Vector>*>(base);
  }
};

%include ../../gestured/features/BlobPos.h
%extend BlobPos {
    static BlobPos* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<BlobPos*>(base);
    }
};

%include ../../gestured/features/BlobGroup.h
%extend BlobGroup {
    static BlobGroup* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<BlobGroup*>(base);
    }
};

%template(FeatureDouble) Feature<double>;
%extend Feature<double> {
  static Feature<double>* dynamic_cast(FeatureBase* base) {
    return dynamic_cast<Feature<double>*>(base);
  }
};

%template(vectorDouble) std::vector<double>;
%include ../../gestured/features/Motion.h
%extend Motion {
    static Motion* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<Motion*>(base);
    }
};

%include ../../gestured/features/Rotation.h
%extend Rotation {
    static Rotation* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<Rotation*>(base);
    }
};

%include ../../gestured/features/MultiBlobRotation.h
%extend MultiBlobRotation {
    static MultiBlobRotation* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<MultiBlobRotation*>(base);
    }
};

%include ../../gestured/features/Scale.h
%extend Scale {
    static Scale* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<Scale*>(base);
    }
};

%include ../../gestured/features/MultiBlobScale.h
%extend MultiBlobScale {
    static MultiBlobScale* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<MultiBlobScale*>(base);
    }
};

%include GL/glext.h
%include ../../libs/simplegl/Texture.h
%include ../../libs/simplegl/GLUTWindow.h
%template(RGBATexture) Texture<DEFAULT_TEXTURE_TARGET,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE>;


/* typemaps for keeping additional references to container contents      */
/* FIXME: this is an ugly hack which introduces a memory leak, as the    */
/* references are kept for all eternity. see also SWIG documentation at: */
/* - http://www.swig.org/Doc1.3/Java.html#java_memory_management_objects */
/* - http://www.swig.org/Doc1.3/SWIGDocumentation.html#imclass_pragmas   */

%typemap(javain) RGBATexture* "libtischJNI.getCPtrAddRef($javainput)"
%typemap(javain) Widget*      "libtischJNI.getCPtrAddRef($javainput)"

%pragma(java) jniclasscode = %{
  static java.util.HashSet<Object> refstore;
  static {
    System.loadLibrary("tisch_java");
    System.out.println("libTISCH Java wrapper loaded.");
    refstore = new java.util.HashSet<Object>();
  }
  static long getCPtrAddRef( RGBATexture tex ) { refstore.add(tex); return RGBATexture.getCPtr(tex); }
  static long getCPtrAddRef( Widget      wdg ) { refstore.add(wdg); return      Widget.getCPtr(wdg); }
%}

%typemap(csin) RGBATexture* "libtischPINVOKE.getCPtrAddRef($csinput)"
%typemap(csin) Widget*      "libtischPINVOKE.getCPtrAddRef($csinput)"

%pragma(csharp) imclasscode = %{
  public static System.Collections.Generic.HashSet<Object> refstore = new System.Collections.Generic.HashSet<Object>();
  public static HandleRef getCPtrAddRef( RGBATexture tex ) { refstore.Add(tex); return RGBATexture.getCPtr(tex); }
  public static HandleRef getCPtrAddRef( Widget      wdg ) { refstore.Add(wdg); return      Widget.getCPtr(wdg); }
%}


/* enable directors (two-way cross-language polymorphism) */
%feature("director") Window;
%feature("director") Widget;
%feature("director") Tile;
%feature("director") Container;
%feature("director") Label;
%feature("director") MasterContainer;
%feature("director") Slider;
%feature("director") Dial;
%feature("director") Button;

%include ../../widgets/Widget.h
%include ../../widgets/Label.h
%include ../../widgets/Button.h
%include ../../widgets/Tile.h
%include ../../widgets/Container.h
%include ../../widgets/Checkbox.h
%include ../../widgets/Textbox.h
%include ../../widgets/Slider.h
%include ../../widgets/Dial.h
%include ../../widgets/MasterContainer.h
%include ../../widgets/Window.h
%include ../../widgets/Mouse.h

