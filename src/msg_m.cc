//
// Generated file, do not edit! Created by nedtool 5.6 from msg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include "msg_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace {
template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)(static_cast<const omnetpp::cObject *>(t));
}

template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)dynamic_cast<const void *>(t);
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)static_cast<const void *>(t);
}

}

class IntVectorDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
    };
  public:
    IntVectorDescriptor();
    virtual ~IntVectorDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(IntVectorDescriptor)

IntVectorDescriptor::IntVectorDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(IntVector)), "")
{
    propertynames = nullptr;
}

IntVectorDescriptor::~IntVectorDescriptor()
{
    delete[] propertynames;
}

bool IntVectorDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<IntVector *>(obj)!=nullptr;
}

const char **IntVectorDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *IntVectorDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname, "existingClass")) return "";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int IntVectorDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int IntVectorDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *IntVectorDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int IntVectorDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *IntVectorDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **IntVectorDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *IntVectorDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int IntVectorDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    IntVector *pp = (IntVector *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *IntVectorDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    IntVector *pp = (IntVector *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string IntVectorDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    IntVector *pp = (IntVector *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool IntVectorDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    IntVector *pp = (IntVector *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *IntVectorDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *IntVectorDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    IntVector *pp = (IntVector *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

class IntVectorVDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
    };
  public:
    IntVectorVDescriptor();
    virtual ~IntVectorVDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(IntVectorVDescriptor)

IntVectorVDescriptor::IntVectorVDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(IntVectorV)), "")
{
    propertynames = nullptr;
}

IntVectorVDescriptor::~IntVectorVDescriptor()
{
    delete[] propertynames;
}

bool IntVectorVDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<IntVectorV *>(obj)!=nullptr;
}

const char **IntVectorVDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *IntVectorVDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname, "existingClass")) return "";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int IntVectorVDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int IntVectorVDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *IntVectorVDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int IntVectorVDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *IntVectorVDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **IntVectorVDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *IntVectorVDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int IntVectorVDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    IntVectorV *pp = (IntVectorV *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *IntVectorVDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    IntVectorV *pp = (IntVectorV *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string IntVectorVDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    IntVectorV *pp = (IntVectorV *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool IntVectorVDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    IntVectorV *pp = (IntVectorV *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *IntVectorVDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *IntVectorVDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    IntVectorV *pp = (IntVectorV *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

class msg_Descriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
    };
  public:
    msg_Descriptor();
    virtual ~msg_Descriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(msg_Descriptor)

msg_Descriptor::msg_Descriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(msg_)), "")
{
    propertynames = nullptr;
}

msg_Descriptor::~msg_Descriptor()
{
    delete[] propertynames;
}

bool msg_Descriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<msg_ *>(obj)!=nullptr;
}

const char **msg_Descriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *msg_Descriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname, "existingClass")) return "";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int msg_Descriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int msg_Descriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *msg_Descriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int msg_Descriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *msg_Descriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **msg_Descriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *msg_Descriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int msg_Descriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    msg_ *pp = (msg_ *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *msg_Descriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    msg_ *pp = (msg_ *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string msg_Descriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    msg_ *pp = (msg_ *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool msg_Descriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    msg_ *pp = (msg_ *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *msg_Descriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *msg_Descriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    msg_ *pp = (msg_ *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

namespace inet {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule to generate operator<< for shared_ptr<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');

    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(Msg)

Msg::Msg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(1));

}

Msg::Msg(const Msg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

Msg::~Msg()
{
}

Msg& Msg::operator=(const Msg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void Msg::copy(const Msg& other)
{
    this->id = other.id;
    this->msg = other.msg;
    this->view = other.view;
    this->installView = other.installView;
    this->SEQcv = other.SEQcv;
    this->join_or_leave = other.join_or_leave;
    this->type = other.type;
    this->cer = other.cer;
    this->Vcer = other.Vcer;
    this->sendTime = other.sendTime;
    this->arrivalTime = other.arrivalTime;
    this->msgId = other.msgId;
    this->install_or_update = other.install_or_update;
    this->typeS = other.typeS;
    this->sender = other.sender;
    this->destId = other.destId;
}

void Msg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->id);
    doParsimPacking(b,this->msg);
    doParsimPacking(b,this->view);
    doParsimPacking(b,this->installView);
    doParsimPacking(b,this->SEQcv);
    doParsimPacking(b,this->join_or_leave);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->cer);
    doParsimPacking(b,this->Vcer);
    doParsimPacking(b,this->sendTime);
    doParsimPacking(b,this->arrivalTime);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->install_or_update);
    doParsimPacking(b,this->typeS);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->destId);
}

void Msg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->id);
    doParsimUnpacking(b,this->msg);
    doParsimUnpacking(b,this->view);
    doParsimUnpacking(b,this->installView);
    doParsimUnpacking(b,this->SEQcv);
    doParsimUnpacking(b,this->join_or_leave);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->cer);
    doParsimUnpacking(b,this->Vcer);
    doParsimUnpacking(b,this->sendTime);
    doParsimUnpacking(b,this->arrivalTime);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->install_or_update);
    doParsimUnpacking(b,this->typeS);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->destId);
}

int Msg::getId() const
{
    return this->id;
}

void Msg::setId(int id)
{
    handleChange();
    this->id = id;
}

const char * Msg::getMsg() const
{
    return this->msg.c_str();
}

void Msg::setMsg(const char * msg)
{
    handleChange();
    this->msg = msg;
}

const IntVector& Msg::getView() const
{
    return this->view;
}

void Msg::setView(const IntVector& view)
{
    handleChange();
    this->view = view;
}

const IntVector& Msg::getInstallView() const
{
    return this->installView;
}

void Msg::setInstallView(const IntVector& installView)
{
    handleChange();
    this->installView = installView;
}

const IntVectorV& Msg::getSEQcv() const
{
    return this->SEQcv;
}

void Msg::setSEQcv(const IntVectorV& SEQcv)
{
    handleChange();
    this->SEQcv = SEQcv;
}

int Msg::getJoin_or_leave() const
{
    return this->join_or_leave;
}

void Msg::setJoin_or_leave(int join_or_leave)
{
    handleChange();
    this->join_or_leave = join_or_leave;
}

int Msg::getType() const
{
    return this->type;
}

void Msg::setType(int type)
{
    handleChange();
    this->type = type;
}

int Msg::getCer() const
{
    return this->cer;
}

void Msg::setCer(int cer)
{
    handleChange();
    this->cer = cer;
}

const IntVector& Msg::getVcer() const
{
    return this->Vcer;
}

void Msg::setVcer(const IntVector& Vcer)
{
    handleChange();
    this->Vcer = Vcer;
}

omnetpp::simtime_t Msg::getSendTime() const
{
    return this->sendTime;
}

void Msg::setSendTime(omnetpp::simtime_t sendTime)
{
    handleChange();
    this->sendTime = sendTime;
}

omnetpp::simtime_t Msg::getArrivalTime() const
{
    return this->arrivalTime;
}

void Msg::setArrivalTime(omnetpp::simtime_t arrivalTime)
{
    handleChange();
    this->arrivalTime = arrivalTime;
}

int Msg::getMsgId() const
{
    return this->msgId;
}

void Msg::setMsgId(int msgId)
{
    handleChange();
    this->msgId = msgId;
}

int Msg::getInstall_or_update() const
{
    return this->install_or_update;
}

void Msg::setInstall_or_update(int install_or_update)
{
    handleChange();
    this->install_or_update = install_or_update;
}

const char * Msg::getTypeS() const
{
    return this->typeS.c_str();
}

void Msg::setTypeS(const char * typeS)
{
    handleChange();
    this->typeS = typeS;
}

int Msg::getSender() const
{
    return this->sender;
}

void Msg::setSender(int sender)
{
    handleChange();
    this->sender = sender;
}

int Msg::getDestId() const
{
    return this->destId;
}

void Msg::setDestId(int destId)
{
    handleChange();
    this->destId = destId;
}

class MsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_id,
        FIELD_msg,
        FIELD_view,
        FIELD_installView,
        FIELD_SEQcv,
        FIELD_join_or_leave,
        FIELD_type,
        FIELD_cer,
        FIELD_Vcer,
        FIELD_sendTime,
        FIELD_arrivalTime,
        FIELD_msgId,
        FIELD_install_or_update,
        FIELD_typeS,
        FIELD_sender,
        FIELD_destId,
    };
  public:
    MsgDescriptor();
    virtual ~MsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MsgDescriptor)

MsgDescriptor::MsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::Msg)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

MsgDescriptor::~MsgDescriptor()
{
    delete[] propertynames;
}

bool MsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Msg *>(obj)!=nullptr;
}

const char **MsgDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MsgDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 16+basedesc->getFieldCount() : 16;
}

unsigned int MsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_id
        FD_ISEDITABLE,    // FIELD_msg
        FD_ISCOMPOUND,    // FIELD_view
        FD_ISCOMPOUND,    // FIELD_installView
        FD_ISCOMPOUND,    // FIELD_SEQcv
        FD_ISEDITABLE,    // FIELD_join_or_leave
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_cer
        FD_ISCOMPOUND,    // FIELD_Vcer
        0,    // FIELD_sendTime
        0,    // FIELD_arrivalTime
        FD_ISEDITABLE,    // FIELD_msgId
        FD_ISEDITABLE,    // FIELD_install_or_update
        FD_ISEDITABLE,    // FIELD_typeS
        FD_ISEDITABLE,    // FIELD_sender
        FD_ISEDITABLE,    // FIELD_destId
    };
    return (field >= 0 && field < 16) ? fieldTypeFlags[field] : 0;
}

const char *MsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "id",
        "msg",
        "view",
        "installView",
        "SEQcv",
        "join_or_leave",
        "type",
        "cer",
        "Vcer",
        "sendTime",
        "arrivalTime",
        "msgId",
        "install_or_update",
        "typeS",
        "sender",
        "destId",
    };
    return (field >= 0 && field < 16) ? fieldNames[field] : nullptr;
}

int MsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'i' && strcmp(fieldName, "id") == 0) return base+0;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msg") == 0) return base+1;
    if (fieldName[0] == 'v' && strcmp(fieldName, "view") == 0) return base+2;
    if (fieldName[0] == 'i' && strcmp(fieldName, "installView") == 0) return base+3;
    if (fieldName[0] == 'S' && strcmp(fieldName, "SEQcv") == 0) return base+4;
    if (fieldName[0] == 'j' && strcmp(fieldName, "join_or_leave") == 0) return base+5;
    if (fieldName[0] == 't' && strcmp(fieldName, "type") == 0) return base+6;
    if (fieldName[0] == 'c' && strcmp(fieldName, "cer") == 0) return base+7;
    if (fieldName[0] == 'V' && strcmp(fieldName, "Vcer") == 0) return base+8;
    if (fieldName[0] == 's' && strcmp(fieldName, "sendTime") == 0) return base+9;
    if (fieldName[0] == 'a' && strcmp(fieldName, "arrivalTime") == 0) return base+10;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msgId") == 0) return base+11;
    if (fieldName[0] == 'i' && strcmp(fieldName, "install_or_update") == 0) return base+12;
    if (fieldName[0] == 't' && strcmp(fieldName, "typeS") == 0) return base+13;
    if (fieldName[0] == 's' && strcmp(fieldName, "sender") == 0) return base+14;
    if (fieldName[0] == 'd' && strcmp(fieldName, "destId") == 0) return base+15;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_id
        "string",    // FIELD_msg
        "IntVector",    // FIELD_view
        "IntVector",    // FIELD_installView
        "IntVectorV",    // FIELD_SEQcv
        "int",    // FIELD_join_or_leave
        "int",    // FIELD_type
        "int",    // FIELD_cer
        "IntVector",    // FIELD_Vcer
        "omnetpp::simtime_t",    // FIELD_sendTime
        "omnetpp::simtime_t",    // FIELD_arrivalTime
        "int",    // FIELD_msgId
        "int",    // FIELD_install_or_update
        "string",    // FIELD_typeS
        "int",    // FIELD_sender
        "int",    // FIELD_destId
    };
    return (field >= 0 && field < 16) ? fieldTypeStrings[field] : nullptr;
}

const char **MsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MsgDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MsgDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    Msg *pp = (Msg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MsgDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Msg *pp = (Msg *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MsgDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Msg *pp = (Msg *)object; (void)pp;
    switch (field) {
        case FIELD_id: return long2string(pp->getId());
        case FIELD_msg: return oppstring2string(pp->getMsg());
        case FIELD_view: {std::stringstream out; out << pp->getView(); return out.str();}
        case FIELD_installView: {std::stringstream out; out << pp->getInstallView(); return out.str();}
        case FIELD_SEQcv: {std::stringstream out; out << pp->getSEQcv(); return out.str();}
        case FIELD_join_or_leave: return long2string(pp->getJoin_or_leave());
        case FIELD_type: return long2string(pp->getType());
        case FIELD_cer: return long2string(pp->getCer());
        case FIELD_Vcer: {std::stringstream out; out << pp->getVcer(); return out.str();}
        case FIELD_sendTime: return simtime2string(pp->getSendTime());
        case FIELD_arrivalTime: return simtime2string(pp->getArrivalTime());
        case FIELD_msgId: return long2string(pp->getMsgId());
        case FIELD_install_or_update: return long2string(pp->getInstall_or_update());
        case FIELD_typeS: return oppstring2string(pp->getTypeS());
        case FIELD_sender: return long2string(pp->getSender());
        case FIELD_destId: return long2string(pp->getDestId());
        default: return "";
    }
}

bool MsgDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    Msg *pp = (Msg *)object; (void)pp;
    switch (field) {
        case FIELD_id: pp->setId(string2long(value)); return true;
        case FIELD_msg: pp->setMsg((value)); return true;
        case FIELD_join_or_leave: pp->setJoin_or_leave(string2long(value)); return true;
        case FIELD_type: pp->setType(string2long(value)); return true;
        case FIELD_cer: pp->setCer(string2long(value)); return true;
        case FIELD_msgId: pp->setMsgId(string2long(value)); return true;
        case FIELD_install_or_update: pp->setInstall_or_update(string2long(value)); return true;
        case FIELD_typeS: pp->setTypeS((value)); return true;
        case FIELD_sender: pp->setSender(string2long(value)); return true;
        case FIELD_destId: pp->setDestId(string2long(value)); return true;
        default: return false;
    }
}

const char *MsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_view: return omnetpp::opp_typename(typeid(IntVector));
        case FIELD_installView: return omnetpp::opp_typename(typeid(IntVector));
        case FIELD_SEQcv: return omnetpp::opp_typename(typeid(IntVectorV));
        case FIELD_Vcer: return omnetpp::opp_typename(typeid(IntVector));
        default: return nullptr;
    };
}

void *MsgDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Msg *pp = (Msg *)object; (void)pp;
    switch (field) {
        case FIELD_view: return toVoidPtr(&pp->getView()); break;
        case FIELD_installView: return toVoidPtr(&pp->getInstallView()); break;
        case FIELD_SEQcv: return toVoidPtr(&pp->getSEQcv()); break;
        case FIELD_Vcer: return toVoidPtr(&pp->getVcer()); break;
        default: return nullptr;
    }
}

class MsgVectorDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
    };
  public:
    MsgVectorDescriptor();
    virtual ~MsgVectorDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MsgVectorDescriptor)

MsgVectorDescriptor::MsgVectorDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::MsgVector)), "")
{
    propertynames = nullptr;
}

MsgVectorDescriptor::~MsgVectorDescriptor()
{
    delete[] propertynames;
}

bool MsgVectorDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MsgVector *>(obj)!=nullptr;
}

const char **MsgVectorDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = { "existingClass",  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MsgVectorDescriptor::getProperty(const char *propertyname) const
{
    if (!strcmp(propertyname, "existingClass")) return "";
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MsgVectorDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount() : 0;
}

unsigned int MsgVectorDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    return 0;
}

const char *MsgVectorDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

int MsgVectorDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MsgVectorDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

const char **MsgVectorDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MsgVectorDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MsgVectorDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MsgVector *pp = (MsgVector *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MsgVectorDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MsgVector *pp = (MsgVector *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MsgVectorDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MsgVector *pp = (MsgVector *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool MsgVectorDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MsgVector *pp = (MsgVector *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *MsgVectorDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    return nullptr;
}

void *MsgVectorDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MsgVector *pp = (MsgVector *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(StateUpdateMessage)

StateUpdateMessage::StateUpdateMessage() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(2));

}

StateUpdateMessage::StateUpdateMessage(const StateUpdateMessage& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

StateUpdateMessage::~StateUpdateMessage()
{
}

StateUpdateMessage& StateUpdateMessage::operator=(const StateUpdateMessage& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void StateUpdateMessage::copy(const StateUpdateMessage& other)
{
    this->id = other.id;
    this->msg = other.msg;
    this->view = other.view;
    this->RECV = other.RECV;
    this->installView = other.installView;
    this->SEQcv = other.SEQcv;
    this->type = other.type;
    this->ack = other.ack;
    this->ack_value = other.ack_value;
    this->conflicting = other.conflicting;
    this->conflicting_value = other.conflicting_value;
    this->stored = other.stored;
    this->stored_value = other.stored_value;
    this->sendTime = other.sendTime;
    this->arrivalTime = other.arrivalTime;
    this->typeS = other.typeS;
}

void StateUpdateMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->id);
    doParsimPacking(b,this->msg);
    doParsimPacking(b,this->view);
    doParsimPacking(b,this->RECV);
    doParsimPacking(b,this->installView);
    doParsimPacking(b,this->SEQcv);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->ack);
    doParsimPacking(b,this->ack_value);
    doParsimPacking(b,this->conflicting);
    doParsimPacking(b,this->conflicting_value);
    doParsimPacking(b,this->stored);
    doParsimPacking(b,this->stored_value);
    doParsimPacking(b,this->sendTime);
    doParsimPacking(b,this->arrivalTime);
    doParsimPacking(b,this->typeS);
}

void StateUpdateMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->id);
    doParsimUnpacking(b,this->msg);
    doParsimUnpacking(b,this->view);
    doParsimUnpacking(b,this->RECV);
    doParsimUnpacking(b,this->installView);
    doParsimUnpacking(b,this->SEQcv);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->ack);
    doParsimUnpacking(b,this->ack_value);
    doParsimUnpacking(b,this->conflicting);
    doParsimUnpacking(b,this->conflicting_value);
    doParsimUnpacking(b,this->stored);
    doParsimUnpacking(b,this->stored_value);
    doParsimUnpacking(b,this->sendTime);
    doParsimUnpacking(b,this->arrivalTime);
    doParsimUnpacking(b,this->typeS);
}

int StateUpdateMessage::getId() const
{
    return this->id;
}

void StateUpdateMessage::setId(int id)
{
    handleChange();
    this->id = id;
}

const char * StateUpdateMessage::getMsg() const
{
    return this->msg.c_str();
}

void StateUpdateMessage::setMsg(const char * msg)
{
    handleChange();
    this->msg = msg;
}

const IntVector& StateUpdateMessage::getView() const
{
    return this->view;
}

void StateUpdateMessage::setView(const IntVector& view)
{
    handleChange();
    this->view = view;
}

const IntVector& StateUpdateMessage::getRECV() const
{
    return this->RECV;
}

void StateUpdateMessage::setRECV(const IntVector& RECV)
{
    handleChange();
    this->RECV = RECV;
}

const IntVector& StateUpdateMessage::getInstallView() const
{
    return this->installView;
}

void StateUpdateMessage::setInstallView(const IntVector& installView)
{
    handleChange();
    this->installView = installView;
}

const IntVectorV& StateUpdateMessage::getSEQcv() const
{
    return this->SEQcv;
}

void StateUpdateMessage::setSEQcv(const IntVectorV& SEQcv)
{
    handleChange();
    this->SEQcv = SEQcv;
}

int StateUpdateMessage::getType() const
{
    return this->type;
}

void StateUpdateMessage::setType(int type)
{
    handleChange();
    this->type = type;
}

bool StateUpdateMessage::getAck() const
{
    return this->ack;
}

void StateUpdateMessage::setAck(bool ack)
{
    handleChange();
    this->ack = ack;
}

const MsgVector& StateUpdateMessage::getAck_value() const
{
    return this->ack_value;
}

void StateUpdateMessage::setAck_value(const MsgVector& ack_value)
{
    handleChange();
    this->ack_value = ack_value;
}

bool StateUpdateMessage::getConflicting() const
{
    return this->conflicting;
}

void StateUpdateMessage::setConflicting(bool conflicting)
{
    handleChange();
    this->conflicting = conflicting;
}

const MsgVector& StateUpdateMessage::getConflicting_value() const
{
    return this->conflicting_value;
}

void StateUpdateMessage::setConflicting_value(const MsgVector& conflicting_value)
{
    handleChange();
    this->conflicting_value = conflicting_value;
}

bool StateUpdateMessage::getStored() const
{
    return this->stored;
}

void StateUpdateMessage::setStored(bool stored)
{
    handleChange();
    this->stored = stored;
}

const MsgVector& StateUpdateMessage::getStored_value() const
{
    return this->stored_value;
}

void StateUpdateMessage::setStored_value(const MsgVector& stored_value)
{
    handleChange();
    this->stored_value = stored_value;
}

omnetpp::simtime_t StateUpdateMessage::getSendTime() const
{
    return this->sendTime;
}

void StateUpdateMessage::setSendTime(omnetpp::simtime_t sendTime)
{
    handleChange();
    this->sendTime = sendTime;
}

omnetpp::simtime_t StateUpdateMessage::getArrivalTime() const
{
    return this->arrivalTime;
}

void StateUpdateMessage::setArrivalTime(omnetpp::simtime_t arrivalTime)
{
    handleChange();
    this->arrivalTime = arrivalTime;
}

const char * StateUpdateMessage::getTypeS() const
{
    return this->typeS.c_str();
}

void StateUpdateMessage::setTypeS(const char * typeS)
{
    handleChange();
    this->typeS = typeS;
}

class StateUpdateMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_id,
        FIELD_msg,
        FIELD_view,
        FIELD_RECV,
        FIELD_installView,
        FIELD_SEQcv,
        FIELD_type,
        FIELD_ack,
        FIELD_ack_value,
        FIELD_conflicting,
        FIELD_conflicting_value,
        FIELD_stored,
        FIELD_stored_value,
        FIELD_sendTime,
        FIELD_arrivalTime,
        FIELD_typeS,
    };
  public:
    StateUpdateMessageDescriptor();
    virtual ~StateUpdateMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(StateUpdateMessageDescriptor)

StateUpdateMessageDescriptor::StateUpdateMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::StateUpdateMessage)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

StateUpdateMessageDescriptor::~StateUpdateMessageDescriptor()
{
    delete[] propertynames;
}

bool StateUpdateMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<StateUpdateMessage *>(obj)!=nullptr;
}

const char **StateUpdateMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *StateUpdateMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int StateUpdateMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 16+basedesc->getFieldCount() : 16;
}

unsigned int StateUpdateMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_id
        FD_ISEDITABLE,    // FIELD_msg
        FD_ISCOMPOUND,    // FIELD_view
        FD_ISCOMPOUND,    // FIELD_RECV
        FD_ISCOMPOUND,    // FIELD_installView
        FD_ISCOMPOUND,    // FIELD_SEQcv
        FD_ISEDITABLE,    // FIELD_type
        FD_ISEDITABLE,    // FIELD_ack
        FD_ISCOMPOUND,    // FIELD_ack_value
        FD_ISEDITABLE,    // FIELD_conflicting
        FD_ISCOMPOUND,    // FIELD_conflicting_value
        FD_ISEDITABLE,    // FIELD_stored
        FD_ISCOMPOUND,    // FIELD_stored_value
        0,    // FIELD_sendTime
        0,    // FIELD_arrivalTime
        FD_ISEDITABLE,    // FIELD_typeS
    };
    return (field >= 0 && field < 16) ? fieldTypeFlags[field] : 0;
}

const char *StateUpdateMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "id",
        "msg",
        "view",
        "RECV",
        "installView",
        "SEQcv",
        "type",
        "ack",
        "ack_value",
        "conflicting",
        "conflicting_value",
        "stored",
        "stored_value",
        "sendTime",
        "arrivalTime",
        "typeS",
    };
    return (field >= 0 && field < 16) ? fieldNames[field] : nullptr;
}

int StateUpdateMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'i' && strcmp(fieldName, "id") == 0) return base+0;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msg") == 0) return base+1;
    if (fieldName[0] == 'v' && strcmp(fieldName, "view") == 0) return base+2;
    if (fieldName[0] == 'R' && strcmp(fieldName, "RECV") == 0) return base+3;
    if (fieldName[0] == 'i' && strcmp(fieldName, "installView") == 0) return base+4;
    if (fieldName[0] == 'S' && strcmp(fieldName, "SEQcv") == 0) return base+5;
    if (fieldName[0] == 't' && strcmp(fieldName, "type") == 0) return base+6;
    if (fieldName[0] == 'a' && strcmp(fieldName, "ack") == 0) return base+7;
    if (fieldName[0] == 'a' && strcmp(fieldName, "ack_value") == 0) return base+8;
    if (fieldName[0] == 'c' && strcmp(fieldName, "conflicting") == 0) return base+9;
    if (fieldName[0] == 'c' && strcmp(fieldName, "conflicting_value") == 0) return base+10;
    if (fieldName[0] == 's' && strcmp(fieldName, "stored") == 0) return base+11;
    if (fieldName[0] == 's' && strcmp(fieldName, "stored_value") == 0) return base+12;
    if (fieldName[0] == 's' && strcmp(fieldName, "sendTime") == 0) return base+13;
    if (fieldName[0] == 'a' && strcmp(fieldName, "arrivalTime") == 0) return base+14;
    if (fieldName[0] == 't' && strcmp(fieldName, "typeS") == 0) return base+15;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *StateUpdateMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_id
        "string",    // FIELD_msg
        "IntVector",    // FIELD_view
        "IntVector",    // FIELD_RECV
        "IntVector",    // FIELD_installView
        "IntVectorV",    // FIELD_SEQcv
        "int",    // FIELD_type
        "bool",    // FIELD_ack
        "inet::MsgVector",    // FIELD_ack_value
        "bool",    // FIELD_conflicting
        "inet::MsgVector",    // FIELD_conflicting_value
        "bool",    // FIELD_stored
        "inet::MsgVector",    // FIELD_stored_value
        "omnetpp::simtime_t",    // FIELD_sendTime
        "omnetpp::simtime_t",    // FIELD_arrivalTime
        "string",    // FIELD_typeS
    };
    return (field >= 0 && field < 16) ? fieldTypeStrings[field] : nullptr;
}

const char **StateUpdateMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *StateUpdateMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int StateUpdateMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    StateUpdateMessage *pp = (StateUpdateMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *StateUpdateMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    StateUpdateMessage *pp = (StateUpdateMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string StateUpdateMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    StateUpdateMessage *pp = (StateUpdateMessage *)object; (void)pp;
    switch (field) {
        case FIELD_id: return long2string(pp->getId());
        case FIELD_msg: return oppstring2string(pp->getMsg());
        case FIELD_view: {std::stringstream out; out << pp->getView(); return out.str();}
        case FIELD_RECV: {std::stringstream out; out << pp->getRECV(); return out.str();}
        case FIELD_installView: {std::stringstream out; out << pp->getInstallView(); return out.str();}
        case FIELD_SEQcv: {std::stringstream out; out << pp->getSEQcv(); return out.str();}
        case FIELD_type: return long2string(pp->getType());
        case FIELD_ack: return bool2string(pp->getAck());
        case FIELD_ack_value: {std::stringstream out; out << pp->getAck_value(); return out.str();}
        case FIELD_conflicting: return bool2string(pp->getConflicting());
        case FIELD_conflicting_value: {std::stringstream out; out << pp->getConflicting_value(); return out.str();}
        case FIELD_stored: return bool2string(pp->getStored());
        case FIELD_stored_value: {std::stringstream out; out << pp->getStored_value(); return out.str();}
        case FIELD_sendTime: return simtime2string(pp->getSendTime());
        case FIELD_arrivalTime: return simtime2string(pp->getArrivalTime());
        case FIELD_typeS: return oppstring2string(pp->getTypeS());
        default: return "";
    }
}

bool StateUpdateMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    StateUpdateMessage *pp = (StateUpdateMessage *)object; (void)pp;
    switch (field) {
        case FIELD_id: pp->setId(string2long(value)); return true;
        case FIELD_msg: pp->setMsg((value)); return true;
        case FIELD_type: pp->setType(string2long(value)); return true;
        case FIELD_ack: pp->setAck(string2bool(value)); return true;
        case FIELD_conflicting: pp->setConflicting(string2bool(value)); return true;
        case FIELD_stored: pp->setStored(string2bool(value)); return true;
        case FIELD_typeS: pp->setTypeS((value)); return true;
        default: return false;
    }
}

const char *StateUpdateMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_view: return omnetpp::opp_typename(typeid(IntVector));
        case FIELD_RECV: return omnetpp::opp_typename(typeid(IntVector));
        case FIELD_installView: return omnetpp::opp_typename(typeid(IntVector));
        case FIELD_SEQcv: return omnetpp::opp_typename(typeid(IntVectorV));
        case FIELD_ack_value: return omnetpp::opp_typename(typeid(MsgVector));
        case FIELD_conflicting_value: return omnetpp::opp_typename(typeid(MsgVector));
        case FIELD_stored_value: return omnetpp::opp_typename(typeid(MsgVector));
        default: return nullptr;
    };
}

void *StateUpdateMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    StateUpdateMessage *pp = (StateUpdateMessage *)object; (void)pp;
    switch (field) {
        case FIELD_view: return toVoidPtr(&pp->getView()); break;
        case FIELD_RECV: return toVoidPtr(&pp->getRECV()); break;
        case FIELD_installView: return toVoidPtr(&pp->getInstallView()); break;
        case FIELD_SEQcv: return toVoidPtr(&pp->getSEQcv()); break;
        case FIELD_ack_value: return toVoidPtr(&pp->getAck_value()); break;
        case FIELD_conflicting_value: return toVoidPtr(&pp->getConflicting_value()); break;
        case FIELD_stored_value: return toVoidPtr(&pp->getStored_value()); break;
        default: return nullptr;
    }
}

} // namespace inet

