#pragma once
#include <memory>

class ReferenceCountedObject : public std::enable_shared_from_this<ReferenceCountedObject> 
{
protected:

    /** Like std::make_shared, but works for protected constructors. Call as createShared<myclass>. */
    template<class T, class ... ArgTypes>
    static std::shared_ptr<T> createShared(ArgTypes&& ... args) {
        // Work around C++ protection problem with std::make_shared
        // http://stackoverflow.com/questions/3530771/passing-variable-arguments-to-another-function-that-accepts-a-variable-argument
        struct make_shared_enabler : public T {
            make_shared_enabler(ArgTypes&& ... args) : T(std::forward<ArgTypes>(args) ...) {}
        };
        return std::make_shared<make_shared_enabler>(std::forward<ArgTypes>(args) ...);
    };

public:
    virtual ~ReferenceCountedObject() {};
};