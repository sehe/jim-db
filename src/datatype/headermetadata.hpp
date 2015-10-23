namespace jimdb
{
    namespace memorymanagement
    {

        inline HeaderMetaData::HeaderMetaData(const size_t& id, const size_t& hash, const size_t& type,
                                              const size_t& pos): m_oid(id), m_hash(hash), m_objektType(type), m_pos(pos) {}
        
		inline HeaderMetaData::~HeaderMetaData()
        {
            //set everything to 0 on delete;
            m_oid = 0;
            m_hash = 0;
            m_objektType = 0;
            m_pos = 0;
        }

        inline size_t HeaderMetaData::getOID() const
        {
            return m_oid;
        }

        inline size_t HeaderMetaData::getHash() const
        {
            return m_hash;
        }

        inline size_t HeaderMetaData::getObjektType() const
        {
            return m_objektType;
        }

        inline size_t HeaderMetaData::getPos() const
        {
            return m_pos;
        }
    }
}