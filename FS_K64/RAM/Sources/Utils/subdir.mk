################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Utils/StTime.c" \
"../Sources/Utils/def.c" \
"../Sources/Utils/stbp.c" \

C_SRCS += \
../Sources/Utils/StTime.c \
../Sources/Utils/def.c \
../Sources/Utils/stbp.c \

OBJS += \
./Sources/Utils/StTime.o \
./Sources/Utils/def.o \
./Sources/Utils/stbp.o \

C_DEPS += \
./Sources/Utils/StTime.d \
./Sources/Utils/def.d \
./Sources/Utils/stbp.d \

OBJS_QUOTED += \
"./Sources/Utils/StTime.o" \
"./Sources/Utils/def.o" \
"./Sources/Utils/stbp.o" \

C_DEPS_QUOTED += \
"./Sources/Utils/StTime.d" \
"./Sources/Utils/def.d" \
"./Sources/Utils/stbp.d" \

OBJS_OS_FORMAT += \
./Sources/Utils/StTime.o \
./Sources/Utils/def.o \
./Sources/Utils/stbp.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Utils/StTime.o: ../Sources/Utils/StTime.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Utils/StTime.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Utils/StTime.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Utils/def.o: ../Sources/Utils/def.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Utils/def.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Utils/def.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Utils/stbp.o: ../Sources/Utils/stbp.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Utils/stbp.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Utils/stbp.o"
	@echo 'Finished building: $<'
	@echo ' '


