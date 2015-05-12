#include <cstdio>

#include "ch.h"
#include "hal.h"

#include <r2p/Middleware.hpp>
#include <r2p/node/led.hpp>
#include <r2p/msg/std_msgs.hpp>

#ifndef R2P_MODULE_NAME
#define R2P_MODULE_NAME "RC"
#endif

static WORKING_AREA(wa_info, 1024);
static r2p::RTCANTransport rtcantra(RTCAND1);

RTCANConfig rtcan_config = { 1000000, 100, 60 };

r2p::Middleware r2p::Middleware::instance(R2P_MODULE_NAME, "BOOT_"R2P_MODULE_NAME);

/*
 * Test node
 */
msg_t test_pub_node(void *arg) {
	r2p::Node node("test_pub");
	r2p::Publisher<r2p::String64Msg> pub;
	r2p::String64Msg * msgp;
	uint16_t * uuid = (uint16_t *)0x1FFFF7AC;

	(void) arg;
	chRegSetThreadName("test_pub");

	node.advertise(pub, "test");

	while (!pub.alloc(msgp)) chThdSleepMilliseconds(1000);

	sprintf(msgp->data, "\n\n"R2P_MODULE_NAME" module [0x%x 0x%x 0x%x 0x%x 0x%x 0x%x]", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5]);
	pub.publish(msgp);
	chThdSleepMilliseconds(100);

	return CH_SUCCESS;
}

/*
 * Application entry point.
 */
extern "C" {
int main(void) {

	halInit();
	chSysInit();

	r2p::Middleware::instance.initialize(wa_info, sizeof(wa_info), r2p::Thread::LOWEST);
	rtcantra.initialize(rtcan_config);
	r2p::Middleware::instance.start();

	r2p::ledsub_conf ledsub_conf = { "led" };
	r2p::Thread::create_heap(NULL, THD_WA_SIZE(512), NORMALPRIO, r2p::ledsub_node, &ledsub_conf);

	r2p::Thread::create_heap(NULL, THD_WA_SIZE(2048), NORMALPRIO + 1, test_pub_node, NULL);

	for (;;) {
		r2p::Thread::sleep(r2p::Time::ms(500));
	}

	return CH_SUCCESS;
}
}
