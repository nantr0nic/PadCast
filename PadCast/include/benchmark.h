#ifndef PADCAST_BENCHMARK_H
#define PADCAST_BENCHMARK_H

#include <raylib.h>

#include <cstdio>

// Lightweight frame timer for ad-hoc benchmarking.
// Usage:
//   FrameTimer bench("label");
//   while (!mWindow.ShouldClose()) {
//       double frameStart = GetTime();
//       // ... frame body ...
//       bench.recordFrame(GetTime() - frameStart);
//       if (bench.done()) break;
//   }
// Destructor prints stats to stderr.

struct FrameTimer
{
	double startTime{ GetTime() };
	int frameCount{ 0 };
	double minDt{ 999.0 };
	double maxDt{ 0.0 };
	int targetFrames;
	const char* label;

	FrameTimer(const char* label_, int target = 600)
		: targetFrames(target), label(label_) {}

	void recordFrame(double frameDt)
	{
		++frameCount;
		if (frameDt < minDt) minDt = frameDt;
		if (frameDt > maxDt) maxDt = frameDt;
	}

	bool done() const
	{
		return frameCount >= targetFrames;
	}

	~FrameTimer()
	{
		if (frameCount == 0) return;

		double elapsed = GetTime() - startTime;
		double avg = elapsed / frameCount;
		fprintf(stderr, "\n=== BENCHMARK (%s) ===\n", label);
		fprintf(stderr, "Frames: %d\n", frameCount);
		fprintf(stderr, "Elapsed: %.4f s\n", elapsed);
		fprintf(stderr, "Average FPS: %.1f\n", frameCount / elapsed);
		fprintf(stderr, "Frame time  avg: %.6f s  min: %.6f s  max: %.6f s\n",
		       avg, minDt, maxDt);
		fprintf(stderr, "==========================\n");
	}
};

#endif
