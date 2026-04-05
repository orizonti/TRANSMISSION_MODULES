#ifndef TYPE_REGISTER_ENGINE_H
#define TYPE_REGISTER_ENGINE_H
#include <QString>
#include <QDebug>
#include <typeinfo>
#include <string>
#include <map>
#include <iostream>

template<int NUM>
class TypeRegisterInfo
{
      public:
      static int TypeMaxSize; 
      static int TypeMinSize; 
      static std::map<int, std::string> typeNames;
      static std::map<int, int> typeSizes;

      static void printTypesSignature() 
      { 
            for(auto& record: typeNames) std::cout << "[ " << record.first << " ] " << record.second  << "[ SIZE ] " << typeSizes[record.first]<< std::endl; 
      }
};

template<int NUM> int TypeRegisterInfo<NUM>::TypeMaxSize; 
template<int NUM> int TypeRegisterInfo<NUM>::TypeMinSize; 
template<int NUM> std::map<int, std::string> TypeRegisterInfo<NUM>::typeNames;
template<int NUM> std::map<int, int>         TypeRegisterInfo<NUM>::typeSizes;
//============================================================================

template<int NUM = 0> 
class TypeRegisterID 
{ 
      public:
      static bool isRegistered(int ID) { auto it = std::find(TYPES.begin(), TYPES.end(), ID); return it != TYPES.end(); };
      static void printRegistered()    { for(auto it = TYPES.begin(); it != end; it++) std::cout << "[ TYPE ID ] " << *it <<  " [ REGISTERED ]" << std::endl; }
      static  int countTypes()         { return end - TYPES.begin(); }
      static void appendTypeID(int NUM_T){ *end = NUM_T; end++; };

      static std::array<int,100> TYPES;
      static std::array<int,100>::iterator end;
};

template<int NUM> std::array<int,100> TypeRegisterID<NUM>::TYPES;
template<int NUM> std::array<int,100>::iterator TypeRegisterID<NUM>::end{TypeRegisterID<NUM>::TYPES.begin()};
//============================================================================


template<typename T = void*> 
class TypeRegister
{
public:
      using TYPE_STORE = std::array<int,100>;
      static constinit const int TYPE_ID;

          static int GetTypeID()   { return TYPE_ID;}
constexpr static int GetTypeSize() { return sizeof(T);}
          static int GetTypeCount() { return TypeRegisterID<>::countTypes();}
          static QString GetTypeName() { return typeid(T).name();}

          static bool isTypeRegistered()               { return TypeRegisterID<0>::isRegistered(TYPE_ID);}
          static bool isTypeRegistered(const int& NUM) { return TypeRegisterID<0>::isRegistered(NUM);}

          static constexpr int GetMinTypeSize() {return 2;}
          static constexpr int GetMaxTypeSize() {return 1000;}

      static void registerType(std::string name) 
      { 
           TYPES_INFO.typeNames[TYPE_ID] = name; 
           TYPES_INFO.typeSizes[TYPE_ID] = GetTypeSize();

        if(TYPES_INFO.TypeMaxSize < TYPES_INFO.typeSizes[TYPE_ID] ) TYPES_INFO.TypeMaxSize = GetTypeSize(); 
        if(TYPES_INFO.TypeMinSize > TYPES_INFO.typeSizes[TYPE_ID] ) TYPES_INFO.TypeMinSize = GetTypeSize(); 

        TypeRegisterID<>::appendTypeID(TYPE_ID);
      };

      static TypeRegisterInfo<0> TYPES_INFO;
};

template<typename T> TypeRegisterInfo<0> TypeRegister<T>::TYPES_INFO;


#endif 