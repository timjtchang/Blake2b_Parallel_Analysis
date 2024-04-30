#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include "blake2.h"
#include "blake2-impl.h"

using namespace std;

#define LENGTH 256
string testFile = "test-large-file.pdf";
int numIterations = 50;
bool averageTest = true;

static const char base64chars[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                   'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

void KAT_Blake2(uint8_t *input, uint8_t *hash)
{
    int i, j;
    struct timespec start, stop; 
	double time;
    double totalTime = 0;
    printf("Running Blake2 :)\n\n");

    for (i = 0; i < LENGTH; ++i)
    {
        if( clock_gettime(CLOCK_REALTIME, &start) == -1) {
            perror("clock gettime");
        }

        blake2b(hash, BLAKE2B_OUTBYTES, input, i, NULL, 0);

        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { 
            perror("clock gettime");
        }		
		time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
        totalTime += time;

        printf("\t{\n\t\t");

        for (j = 0; j < BLAKE2B_OUTBYTES - 3; j += 3)
        {
            // Encode the first character
            printf("%c", base64chars[hash[j] >> 2]);
            // Encode the second character
            printf("%c", base64chars[((hash[j] & 0x03) << 4) | ((hash[j + 1] & 0xf0) >> 4)]);
            // Encode the third character
            printf("%c", base64chars[((hash[j + 1] & 0x0f) << 2) | ((hash[j + 2] & 0xc0) >> 6)]);
            // Encode the fourth character
            printf("%c", base64chars[hash[j + 2] & 0x3f]);

            // printf("\tAdded indexes %i to %i\n", j, j + 2);
        }
        if (j >= 63) {
            printf("%c", base64chars[hash[j] >> 2]);
            printf("%c", base64chars[(hash[j] & 0x03) << 4]);
            printf("=");
            printf("=");
        }

        printf("\n\t},\n");

        printf("Execution Time = %f sec.\n", time);
    }

    printf("};\n\n\n\n\n");

    printf("Total Execution Time for %i hashes = %f sec.\n", LENGTH, totalTime);
    printf("Average Execution Time = %f sec.\n", totalTime / LENGTH);

    return;
}


void KAT_Blake2_Keyed (uint8_t* key, uint8_t* input, uint8_t* hash) {
    int i, j;
    struct timespec start, stop; 
	double time;
    double totalTime = 0;
    printf("Running Keyed Blake2 :)");

    for (i = 0; i < LENGTH; ++i)
    {
        if( clock_gettime(CLOCK_REALTIME, &start) == -1) {
            perror("clock gettime");
        }

        blake2b(hash, BLAKE2B_OUTBYTES, input, i, key, BLAKE2B_KEYBYTES);

        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { 
            perror("clock gettime");
        }		
		time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
        totalTime += time;

        printf("\t{\n\t\t");

        for (j = 0; j < BLAKE2B_OUTBYTES; j += 3)
        {
            // Encode the first character
            printf("%c", base64chars[hash[j] >> 2]);
            // Encode the second character
            printf("%c", base64chars[((hash[j] & 0x03) << 4) | ((hash[j + 1] & 0xf0) >> 4)]);
            // Encode the third character
            printf("%c", base64chars[((hash[j + 1] & 0x0f) << 2) | ((hash[j + 2] & 0xc0) >> 6)]);
            // Encode the fourth character
            printf("%c", base64chars[hash[j + 2] & 0x3f]);
        }

        // Print padding if necessary
        if (i < LENGTH - 1 || (BLAKE2B_OUTBYTES % 3) != 0)
        {
            int padding = ((BLAKE2B_OUTBYTES % 3) == 1) ? 2 : (((BLAKE2B_OUTBYTES % 3) == 2) ? 1 : 0);
            for (int p = 0; p < padding; p++)
            {
                printf("=");
            }
        }

        printf("\n\t},\n");

        printf("Execution Time = %f sec.\n", time);
    }

    printf("};\n\n\n\n\n");

    printf("Total Execution Time for %i hashes = %f sec.\n", LENGTH, totalTime);
    printf("Average Execution Time = %f sec.\n", totalTime / LENGTH);

    return;
}

static int get_file_size(std::string path) {
    struct stat stat_buf;

    if (stat(path.c_str(), &stat_buf) != 0) {
        return (-1);
    }

    return (int) (stat_buf.st_size);
}

void Blake2_file(std::string filename, uint8_t *hash)
{
    int i, j;
    struct timespec start, stop; 
	double time;
    double totalTime = 0;
    int iterations = 1;
    printf("Running Blake2_file :)\n\n");

    if (averageTest) {
        iterations = numIterations;
    }

    for (int i = 0; i < iterations; ++i) {
        ifstream myFile;

        myFile.open(filename, ifstream::in|ios::binary);

        if (myFile.fail()) {
            cerr << "Cannot open " << filename << ", exiting program...\n";
            exit(-1);
        }

        int bytes_remaining = get_file_size(filename);
        blake2b_state S[1];
        if (blake2b_init(S, BLAKE2B_OUTBYTES) < 0) {
            cerr << "Initialization error, exiting program...\n";
            exit(-2);
        }

        if( clock_gettime(CLOCK_REALTIME, &start) == -1) {
                perror("clock gettime");
        }

        int c = 0;
        while (bytes_remaining > 0) {
            char buff[0x1000]; // 4KB buffer

            // printf("iteration - %i\n", c);
            c++;

            int bytes_to_read = ((bytes_remaining > (int)sizeof(buff)) ? sizeof(buff) : bytes_remaining);

            myFile.read(buff, bytes_to_read);
            if (myFile.fail()) {
                break;
            }

            blake2b_update(S, (const uint8_t *) buff, bytes_to_read);
            bytes_remaining -= bytes_to_read;
        }

        blake2b_final(S, hash, BLAKE2B_OUTBYTES);

        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { 
                perror("clock gettime");
        }
        time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
        totalTime += time;

        myFile.close();

        printf("\t{\n\t\t");

        for (j = 0; j < BLAKE2B_OUTBYTES; j += 3)
        {
            // Encode the first character
            printf("%c", base64chars[hash[j] >> 2]);
            // Encode the second character
            printf("%c", base64chars[((hash[j] & 0x03) << 4) | ((hash[j + 1] & 0xf0) >> 4)]);
            // Encode the third character
            printf("%c", base64chars[((hash[j + 1] & 0x0f) << 2) | ((hash[j + 2] & 0xc0) >> 6)]);
            // Encode the fourth character
            printf("%c", base64chars[hash[j + 2] & 0x3f]);
        }

        // Print padding if necessary
        if (i < LENGTH - 1 || (BLAKE2B_OUTBYTES % 3) != 0)
        {
            int padding = ((BLAKE2B_OUTBYTES % 3) == 1) ? 2 : (((BLAKE2B_OUTBYTES % 3) == 2) ? 1 : 0);
            for (int p = 0; p < padding; p++)
            {
                printf("=");
            }
        }

        printf("\n\t}\n");

        printf("Execution Time = %f sec.\n", time);
    }

    printf("Total Execution Time for %i hashes = %f sec.\n", numIterations, totalTime);
    printf("Average Execution Time = %f sec.\n", totalTime / numIterations);

    return;
}

int main() {
    uint8_t key[64] = {0};
    uint8_t input[LENGTH] = {0};
    uint8_t hash[BLAKE2B_OUTBYTES] = {0};
    size_t i;

    for (i = 0; i < sizeof(input); ++i)
      input[i] = i;

    for (i = 0; i < sizeof(key); ++i)
      key[i] = i;

    // KAT_Blake2(input, hash);
    // KAT_Blake2_Keyed(key, input, hash);
    Blake2_file(testFile, hash);

    return 0;
}
