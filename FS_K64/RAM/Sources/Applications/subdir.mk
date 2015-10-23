################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Applications/gCompute.c" \
"../Sources/Applications/gInput.c" \
"../Sources/Applications/gMBox.c" \
"../Sources/Applications/gOutput.c" \
"../Sources/Applications/main.c" \

C_SRCS += \
../Sources/Applications/gCompute.c \
../Sources/Applications/gInput.c \
../Sources/Applications/gMBox.c \
../Sources/Applications/gOutput.c \
../Sources/Applications/main.c \

OBJS += \
./Sources/Applications/gCompute.o \
./Sources/Applications/gInput.o \
./Sources/Applications/gMBox.o \
./Sources/Applications/gOutput.o \
./Sources/Applications/main.o \

C_DEPS += \
./Sources/Applications/gCompute.d \
./Sources/Applications/gInput.d \
./Sources/Applications/gMBox.d \
./Sources/Applications/gOutput.d \
./Sources/Applications/main.d \

OBJS_QUOTED += \
"./Sources/Applications/gCompute.o" \
"./Sources/Applications/gInput.o" \
"./Sources/Applications/gMBox.o" \
"./Sources/Applications/gOutput.o" \
"./Sources/Applications/main.o" \

C_DEPS_QUOTED += \
"./Sources/Applications/gCompute.d" \
"./Sources/Applications/gInput.d" \
"./Sources/Applications/gMBox.d" \
"./Sources/Applications/gOutput.d" \
"./Sources/Applications/main.d" \

OBJS_OS_FORMAT += \
./Sources/Applications/gCompute.o \
./Sources/Applications/gInput.o \
./Sources/Applications/gMBox.o \
./Sources/Applications/gOutput.o \
./Sources/Applications/main.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Applications/gCompute.o: ../Sources/Applications/gCompute.c
	@echo 'Building file: $<'
	@echo 'Executing target #24 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Applications/gCompute.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Applications/gCompute.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Applications/gInput.o: ../Sources/Applications/gInput.c
	@echo 'Building file: $<'
	@echo 'Executing target #25 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Applications/gInput.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Applications/gInput.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Applications/gMBox.o: ../Sources/Applications/gMBox.c
	@echo 'Building file: $<'
	@echo 'Executing target #26 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Applications/gMBox.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Applications/gMBox.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Applications/gOutput.o: ../Sources/Applications/gOutput.c
	@echo 'Building file: $<'
	@echo 'Executing target #27 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Applications/gOutput.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Applications/gOutput.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Applications/main.o: ../Sources/Applications/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #28 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Applications/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Applications/main.o"
	@echo 'Finished building: $<'
	@echo ' '


