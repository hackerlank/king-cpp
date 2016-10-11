#ifndef KING_LIB_HEADER_SERIALIZATION_SINGLETON
#define KING_LIB_HEADER_SERIALIZATION_SINGLETON

namespace king
{
    namespace serialization
    {
        template<typename T>
        class Singleton
        {
        protected:
            static T& getInstance()
            {
                static T t;
                return t;
            }
        public:
            static T& GetInstance()
            {
               return getInstance();
            }
            static const T& GetConstInstance()
            {
               return getInstance();
            }
        };
    };
};

#endif // KING_LIB_HEADER_SERIALIZATION_SINGLETON
