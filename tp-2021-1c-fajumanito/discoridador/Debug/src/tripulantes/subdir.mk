################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/tripulantes/tareas.c 

OBJS += \
./src/tripulantes/tareas.o 

C_DEPS += \
./src/tripulantes/tareas.d 


# Each subdirectory must supply rules for building sources it contributes
src/tripulantes/%.o: ../src/tripulantes/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


