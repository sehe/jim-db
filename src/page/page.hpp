namespace jimdb
{
    namespace memorymanagement
    {
		/**
        template <typename T>
        long long Page::insert(const T& s, const long long& next)
        {
            //insert somehow like this
            BaseType<T>* temp = BaseType<T>(s);
            temp->setNext(next);
            return 0; //position of temp
        }

		// Spezialize for string
        template <>
        long long Page::insert(const std::string& s, const long long& next)
        {
            //insert somehow like this
			StringType* str = new StringType(s);
			str->setNext(next);
            return 0; //position of temp
        }
		*/
    }
}