#ifndef PADCAST_SCALING_H
#define PADCAST_SCALING_H

#include <algorithm>

struct ScalingInfo
{
	float scale{};
	float offsetX{};
	float offsetY{};

	ScalingInfo(int currentWidth, int currentHeight, int originalWidth, int originalHeight);

	// Returns scale clamped to a minimum (used by menus to keep UI readable at small sizes)
	float effectiveScale(float minScale) const { return std::max(scale, minScale); }
};

// Pre-computed menu layout values from a ScalingInfo + fixed base params.
// Handles the common pattern: scale + offset for position, straight scale for dimensions.
struct MenuScaling
{
	float scale;        // effectiveScale(minScale)
	int x, y;           // base * scale + offset
	int width;          // baseWidth * scale
	int lineHeight;     // baseLineHeight * scale

	MenuScaling(const ScalingInfo& s, float minScale,
	            int baseX, int baseY, int w, int lh)
		: scale(s.effectiveScale(minScale))
		, x(static_cast<int>(baseX * scale + s.offsetX))
		, y(static_cast<int>(baseY * scale + s.offsetY))
		, width(static_cast<int>(w * scale))
		, lineHeight(static_cast<int>(lh * scale))
	{}
};

#endif
