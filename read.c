#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isspace and isdigit

int main() {
    FILE *file;
    char filename[] = "menu.txt"; // Adjust to your file path
    char buffer[256];
    char *inrPtr, *priceStart, *priceEnd;
    int price;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Find "INR" in the buffer
        inrPtr = strstr(buffer, "INR");
        if (inrPtr != NULL) {
            // Move backward to find the start of the price
            for (priceEnd = inrPtr - 1; priceEnd > buffer && isspace(*(priceEnd - 1)); priceEnd--);
            for (priceStart = priceEnd; priceStart > buffer && !isspace(*(priceStart - 1)); priceStart--);

            // Extract and print the price
            char priceStr[10] = {0}; // Assuming price won't be longer than 9 digits
            strncpy(priceStr, priceStart, priceEnd - priceStart);
            price = atoi(priceStr);
            printf("Price: %d INR\n", price);
        }
    }

    fclose(file);
    return 0;
}

