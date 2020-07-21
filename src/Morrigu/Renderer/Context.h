#ifndef MRG_CLASS_CONTEXT
#define MRG_CLASS_CONTEXT

namespace MRG
{
	class Context
	{
	public:
		virtual ~Context() {}

		virtual void swapBuffers() = 0;
	};
}  // namespace MRG

#endif