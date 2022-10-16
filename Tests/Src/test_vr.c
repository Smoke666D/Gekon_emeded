/*
 * test_vr.c
 *
 *  Created on: 22 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "vrSensor.h"
#include "fix16.h"

void test_vVRextiCallback ( void )
{
  uint16_t data = vrCounter + 1U;
  vVRextiCallback();
  TEST_ASSERT_EQUAL( data, vrCounter );
  return;
}

void test_fVRgetSpeed ( void )
{
  vrSpeed = ( fix16_t )( 10U );
  TEST_ASSERT_EQUAL( vrSpeed, fVRgetSpeed() );
  vrSpeed = 0U;
  return;
}

void tets_vVRtimCallback ( void )
{
  uint16_t buf = vrTeethNumber;
  vrCounter     = 100U;
  vrTeethNumber = 100U;
  vVRtimCallback();
  TEST_ASSERT_EQUAL( fix16_from_int( 60U ), vrSpeed   );
  TEST_ASSERT_EQUAL( 0U, vrCounter );
  vrTeethNumber = buf;
  return;
}

void runTest_vr ( void )
{
  UnitySetTestFile( "test_vr.c" );
  UnityDefaultTestRun( test_vVRextiCallback, "VR EXTI callback", 0U );
  UnityDefaultTestRun( test_fVRgetSpeed, "VR get speed", 0U );
  UnityDefaultTestRun( tets_vVRtimCallback, "VR TIM callback", 0U );
  vTESTsendReport();
  return;
}
