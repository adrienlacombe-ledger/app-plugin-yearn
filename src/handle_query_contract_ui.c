#include <stdio.h>
#include "yearn_plugin.h"

void copy_amount_with_ticker(const size_t *amount,
                             size_t amount_size,
                             size_t amount_decimals,
                             char *ticker,
                             size_t ticker_size,
                             char *out_buffer,
                             size_t out_buffer_size) {
    char tmp_buffer[100] = {0};
    amountToString(amount, amount_size, amount_decimals, "", tmp_buffer, sizeof(tmp_buffer));
    size_t stringLen = strnlen(tmp_buffer, sizeof(tmp_buffer)) + 1 + ticker_size;
    snprintf(out_buffer, MIN(out_buffer_size, stringLen), "%s %s", tmp_buffer, ticker);
    out_buffer[out_buffer_size - 1] = '\0';
}

void copy_seconds_to_string(size_t seconds, char *out_buffer, size_t out_buffer_size) {
    size_t weeks = seconds / 604800;  // 60 seconds * 60 minutes * 24 hours * 7 days
    seconds %= 604800;

    size_t days = seconds / 86400;  // 60 seconds * 60 minutes * 24 hours

    char tmp_weeks[100] = {0};
    amountToString(weeks, sizeof(weeks), 0, "", tmp_weeks, sizeof(tmp_weeks));

    char tmp_days[100] = {0};
    amountToString(days, sizeof(days), 0, "", tmp_days, sizeof(tmp_days));

    size_t stringLen = strnlen(tmp_weeks, sizeof(tmp_weeks)) + 1 + sizeof("weeks") +
                       strnlen(tmp_days, sizeof(tmp_days)) + 1 + sizeof("weeks");
    snprintf(out_buffer, MIN(out_buffer_size, stringLen), "%s weeks %s days", tmp_weeks, tmp_days);
    out_buffer[out_buffer_size - 1] = '\0';
}

/******************************************************************************
**  Will display the splipage used for this transaction.
**  | Slippage |
**  |   10 %   |
******************************************************************************/
static void set_slippage_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Slippage", msg->titleLength);
    copy_amount_with_ticker(context->slippage,
                            sizeof(context->slippage),
                            2,
                            "%%",
                            2,
                            msg->msg,
                            msg->msgLength);
}

/******************************************************************************
**  Will display the recipient's address (withdraw)
**  |                   Recipient                  |
**  |  0x28bC240B2433B65d3C64EBF168862E60fAb019E4  |
******************************************************************************/
static void set_recipient_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Recipient", msg->titleLength);
    msg->msg[0] = '0';
    msg->msg[1] = 'x';
    uint64_t chainid = 0;
    getEthAddressStringFromBinary(context->extra_address,
                                  msg->msg + 2,
                                  msg->pluginSharedRW->sha3,
                                  chainid);
}

/******************************************************************************
**  Will display the amount of token to be deposit in the vault.
**  |   Amount  |
**  |  200 DAI  |
******************************************************************************/
static void set_amount_with_want(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Amount", msg->titleLength);
    copy_amount_with_ticker(context->amount,
                            sizeof(context->amount),
                            context->decimals,
                            context->want,
                            sizeof(context->want),
                            msg->msg,
                            msg->msgLength);
}

/******************************************************************************
**  Will display the amount of tokens using a custom ticker and decimals
**  |   Amount  |
**  |  200 YFI  |
******************************************************************************/
static void set_amount_with_custom(ethQueryContractUI_t *msg, char *ticker, size_t decimals) {
    strlcpy(msg->title, "Amount", msg->titleLength);
    copy_amount_with_ticker(context->amount,
                            sizeof(context->amount),
                            decimals,
                            ticker,
                            sizeof(ticker),
                            msg->msg,
                            msg->msgLength);
}

/******************************************************************************
**  Will display the amount of token to be withdrawn from the vault.
**  |    Amount   |
**  |  200 yvDAI  |
******************************************************************************/
static void set_amount_with_vault(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Amount", msg->titleLength);
    copy_amount_with_ticker(context->amount,
                            sizeof(context->amount),
                            context->decimals,
                            context->vault,
                            sizeof(context->vault),
                            msg->msg,
                            msg->msgLength);
}

/******************************************************************************
**  Will display ALL to indicate that the full balance should be deposited or
**  withdrawn.
**  |  Amount |
**  |   ALL   |
******************************************************************************/
static void set_amount_with_all(ethQueryContractUI_t *msg) {
    strlcpy(msg->title, "Amount", msg->titleLength);
    strlcpy(msg->msg, "ALL", msg->msgLength);
}

/******************************************************************************
**  Will display the Vault name. The vaults are defined in main.c in the
**  YEARN_VAULTS variable.
**  | Vault |
**  | yvDAI |
******************************************************************************/
static void set_vault_name(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Vault", msg->titleLength);
    strlcpy(msg->msg, context->vault, msg->msgLength);
}

/******************************************************************************
**  Will search for current vault in YEARN_VAULTS.
**  It will set name and want in context.
******************************************************************************/
void set_vault_information(ethQueryContractUI_t *msg, context_t *context) {
    uint8_t i;
    yearnVaultDefinition_t *currentVault = NULL;
    for (i = 0; i < NUM_YEARN_VAULTS; i++) {
        currentVault = (yearnVaultDefinition_t *) PIC(&YEARN_VAULTS[i]);
        if (memcmp(currentVault->address, context->vault_address, ADDRESS_LENGTH) == 0) {
            context->decimals = currentVault->decimals;
            if (context->selectorIndex != ZAP_IN) {
                memcpy(context->want, currentVault->want, MAX_VAULT_TICKER_LEN);
            }
            memcpy(context->vault, currentVault->vault, MAX_VAULT_TICKER_LEN);
            break;
        }
    }

    if (context->vault[0] == '\0') {
        PRINTF("Received an invalid vault\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }
}

/******************************************************************************
**  Will display the unlock time
**  | Unlock Time     |
**  | 12 weeks 6 days |
******************************************************************************/
static void set_unlock_time(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Unlock Time", msg->titleLength);
    copy_seconds_to_string(context->unlock_time, msg->msg, msg->msgLength);
}

void handle_query_contract_ui_zap_in(ethQueryContractUI_t *msg, context_t *context) {
    set_vault_information(msg, context);

    switch (msg->screenIndex) {
        case 0:
            set_amount_with_want(msg, context);
            break;
        case 1:
            set_vault_name(msg, context);
            break;
        default:
            PRINTF("Received an invalid screenIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

void handle_query_contract_ui_track_in(ethQueryContractUI_t *msg, context_t *context) {
    set_vault_information(msg, context);

    switch (msg->screenIndex) {
        case 0:
            switch (context->selectorIndex) {
                case DEPOSIT_ALL:
                    set_amount_with_all(msg);
                    break;
                case DEPOSIT:
                    set_amount_with_want(msg, context);
                    break;
                default:
                    break;
            }
            break;
        case 1:
            set_vault_name(msg, context);
            break;
        default:
            PRINTF("Received an invalid screenIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

void handle_query_contract_ui_modify_lock(ethQueryContractUI_t *msg, context_t *context) {
    switch (msg->screenIndex) {
        case 0:
            set_amount_with_custom(msg, "YFI", 18);
            break;
        case 1:
            set_unlock_time(msg, context);
            break;
        case 2:  // it's modify_lock_to
            set_recipient_ui(msg, context);
            break;
        default:
            PRINTF("Received an invalid screenIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

void handle_query_contract_ui_vaults(ethQueryContractUI_t *msg, context_t *context) {
    // Copy the vault address prior to any process
    ethPluginSharedRO_t *pluginSharedRO = (ethPluginSharedRO_t *) msg->pluginSharedRO;
    copy_parameter(context->vault_address,
                   pluginSharedRO->txContent->destination,
                   sizeof(context->vault_address));

    set_vault_information(msg, context);

    switch (msg->screenIndex) {
        case 0:
            switch (context->selectorIndex) {
                case CLAIM:
                case WITHDRAW_ALL:
                    set_amount_with_all(msg);
                    break;
                case WITHDRAW_TO_SLIPPAGE:
                case WITHDRAW_TO:
                case WITHDRAW:
                    set_amount_with_vault(msg, context);
                    break;
                default:
                    break;
            }
            break;
        case 1:
            set_vault_name(msg, context);
            break;
        case 2:
            set_recipient_ui(msg, context);
            break;
        case 3:
            set_slippage_ui(msg, context);
            break;
        // Keep this
        default:
            PRINTF("Received an invalid screenIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case DEPOSIT:
        case DEPOSIT_ALL:
            handle_query_contract_ui_track_in(msg, context);
            break;
        case ZAP_IN:
            handle_query_contract_ui_zap_in(msg, context);
            break;
        case MODIFY_LOCK:
        case MODIFY_LOCK_TO:
            handle_query_contract_ui_modify_lock(msg, context);
            break;
        default:
            handle_query_contract_ui_vaults(msg, context);
            return;
    }
}
