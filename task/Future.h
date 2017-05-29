#pragma once

#include "TaskBase.h"
#include "AssociatedState.h"

namespace Acoross
{
	namespace FutureImpl
	{
		template <class T>
		class Future
		{
		public:
			Future()
				: _assocState(nullptr)
			{}

			// ctor. from promise
			Future(Sp<AssociatedState<T>> state)
				: _assocState(state)
			{}

			T Get()
			{
				if (!_assocState)
				{
					throw std::exception("invalid future");
				}
				return _assocState->GetValue();
			}

			void Wait()
			{
				if (!_assocState)
				{
					throw std::exception("invalid future");
				}
				_assocState->Wait();
			}

			bool IsReady() const
			{
				return (_assocState && _assocState->IsReady());
			}

			template <class Fty>
			Future<std::result_of_t<Fty(Future<T>)>> Then(Fty func)
			{
				if (!_assocState)
				{
					throw std::exception("invalid future");
				}

				typedef std::result_of_t<Fty(Future<T>)> ReturnT;

				auto next = [state = _assocState, func]()
				{
					return func(Future<T>(state));
				};

				_assocState->Then(next);

				return Future<ReturnT>(Make<TaskAsyncState<ReturnT>>(next));
			}

		private:
			Sp<AssociatedState<T>> _assocState;
		};
	}
}