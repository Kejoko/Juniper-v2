#!/bin/bash

# A script to setup Juniper-v2

# VulkanSDK environment variables
echo "--- Setting VulkanSDK environment variables"
VULKAN_SDK="$(pwd)/juniper/dependencies/vulkansdk/macOS"
#PATH="\$PATH:$VULKAN_SDK/bin"
#DYLD_LIBRARY_PATH="$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH"
#VK_ICD_FILENAMES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
#VK_LAYER_PATH="$VULKAN_SDK/share/vulkan/explicit_layer.d"

# Adding VulkanSDK environment variables to an sourcing .bash_profile
ENVFILE=~/.bash_profile
echo -e "\n\n\n\n\n" >> $ENVFILE
echo "# ----- ----- ----- ----- ----- ----- ----- -----" >> $ENVFILE
echo "# Juniper Engine environment vars" >> $ENVFILE
echo "# ----- ----- ----- ----- ----- ----- ----- -----" >> $ENVFILE
echo "export VULKAN_SDK=\"$VULKAN_SDK\"" >> $ENVFILE
#echo "export PATH=\"$PATH\"" >> $ENVFILE
#echo "export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH" >> $ENVFILE
#echo "export VK_ICD_FILENAMES=$VK_ICD_FILENAMES" >> $ENVFILE
#echo "export VK_LAYER_PATH=$VK_LAYER_PATH" >> $ENVFILE
source $ENVFILE
