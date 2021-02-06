#ifndef __DXF_OBJECTID_H__
#define __DXF_OBJECTID_H__
#pragma once

class CDXFObject;

class CDXFObjectID final
{
    friend class CDXFDatabase;

// Construction/Destruction
public:
    CDXFObjectID() { }
    CDXFObjectID(const CDXFObjectID &x) { *this = x; }
    ~CDXFObjectID() { }

// Operators
public:
    const CDXFObjectID &operator=(const CDXFObjectID &id) {
        m_pObject  = id.m_pObject;
        m_ulHandle = id.m_ulHandle;
        return *this;
    }

    bool operator==(const CDXFObjectID &id) const {
        if (m_ulHandle == id.m_ulHandle &&           
            m_pObject  == id.m_pObject) {
            return true;
        }
        return false;
    }

    bool operator!=(const CDXFObjectID &id) const { return !this->operator==(id); }

// Operations
public:
    bool IsNull() const { return m_ulHandle == 0; }
    void SetNull() {
        m_pObject  = nullptr;
        m_ulHandle = 0;
    }

    CDXFObject *GetObject() const { return m_pObject; } 
    unsigned long GetHandle() const { return m_ulHandle; }

// Attributes
private:
    unsigned long m_ulHandle {0};
    CDXFObject *m_pObject    {nullptr};
};

#endif
