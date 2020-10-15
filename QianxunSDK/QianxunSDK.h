/*
 * QianxunSDK.h
 *
 *  Created on: 2020年10月14日
 *      Author: wxujs
 */

#ifndef QIANXUNSDK_H_
#define QIANXUNSDK_H_
#include "UserConfig.h"
#include "qxwz_sdk.h"

/*********************Debug Test Message <fake GPS message>*********************************/
#define DEMO_GGA_STR        "$GPGGA,000001,3112.518576,N,12127.901251,E,1,8,1,0,M,-32,M,3,0*4B"


/*
 * ** WARNING **
 * PLEASE FIRST CONFIRM THAT YOUR ACCOUNT IS AK OR DSK ?!?
 *
 * If your account is AK (usually with prefix `A`, like: `A00012dwejd`), set the `key_type` to `QXWZ_SDK_KEY_TYPE_AK`.
 * Otherwise, if it is DSK (usually with prefix `D`, like: `D0234jdwejd`), set the `key_type` to `QXWZ_SDK_KEY_TYPE_DSK`.
 * ** WARNING **
 */

void sdk_test();

#endif /* QIANXUNSDK_H_ */
