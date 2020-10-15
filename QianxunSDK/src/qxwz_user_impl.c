/*
 * qxwz_user_impl.c
 *
 *  Created on: 2020年10月14日
 *      Author: wxujs
 */


#include "qxwz_user_impl.h"
#include "UserConfig.h"
#include "stm32f10x.h"
//#include "stdio.h"
#include <stdlib.h>
qxwz_int32_t qxwz_printf(const qxwz_char_t *fmt, ...)
{
	return USART_printf(USART2,fmt);
}

qxwz_int32_t qxwz_sock_create()
{
	return 1;
}

qxwz_int32_t qxwz_sock_connect(qxwz_int32_t sock, qxwz_sock_host_t *serv)
{

}

qxwz_int32_t qxwz_sock_send(qxwz_int32_t sock, const qxwz_uint8_t *send_buf, qxwz_uint32_t len)
{

}

qxwz_int32_t qxwz_sock_recv(qxwz_int32_t sock, qxwz_uint8_t *recv_buf, qxwz_uint32_t len)
{

}

qxwz_int32_t qxwz_sock_close(qxwz_int32_t sock)
{

}

qxwz_void_t *qxwz_malloc(qxwz_uint32_t size)
{
	return malloc(size);
}

qxwz_void_t qxwz_free(qxwz_void_t *ptr)
{
	free(ptr);
}

qxwz_void_t *qxwz_calloc(qxwz_uint32_t nmemb, qxwz_uint32_t size)
{
	return malloc(size*nmemb);
}

qxwz_void_t *qxwz_realloc(qxwz_void_t *ptr, qxwz_uint32_t size)
{
	return realloc(ptr,size);
}

qxwz_int32_t qxwz_mutex_init(qxwz_mutex_t *mutex)
{

}
qxwz_int32_t qxwz_mutex_trylock(qxwz_mutex_t *mutex)
{

}
qxwz_int32_t qxwz_mutex_lock(qxwz_mutex_t *mutex)
{

}
qxwz_int32_t qxwz_mutex_unlock(qxwz_mutex_t *mutex)
{

}
qxwz_int32_t qxwz_mutex_destroy(qxwz_mutex_t *mutex)
{

}
void usleep(int t)
{
#ifdef	INC_FREERTOS_H
	vTaskDelay(t);
#else
	delay_us(t);
#endif
}
