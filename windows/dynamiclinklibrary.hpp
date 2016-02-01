#ifndef WINDOWS_DYNAMICLINKLIBRARY_HPP
#define WINDOWS_DYNAMICLINKLIBRARY_HPP

#include <QString>

#include "utility.h"

#if defined(Q_OS_WIN)
#include <windows.h>
#endif

class DynamicLoadLibrary
{
public:
    DynamicLoadLibrary() :
#if defined(Q_OS_WIN)
        instance(nullptr),
#endif
        loaded_(false)
    {}
    ~DynamicLoadLibrary()
    {
        this->finalize();
    }

    bool initialize(const QString &path)
    {
        this->finalize();

        const QByteArray rawPath = toLocalEncoding(path);
#if defined(Q_OS_WIN)
        this->instance = LoadLibraryA(rawPath.constData());
        if ( ! this->instance ) {
            return false;
        }
#endif
        this->loaded_ = true;
        return true;
    }

    bool finalize()
    {
        if ( ! this->loaded_ ) {
            return false;
        }
#if defined(Q_OS_WIN)
        BOOL ret = FreeLibrary(this->instance);
        if ( ret ) {
            this->instance = nullptr;
            this->loaded_ = false;
        }
        return ret != FALSE;
#endif
    }

#if defined(Q_OS_WIN)
    FARPROC get(const QString &name)
    {
        return GetProcAddress(this->instance, name.toUtf8().constData());
    }
#endif

    bool loaded() const
    { return this->loaded_; }

private:
#if defined(Q_OS_WIN)
    HMODULE instance;
#endif
    bool loaded_;

};

#endif // WINDOWS_DYNAMICLINKLIBRARY_HPP
