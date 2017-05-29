#include <mutex>
#include <atomic>
#include "TaskBase.h"

namespace Acoross
{
	namespace FutureImpl
	{
		template <class T>
		class AssociatedStateBase
		{
		public:
			virtual ~AssociatedStateBase()
			{}

			virtual void Wait()
			{
				std::unique_lock<std::mutex> lock(_mutex);

				while (!_ready)
				{
					_cv.wait(lock);
				}
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

			void Then(std::function<void()> next)
			{
				std::unique_lock<std::mutex> lock(_mutex);
				if (_ready)
				{
					std::cout << "Then, tid: " << std::this_thread::get_id() << std::endl;
					std::thread([next]()
					{
						std::cout << "thread, tid: " << std::this_thread::get_id() << std::endl;
						next();
					}).detach();
				}
				else
				{
					_continuationTasks.push_back(next);
				}
			}

		protected:
			virtual void doNotify()
			{
				for (auto t : _continuationTasks)
				{
					std::thread([t]()
					{
						t();
					}).detach();
				}

				_hasStoredResult = true;
				_ready.store(true);
				_cv.notify_all();
			}

			std::exception_ptr _exception;
			std::mutex _mutex;
			std::condition_variable _cv;
			std::atomic<bool> _ready{ false };
			bool _hasStoredResult{ false };
			bool _running{ false };

			std::list<std::function<void()>> _continuationTasks;
		};

		template <class T>
		class AssociatedState : public NoCopy, public AssociatedStateBase<T>
		{
		public:
			AssociatedState()
			{}

			virtual ~AssociatedState()
			{}

			virtual T& GetValue()
			{
				std::unique_lock<std::mutex> lock(_mutex);
				
				if (_exception)
				{
					std::rethrow_exception(_exception);
				}

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

		public:
			T _result;
		};


		template <>
		class AssociatedState<void> : public NoCopy, public AssociatedStateBase<void>
		{
		public:
			AssociatedState()
			{}

			virtual ~AssociatedState()
			{}

			virtual void GetValue()
			{
				std::unique_lock<std::mutex> lock(_mutex);

				if (_exception)
				{
					std::rethrow_exception(_exception);
				}

				while (!_ready)
				{
					_cv.wait(lock);
				}

				if (_exception)
				{
					std::rethrow_exception(_exception);
				}
			}

			void SetValue()
			{
				std::unique_lock<std::mutex> lock(_mutex);
				if (_hasStoredResult)
				{
					throw std::exception("future has stored value");
				}

				doNotify();
			}
		};

		template <class T>
		class TaskAsyncState : public AssociatedState<T>
		{
			typedef AssociatedState<T> MyBaseT;
		public:
			template <class Fty>
			TaskAsyncState(Fty func)
				: _func(func)
				, MyBaseT()
			{
			}

			virtual ~TaskAsyncState()
			{
			}

			void CallImmediate()
			{
				try {
					SetValue(_func());
				}
				catch (...) {
					SetException(std::current_exception());
				}
			}

		private:
			Func<T()> _func;
		};
	}
}