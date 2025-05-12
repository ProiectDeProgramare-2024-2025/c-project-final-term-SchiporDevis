#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const char clr[] = "cls";

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

#define MAX_PHOTOS 100
#define MAX_ALBUMS 10
#define MAX_NAME 100

typedef struct photo {
    char name[MAX_NAME];
    int memory;
} photo;

typedef struct album {
    char album_name[MAX_NAME];
    int photos_count;
    photo album_photos[MAX_PHOTOS];
} album;

void pressEnter() {
    printf("\nPress ENTER to return to main menu...");
    getchar(); getchar();
}

void clearScreen() {
    system(clr);
}

int findPhoto(photo photos[], int count, const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(photos[i].name, name) == 0)
            return i;
    }
    return -1;
}

int loadAlbums(album albums[]) {
    FILE *file = fopen("albums.txt", "r");
    int count = 0;
    if (file == NULL) return 0;
    while (fscanf(file, "%s %d", albums[count].album_name, &albums[count].photos_count) == 2) {
        for (int i = 0; i < albums[count].photos_count; i++) {
            fscanf(file, "%s %d", albums[count].album_photos[i].name, &albums[count].album_photos[i].memory);
        }
        count++;
    }
    fclose(file);
    return count;
}

void saveAlbums(album albums[], int count) {
    FILE *file = fopen("albums.txt", "w");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %d\n", albums[i].album_name, albums[i].photos_count);
        for (int j = 0; j < albums[i].photos_count; j++) {
            fprintf(file, "%s %d\n", albums[i].album_photos[j].name, albums[i].album_photos[j].memory);
        }
    }
    fclose(file);
}

void showMenu() {
    clearScreen();
    printf(BLUE "-------Main Menu-------\n\n" RESET);
    printf("1) Photo list\n");
    printf("2) Album list\n");
    printf("3) Add photo to list\n");
    printf("4) Delete photo from list\n");
    printf("5) Create album\n");
    printf("6) Delete album\n");
    printf("7) Add photo to album\n");
    printf("8) Delete photo from album\n");
    printf("9) Exit program\n\n");
    printf("Select an option (1-9): ");
}

void showPhotos(photo photos[], int count) {
    clearScreen();
    printf(BLUE "-------Photo list-------\n\n" RESET);
    for (int i = 0; i < count; i++) {
        printf("%4d. " CYAN "%s" RESET " - " GREEN "%dmb\n" RESET, i + 1, photos[i].name, photos[i].memory);
    }
}

void addPhoto(photo photos[], int *count) {
    clearScreen();
    printf(BLUE "-------Add photo to list-------\n\n" RESET);
    char line[MAX_NAME];
    int mem;
    getchar();
    printf("Please enter photo name: ");
    fgets(line, sizeof(line), stdin);
    line[strcspn(line, "\n")] = 0;

    if (findPhoto(photos, *count, line) != -1) {
        printf(RED "%s already exists.\n" RESET, line);
        return;
    }

    printf("Please enter photo's memory: ");
    scanf("%d", &mem);
    FILE *append_photos = fopen("photos.txt", "a");
    fprintf(append_photos, "%s\n%d\n", line, mem);
    fclose(append_photos);
    strcpy(photos[*count].name, line);
    photos[*count].memory = mem;
    (*count)++;
    printf(GREEN "Photo %s was added successfully.\n" RESET, line);
}

void deletePhoto(photo photos[], int *count) {
    clearScreen();
    printf(BLUE "-------Delete photo from list-------\n\n" RESET);
    getchar();
    char line[MAX_NAME];
    printf("Please enter photo name: ");
    fgets(line, sizeof(line), stdin);
    line[strcspn(line, "\n")] = 0;
    int index = findPhoto(photos, *count, line);
    if (index == -1) {
        printf(RED "There is no photo named %s.\n" RESET, line);
        return;
    }
    for (int j = index; j < (*count) - 1; j++) {
        photos[j] = photos[j + 1];
    }
    (*count)--;
    FILE *file = fopen("photos.txt", "w");
    for (int i = 0; i < *count; i++) {
        fprintf(file, "%s\n%d\n", photos[i].name, photos[i].memory);
    }
    fclose(file);
    printf(GREEN "%s was successfully deleted.\n" RESET, line);
}

void showAlbum(album alb) {
    clearScreen();
    printf(BLUE "-------Album: " YELLOW "%s" BLUE "-------\n\n" RESET, alb.album_name);
    int total = 0;
    for (int i = 0; i < alb.photos_count; i++) {
        printf("%4d. " CYAN "%s" RESET " - " GREEN "%dmb\n" RESET, i + 1, alb.album_photos[i].name, alb.album_photos[i].memory);
        total += alb.album_photos[i].memory;
    }
    printf("\nTotal memory: " MAGENTA "%dmb\n" RESET, total);
}

void showAlbums(album albums[], int count) {
    clearScreen();
    if (count == 0) {
        printf(RED "No albums yet.\n" RESET);
        return;
    }
    printf(BLUE "-------Album list-------\n\n" RESET);
    for (int i = 0; i < count; i++) {
        printf("%4d. " YELLOW "%s" RESET " (" CYAN "%d photos" RESET ")\n", i + 1, albums[i].album_name, albums[i].photos_count);
    }
    char s;
    char pick[MAX_NAME];
    printf("\nDo you want to open an album? (y/n): ");
    getchar();
    scanf("%c", &s);
    if (s == 'y') {
        printf("Enter album name: ");
        scanf("%s", pick);
        for (int i = 0; i < count; i++) {
            if (strcmp(albums[i].album_name, pick) == 0) {
                showAlbum(albums[i]);
                return;
            }
        }
        printf(RED "There is no album named %s.\n" RESET, pick);
    }
}

void createAlbum(album albums[], int *count) {
    clearScreen();
    char name[100];
    printf("Please enter album name: ");
    scanf("%s", name);
    for (int i = 0; i < *count; i++) {
        if (strcmp(albums[i].album_name, name) == 0) {
            printf("Album already exists.\n");
            return;
        }
    }
    strcpy(albums[*count].album_name, name);
    albums[*count].photos_count = 0;
    (*count)++;
    saveAlbums(albums, *count);
    printf("Album %s was created succesfully.\n", name);
}

void deleteAlbum(album albums[], int *count) {
    clearScreen();
    char name[100];
    printf("Please enter album name to delete: ");
    scanf("%s", name);
    for (int i = 0; i < *count; i++) {
        if (strcmp(albums[i].album_name, name) == 0) {
            for (int j = i; j < (*count) - 1; j++) {
                albums[j] = albums[j + 1];
            }
            (*count)--;
            saveAlbums(albums, *count);
            printf("Album %s was deleted succesfully.\n", name);
            return;
        }
    }
    printf("Album not found.\n");
}

void addToAlbum(photo photos[], int photo_count, album albums[], int album_count) {
    clearScreen();
    char photoName[100], albumName[100];
    printf("Enter photo name: ");
    scanf("%s", photoName);
    printf("Enter album name: ");
    scanf("%s", albumName);

    int photoIndex = findPhoto(photos, photo_count, photoName);
    if (photoIndex == -1) {
        printf("Photo does not exist in list.\n");
        return;
    }

    for (int i = 0; i < album_count; i++) {
        if (strcmp(albums[i].album_name, albumName) == 0) {
            for (int j = 0; j < albums[i].photos_count; j++) {
                if (strcmp(albums[i].album_photos[j].name, photoName) == 0) {
                    printf("Photo already in album.\n");
                    return;
                }
            }
            albums[i].album_photos[albums[i].photos_count++] = photos[photoIndex];
            saveAlbums(albums, album_count);
            printf("Photo %s added to album %s.\n", photoName, albumName);
            return;
        }
    }
    printf("Album not found.\n");
}

void deleteFromAlbum(album albums[], int album_count) {
    clearScreen();
    char photoName[100], albumName[100];
    printf("Enter photo name: ");
    scanf("%s", photoName);
    printf("Enter album name: ");
    scanf("%s", albumName);

    for (int i = 0; i < album_count; i++) {
        if (strcmp(albums[i].album_name, albumName) == 0) {
            for (int j = 0; j < albums[i].photos_count; j++) {
                if (strcmp(albums[i].album_photos[j].name, photoName) == 0) {
                    for (int k = j; k < albums[i].photos_count - 1; k++) {
                        albums[i].album_photos[k] = albums[i].album_photos[k + 1];
                    }
                    albums[i].photos_count--;
                    saveAlbums(albums, album_count);
                    printf("Photo %s removed from album %s.\n", photoName, albumName);
                    return;
                }
            }
            printf("Photo not found in album.\n");
            return;
        }
    }
    printf("Album not found.\n");
}

void select_option(photo photos[], int photos_count) {
    int option = 0;
    album albums[100];
    int album_count = loadAlbums(albums);
    while (true) {
        showMenu();
        scanf("%d", &option);
        switch (option) {
            case 1:
                showPhotos(photos, photos_count);
                pressEnter();
                break;
            case 2:
                showAlbums(albums, album_count);
                pressEnter();
                break;
            case 3:
                addPhoto(photos, &photos_count);
                pressEnter();
                break;
            case 4:
                deletePhoto(photos, &photos_count);
                pressEnter();
                break;
            case 5:
                createAlbum(albums, &album_count);
                pressEnter();
                break;
            case 6:
                deleteAlbum(albums, &album_count);
                pressEnter();
                break;
            case 7:
                addToAlbum(photos, photos_count, albums, album_count);
                pressEnter();
                break;
            case 8:
                deleteFromAlbum(albums, album_count);
                pressEnter();
                break;
            case 9:
                return;
            default:
                printf("Invalid option.\n");
                pressEnter();
        }
    }
}

int main() {
    photo photos[100];
    FILE *read_photos = fopen("photos.txt", "r");
    if (read_photos == NULL) {
        perror("Eroare la deschiderea fisierului");
        return 1;
    }
    char line[100];
    int mem;
    int photos_count = 0;
    while (fgets(line, sizeof(line), read_photos)) {
        line[strlen(line) - 1] = 0;
        fscanf(read_photos, "%d\n", &mem);
        strcpy(photos[photos_count].name, line);
        photos[photos_count++].memory = mem;
    }
    fclose(read_photos);
    select_option(photos, photos_count);
    return 0;
}
