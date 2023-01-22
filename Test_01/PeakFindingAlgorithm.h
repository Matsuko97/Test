#pragma once

#define iPeak_No 3
#define RoundRatio 100

struct PeakNode {
	int           indPeak;
	int           indStart;
	int           indEnd;
	struct PeakNode* next;
};