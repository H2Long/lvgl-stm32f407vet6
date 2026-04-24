cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(C ASM)
# STM32CubeMX generated symbols (macros)
set(MX_Defines_Syms 
	USE_HAL_DRIVER 
	STM32F407xx
    $<$<CONFIG:Debug>:DEBUG>
)

# STM32CubeMX generated include paths
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Include
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/freertos.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/fsmc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f4xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f4xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f4xx_hal_timebase_tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_stm32f407xx.s
)

# LCD 和 Touch 驱动源文件
set(LCD_Touch_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/lcd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/touch.c
)

# LCD 和 Touch 头文件路径
set(LCD_Touch_Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src
)

# LVGL 源文件
set(LVGL_Src
    # core/ - LVGL 核心模块
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_group.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_indev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_obj.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_obj_class.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_obj_draw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_obj_pos.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_obj_scroll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_obj_style.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_obj_style_gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_obj_tree.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_refr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_theme.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_indev_scroll.c

    # draw/ - 绘图模块
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_img.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_label.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_line.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_mask.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_rect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_triangle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_transform.c

    # draw/sw/ - 软件绘图
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_blend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_dither.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_gradient.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_img.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_letter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_line.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_polygon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_rect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/sw/lv_draw_sw_transform.c

    # draw/arm2d/ - ARM2D加速(可选)
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/arm2d/lv_gpu_arm2d.c

    # 图像缓冲和解码
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_img_buf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_img_cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_img_decoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/draw/lv_draw_layer.c

    # font/ - 字体支持(根据需要选择)
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_fmt_txt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_loader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_montserrat_12.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_montserrat_14.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_montserrat_16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_montserrat_24.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_dejavu_16_persian_hebrew.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_simsun_16_cjk.c

    # hal/ - 硬件抽象层
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/hal/lv_hal_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/hal/lv_hal_indev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/hal/lv_hal_tick.c

    # misc/ - 杂项功能
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_anim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_anim_timeline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_area.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_async.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_bidi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_color.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_gc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_ll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_math.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_printf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_style.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_style_gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_tlsf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_txt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_txt_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/misc/lv_utils.c

    # widgets/ - 基础控件
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_arc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_bar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_btn.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_btnmatrix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_canvas.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_checkbox.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_dropdown.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_img.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_label.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_line.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_roller.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_slider.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_switch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/widgets/lv_textarea.c

    # extra/ - 扩展模块
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/lv_extra.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/layouts/flex/lv_flex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/layouts/grid/lv_grid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/themes/default/lv_theme_default.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/animimg/lv_animimg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/calendar/lv_calendar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/calendar/lv_calendar_header_arrow.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/calendar/lv_calendar_header_dropdown.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/chart/lv_chart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/colorwheel/lv_colorwheel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/imgbtn/lv_imgbtn.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/keyboard/lv_keyboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/led/lv_led.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/list/lv_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/menu/lv_menu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/meter/lv_meter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/msgbox/lv_msgbox.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/span/lv_span.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/spinbox/lv_spinbox.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/spinner/lv_spinner.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/tabview/lv_tabview.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/tileview/lv_tileview.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/win/lv_win.c

    # porting/ - 移植文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/examples/porting/lv_port_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/examples/porting/lv_port_indev.c
)

# LVGL 头文件路径
set(LVGL_Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/examples/porting
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/system_stm32f4xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fsmc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_nor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sram.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_nand.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pccard.c
)

# Drivers Midllewares


set(FreeRTOS_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
)

# Link directories setup
set(MX_LINK_DIRS

)
# Project static libraries
set(MX_LINK_LIBS 
    STM32_Drivers
    ${TOOLCHAIN_LINK_LIBRARIES}
    FreeRTOS
	
)
# Interface library for includes and symbols
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs} ${LCD_Touch_Inc} ${LVGL_Inc})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)


# Create FreeRTOS static library
add_library(FreeRTOS OBJECT)
target_sources(FreeRTOS PRIVATE ${FreeRTOS_Src})
target_link_libraries(FreeRTOS PUBLIC stm32cubemx)

# Add STM32CubeMX generated application sources to the project
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src} ${LCD_Touch_Src} ${LVGL_Src})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
