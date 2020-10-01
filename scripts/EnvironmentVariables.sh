#! /bin/bash

# VulkanSDK environment variables
export VULKAN_ROOT_LOCATION="$(pwd)/juniper/dependencies/"
export VULKAN_SDK="$(pwd)/juniper/dependencies/vulkansdk/macOS"
export PATH="$VULKAN_SDK/bin:$PATH"
export DYLD_LIBRARY_PATH="$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH"
export VK_ICD_FILENAMES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
export VK_LAYER_PATH="$VULKAN_SDK/share/vulkan/explicit_layer.d"
