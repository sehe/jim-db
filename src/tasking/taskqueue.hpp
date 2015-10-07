
namespace jimdb
{
    namespace tasking
    {
		inline bool TaskQueue::push_pack(std::shared_ptr<Task> t)
        {
            //if it's 0 the queue "is unlimited".
            if (m_maxSize != 0 && m_tasks.size() >= m_maxSize)
                return false;

            std::lock_guard<std::mutex> lock(m_mutex);
            m_tasks.push_back(t);
            //notify that there is one more task, so one thread can work now
            m_cond.notify_one();
            return true;
        }

        inline std::shared_ptr<Task> TaskQueue::pop_front()
        {
            //regular lock so noone else acces this area now
            std::unique_lock<std::mutex> lock(m_mutex);
            while (m_tasks.size() == 0)
                m_cond.wait(lock);
            auto task = m_tasks.front();
            m_tasks.pop_front();
            return task;
        }
    }
}