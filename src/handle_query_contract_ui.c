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

void copy_date(size_t timestamp, char *out_buffer, size_t out_buffer_size) {
    size_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
    size_t days = (timestamp / 86400);

	size_t year = 1970;
	size_t month = 1;
	size_t day = 1;

	while(days > 0) {
		char leap_year = (year % 4) == 0;

		for(int i = 0; i < 12 ;i++) {
			size_t days_in_month = monthDays[i];

			if(i == 1 && leap_year) { // February of leap year
				days_in_month++;
			}

			if(days < days_in_month) {
				day = days + 1;
				days = 0;
				break;
			}

			days -= days_in_month;

			month++;
		}

		month = 1;
		year++;
	}

	size_t seconds = (timestamp % 86400);

	size_t hour = seconds / 3600;
	size_t minute = (seconds % 3600) / 60;
	size_t second = (seconds % 3600) % 60;

    snprintf(out_buffer, out_buffer_size,  "%d-%d-%d %d:%d:%d", year, month, day, hour, minute, second);
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
static void set_amount_with_custom(ethQueryContractUI_t *msg, context_t *context, char *ticker, size_t decimals) {
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
    strlcpy(msg->msg, "Unlock Time", msg->msgLength);
    /*copy_date(context->slippage, msg->msg, msg->msgLength);*/
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
            set_amount_with_custom(msg, context, "YFI", 18);
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
