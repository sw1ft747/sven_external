// C++
// Config Parser

#include "config_parser.h"

bool ParseConfig(struct votekill_params_s *params)
{
	char file_buffer[512];
	FILE *config = fopen("votekill_spam.cfg", "r");

	if (config == nullptr)
	{
		printf("[ERROR] Failed to open the config file\n");
		return false;
	}

	bool bAddressFound = false, bPlayerNameFound = false;

	while (fgets(file_buffer, 512, config))
	{
		char *szSplit = strtok(file_buffer, "=");
		char *szValue = strtok(nullptr, "=");

		Trim(szSplit); Trim(szValue);

		if (strcmp(szSplit, "address") == 0)
		{
			params->szServerIP = (const char *)calloc(strlen(szValue) + 1, sizeof(char));
			if (params->szServerIP != nullptr)
			{
				memcpy((void *)params->szServerIP, szValue, strlen(szValue));
				bAddressFound = true;
			}

		#ifdef DEBUG
			printf("Server IP: %s\n", params->szServerIP);
		#endif
		}
		else if (strcmp(szSplit, "playername") == 0)
		{
			params->szPlayerName = (const char *)calloc(strlen(szValue) + 1, sizeof(char));
			if (params->szPlayerName != nullptr)
			{
				memcpy((void *)params->szPlayerName, szValue, strlen(szValue));
				bPlayerNameFound = true;
			}

		#ifdef DEBUG
			printf("Player Name: %s\n", params->szPlayerName);
		#endif
		}
		else if (strcmp(szSplit, "vote_duration") == 0)
		{
			params->flVoteDuration = static_cast<float>(atof(szValue));

		#ifdef DEBUG
			printf("Vote Duration: %f\n", params->flVoteDuration);
		#endif
		}
		else if (strcmp(szSplit, "connection_duration") == 0)
		{
			params->flConnectionDuration = static_cast<float>(atof(szValue));

		#ifdef DEBUG
			printf("Connection Duration: %f\n", params->flConnectionDuration);
		#endif
		}
		else if (strcmp(szSplit, "forced_disconnect_delay") == 0)
		{
			params->flForcedDisconnectDelay = static_cast<float>(atof(szValue));

		#ifdef DEBUG
			printf("Forced Disconnect Delay: %f\n", params->flForcedDisconnectDelay);
		#endif
		}
		else if (strcmp(szSplit, "vote_type") == 0)
		{
			params->nVoteType = atoi(szValue);

		#ifdef DEBUG
			printf("Vote Type: %d\n", params->nVoteType);
		#endif
		}
	}

	fclose(config);

	if (!bAddressFound || !bPlayerNameFound)
	{
		printf("[ERROR] Failed to parse Server Address/Player Name\n");
		return false;
	}

	return true;
}

void Trim(char *str)
{
	size_t i = 0, j;

	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		i++;

	if (i > 0)
	{
		for (j = 0; j < strlen(str); j++)
		{
			str[j] = str[j + i];
		}
		str[j] = '\0';
	}

	i = strlen(str) - 1;

	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		i--;

	if (i < (strlen(str) - 1))
		str[i + 1] = '\0';
}