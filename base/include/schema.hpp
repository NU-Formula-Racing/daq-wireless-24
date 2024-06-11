#define SCHEMA_CONTENTS "\n \
meta {\n \
        .schema: 'daq-schema';\n \
        .version: 1.0.0;\n \
}\n \
\n \
# PDM Messages\n \
def PDM_Current {\n \
        float Unreg_Amps; # A;\n \
        float FiveVolt_Amps; # A;\n \
}\n \
\n \
# WHEEL Messages\n \
def Wheel_Temp_Front {\n \
        float Wheel_Temp_FrontL1; # °C;\n \
        float Wheel_Temp_FrontL2; # °C;\n \
        float Wheel_Temp_FrontR1; # °C;\n \
        float Wheel_Temp_FrontR2; # °C;\n \
}\n \
\n \
def Wheel_Speed_Front {\n \
        float Wheel_Speed_FrontL; # rot/s;\n \
        float Wheel_Speed_FrontR; # rot/s;\n \
}\n \
\n \
def Wheel_Temp_Back {\n \
        float Wheel_Temp_BackL1; # °C;\n \
        float Wheel_Temp_BackL2; # °C;\n \
        float Wheel_Temp_BackR1; # °C;\n \
        float Wheel_Temp_BackR2; # °C;\n \
}\n \
\n \
# COOLANT Messages\n \
def Coolant_Temp_Message {\n \
        float Coolant_Temp; # °C;\n \
}\n \
\n \
def Coolant_Flow_Message {\n \
        float Coolant_Flow; # mL/s;\n \
}\n \
\n \
# SHUTDOWN Messages\n \
def Shutdown_Status {\n \
        bool Voltage_Sag; # bitset, lsbit is shutdown 1, support from 1-6;\n \
}\n \
\n \
# ETC Messages\n \
def Throttle_tx {\n \
        float Throttle_percent; # % but 0-32767;\n \
        bool Throttle_active; # boolean;\n \
}\n \
\n \
def Brake_tx {\n \
        bool Brake_pedal; # boolean;\n \
}\n \
\n \
# VCU Messages\n \
def Drive_Status {\n \
        bool Drive_State; # boolean;\n \
}\n \
\n \
# SUSPENSION Messages\n \
def Sus_Pots {\n \
        float Sus_Front; # cm;\n \
}\n \
\n \
def Car_Data {\n \
        long timestamp;\n \
        PDM_Current PDM_Current;\n \
        Wheel_Temp_Front Wheel_Temp_Front;\n \
        Wheel_Speed_Front Wheel_Speed_Front;\n \
        Wheel_Temp_Back Wheel_Temp_Back;\n \
        Coolant_Temp_Message Coolant_Temp_Message;\n \
        Coolant_Flow_Message Coolant_Flow_Message;\n \
        Shutdown_Status Shutdown_Status;\n \
        Throttle_tx Throttle_tx;\n \
        Brake_tx Brake_tx;\n \
        Drive_Status Drive_Status;\n \
        Sus_Pots Sus_Pots;\n \
}\n \
\n \
frame(Car_Data);\n \
"