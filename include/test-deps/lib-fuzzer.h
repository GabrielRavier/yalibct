#pragma once

#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

// Not gonna be able to be particularly good at fuzzing compared to the actual libFuzzer but it's something
int main() {
    uint8_t *data = NULL;
    size_t largestSizeYet = 0;
    for (size_t i = 0; i < 100000; ++i) {
        size_t thisIterationSize = random() % 1000;
        if (thisIterationSize > largestSizeYet) {
            largestSizeYet = thisIterationSize;
            data = realloc(data, largestSizeYet); // NOLINT(bugprone-suspicious-realloc-usage)
            assert(data != NULL);
        }

        for (size_t j = 0; j < thisIterationSize; ++j)
            data[j] = random() & 0xFF;
        LLVMFuzzerTestOneInput(data, thisIterationSize);
    }
}
