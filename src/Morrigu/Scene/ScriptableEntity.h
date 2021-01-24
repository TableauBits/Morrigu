#ifndef MRG_CLASS_SCRIPTABLENTITY
#define MRG_CLASS_SCRIPTABLENTITY

#include "Scene/Entity.h"

namespace MRG
{
	class ScriptableEntity
	{
	public:
		ScriptableEntity() = default;
		ScriptableEntity(const ScriptableEntity&) = delete;
		ScriptableEntity(ScriptableEntity&&) = default;
		virtual ~ScriptableEntity() = default;

		ScriptableEntity& operator=(const ScriptableEntity&) = delete;
		ScriptableEntity& operator=(ScriptableEntity&&) = default;

		template<typename T>
		[[nodiscard]] T& getComponent()
		{
			return m_entity.getComponent<T>();
		}

	protected:
		virtual void onCreate() {}
		virtual void onDestroy() {}
		virtual void onUpdate(Timestep) {}

	private:
		Entity m_entity;
		friend class Scene;
	};
}  // namespace MRG

#endif
