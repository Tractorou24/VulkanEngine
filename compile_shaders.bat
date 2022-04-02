@echo off

pushd "C:\Dev\Vulkan\Vulkan\shaders"
	C:\Dev\Libraries\VulkanSDK\1.3.204.1\Bin\glslc.exe simple_shader.vert -o simple_shader.vert.spv
	C:\Dev\Libraries\VulkanSDK\1.3.204.1\Bin\glslc.exe simple_shader.frag -o simple_shader.frag.spv
popd

pause