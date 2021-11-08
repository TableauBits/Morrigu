//
// Created by Mathis Lamidey on 2021-04-03.
//
#ifndef MORRIGU_TIMESTEP_H
#define MORRIGU_TIMESTEP_H

namespace MRG
{
	// Stores times as seconds
	class Timestep
	{
	public:
		Timestep(float time) : m_time{time} {}     // NOLINT (explicit)
		operator float() const { return m_time; }  // NOLINT (explicit)

		[[nodiscard]] float getSeconds() const { return m_time; }
		[[nodiscard]] float getMilliseconds() const { return m_time * 1000; }

	private:
		float m_time;
	};
}  // namespace MRG

#endif  // MORRIGU_TIMESTEP_H
