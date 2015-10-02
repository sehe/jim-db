#include "spinlock.h"
namespace jimdb
{
	namespace tasking {
		inline SpinLock::SpinLock()
		{
			lck.clear(std::memory_order_release);
		}

		inline void SpinLock::lock()
		{
			while (lck.test_and_set(std::memory_order_acquire)) {}
		}

		inline void SpinLock::unlock()
		{
			lck.clear(std::memory_order_release);
		}
	}
}