#include "yearn_plugin.h"

const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void handle_provide_token(void *parameters) {
    ethPluginProvideInfo_t *msg = (ethPluginProvideInfo_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    if (context->selectorIndex != DEPOSIT && context->selectorIndex != DEPOSIT_ALL &&
        memcmp(context->extra_address, NULL_ETH_ADDRESS, ADDRESS_LENGTH) == 0) {
        context->decimals = 18;
        strlcpy(context->want, "ETH", sizeof(context->want));
    } else if (msg->item1) {
        context->decimals = msg->item1->token.decimals;
        strlcpy(context->want, (char *) msg->item1->token.ticker, sizeof(context->want));
    } else {
        context->decimals = 18;
        strlcpy(context->want, "???", sizeof(context->want));
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
