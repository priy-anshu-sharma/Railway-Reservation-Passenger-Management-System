#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRAINS 20
#define TRAIN_CAPACITY 50
#define DATA_FILE "reservations.dat"
#define TRAIN_DEFINITION_FILE "trains.dat"

struct Train {
    int train_no;
    char name[50];
    int total_seats;
    int available_seats;
};

struct Passenger {
    int pnr;
    char name[50];
    int age;
    int train_no;
    int seat_no;
    char status[20];
};

int safe_read_int(const char *prompt) {
    int value;
    printf("%s", prompt);
    if (scanf("%d", &value) != 1) {
        while (getchar() != '\n');
        printf("\nInvalid input. Please enter a number.\n");
        return -1;
    }
    while (getchar() != '\n');
    return value;
}

void safe_read_string(const char *prompt, char *buffer, size_t max_size) {
    printf("%s", prompt);
    if (fgets(buffer, max_size, stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
    } else {
        buffer[0] = '\0';
    }
}

int get_last_pnr() {
    FILE *fp = fopen(DATA_FILE, "rb");
    struct Passenger p;
    int last_pnr = 1000;
    if (fp) {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        if (size > 0) {
            fseek(fp, -sizeof(struct Passenger), SEEK_END);
            fread(&p, sizeof(struct Passenger), 1, fp);
            last_pnr = p.pnr;
        }
        fclose(fp);
    }
    return last_pnr;
}

void save_reservation(struct Passenger p) {
    FILE *fp = fopen(DATA_FILE, "ab");
    if (fp == NULL) {
        printf("\nError: Could not open data file for saving.\n");
        return;
    }
    fwrite(&p, sizeof(struct Passenger), 1, fp);
    fclose(fp);
}

void update_reservation_in_file(int target_pnr, struct Passenger updated_p) {
    FILE *fp_read = fopen(DATA_FILE, "rb");
    FILE *fp_temp = fopen("temp.dat", "wb");
    struct Passenger p;

    if (fp_read == NULL || fp_temp == NULL) {
        printf("\nError updating file.\n");
        if (fp_read) fclose(fp_read);
        if (fp_temp) fclose(fp_temp);
        return;
    }

    while (fread(&p, sizeof(struct Passenger), 1, fp_read)) {
        if (p.pnr == target_pnr) {
            fwrite(&updated_p, sizeof(struct Passenger), 1, fp_temp);
        } else {
            fwrite(&p, sizeof(struct Passenger), 1, fp_temp);
        }
    }

    fclose(fp_read);
    fclose(fp_temp);
    remove(DATA_FILE);
    rename("temp.dat", DATA_FILE);
}

int save_train_data(const struct Train trains[], int count) {
    FILE *fp = fopen(TRAIN_DEFINITION_FILE, "wb");
    if (!fp) return 0;
    fwrite(trains, sizeof(struct Train), count, fp);
    fclose(fp);
    return 1;
}

int load_train_data(struct Train trains[]) {
    FILE *fp = fopen(TRAIN_DEFINITION_FILE, "rb");
    int count = 0;
    if (fp) {
        // Read up to MAX_TRAINS structs
        while (fread(&trains[count], sizeof(struct Train), 1, fp) == 1 && count < MAX_TRAINS) {
            count++;
        }
        fclose(fp);
        return count;
    } else {
        // Initialize default trains if file doesn't exist
        trains[0].train_no = 1201; strcpy(trains[0].name, "Express"); trains[0].total_seats = TRAIN_CAPACITY;
        trains[1].train_no = 1202; strcpy(trains[1].name, "Superfast"); trains[1].total_seats = TRAIN_CAPACITY;
        trains[2].train_no = 1203; strcpy(trains[2].name, "Mail"); trains[2].total_seats = TRAIN_CAPACITY;
        save_train_data(trains, 3);
        return 3;
    }
}

void load_train_availability(struct Train trains[], int train_count) {
    struct Passenger p;
    FILE *fp = fopen(DATA_FILE, "rb");
    int i;

    for (i = 0; i < train_count; i++) {
        trains[i].available_seats = TRAIN_CAPACITY;
    }

    if (fp) {
        while (fread(&p, sizeof(struct Passenger), 1, fp)) {
            if (strcmp(p.status, "BOOKED") == 0) {
                for (i = 0; i < train_count; i++) {
                    if (trains[i].train_no == p.train_no) {
                        trains[i].available_seats--;
                        break;
                    }
                }
            }
        }
        fclose(fp);
    }
}

void display_trains(const struct Train trains[], int train_count) {
    printf("\n\n+-------------------------------------------------------------+\n");
    printf("|                    Available Trains List (%d Total)           |\n", train_count);
    printf("+-------+-------------------+---------------+-----------------+\n");
    printf("| Train | Name              | Total Seats   | Available Seats |\n");
    printf("| No.   |                   |               |                 |\n");
    printf("+-------+-------------------+---------------+-----------------+\n");
    for (int i = 0; i < train_count; i++) {
        printf("| %-5d | %-17s | %-13d | %-15d |\n",
               trains[i].train_no, trains[i].name, trains[i].total_seats, trains[i].available_seats);
    }
    printf("+-------+-------------------+---------------+-----------------+\n");
}

void reserve_ticket(struct Train trains[], int train_count) {
    int chosen_train_no, train_index = -1;
    char name[50];
    int age;

    display_trains(trains, train_count);

    chosen_train_no = safe_read_int("\nEnter Train Number to book: ");
    if (chosen_train_no == -1) return;

    for (int i = 0; i < train_count; i++) {
        if (trains[i].train_no == chosen_train_no) {
            train_index = i;
            break;
        }
    }

    if (train_index == -1) { printf("\nTrain number not found.\n"); return; }
    if (trains[train_index].available_seats <= 0) { printf("\nSorry, all seats are booked on this train.\n"); return; }

    safe_read_string("Enter Passenger Name: ", name, 50);
    if (name[0] == '\0') { printf("\nReservation failed. Name cannot be empty.\n"); return; }

    age = safe_read_int("Enter Passenger Age: ");
    if (age <= 0) { printf("\nInvalid age. Reservation failed.\n"); return; }

    struct Passenger new_p;
    new_p.pnr = get_last_pnr() + 1;
    strcpy(new_p.name, name);
    new_p.age = age;
    new_p.train_no = chosen_train_no;
    new_p.seat_no = TRAIN_CAPACITY - trains[train_index].available_seats + 1;
    strcpy(new_p.status, "BOOKED");

    save_reservation(new_p);
    load_train_availability(trains, train_count);

    printf("\n\n<<< SUCCESS: Ticket Booked! >>>\n");
    printf("PNR: %d, Train: %s (%d), Seat No: %d\n",
           new_p.pnr, trains[train_index].name, new_p.train_no, new_p.seat_no);
}

void cancel_ticket(struct Train trains[], int train_count) {
    int pnr_to_cancel = safe_read_int("\nEnter PNR of the ticket to cancel: ");
    if (pnr_to_cancel == -1) return;

    FILE *fp = fopen(DATA_FILE, "rb");
    struct Passenger p;
    int found = 0;

    if (!fp) { printf("\nNo active reservations found.\n"); return; }

    while (fread(&p, sizeof(struct Passenger), 1, fp)) {
        if (p.pnr == pnr_to_cancel && strcmp(p.status, "BOOKED") == 0) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (found) {
        strcpy(p.status, "CANCELLED");
        update_reservation_in_file(pnr_to_cancel, p);
        load_train_availability(trains, train_count);
        printf("\n<<< SUCCESS: Ticket PNR %d has been cancelled. >>>\n", pnr_to_cancel);
    } else {
        printf("\nError: PNR %d not found or already cancelled.\n", pnr_to_cancel);
    }
}

void view_ticket() {
    int pnr_to_view = safe_read_int("\nEnter PNR to view: ");
    if (pnr_to_view == -1) return;

    struct Passenger p;
    int found = 0;
    FILE *fp = fopen(DATA_FILE, "rb");

    if (!fp) { printf("\nNo reservations recorded.\n"); return; }

    while (fread(&p, sizeof(struct Passenger), 1, fp)) {
        if (p.pnr == pnr_to_view) {
            printf("\n+---------------------------------------------------+\n");
            printf("|                 TICKET DETAILS                    |\n");
            printf("+---------------------------------------------------+\n");
            printf("| PNR Number: %-36d |\n", p.pnr);
            printf("| Passenger Name: %-34s |\n", p.name);
            printf("| Age: %-43d |\n", p.age);
            printf("| Train Number: %-36d |\n", p.train_no);
            printf("| Seat Number: %-37d |\n", p.seat_no);
            printf("| Status: %-40s |\n", p.status);
            printf("+---------------------------------------------------+\n");
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("\nError: PNR %d not found in the system.\n", pnr_to_view);
    }
}

void view_all_reservations(const struct Train trains[], int train_count) {
    struct Passenger p;
    int count = 0;

    printf("\n\n+-------------------------------------------------------------------------------------------------------+\n");
    printf("|                                        ALL RESERVATION RECORDS                                        |\n");
    printf("+------+--------------------+-----+----------+-----------+------------+-----------------------------------+\n");
    printf("| PNR  | Passenger Name     | Age | Train No | Train Name| Seat No    | Status                            |\n");
    printf("+------+--------------------+-----+----------+-----------+------------+-----------------------------------+\n");

    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) {
        printf("|                                 NO RESERVATIONS FOUND YET                                           |\n");
        printf("+------+--------------------+-----+----------+-----------+------------+-----------------------------------+\n");
        return;
    }

    while (fread(&p, sizeof(struct Passenger), 1, fp)) {
        char train_name[50] = "Unknown";
        for (int i = 0; i < train_count; i++) {
            if (trains[i].train_no == p.train_no) {
                strcpy(train_name, trains[i].name);
                break;
            }
        }
        printf("| %-4d | %-18s | %-3d | %-8d | %-9s | %-10d | %-31s |\n",
               p.pnr, p.name, p.age, p.train_no, train_name, p.seat_no, p.status);
        count++;
    }
    printf("+------+--------------------+-----+----------+-----------+------------+-----------------------------------+\n");
    printf("| Total Records: %-81d |\n", count);
    printf("+-------------------------------------------------------------------------------------------------------+\n");
    fclose(fp);
}

void update_passenger_details() {
    int pnr_to_update = safe_read_int("\nEnter PNR of the reservation to update: ");
    if (pnr_to_update == -1) return;

    FILE *fp = fopen(DATA_FILE, "rb");
    struct Passenger p;
    int found = 0;

    if (!fp) { printf("\nNo reservations recorded.\n"); return; }

    while (fread(&p, sizeof(struct Passenger), 1, fp)) {
        if (p.pnr == pnr_to_update) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) { printf("\nError: PNR %d not found.\n", pnr_to_update); return; }

    printf("\nFound Reservation (Status: %s) - Current Name: %s, Age: %d\n",
           p.status, p.name, p.age);

    char new_name[50];
    safe_read_string("Enter New Name (leave blank to keep current): ", new_name, 50);
    if (new_name[0] != '\0') {
        strcpy(p.name, new_name);
    }

    int new_age = safe_read_int("Enter New Age (0 to keep current): ");
    if (new_age > 0) {
        p.age = new_age;
    }

    update_reservation_in_file(pnr_to_update, p);
    printf("\n<<< SUCCESS: PNR %d details updated! >>>\n", pnr_to_update);
}

void search_by_name(const struct Train trains[], int train_count) {
    char search_name[50];
    struct Passenger p;
    int count = 0;

    safe_read_string("\nEnter Passenger Name (or part of name) to search: ", search_name, 50);
    if (search_name[0] == '\0') {
        printf("\nSearch name cannot be empty.\n");
        return;
    }

    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) {
        printf("\nNo reservations recorded.\n");
        return;
    }

    printf("\n\n+-------------------------------------------------------------------------------------------------------+\n");
    printf("|                                        SEARCH RESULTS BY NAME                                         |\n");
    printf("+------+--------------------+-----+----------+-----------+------------+-----------------------------------+\n");
    printf("| PNR  | Passenger Name     | Age | Train No | Train Name| Seat No    | Status                            |\n");
    printf("+------+--------------------+-----+----------+-----------+------------+-----------------------------------+\n");

    while (fread(&p, sizeof(struct Passenger), 1, fp)) {
        // Use strstr for partial match (case-sensitive)
        if (strstr(p.name, search_name) != NULL) {
            char train_name[50] = "Unknown";
            for (int i = 0; i < train_count; i++) {
                if (trains[i].train_no == p.train_no) {
                    strcpy(train_name, trains[i].name);
                    break;
                }
            }

            printf("| %-4d | %-18s | %-3d | %-8d | %-9s | %-10d | %-31s |\n",
                   p.pnr, p.name, p.age, p.train_no, train_name, p.seat_no, p.status);
            count++;
        }
    }
    fclose(fp);

    printf("+------+--------------------+-----+----------+-----------+------------+-----------------------------------+\n");
    printf("| Found Records: %-82d |\n", count);
    printf("+-------------------------------------------------------------------------------------------------------+\n");

    if (count == 0) {
        printf("\nNo records found matching '%s'.\n", search_name);
    }
}

void cleanup_records(struct Train trains[], int train_count) {
    FILE *fp_read = fopen(DATA_FILE, "rb");
    if (!fp_read) {
        printf("\nNo data file found to clean up.\n");
        return;
    }

    FILE *fp_temp = fopen("temp.dat", "wb");
    if (!fp_temp) {
        printf("\nError: Could not open temporary file for cleanup.\n");
        fclose(fp_read);
        return;
    }

    struct Passenger p;
    int cleaned_count = 0;
    int total_kept = 0;

    while (fread(&p, sizeof(struct Passenger), 1, fp_read)) {
        if (strcmp(p.status, "CANCELLED") == 0) {
            cleaned_count++; // Don't write to temp file
        } else {
            fwrite(&p, sizeof(struct Passenger), 1, fp_temp);
            total_kept++;
        }
    }

    fclose(fp_read);
    fclose(fp_temp);

    if (cleaned_count > 0) {
        remove(DATA_FILE);
        rename("temp.dat", DATA_FILE);
        printf("\n\n<<< SUCCESS: Data Cleanup Complete! >>>\n");
        printf("%d CANCELLED records were permanently removed.\n", cleaned_count);
        printf("%d active records remain in the system.\n", total_kept);
    } else {
        remove("temp.dat"); 
        printf("\n\nNo CANCELLED records were found to remove. File remains unchanged.\n");
    }

    load_train_availability(trains, train_count); 
}

void add_new_train(struct Train trains[], int *train_count_ptr) {
    if (*train_count_ptr >= MAX_TRAINS) {
        printf("\nMaximum number of trains (%d) reached. Cannot add more.\n", MAX_TRAINS);
        return;
    }

    printf("\n--- Add New Train ---\n");
    int new_train_no = safe_read_int("Enter New Train Number (e.g., 2001): ");
    if (new_train_no <= 0) return;

    // Check for duplicate train number
    for (int i = 0; i < *train_count_ptr; i++) {
        if (trains[i].train_no == new_train_no) {
            printf("\nError: Train Number %d already exists.\n", new_train_no);
            return;
        }
    }

    char new_name[50];
    safe_read_string("Enter Train Name (e.g., Shatabdi): ", new_name, 50);
    if (new_name[0] == '\0') {
        printf("\nTrain name cannot be empty. Addition failed.\n");
        return;
    }

    trains[*train_count_ptr].train_no = new_train_no;
    strcpy(trains[*train_count_ptr].name, new_name);
    trains[*train_count_ptr].total_seats = TRAIN_CAPACITY;
    trains[*train_count_ptr].available_seats = TRAIN_CAPACITY; // Freshly added train has full capacity

    (*train_count_ptr)++;

    save_train_data(trains, *train_count_ptr);
    printf("\n<<< SUCCESS: New Train %d (%s) added to the system! >>>\n", new_train_no, new_name);
}

void delete_database(struct Train trains[], int *train_count_ptr) {
    char confirm[10];
    safe_read_string("\nAre you sure you want to DELETE ALL DATA? (reservations and train definitions) Type 'YES' to confirm: ", confirm, 10);

    if (strcmp(confirm, "YES") != 0) {
        printf("\nDatabase deletion cancelled.\n");
        return;
    }

    if (remove(DATA_FILE) == 0) {
        printf("\nReservation database (%s) successfully deleted.\n", DATA_FILE);
    } else {
        printf("\nReservation database (%s) not found or could not be deleted.\n", DATA_FILE);
    }
    
    if (remove(TRAIN_DEFINITION_FILE) == 0) {
        printf("Train definitions (%s) successfully deleted.\n", TRAIN_DEFINITION_FILE);
    } else {
        printf("Train definitions (%s) not found or could not be deleted.\n", TRAIN_DEFINITION_FILE);
    }

    // Re-initialize the system state
    *train_count_ptr = load_train_data(trains); // This recreates default trains
    load_train_availability(trains, *train_count_ptr);
    printf("\nSystem reset to default state.\n");
}


void display_menu() {
    printf("\n======================================================\n");
    printf("|      RAILWAY RESERVATION & MANAGEMENT SYSTEM (V4)  |\n");
    printf("======================================================\n");
    printf("| 1. View Available Trains                           |\n");
    printf("| 2. Book a Ticket (Reservation)                     |\n");
    printf("| 3. Cancel a Ticket                                 |\n");
    printf("| 4. View Specific Ticket (by PNR)                   |\n");
    printf("| 5. View All Reservation Records                    |\n");
    printf("| 6. Update Passenger Details (Name/Age)             |\n");
    printf("| 7. Search Reservation by Passenger Name            |\n");
    printf("| 8. Cleanup Cancelled Records (Maintenance)         |\n");
    printf("+----------------------------------------------------+\n");
    printf("| * SCALABILITY & MAINTENANCE * |\n");
    printf("| 9. Add New Train Details                           |\n");
    printf("| 10. DELETE ALL DATA (Database Reset)               |\n");
    printf("+----------------------------------------------------+\n");
    printf("| 11. Exit System                                    |\n");
    printf("======================================================\n");
    printf("Enter your choice: ");
}

int main() {
    struct Train trains[MAX_TRAINS];
    int current_train_count;

    // Load initial data
    current_train_count = load_train_data(trains);
    load_train_availability(trains, current_train_count);

    int choice;
    do {
        display_menu();
        choice = safe_read_int("");

        switch (choice) {
            case 1:
                display_trains(trains, current_train_count);
                break;
            case 2:
                reserve_ticket(trains, current_train_count);
                break;
            case 3:
                cancel_ticket(trains, current_train_count);
                break;
            case 4:
                view_ticket();
                break;
            case 5:
                view_all_reservations(trains, current_train_count);
                break;
            case 6:
                update_passenger_details();
                break;
            case 7:
                search_by_name(trains, current_train_count);
                break;
            case 8:
                cleanup_records(trains, current_train_count);
                break;
            case 9:
                add_new_train(trains, &current_train_count);
                break;
            case 10:
                delete_database(trains, &current_train_count);
                break;
            case 11:
                printf("\nThank you for using the Railway Management System. Goodbye!\n");
                break;
            default:
                if (choice != -1) {
                    printf("\nInvalid choice. Please enter a number between 1 and 11.\n");
                }
        }
        if (choice != 11) {
            printf("\nPress ENTER to continue...");
            while(getchar() != '\n');
        }
    } while (choice != 11);

    return 0;
}
