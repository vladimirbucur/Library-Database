// Copyright 2022 Bucur Vladimir 311CA<vladimirbucur1@gmail.com>

#ifndef DIE_H_
#define DIE_H_

#include <errno.h>
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);					\
		}							\
	} while (0)

#endif  // DIE_H_
