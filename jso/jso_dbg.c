/*
 * Copyright (c) 2025 Jakub Zelenka. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "jso_dbg.h"

#ifdef JSO_DEBUG_ENABLED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

typedef struct {
	int fd;
	mode_t file_mode;
	char component[32];
	int initialized;
	int timestamp_enabled;
} jso_dbg_config_t;

static jso_dbg_config_t jso_g_dbg_config = { .fd = -1,
	.file_mode = 0644, // default mode
	.component = { 0 },
	.initialized = 0,
	.timestamp_enabled = 0 };

static char *jso_trim_whitespace(char *str)
{
	char *end;

	// Trim leading space
	while (*str == ' ' || *str == '\t')
		str++;

	if (*str == 0)
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && (*end == ' ' || *end == '\t'))
		end--;

	end[1] = '\0';
	return str;
}

static int jso_parse_mode(const char *mode_str, mode_t *mode)
{
	char *endptr;
	long parsed_mode = strtol(mode_str, &endptr, 8); // octal base

	if (*endptr != '\0' || parsed_mode < 0 || parsed_mode > 0777) {
		return -1; // Invalid mode
	}

	*mode = (mode_t) parsed_mode;
	return 0;
}

void jso_dbg_init_from_config(const char *config_string)
{
	if (!config_string || jso_g_dbg_config.initialized) {
		return;
	}

	// Make a copy of the config string for parsing
	char *config_copy = strdup(config_string);
	if (!config_copy) {
		return;
	}

	char *file_path = NULL;
	mode_t file_mode = 0644; // default
	char component[32] = { 0 };
	int timestamp_enabled = 0;

	// Parse the config string
	char *token = strtok(config_copy, ",");
	while (token != NULL) {
		token = jso_trim_whitespace(token);

		if (strncmp(token, "file:", 5) == 0) {
			file_path = token + 5; // Skip "file:" prefix
		} else if (strncmp(token, "mode:", 5) == 0) {
			char *mode_str = token + 5;
			if (jso_parse_mode(mode_str, &file_mode) != 0) {
				fprintf(stderr, "jso_dbg: Invalid mode format: %s\n", mode_str);
				free(config_copy);
				return;
			}
		} else if (strncmp(token, "component:", 10) == 0) {
			char *comp_str = token + 10;
			strncpy(component, comp_str, sizeof(component) - 1);
			component[sizeof(component) - 1] = '\0';
		} else if (strncmp(token, "timestamp:", 10) == 0) {
			char *ts_str = token + 10;
			if (strcmp(ts_str, "yes") == 0 || strcmp(ts_str, "1") == 0) {
				timestamp_enabled = 1;
			}
		} else {
			fprintf(stderr, "jso_dbg: Unknown config option: %s\n", token);
		}

		token = strtok(NULL, ",");
	}

	// Validate that we have a file path
	if (!file_path || strlen(file_path) == 0) {
		fprintf(stderr, "jso_dbg: No file path specified in config\n");
		free(config_copy);
		return;
	}

	// Open the log file
	int fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, file_mode);
	if (fd == -1) {
		perror("jso_dbg: Failed to open log file");
		free(config_copy);
		return;
	}

	// If mode was specified, try to set it (might fail if not owner)
	if (strstr(config_string, "mode:") != NULL) {
		if (fchmod(fd, file_mode) == -1) {
			// Not a fatal error, just warn
			perror("jso_dbg: Warning - failed to set file mode");
		}
	}

	// Store configuration
	jso_g_dbg_config.fd = fd;
	jso_g_dbg_config.file_mode = file_mode;
	jso_g_dbg_config.timestamp_enabled = timestamp_enabled;
	if (strlen(component) > 0) {
		strncpy(jso_g_dbg_config.component, component, sizeof(jso_g_dbg_config.component) - 1);
	}
	jso_g_dbg_config.initialized = 1;

	free(config_copy);

	// Write initialization message
	dprintf(fd, "[DBG_INIT] Debug logging initialized for component: %s\n",
			strlen(component) > 0 ? component : "ALL");
}

void jso_dbg_init_from_env(const char *env_name)
{
	const char *config_env_name = env_name == NULL ? JSO_DBG_DEFAULT_ENV_NAME : env_name;
	const char *config_string = getenv(config_env_name);
	if (config_string != NULL) {
		jso_dbg_init_from_config(config_string);
	}
}

void jso_dbg_log(const char *type, const char *fmt, ...)
{
	if (!jso_g_dbg_config.initialized || jso_g_dbg_config.fd == -1) {
		return;
	}

	// If component filter is set, check if this log type matches
	if (strlen(jso_g_dbg_config.component) > 0 && strcmp(jso_g_dbg_config.component, type) != 0) {
		return; // Skip this log entry
	}

	va_list args;
	va_start(args, fmt);

	// Write timestamp if enabled
	if (jso_g_dbg_config.timestamp_enabled) {
		time_t now = time(NULL);
		struct tm *tm_info = localtime(&now);
		dprintf(jso_g_dbg_config.fd, "[%04d-%02d-%02d %02d:%02d:%02d] ", tm_info->tm_year + 1900,
				tm_info->tm_mon + 1, tm_info->tm_mday, tm_info->tm_hour, tm_info->tm_min,
				tm_info->tm_sec);
	}

	// Write type
	dprintf(jso_g_dbg_config.fd, "[%s] ", type);

	// Write the formatted message
	vdprintf(jso_g_dbg_config.fd, fmt, args);
	dprintf(jso_g_dbg_config.fd, "\n");

	va_end(args);
}

void jso_dbg_cleanup(void)
{
	if (jso_g_dbg_config.initialized && jso_g_dbg_config.fd != -1) {
		close(jso_g_dbg_config.fd);
		jso_g_dbg_config.fd = -1;
		jso_g_dbg_config.initialized = 0;
	}
}

#endif
