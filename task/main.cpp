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
		// ���:
		//     �۾��� �ʱ�ȭ�Ǿ����� ��������� �ʾҽ��ϴ�.
		Created = 0,
		//
		// ���:
		//     �۾��� .NET Framework ������ ���� ���������� Ȱ��ȭ�ǰ� ����Ǳ� ���� ��� ���Դϴ�.
		WaitingForActivation = 1,
		//
		// ���:
		//     �۾��� ����ǵ��� ����Ǿ����� ���� ������� �ʾҽ��ϴ�.
		WaitingToRun = 2,
		//
		// ���:
		//     �۾��� ����ǰ� ������ �Ϸ���� �ʾҽ��ϴ�.
		Running = 3,
		//
		// ���:
		//     �۾��� ������ �������� ����� �ڽ� �۾��� �Ϸ�Ǳ���� �Ͻ������� ��� ���Դϴ�.
		WaitingForChildrenToComplete = 4,
		//
		// ���:
		//     �۾��� ������ �Ϸ��߽��ϴ�.
		RanToCompletion = 5,
		//
		// ���:
		//     �۾��� �����ϱ� ���� ��ū�� ��ȣ�� ���� �����̰ų� �۾��� CancellationToken�� �̹� ��ȣ�� ���� ���¿��� ��ü CancellationToken��
		//     �Բ� OperationCanceledException�� throw�Ͽ� �۾��� ��Ҹ� �����߽��ϴ�.�ڼ��� ������ �۾� ��Ҹ� �����Ͻʽÿ�.
		Canceled = 6,
		//
		// ���:
		//     �۾��� ó������ ���� ���ܷ� ���� �Ϸ�Ǿ����ϴ�.
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
		// �� �ѹ��� ȣ�� ����. Promise �� ���� ȣ���.
		void SetResult(T result)
		{
			_result = result;

			PushContinuationTasks();
		}

		// Promise �� ���� ȣ���.
		void SetException(std::exception_ptr exp)
		{
			_exp = exp;

			PushContinuationTasks();
		}

		// worker �ȿ��� ȣ��Ǿ�� �Ѵ�.
		// �� �ѹ��� ȣ�� ����. (�� �� �̻� ȣ��Ǵ� ���� ����!)
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

		// "�� task" �� caller ���� ���������� ����
		// ContinueWith �� ����� Task ����� �ѹ��� ���� ��������� ����
		// ������ ������ �Ұ����� task �� ��� throw exception
		void RunSync()
		{

		}

		// "�� task" �� �Ϸ�� �� ���� block
		// �Ϸ�Ǹ� ����� return
		// �� �Լ��� ������ ȣ�� ����. �ι�° ���ķδ� ����� �ٷ� return
		// "task func" ���� �� �߻��� ����, Ȥ�� SetException ���� ��ġ�� ���ܴ�
		// Wait() �Լ��� caller ���� throw �ȴ�.
		// Wait() �� ȣ���� �� ���� throw
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

		// �ѹ��� ȣ�Ⱑ��. �ι� ȣ�� �� throw
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