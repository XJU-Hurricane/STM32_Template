# STM32新建工程模板文件

STM32F1与STM32F4新建模板文件。`Full`包含整个芯片家族的CMSIS支持包，可以作为平台迁移使用；`Little`只包含F103与F429的CMSIS文件，可以作为新建工程使用；`FreeRTOS`以`Little`为基础，移植了FreeRTOS的代码，可在`Application/src/rtos_tasks.c`中可以编写rtos任务。

使用`git clone https://github.com/XJU-Hurricane/STM32_Template.git`即可拷贝到本地。
