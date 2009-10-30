/* Arbitrary module name */

%module (directors="1", allprotected="1") libtisch


/* Includes that will be added to the generated
xxx_wrap.c/xxx_wrap.cpp wrapper file. They will not be interpreted by
SWIG */

%{
#if _MSC_VER
  #undef  TISCH_SHARED
  #define TISCH_SHARED __declspec( dllimport )
#endif
void* __dso_handle;
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
#include <features/Scale.h>
#include <Widget.h>
#include <Button.h>
#include <Tile.h>
#include <Container.h>
#include <MasterContainer.h>
%}


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

%include ../../gestured/features/Scale.h
%extend Scale {
    static Scale* dynamic_cast(FeatureBase *base) {
        return dynamic_cast<Scale*>(base);
    }
};

%include GL/glext.h
%include ../../libs/simplecv/Texture.h
%template(RGBATexture) Texture<DEFAULT_TEXTURE_TARGET,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE>;

/* enable directors (two-way cross-language polymorphism) for Widget */
%feature("director") Widget;
%feature("director") Container;

%include ../../widgets/Widget.h
%include ../../widgets/Button.h
%include ../../widgets/Tile.h
%include ../../widgets/Container.h
%include ../../widgets/MasterContainer.h
