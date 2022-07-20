#include "rpc.h"
#include "common.h"

#include <assert.h>
#include <sysrepo.h>

#include <time.h>

// helpers //

static int system_set_current_datetime(const char *datetime);

////

int system_subscription_rpc_set_current_datetime(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
	int error = SR_ERR_OK;

	const char *current_datetime = NULL;

	// assert only one input value - datetime
	assert(input_cnt == 1);

	current_datetime = input[0].data.string_val;

	error = system_set_current_datetime(current_datetime);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_set_datetime() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "System time successfully set.");

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;
	SRPLG_LOG_ERR(PLUGIN_NAME, "Failed to set system time.");

out:
	return error;
}

int system_subscription_rpc_restart(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
	int error = SR_ERR_OK;

	sync();
	system("shutdown -r");
	SRPLG_LOG_INF(PLUGIN_NAME, "Restarting the system!");

	return error;
}

int system_subscription_rpc_shutdown(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
	int error = SR_ERR_OK;

	sync();
	system("shutdown -P");
	SRPLG_LOG_INF(PLUGIN_NAME, "Shutting down the system!");

	return error;
}

static int system_set_current_datetime(const char *current_datetime)
{
	struct tm t = {0};
	time_t time_to_set = 0;
	struct timespec stime = {0};

	if (strptime(current_datetime, "%FT%TZ", &t) == 0) {
		return -1;
	}

	time_to_set = mktime(&t);
	if (time_to_set == -1) {
		return -1;
	}

	stime.tv_sec = time_to_set;

	if (clock_settime(CLOCK_REALTIME, &stime) == -1) {
		return -1;
	}

	return 0;
}