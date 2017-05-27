#pragma once

#include "TaskBase.h"
#include "AssociatedState.h"

namespace Acoross
{
	namespace FutureImpl
	{
		template <class T>
		class Future
			: public StateManager<T>
		{
		public:
			// ctor. from promise
			Future(Sp<AssociatedState<T>> state)
				: _assocState(state)
			{}

			T Get()
			{
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

		private:
			Sp<AssociatedState<T>> _assocState;
		};
	}
}