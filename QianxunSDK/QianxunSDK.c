/*
 * QianxunSDK.c
 *
 *  Created on: 2020年10月14日
 *      Author: wxujs
 */

#include "QianxunSDK.h"

/*********************Static Values*********************************************************/
static qxwz_uint32_t glob_flag = 0;
static qxwz_uint64_t glob_ts = 0;

static qxwz_uint32_t sdk_auth_flag = 0;
static qxwz_uint32_t sdk_start_flag = 0;
qxwz_sdk_cap_info_t sdk_cap_info = { 0 };

void sdk_test()
{

	qxwz_sdk_config_t sdk_config = { 0 };
	/* AK or DSK ? Only choose one in the following codes! */
	/*
	 * AK
	 */
	sdk_config.key_type = QXWZ_SDK_KEY_TYPE_AK, strcpy(sdk_config.key, "Your AK");
	strcpy(sdk_config.secret, "Your AS");
	/*
	 * DSK
	 */
	sdk_config.key_type = QXWZ_SDK_KEY_TYPE_DSK, strcpy(sdk_config.key, "Your DSK");
	strcpy(sdk_config.secret, "Your DSS");

	/* set device info */
	strcpy(sdk_config.dev_id, "Your device id");
	strcpy(sdk_config.dev_type, "Your device type");

	/* set callbacks */
//	sdk_config.status_cb = status_cb;
//	sdk_config.data_cb = data_cb;
//	sdk_config.auth_cb = auth_cb;
//	sdk_config.start_cb = start_cb;

	int ret = 0;
	unsigned int tick = 0;
	struct timeval tval = { 0 };
	gettimeofday(&tval, NULL);

	/*
	 * init sdk
	 */
	ret = qxwz_sdk_init(&sdk_config);
	if (ret < 0)
	{
		printf("sdk init failed\n");
		goto END;
	}

	/*
	 * do authentication
	 */
	ret = qxwz_sdk_auth();
	if (ret < 0)
	{
		printf("call sdk auth failed\n");
		goto END;
	}

	while (1)
	{
		gettimeofday(&tval, NULL);
		ret = qxwz_sdk_tick(tval.tv_sec);
		if (ret < 0)
		{
			printf("sdk tick failed\n");
			break;
		}
		usleep(100 * 1000); /* 100ms */
		if ((++tick % 10) == 0)
		{
			/* upload GGA */
			qxwz_sdk_upload_gga(DEMO_GGA_STR, strlen(DEMO_GGA_STR));

		}

		if (sdk_auth_flag > 0)
		{
			sdk_auth_flag = 0;
			if (sdk_cap_info.caps_num > 0)
			{
				qxwz_sdk_start(QXWZ_SDK_CAP_ID_NOSR); /* start NOSR capability */
			}
		}
	}

	qxwz_sdk_stop(QXWZ_SDK_CAP_ID_NOSR); /* stop NOSR capability */

	END: while (qxwz_sdk_cleanup() != 0)
	{
		usleep(100 * 1000);
	}
}

