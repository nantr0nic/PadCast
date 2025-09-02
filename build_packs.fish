#!/usr/bin/env fish

echo "Starting PadCast build process..."

# Step 2: Build regular Linux packages (RPM & TGZ)
echo "===> Building Linux packages..."
rm -rf out/
cmake --preset=linux-release
cmake --build --preset=linux-release
cd out/build/linux-release
cpack
cd ../../..
echo "===> Finished: RPM and TGZ packages built in out/build/linux-release"

# Step 3: Build Flatpak package
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
echo "Generated packages:"
echo "  - out/build/linux-release/padcast-*.rpm"
echo "  - out/build/linux-release/padcast-*.tar.gz"
echo "  - out/build/linux-release/PadCast.flatpak"