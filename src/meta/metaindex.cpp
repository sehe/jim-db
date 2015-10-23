#include "metaindex.h"
namespace jimdb
{
    namespace meta
    {
        MetaIndex MetaIndex::m_instance;
        MetaIndex& MetaIndex::getInstance()
        {
            return m_instance;
        }
    }
}