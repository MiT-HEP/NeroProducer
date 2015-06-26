// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "../interface/BareAll.hpp"
#include "../interface/BareEvent.hpp"
#include "../interface/BareFatJets.hpp"
#include "../interface/BareJets.hpp"
#include "../interface/BareLeptons.hpp"
#include "../interface/BareMet.hpp"
#include "../interface/BareMonteCarlo.hpp"
#include "../interface/BarePhotons.hpp"
#include "../interface/BareTaus.hpp"
#include "../interface/BareTrigger.hpp"
#include "../interface/BareVertex.hpp"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *BareCollection_Dictionary();
   static void BareCollection_TClassManip(TClass*);
   static void delete_BareCollection(void *p);
   static void deleteArray_BareCollection(void *p);
   static void destruct_BareCollection(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareCollection*)
   {
      ::BareCollection *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareCollection));
      static ::ROOT::TGenericClassInfo 
         instance("BareCollection", "NeroProducer/Core/interface/BareCollection.hpp", 16,
                  typeid(::BareCollection), DefineBehavior(ptr, ptr),
                  &BareCollection_Dictionary, isa_proxy, 4,
                  sizeof(::BareCollection) );
      instance.SetDelete(&delete_BareCollection);
      instance.SetDeleteArray(&deleteArray_BareCollection);
      instance.SetDestructor(&destruct_BareCollection);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareCollection*)
   {
      return GenerateInitInstanceLocal((::BareCollection*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareCollection*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareCollection_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareCollection*)0x0)->GetClass();
      BareCollection_TClassManip(theClass);
   return theClass;
   }

   static void BareCollection_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareEvent_Dictionary();
   static void BareEvent_TClassManip(TClass*);
   static void *new_BareEvent(void *p = 0);
   static void *newArray_BareEvent(Long_t size, void *p);
   static void delete_BareEvent(void *p);
   static void deleteArray_BareEvent(void *p);
   static void destruct_BareEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareEvent*)
   {
      ::BareEvent *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareEvent));
      static ::ROOT::TGenericClassInfo 
         instance("BareEvent", "../interface/BareEvent.hpp", 6,
                  typeid(::BareEvent), DefineBehavior(ptr, ptr),
                  &BareEvent_Dictionary, isa_proxy, 4,
                  sizeof(::BareEvent) );
      instance.SetNew(&new_BareEvent);
      instance.SetNewArray(&newArray_BareEvent);
      instance.SetDelete(&delete_BareEvent);
      instance.SetDeleteArray(&deleteArray_BareEvent);
      instance.SetDestructor(&destruct_BareEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareEvent*)
   {
      return GenerateInitInstanceLocal((::BareEvent*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareEvent*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareEvent_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareEvent*)0x0)->GetClass();
      BareEvent_TClassManip(theClass);
   return theClass;
   }

   static void BareEvent_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareFatJets_Dictionary();
   static void BareFatJets_TClassManip(TClass*);
   static void *new_BareFatJets(void *p = 0);
   static void *newArray_BareFatJets(Long_t size, void *p);
   static void delete_BareFatJets(void *p);
   static void deleteArray_BareFatJets(void *p);
   static void destruct_BareFatJets(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareFatJets*)
   {
      ::BareFatJets *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareFatJets));
      static ::ROOT::TGenericClassInfo 
         instance("BareFatJets", "../interface/BareFatJets.hpp", 7,
                  typeid(::BareFatJets), DefineBehavior(ptr, ptr),
                  &BareFatJets_Dictionary, isa_proxy, 4,
                  sizeof(::BareFatJets) );
      instance.SetNew(&new_BareFatJets);
      instance.SetNewArray(&newArray_BareFatJets);
      instance.SetDelete(&delete_BareFatJets);
      instance.SetDeleteArray(&deleteArray_BareFatJets);
      instance.SetDestructor(&destruct_BareFatJets);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareFatJets*)
   {
      return GenerateInitInstanceLocal((::BareFatJets*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareFatJets*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareFatJets_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareFatJets*)0x0)->GetClass();
      BareFatJets_TClassManip(theClass);
   return theClass;
   }

   static void BareFatJets_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareJets_Dictionary();
   static void BareJets_TClassManip(TClass*);
   static void *new_BareJets(void *p = 0);
   static void *newArray_BareJets(Long_t size, void *p);
   static void delete_BareJets(void *p);
   static void deleteArray_BareJets(void *p);
   static void destruct_BareJets(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareJets*)
   {
      ::BareJets *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareJets));
      static ::ROOT::TGenericClassInfo 
         instance("BareJets", "../interface/BareJets.hpp", 6,
                  typeid(::BareJets), DefineBehavior(ptr, ptr),
                  &BareJets_Dictionary, isa_proxy, 4,
                  sizeof(::BareJets) );
      instance.SetNew(&new_BareJets);
      instance.SetNewArray(&newArray_BareJets);
      instance.SetDelete(&delete_BareJets);
      instance.SetDeleteArray(&deleteArray_BareJets);
      instance.SetDestructor(&destruct_BareJets);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareJets*)
   {
      return GenerateInitInstanceLocal((::BareJets*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareJets*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareJets_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareJets*)0x0)->GetClass();
      BareJets_TClassManip(theClass);
   return theClass;
   }

   static void BareJets_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareLeptons_Dictionary();
   static void BareLeptons_TClassManip(TClass*);
   static void *new_BareLeptons(void *p = 0);
   static void *newArray_BareLeptons(Long_t size, void *p);
   static void delete_BareLeptons(void *p);
   static void deleteArray_BareLeptons(void *p);
   static void destruct_BareLeptons(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareLeptons*)
   {
      ::BareLeptons *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareLeptons));
      static ::ROOT::TGenericClassInfo 
         instance("BareLeptons", "../interface/BareLeptons.hpp", 6,
                  typeid(::BareLeptons), DefineBehavior(ptr, ptr),
                  &BareLeptons_Dictionary, isa_proxy, 4,
                  sizeof(::BareLeptons) );
      instance.SetNew(&new_BareLeptons);
      instance.SetNewArray(&newArray_BareLeptons);
      instance.SetDelete(&delete_BareLeptons);
      instance.SetDeleteArray(&deleteArray_BareLeptons);
      instance.SetDestructor(&destruct_BareLeptons);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareLeptons*)
   {
      return GenerateInitInstanceLocal((::BareLeptons*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareLeptons*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareLeptons_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareLeptons*)0x0)->GetClass();
      BareLeptons_TClassManip(theClass);
   return theClass;
   }

   static void BareLeptons_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareMet_Dictionary();
   static void BareMet_TClassManip(TClass*);
   static void *new_BareMet(void *p = 0);
   static void *newArray_BareMet(Long_t size, void *p);
   static void delete_BareMet(void *p);
   static void deleteArray_BareMet(void *p);
   static void destruct_BareMet(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareMet*)
   {
      ::BareMet *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareMet));
      static ::ROOT::TGenericClassInfo 
         instance("BareMet", "../interface/BareMet.hpp", 7,
                  typeid(::BareMet), DefineBehavior(ptr, ptr),
                  &BareMet_Dictionary, isa_proxy, 4,
                  sizeof(::BareMet) );
      instance.SetNew(&new_BareMet);
      instance.SetNewArray(&newArray_BareMet);
      instance.SetDelete(&delete_BareMet);
      instance.SetDeleteArray(&deleteArray_BareMet);
      instance.SetDestructor(&destruct_BareMet);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareMet*)
   {
      return GenerateInitInstanceLocal((::BareMet*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareMet*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareMet_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareMet*)0x0)->GetClass();
      BareMet_TClassManip(theClass);
   return theClass;
   }

   static void BareMet_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareMonteCarlo_Dictionary();
   static void BareMonteCarlo_TClassManip(TClass*);
   static void *new_BareMonteCarlo(void *p = 0);
   static void *newArray_BareMonteCarlo(Long_t size, void *p);
   static void delete_BareMonteCarlo(void *p);
   static void deleteArray_BareMonteCarlo(void *p);
   static void destruct_BareMonteCarlo(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareMonteCarlo*)
   {
      ::BareMonteCarlo *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareMonteCarlo));
      static ::ROOT::TGenericClassInfo 
         instance("BareMonteCarlo", "../interface/BareMonteCarlo.hpp", 7,
                  typeid(::BareMonteCarlo), DefineBehavior(ptr, ptr),
                  &BareMonteCarlo_Dictionary, isa_proxy, 4,
                  sizeof(::BareMonteCarlo) );
      instance.SetNew(&new_BareMonteCarlo);
      instance.SetNewArray(&newArray_BareMonteCarlo);
      instance.SetDelete(&delete_BareMonteCarlo);
      instance.SetDeleteArray(&deleteArray_BareMonteCarlo);
      instance.SetDestructor(&destruct_BareMonteCarlo);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareMonteCarlo*)
   {
      return GenerateInitInstanceLocal((::BareMonteCarlo*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareMonteCarlo*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareMonteCarlo_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareMonteCarlo*)0x0)->GetClass();
      BareMonteCarlo_TClassManip(theClass);
   return theClass;
   }

   static void BareMonteCarlo_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BarePhotons_Dictionary();
   static void BarePhotons_TClassManip(TClass*);
   static void *new_BarePhotons(void *p = 0);
   static void *newArray_BarePhotons(Long_t size, void *p);
   static void delete_BarePhotons(void *p);
   static void deleteArray_BarePhotons(void *p);
   static void destruct_BarePhotons(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BarePhotons*)
   {
      ::BarePhotons *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BarePhotons));
      static ::ROOT::TGenericClassInfo 
         instance("BarePhotons", "../interface/BarePhotons.hpp", 7,
                  typeid(::BarePhotons), DefineBehavior(ptr, ptr),
                  &BarePhotons_Dictionary, isa_proxy, 4,
                  sizeof(::BarePhotons) );
      instance.SetNew(&new_BarePhotons);
      instance.SetNewArray(&newArray_BarePhotons);
      instance.SetDelete(&delete_BarePhotons);
      instance.SetDeleteArray(&deleteArray_BarePhotons);
      instance.SetDestructor(&destruct_BarePhotons);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BarePhotons*)
   {
      return GenerateInitInstanceLocal((::BarePhotons*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BarePhotons*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BarePhotons_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BarePhotons*)0x0)->GetClass();
      BarePhotons_TClassManip(theClass);
   return theClass;
   }

   static void BarePhotons_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareTaus_Dictionary();
   static void BareTaus_TClassManip(TClass*);
   static void *new_BareTaus(void *p = 0);
   static void *newArray_BareTaus(Long_t size, void *p);
   static void delete_BareTaus(void *p);
   static void deleteArray_BareTaus(void *p);
   static void destruct_BareTaus(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareTaus*)
   {
      ::BareTaus *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareTaus));
      static ::ROOT::TGenericClassInfo 
         instance("BareTaus", "../interface/BareTaus.hpp", 6,
                  typeid(::BareTaus), DefineBehavior(ptr, ptr),
                  &BareTaus_Dictionary, isa_proxy, 4,
                  sizeof(::BareTaus) );
      instance.SetNew(&new_BareTaus);
      instance.SetNewArray(&newArray_BareTaus);
      instance.SetDelete(&delete_BareTaus);
      instance.SetDeleteArray(&deleteArray_BareTaus);
      instance.SetDestructor(&destruct_BareTaus);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareTaus*)
   {
      return GenerateInitInstanceLocal((::BareTaus*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareTaus*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareTaus_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareTaus*)0x0)->GetClass();
      BareTaus_TClassManip(theClass);
   return theClass;
   }

   static void BareTaus_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareVertex_Dictionary();
   static void BareVertex_TClassManip(TClass*);
   static void *new_BareVertex(void *p = 0);
   static void *newArray_BareVertex(Long_t size, void *p);
   static void delete_BareVertex(void *p);
   static void deleteArray_BareVertex(void *p);
   static void destruct_BareVertex(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareVertex*)
   {
      ::BareVertex *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareVertex));
      static ::ROOT::TGenericClassInfo 
         instance("BareVertex", "../interface/BareVertex.hpp", 7,
                  typeid(::BareVertex), DefineBehavior(ptr, ptr),
                  &BareVertex_Dictionary, isa_proxy, 4,
                  sizeof(::BareVertex) );
      instance.SetNew(&new_BareVertex);
      instance.SetNewArray(&newArray_BareVertex);
      instance.SetDelete(&delete_BareVertex);
      instance.SetDeleteArray(&deleteArray_BareVertex);
      instance.SetDestructor(&destruct_BareVertex);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareVertex*)
   {
      return GenerateInitInstanceLocal((::BareVertex*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareVertex*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareVertex_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareVertex*)0x0)->GetClass();
      BareVertex_TClassManip(theClass);
   return theClass;
   }

   static void BareVertex_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *BareAll_Dictionary();
   static void BareAll_TClassManip(TClass*);
   static void *new_BareAll(void *p = 0);
   static void *newArray_BareAll(Long_t size, void *p);
   static void delete_BareAll(void *p);
   static void deleteArray_BareAll(void *p);
   static void destruct_BareAll(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::BareAll*)
   {
      ::BareAll *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::BareAll));
      static ::ROOT::TGenericClassInfo 
         instance("BareAll", "../interface/BareAll.hpp", 6,
                  typeid(::BareAll), DefineBehavior(ptr, ptr),
                  &BareAll_Dictionary, isa_proxy, 4,
                  sizeof(::BareAll) );
      instance.SetNew(&new_BareAll);
      instance.SetNewArray(&newArray_BareAll);
      instance.SetDelete(&delete_BareAll);
      instance.SetDeleteArray(&deleteArray_BareAll);
      instance.SetDestructor(&destruct_BareAll);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::BareAll*)
   {
      return GenerateInitInstanceLocal((::BareAll*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::BareAll*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *BareAll_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::BareAll*)0x0)->GetClass();
      BareAll_TClassManip(theClass);
   return theClass;
   }

   static void BareAll_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_BareCollection(void *p) {
      delete ((::BareCollection*)p);
   }
   static void deleteArray_BareCollection(void *p) {
      delete [] ((::BareCollection*)p);
   }
   static void destruct_BareCollection(void *p) {
      typedef ::BareCollection current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareCollection

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareEvent(void *p) {
      return  p ? new(p) ::BareEvent : new ::BareEvent;
   }
   static void *newArray_BareEvent(Long_t nElements, void *p) {
      return p ? new(p) ::BareEvent[nElements] : new ::BareEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareEvent(void *p) {
      delete ((::BareEvent*)p);
   }
   static void deleteArray_BareEvent(void *p) {
      delete [] ((::BareEvent*)p);
   }
   static void destruct_BareEvent(void *p) {
      typedef ::BareEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareEvent

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareFatJets(void *p) {
      return  p ? new(p) ::BareFatJets : new ::BareFatJets;
   }
   static void *newArray_BareFatJets(Long_t nElements, void *p) {
      return p ? new(p) ::BareFatJets[nElements] : new ::BareFatJets[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareFatJets(void *p) {
      delete ((::BareFatJets*)p);
   }
   static void deleteArray_BareFatJets(void *p) {
      delete [] ((::BareFatJets*)p);
   }
   static void destruct_BareFatJets(void *p) {
      typedef ::BareFatJets current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareFatJets

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareJets(void *p) {
      return  p ? new(p) ::BareJets : new ::BareJets;
   }
   static void *newArray_BareJets(Long_t nElements, void *p) {
      return p ? new(p) ::BareJets[nElements] : new ::BareJets[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareJets(void *p) {
      delete ((::BareJets*)p);
   }
   static void deleteArray_BareJets(void *p) {
      delete [] ((::BareJets*)p);
   }
   static void destruct_BareJets(void *p) {
      typedef ::BareJets current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareJets

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareLeptons(void *p) {
      return  p ? new(p) ::BareLeptons : new ::BareLeptons;
   }
   static void *newArray_BareLeptons(Long_t nElements, void *p) {
      return p ? new(p) ::BareLeptons[nElements] : new ::BareLeptons[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareLeptons(void *p) {
      delete ((::BareLeptons*)p);
   }
   static void deleteArray_BareLeptons(void *p) {
      delete [] ((::BareLeptons*)p);
   }
   static void destruct_BareLeptons(void *p) {
      typedef ::BareLeptons current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareLeptons

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareMet(void *p) {
      return  p ? new(p) ::BareMet : new ::BareMet;
   }
   static void *newArray_BareMet(Long_t nElements, void *p) {
      return p ? new(p) ::BareMet[nElements] : new ::BareMet[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareMet(void *p) {
      delete ((::BareMet*)p);
   }
   static void deleteArray_BareMet(void *p) {
      delete [] ((::BareMet*)p);
   }
   static void destruct_BareMet(void *p) {
      typedef ::BareMet current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareMet

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareMonteCarlo(void *p) {
      return  p ? new(p) ::BareMonteCarlo : new ::BareMonteCarlo;
   }
   static void *newArray_BareMonteCarlo(Long_t nElements, void *p) {
      return p ? new(p) ::BareMonteCarlo[nElements] : new ::BareMonteCarlo[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareMonteCarlo(void *p) {
      delete ((::BareMonteCarlo*)p);
   }
   static void deleteArray_BareMonteCarlo(void *p) {
      delete [] ((::BareMonteCarlo*)p);
   }
   static void destruct_BareMonteCarlo(void *p) {
      typedef ::BareMonteCarlo current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareMonteCarlo

namespace ROOT {
   // Wrappers around operator new
   static void *new_BarePhotons(void *p) {
      return  p ? new(p) ::BarePhotons : new ::BarePhotons;
   }
   static void *newArray_BarePhotons(Long_t nElements, void *p) {
      return p ? new(p) ::BarePhotons[nElements] : new ::BarePhotons[nElements];
   }
   // Wrapper around operator delete
   static void delete_BarePhotons(void *p) {
      delete ((::BarePhotons*)p);
   }
   static void deleteArray_BarePhotons(void *p) {
      delete [] ((::BarePhotons*)p);
   }
   static void destruct_BarePhotons(void *p) {
      typedef ::BarePhotons current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BarePhotons

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareTaus(void *p) {
      return  p ? new(p) ::BareTaus : new ::BareTaus;
   }
   static void *newArray_BareTaus(Long_t nElements, void *p) {
      return p ? new(p) ::BareTaus[nElements] : new ::BareTaus[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareTaus(void *p) {
      delete ((::BareTaus*)p);
   }
   static void deleteArray_BareTaus(void *p) {
      delete [] ((::BareTaus*)p);
   }
   static void destruct_BareTaus(void *p) {
      typedef ::BareTaus current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareTaus

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareVertex(void *p) {
      return  p ? new(p) ::BareVertex : new ::BareVertex;
   }
   static void *newArray_BareVertex(Long_t nElements, void *p) {
      return p ? new(p) ::BareVertex[nElements] : new ::BareVertex[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareVertex(void *p) {
      delete ((::BareVertex*)p);
   }
   static void deleteArray_BareVertex(void *p) {
      delete [] ((::BareVertex*)p);
   }
   static void destruct_BareVertex(void *p) {
      typedef ::BareVertex current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareVertex

namespace ROOT {
   // Wrappers around operator new
   static void *new_BareAll(void *p) {
      return  p ? new(p) ::BareAll : new ::BareAll;
   }
   static void *newArray_BareAll(Long_t nElements, void *p) {
      return p ? new(p) ::BareAll[nElements] : new ::BareAll[nElements];
   }
   // Wrapper around operator delete
   static void delete_BareAll(void *p) {
      delete ((::BareAll*)p);
   }
   static void deleteArray_BareAll(void *p) {
      delete [] ((::BareAll*)p);
   }
   static void destruct_BareAll(void *p) {
      typedef ::BareAll current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::BareAll

namespace ROOT {
   static TClass *vectorlEintgR_Dictionary();
   static void vectorlEintgR_TClassManip(TClass*);
   static void *new_vectorlEintgR(void *p = 0);
   static void *newArray_vectorlEintgR(Long_t size, void *p);
   static void delete_vectorlEintgR(void *p);
   static void deleteArray_vectorlEintgR(void *p);
   static void destruct_vectorlEintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int>*)
   {
      vector<int> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int>", -2, "vector", 214,
                  typeid(vector<int>), DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<int>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<int>*)0x0)->GetClass();
      vectorlEintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<int> : new vector<int>;
   }
   static void *newArray_vectorlEintgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<int>[nElements] : new vector<int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintgR(void *p) {
      delete ((vector<int>*)p);
   }
   static void deleteArray_vectorlEintgR(void *p) {
      delete [] ((vector<int>*)p);
   }
   static void destruct_vectorlEintgR(void *p) {
      typedef vector<int> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<int>

namespace ROOT {
   static TClass *vectorlEfloatgR_Dictionary();
   static void vectorlEfloatgR_TClassManip(TClass*);
   static void *new_vectorlEfloatgR(void *p = 0);
   static void *newArray_vectorlEfloatgR(Long_t size, void *p);
   static void delete_vectorlEfloatgR(void *p);
   static void deleteArray_vectorlEfloatgR(void *p);
   static void destruct_vectorlEfloatgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<float>*)
   {
      vector<float> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<float>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<float>", -2, "vector", 214,
                  typeid(vector<float>), DefineBehavior(ptr, ptr),
                  &vectorlEfloatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<float>) );
      instance.SetNew(&new_vectorlEfloatgR);
      instance.SetNewArray(&newArray_vectorlEfloatgR);
      instance.SetDelete(&delete_vectorlEfloatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEfloatgR);
      instance.SetDestructor(&destruct_vectorlEfloatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<float> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<float>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEfloatgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<float>*)0x0)->GetClass();
      vectorlEfloatgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEfloatgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEfloatgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<float> : new vector<float>;
   }
   static void *newArray_vectorlEfloatgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<float>[nElements] : new vector<float>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEfloatgR(void *p) {
      delete ((vector<float>*)p);
   }
   static void deleteArray_vectorlEfloatgR(void *p) {
      delete [] ((vector<float>*)p);
   }
   static void destruct_vectorlEfloatgR(void *p) {
      typedef vector<float> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<float>

namespace ROOT {
   static TClass *vectorlEboolgR_Dictionary();
   static void vectorlEboolgR_TClassManip(TClass*);
   static void *new_vectorlEboolgR(void *p = 0);
   static void *newArray_vectorlEboolgR(Long_t size, void *p);
   static void delete_vectorlEboolgR(void *p);
   static void deleteArray_vectorlEboolgR(void *p);
   static void destruct_vectorlEboolgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<bool>*)
   {
      vector<bool> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<bool>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<bool>", -2, "vector", 526,
                  typeid(vector<bool>), DefineBehavior(ptr, ptr),
                  &vectorlEboolgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<bool>) );
      instance.SetNew(&new_vectorlEboolgR);
      instance.SetNewArray(&newArray_vectorlEboolgR);
      instance.SetDelete(&delete_vectorlEboolgR);
      instance.SetDeleteArray(&deleteArray_vectorlEboolgR);
      instance.SetDestructor(&destruct_vectorlEboolgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<bool> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<bool>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEboolgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<bool>*)0x0)->GetClass();
      vectorlEboolgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEboolgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEboolgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<bool> : new vector<bool>;
   }
   static void *newArray_vectorlEboolgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<bool>[nElements] : new vector<bool>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEboolgR(void *p) {
      delete ((vector<bool>*)p);
   }
   static void deleteArray_vectorlEboolgR(void *p) {
      delete [] ((vector<bool>*)p);
   }
   static void destruct_vectorlEboolgR(void *p) {
      typedef vector<bool> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<bool>

namespace {
  void TriggerDictionaryInitialization_dict_Impl() {
    static const char* headers[] = {
"../interface/BareAll.hpp",
"../interface/BareEvent.hpp",
"../interface/BareFatJets.hpp",
"../interface/BareJets.hpp",
"../interface/BareLeptons.hpp",
"../interface/BareMet.hpp",
"../interface/BareMonteCarlo.hpp",
"../interface/BarePhotons.hpp",
"../interface/BareTaus.hpp",
"../interface/BareTrigger.hpp",
"../interface/BareVertex.hpp",
0
    };
    static const char* includePaths[] = {
"../../..",
"/cvmfs/cms.cern.ch/slc6_amd64_gcc491/lcg/root/6.02.00-odfocd4/include",
"/afs/cern.ch/work/j/jsalfeld/MIT/vdmAnalysis/CMSSW_7_4_5/src/NeroProducer/Core/bin/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$../interface/BareAll.hpp")))  BareCollection;
class __attribute__((annotate("$clingAutoload$../interface/BareEvent.hpp")))  BareEvent;
class __attribute__((annotate("$clingAutoload$../interface/BareFatJets.hpp")))  BareFatJets;
class __attribute__((annotate("$clingAutoload$../interface/BareJets.hpp")))  BareJets;
class __attribute__((annotate("$clingAutoload$../interface/BareLeptons.hpp")))  BareLeptons;
class __attribute__((annotate("$clingAutoload$../interface/BareMet.hpp")))  BareMet;
class __attribute__((annotate("$clingAutoload$../interface/BareMonteCarlo.hpp")))  BareMonteCarlo;
class __attribute__((annotate("$clingAutoload$../interface/BarePhotons.hpp")))  BarePhotons;
class __attribute__((annotate("$clingAutoload$../interface/BareTaus.hpp")))  BareTaus;
class __attribute__((annotate("$clingAutoload$../interface/BareVertex.hpp")))  BareVertex;
class __attribute__((annotate("$clingAutoload$../interface/BareAll.hpp")))  BareAll;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "../interface/BareAll.hpp"
#include "../interface/BareEvent.hpp"
#include "../interface/BareFatJets.hpp"
#include "../interface/BareJets.hpp"
#include "../interface/BareLeptons.hpp"
#include "../interface/BareMet.hpp"
#include "../interface/BareMonteCarlo.hpp"
#include "../interface/BarePhotons.hpp"
#include "../interface/BareTaus.hpp"
#include "../interface/BareTrigger.hpp"
#include "../interface/BareVertex.hpp"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"BareAll", payloadCode, "@",
"BareCollection", payloadCode, "@",
"BareEvent", payloadCode, "@",
"BareFatJets", payloadCode, "@",
"BareJets", payloadCode, "@",
"BareLeptons", payloadCode, "@",
"BareMet", payloadCode, "@",
"BareMonteCarlo", payloadCode, "@",
"BarePhotons", payloadCode, "@",
"BareTaus", payloadCode, "@",
"BareVertex", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_dict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_dict() {
  TriggerDictionaryInitialization_dict_Impl();
}
