// C
// Config Parser Header

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "votekill_spam.h"

bool ParseConfig(struct votekill_params_s *params);
void Trim(char *);

#endif