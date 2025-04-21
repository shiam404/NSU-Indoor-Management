#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COURTS 3
#define FILENAME "bookings.txt"

struct Booking {
    int courtNumber;
    int slotHour;
    char studentName[50];
    int studentID;
    int numPlayers;
};

void printTimeSlot(int hour) {
    printf("%d:00 - %d:00", hour, hour + 1);
}

int isSlotAvailable(int court, int hour) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) return 1;

    struct Booking booking;
    while (fread(&booking, sizeof(struct Booking), 1, file)) {
        if (booking.courtNumber == court && booking.slotHour == hour) {
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}

void bookSlot() {
    struct Booking booking;
   
    printf("Enter court number (1-3): ");
    scanf("%d", &booking.courtNumber);
   
    if (booking.courtNumber < 1 || booking.courtNumber > MAX_COURTS) {
        printf("Invalid court number!\n");
        return;
    }

    printf("Enter time slot to book (10 to 16): ");
    scanf("%d", &booking.slotHour);
   
    if (booking.slotHour < 10 || booking.slotHour > 16) {
        printf("Invalid time slot!\n");
        return;
    }

    if (!isSlotAvailable(booking.courtNumber, booking.slotHour)) {
        printf("This slot is already booked.\n");
        return;
    }

    printf("Enter your name: ");
    getchar();
    fgets(booking.studentName, sizeof(booking.studentName), stdin);
    booking.studentName[strcspn(booking.studentName, "\n")] = 0;

    printf("Enter your NSU ID: ");
    scanf("%d", &booking.studentID);

    printf("Enter number of players: ");
    scanf("%d", &booking.numPlayers);

    FILE *file = fopen(FILENAME, "ab");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fwrite(&booking, sizeof(struct Booking), 1, file);
    fclose(file);
   
    printf("Booking confirmed for Court %d at ", booking.courtNumber);
    printTimeSlot(booking.slotHour);
    printf("\n");
}

void viewAvailableSlots() {
    for (int court = 1; court <= MAX_COURTS; court++) {
        printf("\nCourt %d:\n", court);
        for (int hour = 10; hour <= 16; hour++) {
            printTimeSlot(hour);
            if (isSlotAvailable(court, hour)) {
                printf(" - Available\n");
            } else {
                printf(" - Booked\n");
            }
        }
    }
}

void showAllBookings() {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("No bookings yet!\n");
        return;
    }

    struct Booking booking;
    printf("\n--- All Bookings ---\n");
    while (fread(&booking, sizeof(struct Booking), 1, file)) {
        printf("Court %d | ", booking.courtNumber);
        printTimeSlot(booking.slotHour);
        printf(" | Name: %s | ID: %d | Players: %d\n", booking.studentName, booking.studentID, booking.numPlayers);
    }
    fclose(file);
}

void showTotalPlayers() {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("No bookings yet!\n");
        return;
    }

    struct Booking booking;
    int totalPlayers = 0;
    while (fread(&booking, sizeof(struct Booking), 1, file)) {
        totalPlayers += booking.numPlayers;
    }
    fclose(file);

    printf("\nTotal players booked today: %d\n", totalPlayers);
}

void cancelBooking() {
    int court, slot, id;
    printf("Enter court number (1-3): ");
    scanf("%d", &court);
    printf("Enter time slot to cancel (10 to 16): ");
    scanf("%d", &slot);
    printf("Enter your NSU ID: ");
    scanf("%d", &id);

    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("No bookings found!\n");
        return;
    }

    FILE *tempFile = fopen("temp.txt", "wb");
    if (tempFile == NULL) {
        printf("Error creating temporary file!\n");
        fclose(file);
        return;
    }

    struct Booking booking;
    int found = 0;
    while (fread(&booking, sizeof(struct Booking), 1, file)) {
        if (booking.courtNumber == court && booking.slotHour == slot && booking.studentID == id) {
            found = 1;
        } else {
            fwrite(&booking, sizeof(struct Booking), 1, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found) {
        printf("Booking canceled successfully.\n");
    } else {
        printf("No matching booking found.\n");
    }
}

int main() {
    int choice;
   
    do {
        printf("\n--- NSU Indoor Management System ---\n");
        printf("1. Book a time slot\n");
        printf("2. View available time slots\n");
        printf("3. Show all current bookings\n");
        printf("4. Show total players booked today\n");
        printf("5. Cancel a booking\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                bookSlot();
                break;
            case 2:
                viewAvailableSlots();
                break;
            case 3:
                showAllBookings();
                break;
            case 4:
                showTotalPlayers();
                break;
            case 5:
                cancelBooking();
                break;
            case 6:
                printf("Exiting... Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);

    return 0;
}