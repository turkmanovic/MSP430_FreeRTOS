################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
ETF5529_HAL/hal_7seg.obj: H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/ETF5529_HAL/hal_7seg.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccs1010/ccs/ccs_base/msp430/include" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/SRV_2_3" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/drivers" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/FreeRTOS_source/include" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/FreeRTOS_source/portable/CCS/MSP430X" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/SRV_2_3" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/drivers/MSP430F5xx_6xx" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include" --advice:power=all --define=__MSP430F5529__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="ETF5529_HAL/$(basename $(<F)).d_raw" --obj_directory="ETF5529_HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

ETF5529_HAL/hal_board.obj: H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/ETF5529_HAL/hal_board.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccs1010/ccs/ccs_base/msp430/include" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/SRV_2_3" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/drivers" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/FreeRTOS_source/include" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/FreeRTOS_source/portable/CCS/MSP430X" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/SRV_2_3" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/drivers/MSP430F5xx_6xx" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include" --advice:power=all --define=__MSP430F5529__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="ETF5529_HAL/$(basename $(<F)).d_raw" --obj_directory="ETF5529_HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

ETF5529_HAL/hal_led.obj: H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/ETF5529_HAL/hal_led.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccs1010/ccs/ccs_base/msp430/include" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/SRV_2_3" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/drivers" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/FreeRTOS_source/include" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/FreeRTOS_source/portable/CCS/MSP430X" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/SRV_2_3" --include_path="H:/Posao/ETF/SRV/Novo/Workspace/Examples/common/drivers/MSP430F5xx_6xx" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include" --advice:power=all --define=__MSP430F5529__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="ETF5529_HAL/$(basename $(<F)).d_raw" --obj_directory="ETF5529_HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


