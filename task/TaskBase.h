#pragma once

#pragma once

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

	
}