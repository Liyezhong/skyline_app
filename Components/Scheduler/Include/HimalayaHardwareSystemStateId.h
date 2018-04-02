/****************************************************************************/
/*! \file HimalayaHardwareSystemStateId.h
 *
 *  \brief All State Ids used by Himalaya hardware system.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013_2_28
 *  $Author:    $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef HIMALAYAHARDWARESYSTEMSTATEID_H
#define HIMALAYAHARDWARESYSTEMSTATEID_H

const quint32 STATE_START_UP	= 520000001;
const quint32 STATE_SELF_TEST	= 520000002;
const quint32 STATE_PRE_HEATING	= 520000003;
const quint32 STATE_STANDBY	= 520000004;
const quint32 STATE_RELEASING_PRESSURE	= 520000006;
	
const quint32 STATE_PRE_TEST	= 520000200;
const quint32 STATE_PROTOCOL_FIXATION_SUCKING	= 520000201;
const quint32 STATE_PROTOCOL_FIXATION_DRAINING	= 520000202;
const quint32 STATE_PROTOCOL_FIXATION_AGATATION_DRAINING	= 520000203;
const quint32 STATE_PROTOCOL_FIXATION_AGATATION_VORPORA	= 520000204;
const quint32 STATE_PROTOCOL_FIXATION_AGATATION_RV_TOSEAL	= 520000205;
const quint32 STATE_PROTOCOL_FIXATION_AGATATION_RV_TOTUBE	= 520000206;
const quint32 STATE_PROTOCOL_FIXATION_STANDBY	= 520000207;
const quint32 STATE_PROTOCOL_FIXATION_LEVELSENSOR_SUCKING_HEATING	= 520000208;
const quint32 STATE_PROTOCOL_FIXATION_LEVELSENSOR_AGATITION_HEATING	= 520000209;
	
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_DILUTED_SUCKING	= 520000221;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_DILUTED_DRAINING	= 520000222;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_DILUTED_AGATATION_DRAINING	= 520000223;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_DILUTED_VOR_PORA	= 520000224;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_DILUTED_RV_TOSEAL	= 520000225;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_DILUTED_RV_TOTUBE	= 520000226;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_DILUTED_LEVELSENSOR_SUCKING_HEATING	= 520000227;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_DILUTED_LEVELSENSOR_AGATITION_HEATING	= 520000228;
	
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_UNDILUTED_SUCKING	= 520000241;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_UNDILUTED_DRAINING	= 520000242;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_UNDILUTED_AGATATION_DRAINING	= 520000243;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_UNDILUTED_VORPORA	= 520000244;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_UNDILUTED_RV_TOSEAL	= 520000245;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_UNDILUTED_RV_TOTUBE	= 520000246;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_UNDILUTED_LEVELSENSOR_SUCKING_HEATING	= 520000247;
const quint32 STATE_PROTOCOL_DEHYDRATION_ALCOHOL_UNDILUTED_LEVELSENSOR_AGATITION_HEATING	= 520000248;
	
const quint32 STATE_PROTOCOL_INTERMEDIUIM_XYLENE_SUCKING	= 520000261;
const quint32 STATE_PROTOCOL_INTERMEDIUIM_XYLENE_DRAINING	= 520000262;
const quint32 STATE_PROTOCOL_INTERMEDIUIM_XYLENE_AGATATION_DRAINING	= 520000263;
const quint32 STATE_PROTOCOL_INTERMEDIUIM_XYLENE_VORPORA	= 520000264;
const quint32 STATE_PROTOCOL_INTERMEDIUIM_XYLENE_RV_TOSEAL	= 520000265;
const quint32 STATE_PROTOCOL_INTERMEDIUIM_XYLENE_RV_TOTUBE	= 520000266;
const quint32 STATE_PROTOCOL_INTERMEDIUIM_XYLENE_LEVELSENSOR_SUCKING_HEATING	= 520000267;
const quint32 STATE_PROTOCOL_INTERMEDIUIM_XYLENE_LEVELSENSOR_AGATITION_HEATING	= 520000268;
	
const quint32 STATE_PROTOCOL_GETREADYFOR_PARAFFIN	= 520000281;
	
const quint32 STATE_PROTOCOL_INFILTRATION_PARAFFIN_SUCKING	= 520000291;
const quint32 STATE_PROTOCOL_INFILTRATION_PARAFFIN_DRAINING	= 520000292;
const quint32 STATE_PROTOCOL_INFILTRATION_PARAFFIN_VOR_PORA	= 520000293;
const quint32 STATE_PROTOCOL_INFILTRATION_PARAFFIN_RV_TO_SEAL	= 520000294;
const quint32 STATE_PROTOCOL_INFILTRATION_PARAFFIN_RV_TO_TUBE	= 520000295;
const quint32 STATE_PROTOCOL_INFILTRATION_PARAFFIN_STANDBY	= 520000296;
const quint32 STATE_PROTOCOL_INFILTRATION_PARAFFIN_LEVELSENSOR_SUCKING_HEATING	= 520000297;
	
const quint32 STATE_PROTOCOL_CLEANING_XYLENE_SUCKING	= 520000311;
const quint32 STATE_PROTOCOL_CLEANING_XYLENE_DRAINING	= 520000312;
const quint32 STATE_PROTOCOL_CLEANING_XYLENE_RV_TOSEAL	= 520000313;
const quint32 STATE_PROTOCOL_CLEANING_XYLENE_RV_TOTUBE	= 520000314;
const quint32 STATE_PROTOCOL_CLEANING_XYLENE_LEVELSENSOR_SUCKING_HEATING	= 520000315;
	
const quint32 STATE_PROTOCOL_CLEANING_ALCOHOL_SUCKING	= 520000331;
const quint32 STATE_PROTOCOL_CLEANING_ALCOHOL_DRAINING	= 520000332;
const quint32 STATE_PROTOCOL_CLEANING_ALCOHOL_RV_TOSEAL	= 520000333;
const quint32 STATE_PROTOCOL_CLEANING_ALCOHOL_RV_TOTUBE	= 520000334;
const quint32 STATE_PROTOCOL_CLEANING_ALCOHOL_LEVELSENSOR_SUCKING_HEATING	= 520000335;
	
const quint32 STATE_PROTOCOL_CLEANING_WATER_SUCKING	= 520000351;
const quint32 STATE_PROTOCOL_CLEANING_WATER_DRAINING	= 520000352;
const quint32 STATE_PROTOCOL_CLEANING_WATER_RV_TOSEAL	= 520000353;
const quint32 STATE_PROTOCOL_CLEANING_WATER_RV_TOTUBE	= 520000354;
const quint32 STATE_PROTOCOL_CLEANING_WATER_LEVELSENSOR_SUCKING_HEATING	= 520000355;

#endif // HIMALAYAHARDWARESYSTEMSTATEID_H