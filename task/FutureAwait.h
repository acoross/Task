#include "Future.h"
#include "Promise.h"

template <class... ArgsT>
struct std::experimental::coroutine_traits<Acoross::FutureImpl::Future<void>, ArgsT...>
{
	struct promise_type {
		Acoross::FutureImpl::Promise<void> p;
		auto get_return_object() {
			return p.GetFuture();
		}
		std::experimental::suspend_never initial_suspend() { return{}; }
		std::experimental::suspend_never final_suspend() { return{}; }
		void set_exception(std::exception_ptr e) { p.SetException(e); }
		void return_void() { p.SetValue(); }
	};
};

template <class Rt, class... ArgsT>
struct std::experimental::coroutine_traits<Acoross::FutureImpl::Future<Rt>, ArgsT...>
{
	struct promise_type {
		Acoross::FutureImpl::Promise<Rt> p;
		auto get_return_object() {
			return p.GetFuture();
		}
		std::experimental::suspend_never initial_suspend() { return{}; }
		std::experimental::suspend_never final_suspend() { return{}; }
		void set_exception(std::exception_ptr e) { p.SetException(e); }
		template <class U> void return_value(U&& u) {
			p.SetValue(std::forward<U>(u));
		}
	};
};

template <class Rt>
auto operator co_await(Acoross::FutureImpl::Future<Rt>&& f)
{
	using namespace Acoross::FutureImpl;

	struct Awaiter 
	{
		Future<Rt> input;
		Future<Rt> output;
		bool await_ready() { return false; }
		auto await_resume() { return output.Get(); }
		void await_suspend(std::experimental::coroutine_handle<> coro)
		{
			input.Then([this, coro](auto result_future)
			{
				this->output = std::move(result_future);
				coro.resume();
			});
		}
	};

	Awaiter awaiter{ f };
	return awaiter;
};

namespace Acoross
{
	namespace FutureImpl
	{

	}
}