#!/usr/bin/env fish
echo "===> Building Flatpak package..."

# COMPLETELY clean all build artifacts and cache
rm -rf .flatpak-builder/ build-dir/ repo/

flatpak-builder --repo=repo --force-clean build-dir flatpak/com.github.nantr0nic.PadCast.yml

# Only create bundle if the build succeeded
if test -d repo
    flatpak build-bundle repo out/build/linux-release/PadCast.flatpak com.github.nantr0nic.PadCast
    echo "(^_^) Flatpak created successfully!"
else
    echo "(T_T) Flatpak build failed - no repository created"
    exit 1
end

# Clean up build artifacts but keep the packages
rm -rf repo/ build-dir/ .flatpak-builder/

echo " ===> Build complete! <==="