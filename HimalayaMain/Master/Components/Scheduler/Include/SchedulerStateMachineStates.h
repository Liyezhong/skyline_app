#ifndef SCHEDULERSTATEMACHINESTATES_H
#define SCHEDULERSTATEMACHINESTATES_H
namespace Scheduler{
#define STATE_UNDEFINED                     (0x0)
#define L1_INIT                             (0x1)
#define L1_IDLE                             (0x2)
#define L1_BUSY                             (0x3)
#define L1_ERROR                            (0x4)

#define L2_PRO_ST                             (0x1)
#define L2_PRO_INIT                           (0x2)
#define L2_PRO_READY_TO_HEAT_LEVEL_SENSOR_S1  (0x3)
#define L2_PRO_READY_TO_HEAT_LEVEL_SENSOR_S2  (0x4)
#define L2_PRO_READY_TO_TUBE_BEFORE           (0x5)
#define L2_PRO_READY_TO_FILL                  (0x6)
#define L2_PRO_READY_TO_SEAL                  (0x7)
#define L2_PRO_SOAK                           (0x8)
#define L2_PRO_READY_TO_TUBE_AFTER            (0x9)
#define L2_PRO_READY_TO_DRAIN                 (0xA)
#define L2_PRO_STEP_FINISH                    (0xB)
#define L2_PRO_PROGRAM_FINISH                 (0xC)
#define L2_PRO_PAUSE                          (0xD)
#define L2_PRO_PAUSE_DRAIN                    (0xE)
#define L2_PRO_ABORTING                       (0xF)
#define L2_PRO_ABORTED                        (0x10)

#define L3_ST_INIT                            (0x1)
#define L3_ST_TEMP_CHECKING                   (0x2)
#define L3_ST_CURRENT_CHECKING                (0x3)
#define L3_ST_VOLTAGE_CHECKING                (0x4)
#define L3_ST_RV_POSITION_CHECKING            (0x5)
#define L3_ST_PRESSURE_CHECKING               (0x6)
#define L3_ST_SEALING_CHECKING                (0x7)
#define L3_ST_STATION_CHECKING                (0x8)
#define L3_ST_STATION_CHECK_FINISH            (0x9)
#define L3_ST_DONE                            (0xA)

#define L2_ERR_WAIT                           (0x1)
#define L2_ERR_RS_RV_MOV_TO_INIT_POS_AGAIN    (0x2)
#define L2_ERR_RC_REPORT                      (0x3)
#define L2_ERR_RS_STANDBY                     (0x4)
#define L3_ERR_RS_RV_MOVING_TO_INIT_POS       (0x1)
#define L3_ERR_RC_REPORT                      (0x1)
#define L3_ERR_RS_RELEASE_PRESSURE            (0x1)
#define L3_ERR_RS_SHUTDOWN_FAILED_HEATER      (0x2)


typedef enum
{
    SM_UNDEF = (STATE_UNDEFINED),
    SM_INIT = (L1_INIT),
    SM_IDLE = (L1_IDLE),
    SM_BUSY = (L1_BUSY),
    SM_ERROR = (L1_ERROR),

    PSSM_INIT = ((L2_PRO_INIT << 8) | L1_BUSY),
    PSSM_READY_TO_HEAT_LEVEL_SENSOR_S1 = ((L2_PRO_READY_TO_HEAT_LEVEL_SENSOR_S1 << 8) | L1_BUSY),
    PSSM_READY_TO_HEAT_LEVEL_SENSOR_S2 = ((L2_PRO_READY_TO_HEAT_LEVEL_SENSOR_S2 << 8) | L1_BUSY),
    PSSM_READY_TO_TUBE_BEFORE = ((L2_PRO_READY_TO_TUBE_BEFORE << 8) | L1_BUSY),
    PSSM_READY_TO_FILL = ((L2_PRO_READY_TO_FILL << 8) | L1_BUSY),
    PSSM_READY_TO_SEAL = ((L2_PRO_READY_TO_SEAL << 8) | L1_BUSY),
    PSSM_SOAK = ((L2_PRO_SOAK << 8) | L1_BUSY),
    PSSM_READY_TO_TUBE_AFTER = ((L2_PRO_READY_TO_TUBE_AFTER << 8) | L1_BUSY),
    PSSM_READY_TO_DRAIN = ((L2_PRO_READY_TO_DRAIN << 8) | L1_BUSY),
    PSSM_STEP_FINISH = ((L2_PRO_STEP_FINISH << 8) | L1_BUSY),
    PSSM_PROGRAM_FINISH = ((L2_PRO_PROGRAM_FINISH << 8) | L1_BUSY),
    PSSM_PAUSE = ((L2_PRO_PAUSE << 8) | L1_BUSY),
    PSSM_PAUSE_DRAIN = ((L2_PRO_PAUSE_DRAIN << 8) | L1_BUSY),
    PSSM_ABORTING = ((L2_PRO_ABORTING << 8) | L1_BUSY),
    PSSM_ABORTED = ((L2_PRO_ABORTED << 8) | L1_BUSY),
    PSSM_ST = ((L2_PRO_ST << 8) | L1_BUSY),

    PSSM_ST_INIT = ((L3_ST_INIT << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_TEMP_CHECKING = ((L3_ST_TEMP_CHECKING << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_CURRENT_CHECKING =((L3_ST_CURRENT_CHECKING << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_VOLTAGE_CHECKING = ((L3_ST_VOLTAGE_CHECKING << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_RV_POSITION_CHECKING =((L3_ST_RV_POSITION_CHECKING << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_PRESSURE_CHECKING =((L3_ST_PRESSURE_CHECKING << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_SEALING_CHECKING =((L3_ST_SEALING_CHECKING << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_STATION_CHECKING = ((L3_ST_STATION_CHECKING << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_STATION_CHECK_FINISH =((L3_ST_STATION_CHECK_FINISH << 16) | (L2_PRO_ST << 8) | L1_BUSY),
    PSSM_ST_DONE = ((L3_ST_DONE << 16) | (L2_PRO_ST << 8) | L1_BUSY),

    SM_ERR_WAIT = ((L2_ERR_WAIT << 8) | L1_ERROR),
    SM_ERR_RS_RV_MOVING_TO_INIT_POS = ((L3_ERR_RS_RV_MOVING_TO_INIT_POS << 16) | (L2_ERR_RS_RV_MOV_TO_INIT_POS_AGAIN << 8) | L1_ERROR),
    SM_ERR_RS_RELEASE_PRESSURE = ((L3_ERR_RS_RELEASE_PRESSURE << 16) | (L2_ERR_RS_STANDBY << 8) | L1_ERROR),
    SM_ERR_RS_SHUTDOWN_FAILED_HEATER = ((L3_ERR_RS_SHUTDOWN_FAILED_HEATER << 16) | (L2_ERR_RS_STANDBY << 8) | L1_ERROR),
    SM_ERR_RC_REPORT = ((L3_ERR_RC_REPORT << 16) | (L2_ERR_RC_REPORT << 8) | L1_ERROR)

} SchedulerStateMachine_t;
}
#endif // SCHEDULERSTATEMACHINESTATES_H
