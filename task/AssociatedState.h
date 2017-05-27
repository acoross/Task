#include <mutex>
#include <atomic>
#include "TaskBase.h"

namespace Acoross
{
	namespace FutureImpl
	{
		template <class T>
		class AssociatedState : public NoCopy
		{
		public:
			AssociatedState()
			{}

			virtual ~AssociatedState()
			{
			}

		public:
			virtual void Wait()
			{
				std::unique_lock<std::mutex> lock(_mutex);
				maybeRunDeferredFunction(lock);
				while (!_ready)
				{
					_cv.wait(lock);
				}
			}

			virtual T& GetValue()
			{
				std::unique_lock<std::mutex> lock(_mutex);
				
			if (_exception)
				{
					std::rethrow_exception(_exception);
				}

				maybeRunDeferredFunction(lock);

				while (!_ready)
				{
					_cv.wait(lock);
				}

				if (_exception)
				{
					std::rethrow_exception(_exception);
				}

				return _result;
			}

		public:
			void SetValue(T val)
			{
				std::unique_lock<std::mutex> lock(_mutex);
				if (_hasStoredResult)
				{
					throw std::exception("future has stored value");
				}

				_result = val;
				doNotify();
			}

			void SetException(std::exception_ptr exc)
			{
				std::unique_lock<std::mutex> lock(_mutex);
				if (_hasStoredResult)
				{
					throw std::exception("future has stored value");
				}

				_exception = exc;
				doNotify();
			}

			bool IsReady() const
			{
				return _ready;
			}

		protected:
			void maybeRunDeferredFunction(std::unique_lock<std::mutex>& lock)
			{
				if (!_running)
				{
					_running = true;
					runDeferredFunction(lock);
				}
			}

		private:
			virtual void doNotify()
			{
				_hasStoredResult = true;
				_ready.store(true);
				_cv.notify_all();
			}

			virtual void runDeferredFunction(std::unique_lock<std::mutex>& lock)
			{
				// empty
			}

		public:
			T _result;
			std::exception_ptr _exception;
			std::mutex _mutex;
			std::condition_variable _cv;
			std::atomic<bool> _ready{ false };
			bool _hasStoredResult{ false };
			bool _running{ false };
		};

		template <class T>
		class TaskAsyncState : public AssociatedState<T>
		{};

		template <class T>
		class DeferredAsyncState : public AssociatedState<T>
		{};

	}
}