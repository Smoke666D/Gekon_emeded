/*
 * adc.c
 *
 *  Created on: Aug 31, 2020
 *      Author: igor.dymov
 */

//#define ARM_MATH_CM3

#include "adc.h"

typedef struct
{
  SENSOR_TYPE xSensType;
  uint16_t    uData;
  fix16_t     xData;
  uint8_t     Error;
} xAINStruct;

xAINStruct xDeviceAIN[3];


//static   uint8_t            uADCError = 0U;
static   EventGroupHandle_t xADCEvent;
static   StaticEventGroup_t xADCCreatedEventGroup;
volatile int16_t            ADC1_IN_Buffer[ADC_FRAME_SIZE*ADC1_CHANNELS] = { 0U };   //ADC1 input data buffer
volatile int16_t            ADC2_IN_Buffer[ADC_FRAME_SIZE*ADC2_CHANNELS] = { 0U };   //ADC2 input data buffer
volatile int16_t            ADC3_IN_Buffer[ADC_FRAME_SIZE*ADC3_CHANNELS] = { 0U };   //ADC3 input data buffer
static   uint8_t            ADC_VALID_DATA                               =  0;
static   float              MIN_PRESENT_FREQ  =20.0;
static   uint8_t            adc_count = 0;
static   fix16_t xNET_FREQ =0;
static   xADCGenDetectType uFreqPresent = NO_FREQ;
static   fix16_t xGEN_FREQ =0;
static   uint32_t ADC3Freq =10000;
static   uint32_t ADC2Freq =10000;
static   xADCRotatinType uNetFaseRotation = NO_ROTATION;
static   xADCRotatinType uGenFaseRotation = NO_ROTATION;
static   ELECTRO_SCHEME xNetWiring  =ELECTRO_SCHEME_STAR;
static   uint16_t uCosFiPeriod =0;
static   uint16_t uCosFiMax =0;
static   uint16_t F2uCosFiPeriod =0;
static   uint16_t F2uCosFiMax =0;
static   uint16_t F3uCosFiPeriod =0;
static   uint16_t F3uCosFiMax =0;
static   fix16_t  xTransCoof =0;
fix16_t  GENERATOR_DATA[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;






xADCValue  xDeviceADC[]=
    {
        { SEN_TYPE_NONE, 0,  0,  (void *)xADCConvert },     /*  VDD  */
        { SEN_TYPE_NONE, 0,  0,  (void *)xADCConvert },     /* CHARGE_VDD */
        { SEN_TYPE_NONE, 0,  0,  (void *)xADCConvertTemp }, /* TEMP  */
        { SEN_TYPE_NONE, 0,  0,  (void *)vADCSetAinState },    /* SOP  */
        { SEN_TYPE_NONE, 0,  0,  (void *)vADCSetAinState },    /* SCL  */
        { SEN_TYPE_NONE, 0,  0,  (void *)vADCSetAinState},     /* SFL  */
        { SEN_TYPE_NONE, 0,  0,  NULL},                        /* CSA  */
        { SEN_TYPE_NONE, 0,  0,  NULL},                        /* CSD  */
        { SEN_TYPE_NONE, 0,  0,  NULL},                       /* CAS  */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseV},      /* NET_F1 */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseV},      /* NET_F2 */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseV},      /* NET_F3 */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseV},      /* GEN_F1 */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseV},      /* GEN_F2 */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseV},      /* GEN_F3 */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseC},      /* GEN_F1_CUR */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseC},      /* GEN_F2_CUR */
        { SEN_TYPE_NONE, 0,  0,  (void *)xConvertFaseC},      /* GEN_F3_CUR */

    };



uint8_t  vADCGetADC3Data();
uint8_t  vADCGetADC12Data();
void     vADCInit(void);
uint8_t  vADCFindFreq(int16_t * data, uint16_t * count,uint8_t off, int16_t AMP);
int16_t  xADCMax( int16_t * source, uint8_t off, uint16_t size , uint16_t * delay, uint8_t cc );
uint16_t GetAverVDD(uint8_t channel,uint8_t size,uint8_t offset,int16_t * source);
void     vADCSetFreqDetectParam(int16_t AMP,uint8_t * del,uint8_t * fd);
void     vADC3FrInit(uint16_t freq);
void     vADC12FrInit(uint16_t freq);
void     vADCConvertToVDD(uint8_t AnalogSwitch);
void     vDecNetural(int16_t * data)  __attribute__((optimize("-O1")));
uint8_t   vADCGeneratorDataUpdate();
uint16_t  uADCRMS(int16_t * source, uint8_t off, uint16_t size, uint8_t cc );

/*
 *  Константы
 */

static const fix16_t  xLCurCoof       = F16 (( VRef /4095)/(RCSHUNT * OPTRANSCOOF) );  //Коофицент пересчета значений токовых АЦП в ток на шунтирующих ризисторах
static const fix16_t  MIN_CUR         = F16 (1.0);


/*
 * API функции
 *
 */
fix16_t  xADCConvert (void * data)
{
 xADCValue * p = (xADCValue *) data ;

   p->xDataConvert = fix16_mul( fix16_from_int( p->uDataSrc ), F16 (VDD_CF) );
   p->xDataConvert = fix16_sub( p->xDataConvert,F16(VT4) );

 return (p->xDataConvert);
}

fix16_t  xADCConvertTemp (void * data)
{
  xADCValue * p = (xADCValue *) data ;
   /* Преобразования строенного термодатчика */

    p->xDataConvert = fix16_sub( fix16_from_float( p->uDataSrc * 3.3 / 4095U ), F16( 0.76 ) );
    p->xDataConvert = fix16_div( p->xDataConvert, F16 (0.0025) );
    p->xDataConvert = fix16_add( p->xDataConvert, F16( 25 ) );

  return  (p->xDataConvert);
}

fix16_t  vADCSetAinState( void * data)
{
    xADCValue * p = (xADCValue *) data ;
    if ( ( xDeviceADC[DEV_CSA].uDataSrc - xDeviceADC[DEV_CAS].uDataSrc ) <= DELTA )
    {
          p->xDataConvert = F16( MAX_RESISTANCE );
          p->StateConfig = COMMON_SENS_ERROR | ( p->StateConfig & SEN_TYPE_MASK );
    }
    else
    {
          switch ( p->StateConfig & SEN_TYPE_MASK )
          {
             case SEN_TYPE_RESISTIVE:
             if ( ( ( xDeviceADC[DEV_CSD].uDataSrc - p->uDataSrc ) <= DELTA ) || ( p->uDataSrc < xDeviceADC[DEV_CAS].uDataSrc ) )
             {
                p->xDataConvert = 0U;
                p->StateConfig = CHANNEL_SENS_ERROR | ( p->StateConfig & SEN_TYPE_MASK );
             }
             else
             {
                 p->xDataConvert = fix16_from_int( ( ( p->uDataSrc - xDeviceADC[DEV_CAS].uDataSrc ) * R3 ) / ( xDeviceADC[DEV_CSD].uDataSrc - p->uDataSrc ) );
             }
             break;
           case SEN_TYPE_NORMAL_OPEN:
           case SEN_TYPE_NORMAL_CLOSE:
              p->xDataConvert  = ( p->uDataSrc < ( xDeviceADC[DEV_CSD].uDataSrc / 2U ) )? 0U : F16(MAX_RESISTANCE) ;
              break;
           case SEN_TYPE_CURRENT:
           default:
              p->xDataConvert  = 0U;
              break;
           }
    }

  return  (p->xDataConvert);
}

uint8_t vADCDCUpdate()
{
  if (xEventGroupGetBits(xADCEvent) & DC_UPDATE)
  {
    xEventGroupClearBits (xADCEvent, DC_UPDATE );
    xDeviceADC[DEV_VDD].pFunc(&xDeviceADC[DEV_VDD]);
    xDeviceADC[DEV_CHARGE_VDD ].pFunc(&xDeviceADC[DEV_CHARGE_VDD ]);
    xDeviceADC[DEV_TEMP ].pFunc( &xDeviceADC[DEV_TEMP ] );
    xDeviceADC[DEV_SOP].pFunc( &xDeviceADC[DEV_SOP] );
    xDeviceADC[DEV_SCT].pFunc( &xDeviceADC[DEV_SCT] );
    xDeviceADC[DEV_SFL].pFunc( &xDeviceADC[DEV_SFL] );
    return  ( 1U);
  }
  return ( 0U );
}


/*Функции работы с каналами постоянного тока*/
/*
 *  Функция возвращает тип канала урвоня топлива
 */
uint8_t xADCGetFLChType(void)
{
 return  (xDeviceADC[DEV_SFL].StateConfig & SEN_TYPE_MASK);
}
/*
 *  Функция возвращает тип канала давления масла
 */
uint8_t xADCGetxOPChType(void)
{
 return (xDeviceADC[DEV_SOP].StateConfig & SEN_TYPE_MASK);
}
/*
 *  Функция возвращает тип канала температуры охлаждающей жидкости масла
 */
uint8_t xADCGetxCTChType(void)
{
 return (xDeviceADC[DEV_SCT].StateConfig & SEN_TYPE_MASK);

}
/*
 *  Функция возращает наряжения АКБ.
 */
fix16_t xADCGetVDD()
{
  vADCDCUpdate();
  return  ( xDeviceADC[DEV_VDD].xDataConvert );
}
/*
 * Функция возвращает напряжение заряного генератора
 */
fix16_t xADCGetCAC()
{
  vADCDCUpdate();
  return  ( xDeviceADC[DEV_CHARGE_VDD ].xDataConvert);
}
fix16_t xADCGetSOP()
{
  vADCDCUpdate();
  return (xDeviceADC[DEV_SOP].xDataConvert );
}
fix16_t xADCGetSCT()
{
  vADCDCUpdate();
  return ( xDeviceADC[DEV_SCT].xDataConvert );
}
fix16_t xADCGetSFL()
{
  vADCDCUpdate();
  return ( xDeviceADC[DEV_SFL].xDataConvert );
}

uint8_t uADCGetDCChError()
{
  static uint8_t reg_buf;
  reg_buf = ( vADCDCUpdate() == 1) ? (xDeviceADC[DEV_SOP].StateConfig >>4) | ((xDeviceADC[DEV_SCT].StateConfig & 0x20)>>3) | ((xDeviceADC[DEV_SFL].StateConfig & 0x20)>>2) : reg_buf;
  return ( reg_buf ) ;

}




/***Функции работы с каналами переменного тогка ***/

fix16_t xConvertFaseV(void * data)
{
  xADCValue * p = (xADCValue *) data ;
  p->xDataConvert = fix16_mul( fix16_from_int(p->uDataSrc), (fix16_t) 21178);
  return (p->xDataConvert);
}

fix16_t xConvertFaseC(void * data)
{
  xADCValue * p = (xADCValue *) data ;
  p->xDataConvert = fix16_mul ( fix16_from_int ( p->uDataSrc ), fix16_mul ( xLCurCoof, xTransCoof ) );
  return (p->xDataConvert);
}

void uADCNETUpdate ()
{
  if (xEventGroupGetBits(xADCEvent) & NET_UPDATE)
  {
    xEventGroupClearBits (xADCEvent, NET_UPDATE );
    if ( xNET_FREQ != 0U )      /*Если  обраружена частота, то производим расчеты конечных значений  */
    {
      GENERATOR_DATA[ NET_FREQ ] = fix16_mul( xNET_FREQ, F16( 10U ) );
      xDeviceADC[NET_FASE_V_L1].pFunc( &xDeviceADC[NET_FASE_V_L1] );
      xDeviceADC[NET_FASE_V_L2].pFunc( &xDeviceADC[NET_FASE_V_L2] );
      xDeviceADC[NET_FASE_V_L3].pFunc( &xDeviceADC[NET_FASE_V_L3] );
      GENERATOR_DATA[NET_L1_LINE_V ] = ( xNetWiring == ELECTRO_SCHEME_STAR) ? fix16_mul ( xDeviceADC[NET_FASE_V_L1].xDataConvert, fix16_sqrt( F16 (3) ) ) :xDeviceADC[NET_FASE_V_L1].xDataConvert;
      GENERATOR_DATA[NET_L2_LINE_V ] = ( xNetWiring == ELECTRO_SCHEME_STAR) ? fix16_mul ( xDeviceADC[NET_FASE_V_L2].xDataConvert, fix16_sqrt( F16 (3) ) ) :xDeviceADC[NET_FASE_V_L2].xDataConvert;
      GENERATOR_DATA[NET_L3_LINE_V ] = ( xNetWiring == ELECTRO_SCHEME_STAR) ? fix16_mul ( xDeviceADC[NET_FASE_V_L3].xDataConvert, fix16_sqrt( F16 (3) ) ) :xDeviceADC[NET_FASE_V_L3].xDataConvert;
    }
    else
    {
      /* Если частота не обранружена, то обнуляем все значения. Этот блок вынесен из задачи ацп, что бы оптимизировать скорость ее выполнения */
      GENERATOR_DATA[ NET_FREQ ] = 0U;
      xDeviceADC[NET_FASE_V_L1].xDataConvert = 0U;
      xDeviceADC[NET_FASE_V_L2].xDataConvert = 0U;
      xDeviceADC[NET_FASE_V_L3].xDataConvert = 0U;
      GENERATOR_DATA[NET_L1_LINE_V ] = 0U;
      GENERATOR_DATA[NET_L2_LINE_V ] = 0U;
      GENERATOR_DATA[NET_L3_LINE_V ] = 0U;
      uNetFaseRotation = NO_ROTATION;
    }
  }
  return;
}

fix16_t xADCGetNETL1()
{
  uADCNETUpdate ();
  return ( xDeviceADC[NET_FASE_V_L1].xDataConvert);
}
fix16_t xADCGetNETL2()
{
  uADCNETUpdate ();
  return ( xDeviceADC[NET_FASE_V_L2].xDataConvert );
}
fix16_t xADCGetNETL3()
{
  uADCNETUpdate ();
  return ( xDeviceADC[NET_FASE_V_L3].xDataConvert );
}

fix16_t xADCGetNETL1Lin()
{
  uADCNETUpdate ();
  return   (  GENERATOR_DATA[NET_L1_LINE_V ] );
}
fix16_t xADCGetNETL2Lin()
{
  uADCNETUpdate ();
    return   (  GENERATOR_DATA[NET_L2_LINE_V ] );
}
fix16_t xADCGetNETL3Lin()
{
  uADCNETUpdate ();
  return   (  GENERATOR_DATA[NET_L3_LINE_V ] );
}

fix16_t xADCGetNETLFreq()
{
  uADCNETUpdate ();
  return ( GENERATOR_DATA[NET_FREQ ] );
}

xADCRotatinType xADCGetNetFaseRotation()
{
  static xADCRotatinType net_fase_buf;
  net_fase_buf = ( vADCDCUpdate() == 1) ? uNetFaseRotation : net_fase_buf;
  return (net_fase_buf);

}

ELECTRO_SCHEME xADCGetScheme(void)
{
  return (xNetWiring);

}

xADCRotatinType xADCGetGenFaseRotation()
{
  static xADCRotatinType gen_fase_buf;
  gen_fase_buf = ( vADCGeneratorDataUpdate() == 1 ) ? uGenFaseRotation : gen_fase_buf;
  return (gen_fase_buf);
}

fix16_t xADCGetGENLFreq()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_FREQ ] );
}

fix16_t xADCGetGENL1()
{
  vADCGeneratorDataUpdate();
  return ( xDeviceADC[GEN_FASE_V_L1].xDataConvert );
}

fix16_t xADCGetGENL2()
{
  vADCGeneratorDataUpdate();
  return ( xDeviceADC[GEN_FASE_V_L2].xDataConvert );
}

fix16_t xADCGetGENL3()
{
  vADCGeneratorDataUpdate();
  return ( xDeviceADC[GEN_FASE_V_L3].xDataConvert );
}

fix16_t xADCGetGENL1Lin()
{
  vADCGeneratorDataUpdate();
  return  ( GENERATOR_DATA[GEN_L1_LINE_V] );
}

fix16_t xADCGetGENL2Lin()
{
  vADCGeneratorDataUpdate();
  return  ( GENERATOR_DATA[GEN_L2_LINE_V] );
}

fix16_t xADCGetGENL3Lin()
{
  vADCGeneratorDataUpdate();
  return  ( GENERATOR_DATA[GEN_L3_LINE_V] );
}

fix16_t xADCGetGENAverV()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_AVER_V] );
}

fix16_t xADCGetGENL1Cur()
{
  vADCGeneratorDataUpdate();
  return ( xDeviceADC[GEN_FASE_C_L1].xDataConvert );
}
fix16_t xADCGetGENL2Cur()
{
  vADCGeneratorDataUpdate();
  return ( xDeviceADC[GEN_FASE_C_L2].xDataConvert ) ;
}
fix16_t xADCGetGENL3Cur()
{
  vADCGeneratorDataUpdate();
  return ( xDeviceADC[GEN_FASE_C_L3].xDataConvert );
}
fix16_t xADCGetGENL1CurLin()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L1_LINE_C ] );
}
fix16_t xADCGetGENL2CurLin()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L2_LINE_C ] ) ;
}
fix16_t xADCGetGENL3CurLin()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L3_LINE_C ] );
}


fix16_t xADCGetGENMaxC()
{
  vADCGeneratorDataUpdate();
  return (GENERATOR_DATA[GEN_MAX_C]);
}

fix16_t xADCGetCOSFi()
{
  vADCGeneratorDataUpdate();
  return  (( xNetWiring != ELECTRO_SCHEME_SINGLE_PHASE ) ? fix16_div(GENERATOR_DATA[GEN_AVER_COS], F16 ( 3 ) )  : GENERATOR_DATA[GEN_AVER_COS] ) ;
}


fix16_t xADCGetGENL1ActivePower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L1_ACTIVE_POWER] );
}

fix16_t xADCGetGENL2ActivePower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L2_ACTIVE_POWER] );
}

fix16_t xADCGetGENL3ActivePower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L3_ACTIVE_POWER] );
}

fix16_t xADCGetGENActivePower()
{
  return ( GENERATOR_DATA[GEN_ACTIVE_POWER] );
}


fix16_t xADCGetGENL1RealPower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L1_REAL_POWER] );
}

fix16_t xADCGetGENL2RealPower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L2_REAL_POWER] );
}

fix16_t xADCGetGENL3RealPower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L3_REAL_POWER] );
}

fix16_t xADCGetGENRealPower()
{

  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_REAL_POWER] );
}

fix16_t xADCGetGENL1ReactivePower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L1_REAC_POWER] );
}

fix16_t xADCGetGENL2ReactivePower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L2_REAC_POWER]  );
}

fix16_t xADCGetGENL3ReactivePower()
{
  vADCGeneratorDataUpdate();
  return ( GENERATOR_DATA[GEN_L3_REAC_POWER] );
}

fix16_t xADCGetGENReactivePower()
{

    vADCGeneratorDataUpdate() ;

    return ( GENERATOR_DATA[GEN_REACTIVE_POWER]);
}
/*
 *  Функция возвращает мговенную мощность переменного тока
 */

uint8_t uADCGetValidDataFlag()
{
  xEventGroupWaitBits(xADCEvent,DC_READY,pdTRUE,pdTRUE,5);

  return ADC_VALID_DATA;

}

void vADCGenFaseUpdate( uint8_t FaseV, uint8_t FaseC, uint8_t ACTIVEP, uint8_t REACP, uint8_t REALP, uint16_t uCosPer, uint16_t uCosMax, fix16_t * av_cos)
{
  fix16_t temp;
  fix16_t xCosFi =0;
  if (xDeviceADC[FaseC].xDataConvert  < MIN_CUR  )
   {
       xDeviceADC[FaseC].xDataConvert     = 0;
       GENERATOR_DATA[ACTIVEP]            = 0;
       GENERATOR_DATA[REACP  ]            = 0;
       GENERATOR_DATA[REALP]              = 0;
       GENERATOR_DATA[GEN_AVER_COS]       = 0;
   }
   else
   {
       //Расчзет косинуса Фм L1
       xCosFi                              = fix16_div( fix16_mul( fix16_pi, F16( 2U ) ), fix16_from_int ( uCosPer ) );  /* 2Pi/uCurPeriod   */
       xCosFi                              = fix16_mul( fix16_from_int( uCosMax ), xCosFi );
       xCosFi                              = fix16_cos( xCosFi );
       *av_cos                             = fix16_add( *av_cos , xCosFi );
       //Расяет синуса Фи
       temp                                = fix16_sub( F16( 1U ), fix16_sq( xCosFi ) );
       GENERATOR_DATA[REACP]               = fix16_sqrt( temp );
       temp                                = fix16_mul( xDeviceADC[FaseV].xDataConvert, xDeviceADC[FaseC].xDataConvert );
       GENERATOR_DATA[ACTIVEP]             = fix16_mul( temp,xCosFi );
       GENERATOR_DATA[ACTIVEP]             = fix16_div( GENERATOR_DATA[ACTIVEP],F16(1000));
       temp                                = fix16_mul( GENERATOR_DATA[REACP],temp);
       GENERATOR_DATA[REACP]               = fix16_div( temp, F16( 1000 ) );
       GENERATOR_DATA[REALP]               = fix16_add( fix16_sq (GENERATOR_DATA[REACP] ) , fix16_sq( GENERATOR_DATA[ACTIVEP] ) );
       GENERATOR_DATA[REALP]               = fix16_sqrt( GENERATOR_DATA [REALP] );

   }
}


/*
 * Функция расчета парамертов генератора
 */
uint8_t vADCGeneratorDataUpdate()
{
  fix16_t cos_temp = 0;
  if (xEventGroupGetBits(xADCEvent) & GEN_UPDATE)
  {
    xEventGroupClearBits (xADCEvent, GEN_UPDATE );
    if  (xGEN_FREQ != 0U )
    {
      GENERATOR_DATA[ GEN_FREQ ] = fix16_mul( xGEN_FREQ, F16( 10U ) );
      xDeviceADC[GEN_FASE_V_L1].pFunc( &xDeviceADC[GEN_FASE_V_L1] );
      xDeviceADC[GEN_FASE_V_L2].pFunc( &xDeviceADC[GEN_FASE_V_L2] );
      xDeviceADC[GEN_FASE_V_L3].pFunc( &xDeviceADC[GEN_FASE_V_L3] );
      GENERATOR_DATA[GEN_L1_LINE_V] = ( xNetWiring == ELECTRO_SCHEME_STAR ) ? fix16_mul( xDeviceADC[GEN_FASE_V_L1].xDataConvert, fix16_sqrt( F16 ( 3U ) ) ) : xDeviceADC[GEN_FASE_V_L1].xDataConvert;
      GENERATOR_DATA[GEN_L2_LINE_V] = ( xNetWiring == ELECTRO_SCHEME_STAR ) ? fix16_mul( xDeviceADC[GEN_FASE_V_L2].xDataConvert, fix16_sqrt( F16 ( 3U ) ) ) : xDeviceADC[GEN_FASE_V_L2].xDataConvert;
      GENERATOR_DATA[GEN_L3_LINE_V] = ( xNetWiring == ELECTRO_SCHEME_STAR ) ? fix16_mul( xDeviceADC[GEN_FASE_V_L3].xDataConvert, fix16_sqrt( F16 ( 3U ) ) ) : xDeviceADC[GEN_FASE_V_L3].xDataConvert;
      xDeviceADC[GEN_FASE_C_L1].pFunc( &xDeviceADC[GEN_FASE_C_L1] );
      xDeviceADC[GEN_FASE_C_L2].pFunc( &xDeviceADC[GEN_FASE_C_L2] );
      xDeviceADC[GEN_FASE_C_L3].pFunc( &xDeviceADC[GEN_FASE_C_L3] );
      GENERATOR_DATA[GEN_L1_LINE_C] = ( xNetWiring ==  ELECTRO_SCHEME_TRIANGLE ) ? fix16_mul( xDeviceADC[GEN_FASE_C_L1].xDataConvert, fix16_sqrt( F16 ( 3U ) ) ) : xDeviceADC[GEN_FASE_C_L1].xDataConvert;
      GENERATOR_DATA[GEN_L2_LINE_C] = ( xNetWiring ==  ELECTRO_SCHEME_TRIANGLE ) ? fix16_mul( xDeviceADC[GEN_FASE_C_L2].xDataConvert, fix16_sqrt( F16 ( 3U ) ) ) : xDeviceADC[GEN_FASE_C_L2].xDataConvert;
      GENERATOR_DATA[GEN_L3_LINE_C] = ( xNetWiring ==  ELECTRO_SCHEME_TRIANGLE ) ? fix16_mul( xDeviceADC[GEN_FASE_C_L3].xDataConvert, fix16_sqrt( F16 ( 3U ) ) ) : xDeviceADC[GEN_FASE_C_L3].xDataConvert;
      GENERATOR_DATA[GEN_AVER_V] = fix16_add(GENERATOR_DATA[GEN_L1_LINE_V],GENERATOR_DATA[GEN_L2_LINE_V]);
      GENERATOR_DATA[GEN_AVER_V] = fix16_add( GENERATOR_DATA[GEN_AVER_V],GENERATOR_DATA[GEN_L3_LINE_V]);
      GENERATOR_DATA[GEN_AVER_V] = ( xNetWiring !=  ELECTRO_SCHEME_SINGLE_PHASE ) ?  fix16_div(GENERATOR_DATA[GEN_AVER_V], F16 (3) ) : GENERATOR_DATA[GEN_AVER_V];
      GENERATOR_DATA[GEN_MAX_C] = xDeviceADC[GEN_FASE_C_L1].xDataConvert;
      GENERATOR_DATA[GEN_MAX_C] = (GENERATOR_DATA[GEN_MAX_C] < xDeviceADC[GEN_FASE_C_L2].xDataConvert) ? xDeviceADC[GEN_FASE_C_L2].xDataConvert : GENERATOR_DATA[GEN_MAX_C];
      GENERATOR_DATA[GEN_MAX_C] = (GENERATOR_DATA[GEN_MAX_C] < xDeviceADC[GEN_FASE_C_L3].xDataConvert) ? xDeviceADC[GEN_FASE_C_L3].xDataConvert : GENERATOR_DATA[GEN_MAX_C];
    }
    else
    {
      GENERATOR_DATA[GEN_FREQ ] = 0U;
      xDeviceADC[GEN_FASE_V_L1].xDataConvert = 0U;
      xDeviceADC[GEN_FASE_V_L2].xDataConvert = 0U;
      xDeviceADC[GEN_FASE_V_L3].xDataConvert = 0U;
      GENERATOR_DATA[GEN_L1_LINE_V ] = 0U;
      GENERATOR_DATA[GEN_L2_LINE_V ] = 0U;
      GENERATOR_DATA[GEN_L3_LINE_V ] = 0U;
      xDeviceADC[GEN_FASE_C_L1].xDataConvert = 0U;
      xDeviceADC[GEN_FASE_C_L2].xDataConvert = 0U;
      xDeviceADC[GEN_FASE_C_L3].xDataConvert = 0U;
      GENERATOR_DATA[GEN_L1_LINE_C] = 0U;
      GENERATOR_DATA[GEN_L2_LINE_C] = 0U;
      GENERATOR_DATA[GEN_L3_LINE_C] = 0U;
      uNetFaseRotation = NO_ROTATION;
    }

    vADCGenFaseUpdate( GEN_FASE_V_L1, GEN_FASE_C_L1, GEN_L1_ACTIVE_POWER, GEN_L1_REAC_POWER, GEN_L1_REAL_POWER, uCosFiPeriod, uCosFiMax,&cos_temp);
    vADCGenFaseUpdate( GEN_FASE_V_L2, GEN_FASE_C_L2, GEN_L2_ACTIVE_POWER, GEN_L2_REAC_POWER, GEN_L2_REAL_POWER, F2uCosFiPeriod, F2uCosFiMax,&cos_temp);
    vADCGenFaseUpdate( GEN_FASE_V_L3, GEN_FASE_C_L3, GEN_L3_ACTIVE_POWER, GEN_L3_REAC_POWER, GEN_L3_REAL_POWER, F3uCosFiPeriod, F3uCosFiMax,&cos_temp);
    GENERATOR_DATA[GEN_AVER_COS]   = cos_temp;

    GENERATOR_DATA[GEN_REACTIVE_POWER] = fix16_add( GENERATOR_DATA[GEN_L3_REAC_POWER], GENERATOR_DATA[GEN_L2_REAC_POWER] );
    GENERATOR_DATA[GEN_REACTIVE_POWER] = fix16_add( GENERATOR_DATA[GEN_REACTIVE_POWER], GENERATOR_DATA[GEN_L1_REAC_POWER] );
    GENERATOR_DATA[GEN_REAL_POWER]     = fix16_add( GENERATOR_DATA[GEN_L3_REAL_POWER], GENERATOR_DATA[GEN_L2_REAL_POWER] );
    GENERATOR_DATA[GEN_REAL_POWER]     = fix16_add( GENERATOR_DATA[GEN_REAL_POWER], GENERATOR_DATA[GEN_L1_REAL_POWER] );
    GENERATOR_DATA[GEN_ACTIVE_POWER] = fix16_add( GENERATOR_DATA[GEN_L3_ACTIVE_POWER], GENERATOR_DATA[GEN_L2_ACTIVE_POWER] );
    GENERATOR_DATA[GEN_ACTIVE_POWER] = fix16_add( GENERATOR_DATA[GEN_ACTIVE_POWER], GENERATOR_DATA[GEN_L1_ACTIVE_POWER] );

    return ( 1U );
  }
  return ( 0U );
}




/*
 * Сервисная функция для перевода значений АЦП в напряжения
 */

void vADCConvertToVDD ( uint8_t AnalogSwitch )
{
  xEventGroupClearBits( xADCEvent, DC_UPDATE );
  switch ( AnalogSwitch )
  {
    case 1U:
      /* Получаем средние значения из буфера АЦП */
      xDeviceADC[DEV_VDD] .uDataSrc  = GetAverVDD( 4U, DC_SIZE, 9, (int16_t *)&ADC3_IN_Buffer );
      xDeviceADC[DEV_TEMP].uDataSrc  = GetAverVDD( 3U, DC_SIZE, 4, (int16_t *)&ADC1_IN_Buffer );
      xDeviceADC[DEV_CSA].uDataSrc   = GetAverVDD( 5U, DC_SIZE, 9, (int16_t *)&ADC3_IN_Buffer );
      xDeviceADC[DEV_CAS].uDataSrc   = GetAverVDD( 6U, DC_SIZE, 9, (int16_t *)&ADC3_IN_Buffer );
      xDeviceADC[DEV_SCT].uDataSrc   = GetAverVDD( 7U, DC_SIZE, 9, (int16_t *)&ADC3_IN_Buffer );
      /* Расчет значений на аналогвых входах постянного тока, расчет производится здесть, поскольку для него нужны мгновенные заначения переменных внутренних напряжений */

      if (adc_count==0)
      {
        adc_count=1;
      }
      else
      {
        ADC_VALID_DATA =1;
      }
      //Переключаем обратно аналоговый коммутатор
      HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_RESET );
      break;
    case 0U:
      /* Получаем усреденные данные из буфером DMA АЦП */
      xDeviceADC[DEV_CSD].uDataSrc         = GetAverVDD( 5U, DC_SIZE, 9, (int16_t *)&ADC3_IN_Buffer );
      xDeviceADC[DEV_SFL].uDataSrc         = GetAverVDD( 6U, DC_SIZE, 9, (int16_t *)&ADC3_IN_Buffer );
      xDeviceADC[DEV_SOP].uDataSrc         = GetAverVDD( 7U, DC_SIZE, 9, (int16_t *)&ADC3_IN_Buffer );
      xDeviceADC[DEV_CHARGE_VDD].uDataSrc  = GetAverVDD( 8U, DC_SIZE, 9, (int16_t *)&ADC3_IN_Buffer );
      /* Переключаем аналоговый комутатор и ждем пока напряжения за комутатором стабилизируются */
      HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_SET );
      osDelay(10);
      break;
    default:
      break;
  }
  xEventGroupSetBits( xADCEvent, DC_UPDATE );
  return;
}



void  vADC3FrInit(uint16_t freq)
{
  htim3.Init.Period = 60000000U / ( freq  );
  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_Base_Start_IT( &htim3 );
  return;
}

void  vADC12FrInit(uint16_t freq)
{

    htim8.Init.Period = 120000000U/ ( freq );
    HAL_TIM_Base_Init(&htim8);
    htim2.Init.Period = 60000000U / ( freq );
    HAL_TIM_Base_Init(&htim2);
    HAL_TIM_Base_Start_IT( &htim2 );
    HAL_TIM_Base_Start_IT( &htim8 );
    return;
}


/*
 *
 *
 */
void vADC3DCInit(xADCFSMType xADCType)
{

  HAL_ADC_DeInit(&hadc3);
  HAL_ADC_DeInit(&hadc1);
  hadc3.Init.ScanConvMode = ENABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.DMAContinuousRequests = ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (xADCType == DC)
      hadc3.Init.NbrOfConversion = 9;
  else
     hadc3.Init.NbrOfConversion = 4;

  HAL_ADC_Init(&hadc3);


  if (xADCType == DC)
        hadc1.Init.NbrOfConversion = 4;
    else
       hadc1.Init.NbrOfConversion = 3;

    HAL_ADC_Init(&hadc1);

}





void vADC_Ready ( uint8_t adc_number )
{
  static portBASE_TYPE xHigherPriorityTaskWoken;

  xHigherPriorityTaskWoken = pdFALSE;
  switch ( adc_number )
  {
    case ADC3_READY:
      __HAL_TIM_DISABLE(&htim3);
      xEventGroupSetBitsFromISR( xADCEvent, ADC3_READY, &xHigherPriorityTaskWoken );
      break;
    case ADC2_READY:
      xEventGroupSetBitsFromISR( xADCEvent, ADC2_READY, &xHigherPriorityTaskWoken );
      break;
    case ADC1_READY:
      xEventGroupSetBitsFromISR( xADCEvent, ADC1_READY, &xHigherPriorityTaskWoken );
      break;
    default:
      break;
  }
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  return;
}


static void ADC_DMAConv(DMA_HandleTypeDef *hdma)
{
  /* Retrieve ADC handle corresponding to current DMA handle */
  ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Update state machine on conversion status if not in error state */
  if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL | HAL_ADC_STATE_ERROR_DMA))
  {
    /* Update ADC state machine */
    SET_BIT(hadc->State, HAL_ADC_STATE_REG_EOC);

    /* Determine whether any further conversion upcoming on group regular   */
    /* by external trigger, continuous mode or scan sequence on going.      */
    /* Note: On STM32F2, there is no independent flag of end of sequence.   */
    /*       The test of scan sequence on going is done either with scan    */
    /*       sequence disabled or with end of conversion flag set to        */
    /*       of end of sequence.                                            */
    if(ADC_IS_SOFTWARE_START_REGULAR(hadc)                   &&
       (hadc->Init.ContinuousConvMode == DISABLE)            &&
       (HAL_IS_BIT_CLR(hadc->Instance->SQR1, ADC_SQR1_L) ||
        HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)  )   )
    {
      /* Disable ADC end of single conversion interrupt on group regular */
      /* Note: Overrun interrupt was enabled with EOC interrupt in          */
      /* HAL_ADC_Start_IT(), but is not disabled here because can be used   */
      /* by overrun IRQ process below.                                      */
      __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);

      /* Set ADC state */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

      if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_INJ_BUSY))
      {
        SET_BIT(hadc->State, HAL_ADC_STATE_READY);
      }
    }

    /* Conversion complete callback */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
    hadc->ConvCpltCallback(hadc);
#else
    HAL_ADC_ConvCpltCallback(hadc);
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
  }
  else /* DMA and-or internal error occurred */
  {
    if ((hadc->State & HAL_ADC_STATE_ERROR_INTERNAL) != 0UL)
    {
      /* Call HAL ADC Error Callback function */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
      hadc->ErrorCallback(hadc);
#else
      HAL_ADC_ErrorCallback(hadc);
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
    }
  else
  {
      /* Call DMA error callback */
      hadc->DMA_Handle->XferErrorCallback(hdma);
    }
  }
}

static void ADC_DMAErro(DMA_HandleTypeDef *hdma)
{
  ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  hadc->State= HAL_ADC_STATE_ERROR_DMA;
  /* Set ADC error code to DMA error */
  hadc->ErrorCode |= HAL_ADC_ERROR_DMA;

}





void StartADCDMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
{

   __HAL_LOCK(hadc);
   __HAL_ADC_DISABLE(hadc);
   __HAL_UNLOCK(hadc);
    if (hadc->Instance == ADC3)
    {
        __HAL_TIM_DISABLE(&htim3);
    }
    if (hadc->Instance == ADC2)
    {
        __HAL_TIM_DISABLE(&htim8);
    }
    if (hadc->Instance == ADC1)
      {
          __HAL_TIM_DISABLE(&htim2);
      }
   /* Start the DMA channel */
   HAL_DMA_Start_IT(hadc->DMA_Handle, (uint32_t)&hadc->Instance->DR, (uint32_t)pData, Length);

   /* Enable the Peripheral */
   __HAL_ADC_ENABLE(hadc);

   osDelay(3);




    /* Start conversion if ADC is effectively enabled */
    if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
    {
      /* Set ADC state                                                          */
      /* - Clear state bitfield related to regular group conversion results     */
      /* - Set state bitfield related to regular group operation                */
      ADC_STATE_CLR_SET(hadc->State,
                        HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR,
                        HAL_ADC_STATE_REG_BUSY);

      /* If conversions on group regular are also triggering group injected,    */
      /* update ADC state.                                                      */
      if (READ_BIT(hadc->Instance->CR1, ADC_CR1_JAUTO) != RESET)
      {
        ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_EOC, HAL_ADC_STATE_INJ_BUSY);
      }

      /* State machine update: Check if an injected conversion is ongoing */
      if (HAL_IS_BIT_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY))
      {
        /* Reset ADC error code fields related to conversions on group regular */
        CLEAR_BIT(hadc->ErrorCode, (HAL_ADC_ERROR_OVR | HAL_ADC_ERROR_DMA));
      }
      else
      {
        /* Reset ADC all error code fields */
        ADC_CLEAR_ERRORCODE(hadc);
      }

      /* Process unlocked */
      /* Unlock before starting ADC conversions: in case of potential           */
      /* interruption, to let the process to ADC IRQ Handler.                   */
      __HAL_UNLOCK(hadc);


      /* Clear regular group conversion flag and overrun flag */
      /* (To ensure of no unknown state from potential previous ADC operations) */
      __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC | ADC_FLAG_OVR);

      /* Enable ADC overrun interrupt */
      __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);

      /* Enable ADC DMA mode */
      hadc->Instance->CR2 |= ADC_CR2_DMA;

        /* if instance of handle correspond to ADC1 and  no external trigger present enable software conversion of regular channels */
        if((hadc->Instance == ADC1) && ((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET))
        {
          /* Enable the selected ADC software conversion for regular group */
            hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
        }
    }

    /* Return function status */
    return;

}
void vADCConfigInit(void)
{
  uint16_t bitmask;
  eConfigAttributes atrib;
  uint16_t tempdata=0;
  for (uint8_t i=0;i<20;i++)
       {
         osDelay( 100U );
         if  ( eDATAAPIconfigValue(DATA_API_CMD_READ, COOLANT_TEMP_SETUP_ADR ,&bitmask) == DATA_API_STAT_OK)
         {
            eDATAAPIconfigAtrib (DATA_API_CMD_READ, COOLANT_TEMP_SETUP_ADR, &atrib );
            xDeviceADC[DEV_SCT].StateConfig = (bitmask  & atrib.bitMap[COOLANT_TEMP_SENSOR_TYPE_ADR].mask) >>atrib.bitMap[COOLANT_TEMP_SENSOR_TYPE_ADR].shift;

            eDATAAPIconfigValue(DATA_API_CMD_READ, OIL_PRESSURE_SETUP_ADR ,&bitmask);
            eDATAAPIconfigAtrib (DATA_API_CMD_READ, OIL_PRESSURE_SETUP_ADR, &atrib );
            xDeviceADC[DEV_SOP].StateConfig = (bitmask  & atrib.bitMap[OIL_PRESSURE_SENSOR_TYPE_ADR].mask) >>atrib.bitMap[OIL_PRESSURE_SENSOR_TYPE_ADR].shift;

            eDATAAPIconfigValue(DATA_API_CMD_READ,FUEL_LEVEL_SETUP_ADR  ,&bitmask);
            eDATAAPIconfigAtrib (DATA_API_CMD_READ, FUEL_LEVEL_SETUP_ADR , &atrib );
            xDeviceADC[DEV_SFL].StateConfig = (bitmask  & atrib.bitMap[FUEL_LEVEL_SENSOR_TYPE_ADR].mask) >>atrib.bitMap[FUEL_LEVEL_SENSOR_TYPE_ADR].shift;

            //считываем коофицент трансформамции
            eDATAAPIconfigValue(DATA_API_CMD_READ,GEN_CURRENT_TRASFORM_RATIO_LEVEL_ADR , (uint16_t*)&tempdata);
            xTransCoof = fix16_from_float(tempdata);

            //Считываем уставку типа включения генератора
            eDATAAPIconfigValue(DATA_API_CMD_READ, GEN_SETUP_ADR ,&bitmask);
            eDATAAPIconfigAtrib (DATA_API_CMD_READ, GEN_SETUP_ADR, &atrib );
            xNetWiring  = (bitmask  & atrib.bitMap[GEN_AC_SYS_ADR ].mask) >>atrib.bitMap[GEN_AC_SYS_ADR ].shift;


            //Считываем уставку минимальной частоты отключения стартера
            eDATAAPIconfigValue(DATA_API_CMD_READ,  STARTER_STOP_GEN_FREQ_LEVEL_ADR  ,(uint16_t*)&tempdata);
            eDATAAPIconfigAtrib (DATA_API_CMD_READ, STARTER_STOP_GEN_FREQ_LEVEL_ADR, &atrib );
            MIN_PRESENT_FREQ =fxParToFloat(tempdata, atrib.scale);
           break;
         }


       }

}

void vADCInit(void)
{

      HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_SET );
      HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_RESET );
      HAL_GPIO_WritePin( DIN_OFFSET_GPIO_Port,DIN_OFFSET_Pin, GPIO_PIN_SET );
      vADC3DCInit(DC);
      vADC3FrInit(ADC3Freq);
      vADC12FrInit(ADC2Freq);
      hadc3.DMA_Handle->XferCpltCallback =ADC_DMAConv;
      hadc2.DMA_Handle->XferCpltCallback =ADC_DMAConv;
      hadc1.DMA_Handle->XferCpltCallback =ADC_DMAConv;
      hadc3.DMA_Handle->XferHalfCpltCallback = NULL;
      hadc2.DMA_Handle->XferHalfCpltCallback = NULL;
      hadc1.DMA_Handle->XferHalfCpltCallback = NULL;
      hadc3.DMA_Handle->XferErrorCallback = ADC_DMAErro;
      hadc2.DMA_Handle->XferErrorCallback = ADC_DMAErro;
      hadc1.DMA_Handle->XferErrorCallback = ADC_DMAErro;
      ADC_VALID_DATA =0;
      vADCConfigInit();

}



void StartADCTask(void *argument)
{
   uint8_t uADC3FreqChange = 0U,
           uADC2FreqChange = 0U;
  //Создаем флаг готовности АПЦ
   xADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );
   //Иницилиазация АЦП
   vADCInit();

   for(;;)
   {
     if ( ( xEventGroupGetBits( xDATAAPIgetEventGroup() ) &   DATA_API_FLAG_ADC_TASK_CONFIG_REINIT ) > 0U )
     {
         vADCConfigInit();
         xEventGroupClearBits( xDATAAPIgetEventGroup(),   DATA_API_FLAG_ADC_TASK_CONFIG_REINIT );
     }
    osDelay( 100U );
    vADC3DCInit( DC );
    StartADCDMA( &hadc3, ( uint32_t* )&ADC3_IN_Buffer, ( DC_SIZE * 9U ) );         /* Запускаем преобразвоание АЦП */
    __HAL_TIM_ENABLE(&htim3);

    xEventGroupWaitBits( xADCEvent, ADC3_READY, pdTRUE, pdTRUE, portMAX_DELAY);   /* Ожидаем флага готовонсти о завершении преобразования */
    vADCConvertToVDD(0);
    //Запускаем новоей преобразование
    StartADCDMA(&hadc3,(uint32_t*)&ADC3_IN_Buffer,DC_SIZE*9);
    StartADCDMA(&hadc1,(uint32_t*)&ADC1_IN_Buffer,DC_SIZE*4);
    __HAL_TIM_ENABLE(&htim3);
    __HAL_TIM_ENABLE(&htim2);
    xEventGroupWaitBits(xADCEvent, ADC3_READY | ADC1_READY , pdTRUE, pdTRUE, portMAX_DELAY );
    vADCConvertToVDD(1);
    //Переключем АЦП в режим имзерения AC каналов
    vADC3DCInit(AC);
    osDelay(1);
    /*  Запускаем преобразвоания по всем каналам АЦП  */
    StartADCDMA( &hadc2, (uint32_t*)&ADC2_IN_Buffer, ADC_FRAME_SIZE*ADC2_CHANNELS );
    StartADCDMA( &hadc1, (uint32_t*)&ADC1_IN_Buffer, ADC_FRAME_SIZE*ADC1_CHANNELS );
    StartADCDMA( &hadc3, (uint32_t*)&ADC3_IN_Buffer, ADC_FRAME_SIZE*ADC3_CHANNELS );
    __HAL_TIM_ENABLE(&htim3);
    __HAL_TIM_ENABLE(&htim2);
    __HAL_TIM_ENABLE(&htim8);
    /* Задача блокируется до окончания преобразований по всем каналам */
    xEventGroupWaitBits( xADCEvent, ADC3_READY | ADC2_READY | ADC1_READY, pdTRUE, pdTRUE, portMAX_DELAY );
    /*Обработка значений АЦП3. */
     switch ( vADCGetADC3Data( ) )
     {
       /*Если частота дискетезации высокая, то поступательно снижаем ее, флаг uADC3FreqChange использутся
        * что бы изменять частоту с задержкой, на случай случайного искажения данных в выборке */
       case HIGH_FREQ:
         if ( uADC3FreqChange > 0 )
         {
           ADC3Freq = ADC3Freq - ADC3Freq/10U;
           if ( ADC3Freq < LOW_ADC_FREQ )  ADC3Freq = LOW_ADC_FREQ;
           vADC3FrInit(ADC3Freq);
           uADC3FreqChange = 0U;
         }
         uADC3FreqChange++;
         break;
       case LOW_FREQ:
         if (uADC3FreqChange > 0 )
         {
           ADC3Freq = ADC3Freq + ADC3Freq/4;
           if ( ADC3Freq > HIGH_ADC_FREQ )  ADC3Freq = MIDLE_ADC_FREQ;
           vADC3FrInit( ADC3Freq );
           uADC3FreqChange = 0U;
         }
         uADC3FreqChange++;
         break;
       default:
         break;
     }
     /*Обработка значений АЦП3 */
    switch ( vADCGetADC12Data( ) )
    {
      /*Если частота дискетезации высокая, то поступательно снижаем ее, флаг uADC2FreqChange использутся
              * что бы изменять частоту с задержкой, на случай случайного искажения данных в выборке */
      case HIGH_FREQ:
        if (uADC2FreqChange > 0U )
        {
           ADC2Freq = ADC2Freq - ADC2Freq/10U;
          if ( ADC2Freq < LOW_ADC_FREQ )
          {
            ADC2Freq = LOW_ADC_FREQ;
          }
          vADC12FrInit( ADC2Freq );
          uADC2FreqChange = 0U;
        }
        uADC2FreqChange++;
        break;
      case LOW_FREQ:
        if (uADC2FreqChange > 0U )
        {
           ADC2Freq = ADC2Freq + ADC2Freq/4U;
           if ( ADC2Freq > HIGH_ADC_FREQ )
           {
             ADC2Freq =  MIDLE_ADC_FREQ;
           }
           vADC12FrInit( ADC2Freq );
           uADC2FreqChange =0U;
         }
         uADC2FreqChange++;
         break;
      default:
        break;
    }
   }
   return;
}



void vCurConvert(int16_t * data, int16_t * ref, int16_t off)
{
 for (int16_t i = 0;i<ADC_FRAME_SIZE*3;i=i+3)
 {
   data[i]   =data[i] - ref[i/3*4+3];
   data[i+1] =data[i+1]-ref[i/3*4+3];
   data[i+2] =data[i+2]-ref[i/3*4+3];
 }
  return;
}


void vDecNetural(int16_t * data)
{
 for (int16_t i = 0;i<ADC_FRAME_SIZE<<2;i=i+4)
 {
   data[i]   = data[i]   - data[i+3];
   data[i+1] = data[i+1] - data[i+3];
   data[i+2] = data[i+2] - data[i+3];
 }
  return;
}


#define MAX_ZERO_POINT 20
#define FASE_DETECT_HISTERESIS  30





uint8_t vADCFindFreq(int16_t * data, uint16_t * count, uint8_t off, int16_t AMP)
{
  uint8_t AMP_DELTA = 15,
                 FD = 15,
                 F1 = 0,
                 F2 = 0,
                 CNT = 0,
                 index = 0,
                 res =ADC_OK;
  uint16_t tt=0;
  uint16_t PER[MAX_ZERO_POINT];

  vADCSetFreqDetectParam(AMP,&AMP_DELTA,&FD);

  for (uint16_t i=FD;i<ADC_FRAME_SIZE-FD;i++)
  {
    //Если значение попадет в корридор окло нуля
    if ((data[i*4+off] > -AMP_DELTA) && (data[i*4+off] < AMP_DELTA))
    {
        //то прверяем текущую фазу
        if ((data[i*4+off] > data[i*4-FD*4+off]) || (data[i*4+off] < data[i*4+off+FD*4]))
            F2  = 1U;
        else
            F2 = 0U;
        if (CNT > 0)
        {
          if (((F2==1) && (F1 == 1)) || ((F2==0) &&(F1==0)))
          {
            index = 5;
            break;
          }
        }
        F1 = F2;
        PER[index++] =i;
        i = i+ FD*2;
        CNT++;
        if (index> MAX_ZERO_POINT) break;
    }
  }
  tt =0;
  if (index>1)
  {
    for (uint8_t i =1;i<index;i++)
    {
      tt =tt+ + PER[i]-PER[i-1];
    }
    tt = (tt/(index-1))*2;
  }
  *count = tt;
  if (index < 3) res = HIGH_FREQ;
  if (index > 4) res = LOW_FREQ;
  return (res);
}



uint8_t vADCGetADC3Data()
{
  uint8_t result=ADC_ERROR;
  uint16_t uCurPeriod = ADC_FRAME_SIZE-1;
  int16_t iMax =0;
  uint16_t DF1,DF2,DF3;

  vDecNetural( (int16_t *)&ADC3_IN_Buffer );
  iMax = xADCMax( (int16_t *)&ADC3_IN_Buffer, 2, uCurPeriod, &DF1,4U );
  xEventGroupClearBits (xADCEvent, NET_UPDATE );
  if( iMax >= MIN_AMP_VALUE )
  {
        result =  vADCFindFreq( (int16_t *)&ADC3_IN_Buffer, &uCurPeriod, 2U ,iMax );
        if (result==ADC_OK)
        {
          iMax =xADCMax( (int16_t *)  &ADC3_IN_Buffer, 2, uCurPeriod, &DF1, 4U );
          xNET_FREQ = fix16_div( fix16_from_int(ADC3Freq/10U), fix16_from_int(uCurPeriod) );
        }
        else
        {
          xEventGroupSetBits( xADCEvent, NET_READY );
          return (result);
        }
        xDeviceADC[NET_FASE_V_L1].uDataSrc = uADCRMS((int16_t *)&ADC3_IN_Buffer, 2U, uCurPeriod, 4U );
 }
 else
 {
   xNET_FREQ = 0;
   xEventGroupSetBits( xADCEvent, NET_UPDATE );
   return (LOW_AMP);
 }
  /* Проверям что максимум амплитуды выше заданого и если да, расчитываем RMS */
  iMax =xADCMax( (int16_t *) &ADC3_IN_Buffer, 1, uCurPeriod, &DF2, 4 );
  xDeviceADC[NET_FASE_V_L2].uDataSrc = ( iMax  >= MIN_AMP_VALUE ) ?  uADCRMS((int16_t *)&ADC3_IN_Buffer, 1, uCurPeriod,4) : 0U ;

  /* Проверям что максимум амплитуды выше заданого и если да, расчитываем RMS */
  iMax =xADCMax( (int16_t *) &ADC3_IN_Buffer, 0, uCurPeriod, &DF3, 4 );
  xDeviceADC[NET_FASE_V_L3].uDataSrc = ( iMax  >= MIN_AMP_VALUE ) ?  uADCRMS((int16_t *)&ADC3_IN_Buffer, 0, uCurPeriod,4) : 0U ;

 /*Если на фазах есть наряжения*/
 if ( ( xDeviceADC[NET_FASE_V_L3].uDataSrc != 0U ) && ( xDeviceADC[NET_FASE_V_L2].uDataSrc != 0U ) )
 {
   if (    ( (uCurPeriod - DF1 ) > FASE_DETECT_HISTERESIS ) || ((uCurPeriod - DF2 ) > FASE_DETECT_HISTERESIS )
    || ((uCurPeriod - DF3 ) > FASE_DETECT_HISTERESIS ) )
     //Проверяем что максисмумы амплитуды каждой фазы были зафиксировны
     //не в конце перидоа. Это может провести к ошибки, поэтому пропускаем это измерение и ждем удачного.
   {
     if (( abs (DF1 - DF2 )> FASE_DETECT_HISTERESIS ) &&  ( abs(DF2 - DF3 )> FASE_DETECT_HISTERESIS ))
     {
       uNetFaseRotation = (DF2 < DF3) ? B_C_ROTATION : C_B_ROTATION;
     }
   }
 }
 else
 {
   uNetFaseRotation = NO_ROTATION;
 }
 xEventGroupSetBits( xADCEvent, NET_UPDATE );
 return (ADC_OK);
}

#define COS_DATA_COUNT  10
static int16_t CosBuffer[COS_DATA_COUNT][2];
static int16_t F2CosBuffer[COS_DATA_COUNT][2];
static int16_t F3CosBuffer[COS_DATA_COUNT][2];
static uint8_t           uCosCount    = 0U,
                         F2uCosCount  = 0U,
                         F3uCosCount  = 0U;


uint8_t vADCGetADC12Data()
{
  uint8_t result=ADC_ERROR;
  uint8_t t;
  uint16_t uCurPeriod = ADC_FRAME_SIZE-1;
  int16_t iMax =0;
  uint16_t DF1,DF2,DF3;

  vDecNetural((int16_t *)&ADC2_IN_Buffer);  //Вычитаем из фазы, значение на линии нейтрали
  iMax=xADCMax((int16_t *) &ADC2_IN_Buffer, 2, uCurPeriod, &DF1 ,4); //Вычисляем максимальное амплитудное значение
  xEventGroupClearBits( xADCEvent, GEN_UPDATE );
  if( iMax  >= MIN_AMP_VALUE ) //Если маскимальное омплитудно значение меньше нижнего предела, то фиксируем нулевое напряжение на входе
  {
      result =  vADCFindFreq((int16_t *)&ADC2_IN_Buffer, &uCurPeriod,2,iMax);  //Вычисляем частоту
      if (result==ADC_OK)
      {
        xGEN_FREQ =  fix16_div(fix16_from_int(ADC2Freq/10),fix16_from_int(uCurPeriod));
        iMax=xADCMax((int16_t *) &ADC2_IN_Buffer, 2, uCurPeriod, &DF1 ,4);
        xDeviceADC[GEN_FASE_V_L1].uDataSrc = uADCRMS((int16_t *)&ADC2_IN_Buffer, 2, uCurPeriod,4 );
        uFreqPresent =  FREQ_DETECTED;
      }
      else
      {
        if ( uCurPeriod !=0 )
        {
          if  ( (ADC2Freq / uCurPeriod ) >  MIN_PRESENT_FREQ )
          {
              uFreqPresent =   FREQ_DETECTED;
          }
        }
        uCurPeriod = ADC_FRAME_SIZE;
      }
  }
  else
  {
    uFreqPresent = NO_FREQ;
    xGEN_FREQ   = 0;
    xEventGroupSetBits( xADCEvent, GEN_UPDATE);
    return ( LOW_AMP );
  }
   if (result==ADC_OK)
   {
       //Проверям есть ли на канале напряжение.
       iMax=xADCMax((int16_t *) &ADC2_IN_Buffer, 1, uCurPeriod, &DF2,4 );
       xDeviceADC[GEN_FASE_V_L2].uDataSrc  = ( iMax >= MIN_AMP_VALUE ) ? uADCRMS( (int16_t *)&ADC2_IN_Buffer, 1, uCurPeriod,4 ) : 0U;
       iMax =xADCMax((int16_t *) &ADC2_IN_Buffer, 0, uCurPeriod, &DF3,4 );  //Проверям есть ли на канале напряжение.
       xDeviceADC[GEN_FASE_V_L3].uDataSrc  = ( iMax >= MIN_AMP_VALUE ) ? uADCRMS( (int16_t *)&ADC2_IN_Buffer, 0, uCurPeriod,4 ) : 0U;
       //Проверяем флаг чередования фаз.Если он сброшен, значит детектировали нулевое напряжение на 1-й фазе и
       //нужно проверить чередование фаз полсе востановления напряжния
       if ( ( xDeviceADC[GEN_FASE_V_L2].uDataSrc  != 0 ) && ( xDeviceADC[GEN_FASE_V_L3].uDataSrc  != 0) )
       {
         if (    ( (uCurPeriod - DF1 ) > FASE_DETECT_HISTERESIS ) || ((uCurPeriod - DF2 ) > FASE_DETECT_HISTERESIS )
             || ((uCurPeriod - DF3 ) > FASE_DETECT_HISTERESIS ) )
           //Проверяем что максисмумы амплитуды каждой фазы были зафиксировны
           //не в конце перидоа. Это может провести к ошибки, поэтому пропускаем это измерение и ждем удачного.
         {
           if (( abs (DF1 - DF2 )> FASE_DETECT_HISTERESIS ) &&  ( abs(DF2 - DF3 )> FASE_DETECT_HISTERESIS ))
           {
             uGenFaseRotation = ( DF2 < DF3 ) ? B_C_ROTATION : C_B_ROTATION;
           }
         }
       }
       else
       {
         uGenFaseRotation = NO_ROTATION;
       }
   }
  // Расчет значения тока и косинуса фи

   vCurConvert((int16_t *)&ADC1_IN_Buffer,(int16_t *)&ADC2_IN_Buffer,4);   //Вычитаем из значений тока значения нейтрали напряжения генератора
   if (result==ADC_OK)     //Если мы определили значение частоты напряжения генератора, то считаем ток через TrueRMS
   {
     //Вычисяем значение TrueRMS для токовых каналов
     xDeviceADC[GEN_FASE_C_L1].uDataSrc =   uADCRMS((int16_t *)&ADC1_IN_Buffer, 0, uCurPeriod,3 );
     xDeviceADC[GEN_FASE_C_L1].uDataSrc =   uADCRMS((int16_t *)&ADC1_IN_Buffer, 1, uCurPeriod,3 );
     xDeviceADC[GEN_FASE_C_L1].uDataSrc =   uADCRMS((int16_t *)&ADC1_IN_Buffer, 2, uCurPeriod,3 );
     /*
      * расчет косинуса фи.
      */
     xADCMax((int16_t *) &ADC1_IN_Buffer, 0, uCurPeriod, &DF3,3 );//находим сдвиг относительно начал отсчета максимальной амплитуды тока
     //Проверяем, что валидность максимумов тока и напряжения, сравнение FASE_DETECT_HISTERESIS позволяет отсечь систуацию, когда данные зафиксированы
     //со сдвигом фазы окло 90 градусов. В этом случае в пероид пападают 2 мксимума.
     if ( ((uCurPeriod - DF1 ) > FASE_DETECT_HISTERESIS ) && ((uCurPeriod - DF3 ) > FASE_DETECT_HISTERESIS ) && (DF1 <= DF3))
     {
       // Делаем усреднение
       CosBuffer[uCosCount][0] = DF3-DF1;
       CosBuffer[uCosCount][1] = uCurPeriod;
       uCosCount++;
       if (uCosCount>=COS_DATA_COUNT)
       {
         uCosCount=0;
         iMax =0;
         uCosFiPeriod  =0;
         for (t=0;t<COS_DATA_COUNT;t++)
         {
           iMax = iMax+  CosBuffer[t][0];
           uCosFiPeriod = uCosFiPeriod  +CosBuffer[t][1];
         }
         uCosFiMax =    iMax/COS_DATA_COUNT;
         uCosFiPeriod = uCosFiPeriod /COS_DATA_COUNT;
       }
     }

     xADCMax((int16_t *) &ADC1_IN_Buffer, 1, uCurPeriod, &DF3,3 );//находим сдвиг относительно начал отсчета максимальной амплитуды тока
     //Проверяем, что валидность максимумов тока и напряжения, сравнение FASE_DETECT_HISTERESIS позволяет отсечь систуацию, когда данные зафиксированы
    //со сдвигом фазы окло 90 градусов. В этом случае в пероид пападают 2 мксимума.
     if ( ((uCurPeriod - DF1 ) > FASE_DETECT_HISTERESIS ) && ((uCurPeriod - DF3 ) > FASE_DETECT_HISTERESIS ) && (DF1 <= DF3))
          {
            // Делаем усреднение
            F2CosBuffer[F2uCosCount][0] = DF3-DF1;
            F2CosBuffer[F2uCosCount][1] = uCurPeriod;
            F2uCosCount++;
            if (F2uCosCount>=COS_DATA_COUNT)
            {
              F2uCosCount=0;
              iMax =0;
              F2uCosFiPeriod  =0;
              for (t=0;t<COS_DATA_COUNT;t++)
              {
                iMax = iMax+  F2CosBuffer[t][0];
                F2uCosFiPeriod = F2uCosFiPeriod  +F2CosBuffer[t][1];
              }
              F2uCosFiMax =  iMax/COS_DATA_COUNT;
              F2uCosFiPeriod = F2uCosFiPeriod /COS_DATA_COUNT;
            }
          }

          xADCMax((int16_t *) &ADC1_IN_Buffer, 2, uCurPeriod, &DF3,3 );//находим сдвиг относительно начал отсчета максимальной амплитуды тока
          //Проверяем, что валидность максимумов тока и напряжения, сравнение FASE_DETECT_HISTERESIS позволяет отсечь систуацию, когда данные зафиксированы
          //со сдвигом фазы окло 90 градусов. В этом случае в пероид пападают 2 мксимума.
          if ( ((uCurPeriod - DF1 ) > FASE_DETECT_HISTERESIS ) && ((uCurPeriod - DF3 ) > FASE_DETECT_HISTERESIS ) && (DF1 <= DF3))
          {
             // Делаем усреднение
             F3CosBuffer[F3uCosCount][0] = DF3-DF1;
             F3CosBuffer[F3uCosCount][1] = uCurPeriod;
             F3uCosCount++;
             if (F3uCosCount>=COS_DATA_COUNT)
             {
               F3uCosCount=0;
               iMax =0;
               F3uCosFiPeriod  =0;
               for (t=0;t<COS_DATA_COUNT;t++)
               {
                  iMax = iMax+  F3CosBuffer[t][0];
                  F3uCosFiPeriod = F3uCosFiPeriod  +F3CosBuffer[t][1];
               }
               F3uCosFiMax =  iMax/COS_DATA_COUNT;
               F3uCosFiPeriod = F3uCosFiPeriod /COS_DATA_COUNT;
             }
          }
   }
   else
   {
     //Если по каким-то причинам мы не можем в текущий момент расчитать значение частоты напрежения генератора, то считаем ток арефмечтический исходя из максимального значения
     xDeviceADC[GEN_FASE_C_L1].uDataSrc =  xADCMax( (int16_t *)&ADC1_IN_Buffer, 0, uCurPeriod, &DF3,3 ) / sqrt (2U);
     xDeviceADC[GEN_FASE_C_L1].uDataSrc =  xADCMax( (int16_t *)&ADC1_IN_Buffer, 1, uCurPeriod, &DF3,3 ) / sqrt (2U);
     xDeviceADC[GEN_FASE_C_L1].uDataSrc =  xADCMax( (int16_t *)&ADC1_IN_Buffer, 2, uCurPeriod, &DF3,3 ) / sqrt (2U);
   }
   xEventGroupSetBits( xADCEvent, GEN_UPDATE);
  return ( result );

}


/*
 *
 *
 */
xADCGenDetectType vADCGetGenFreqPres( void )
{
  return (uFreqPresent);

}

/*
 * Сервисная функция для расчета RMS
 */
uint16_t  uADCRMS(int16_t * source, uint8_t off, uint16_t size, uint8_t cc )
{
  uint64_t sum =0;
  for (uint16_t i=0; i < size*cc; i= i + cc )
  {
    sum = sum + source[ i+off ] * source[ i+off ];
  }
  sum = sum / size;
  return ( sqrt( sum )  );

}
/*
 * Сервисная функция для вычисления максимального значения
 *  source  -адрес буфера DMA
 *  off     -номер канала, скоторым работает функция
 *  size    -размер буфера для обработки
 */
int16_t  xADCMax( int16_t * source, uint8_t off, uint16_t size, uint16_t * delay, uint8_t cc )
{

  int16_t max =0;
  uint16_t del =0,i;

  for (i=0;i<size;i++)
  {
      if ( source[i*cc + off] >  max)
      {
        max = source[i*cc + off];
        del = i;
      }
  }
  *delay = del;
  return ( max );

}
/*
 *
 */
uint16_t GetAverVDD(uint8_t channel, uint8_t size, uint8_t offset, int16_t * source)
{
   uint32_t Buffer=0;
   for (uint8_t i=0; i<size; i++)
   {
     Buffer = Buffer + source[i*offset + channel];
   }
   Buffer =Buffer/size;
   return  ( (uint16_t)Buffer );
}


/*
 * Процедура настройки параметров алгоритма определния частоты в зависимости от амплитуды входного сигнала
 */
void  vADCSetFreqDetectParam(int16_t AMP,uint8_t * del,uint8_t * fd)
{
  switch (AMP/100)
         {
           case 0:
             *del = 0;
             *fd =  70;
             break;
           case 1:
             *del = 2;
             *fd =  40;
             break;
           case 2:
             *del = 4;
             *fd =  25;
             break;
           case 3:
             *del = 5;
             *fd =  20;
             break;
           case 4:
             *del = 7;
             *fd =  15;
             break;
           case 5:
             *del = 8;
             *fd =  10;
             break;
           case 6:
             *del = 11;
             *fd =  8;
             break;
           case 7:
             *del = 12;
             *fd =  7;
             break;
           case 8:
             *del = 14;
             *fd =  6;
             break;
           case 9:
             *del = 15;
             *fd =  5;
             break;
           case 10:
             *del = 16;
             *fd =  4;
             break;
           case 11:
           case 12:
           case 13:
           case 14:
           case 15:
           case 16:
           case 17:
           case 18:
           case 19:
           case 20:
           default:
             *del = 20;
             *fd =  4;
             break;
         }
}


