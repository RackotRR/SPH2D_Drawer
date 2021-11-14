#pragma once

class RRGameTime {
public:
	static RRGameTime& Instance();

	void Update();

	unsigned int GetPassedTime() const;
	unsigned int GetTotalTime() const;

private:
	unsigned int previousFrameTicks{};
	unsigned int currentFrameTicks{};
	unsigned int passedTime{}; 

	RRGameTime() = default;
	RRGameTime(RRGameTime&) = delete;
	void operator=(RRGameTime&) = delete;
};