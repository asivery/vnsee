#ifndef APP_DEP_PROVIDER_HPP
#define APP_DEP_PROVIDER_HPP

namespace app
{

    template <class T, typename... constructor_arguments>
    class provider
    {
        public:
        virtual T* fetch() = 0;
        virtual void create(constructor_arguments...) = 0;
    };

}

#endif
