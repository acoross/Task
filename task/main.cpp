#include <memory>
#include <functional>
#include <list>
#include <future>
#include <optional>

namespace Acoross
{
	class NoCopy
	{
	public:
		NoCopy() = default;
		NoCopy(NoCopy&) = delete;
		NoCopy& operator=(NoCopy&) = delete;
	};
}

// collections
namespace Acoross
{
	template <class T>
	using List = std::list<T>;
}

// memory
namespace Acoross
{
	template <class T>
	using Sp = std::shared_ptr<T>;

	template<typename T>
	constexpr auto Make = std::make_shared<T>;
}

// functional
namespace Acoross
{
	template <class T>
	using Func = std::function<T>;
}

namespace Acoross
{
	enum class TaskStatus
	{
		//
		// 요약:
		//     작업이 초기화되었지만 예약되지는 않았습니다.
		Created = 0,
		//
		// 요약:
		//     작업이 .NET Framework 인프라를 통해 내부적으로 활성화되고 예약되기 위해 대기 중입니다.
		WaitingForActivation = 1,
		//
		// 요약:
		//     작업이 실행되도록 예약되었지만 아직 실행되지 않았습니다.
		WaitingToRun = 2,
		//
		// 요약:
		//     작업이 실행되고 있지만 완료되지 않았습니다.
		Running = 3,
		//
		// 요약:
		//     작업이 실행을 마쳤지만 연결된 자식 작업이 완료되기까지 암시적으로 대기 중입니다.
		WaitingForChildrenToComplete = 4,
		//
		// 요약:
		//     작업이 실행을 완료했습니다.
		RanToCompletion = 5,
		//
		// 요약:
		//     작업을 실행하기 전에 토큰이 신호를 받은 상태이거나 작업의 CancellationToken이 이미 신호를 받은 상태에서 자체 CancellationToken과
		//     함께 OperationCanceledException을 throw하여 작업이 취소를 승인했습니다.자세한 내용은 작업 취소를 참조하십시오.
		Canceled = 6,
		//
		// 요약:
		//     작업이 처리되지 않은 예외로 인해 완료되었습니다.
		Faulted = 7
	};

	class ITaskWorker
	{
	public:
		~ITaskWorker(){}

		virtual void Push(Sp<class TaskStateBase> taskState) = 0;
	};

	class TaskCommon
	{
	public:
		static void SetDefaultWorker(Sp<class ITaskWorker> worker)
		{
			Inst()._worker = worker;
		}

		static Sp<class ITaskWorker> GetDefaultWorker()
		{
			return Inst()._worker;
		}

	private:
		static TaskCommon& Inst()
		{
			static TaskCommon inst;
			return inst;
		}

		Sp<class ITaskWorker> _worker;
	};

	class TaskStateBase : public NoCopy
	{
	public:
		virtual void RunByWorker() = 0;

		void ContinueWith(Sp<TaskStateBase> continuationTask)
		{
			_continuationTasks.push_back(continuationTask);
		}

		void PushContinuationTasks()
		{
			auto worker = TaskCommon::GetDefaultWorker();

			for (auto cont : _continuationTasks)
			{
				worker->Push(cont);
			}
		}

		std::list<Sp<TaskStateBase>> _continuationTasks;
		std::optional<std::exception_ptr> _exp;
		TaskStatus _status;
	};

	template <class T>
	class TaskState : public TaskStateBase
	{
	public:
		// 단 한번만 호출 가능. Promise 로 부터 호출됨.
		void SetResult(T result)
		{
			_result = result;

			PushContinuationTasks();
		}

		// Promise 로 부터 호출됨.
		void SetException(std::exception_ptr exp)
		{
			_exp = exp;

			PushContinuationTasks();
		}

		// worker 안에서 호출되어야 한다.
		// 단 한번만 호출 가능. (두 번 이상 호출되는 것은 버그!)
		virtual void RunByWorker() final
		{
			try {
				_result = _job();
			}
			catch (...) {
				_exp = std::current_exception();
			}

			PushContinuationTasks();
		}

		Func<T()> _job;
		std::optional<T> _result;
	};

	template <class T>
	class Task
	{
	public:
		Task(Sp<TaskState<T>> taskState)
			: _sharedState(taskState)
		{
		}

		template <class TOther>
		Task<TOther> ContinueWith(Func<TOther(Task<T>)> continuationAction)
		{
			_sharedState->
		}

		bool IsReady() const
		{

		}

		// "이 task" 를 caller 에게 동기적으로 실행
		// ContinueWith 로 연결된 Task 들까지 한번에 동기 실행되지는 않음
		// 동기적 실행이 불가능한 task 의 경우 throw exception
		void RunSync()
		{

		}

		// "이 task" 가 완료될 때 까지 block
		// 완료되면 결과를 return
		// 이 함수는 여러번 호출 가능. 두번째 이후로는 결과만 바로 return
		// "task func" 실행 중 발생한 예외, 혹은 SetException 으로 설치된 예외는
		// Wait() 함수의 caller 에게 throw 된다.
		// Wait() 을 호출할 때 마다 throw
		T Wait()
		{

		}

	private:
		Sp<TaskState> _sharedState;
	};

	template <class T>
	class TaskCompletionSource : public NoCopy
	{
	public:
		TaskCompletionSource()
			: _sharedTaskState(Make<T>())
		{

		}

		Task<T> GetTask()
		{
			return Task<T>(_sharedTaskState);
		}

		// 한번만 호출가능. 두번 호출 시 throw
		void SetResult(T val)
		{
			_sharedTaskState->SetResult(val);
		}

		void SetException(std::exception_ptr exp)
		{
			_sharedTaskState->SetException(exp);
		}

		Sp<TaskState> _sharedTaskState;
	};
}

int main()
{
	std::unique_ptr<int> ui;
	std::future<int> f;
}