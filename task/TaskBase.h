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

	
}