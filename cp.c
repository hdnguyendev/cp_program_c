// copy file 1 - 1
// copy multi file to folder
// copy folder to folder
// copy folder to folder + ignore
// copy folder - no overwrite

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_FILES 10
#define MAX_FILENAME_LENGTH 100

int fileExists(const char *path)
{
    struct stat fileStat;
    return stat(path, &fileStat) == 0;
}
int shouldIgnore(const char *name)
{
    FILE *ignoreFile = fopen(".cpignore", "r");
    if (ignoreFile == NULL)
    {
        // Không tìm thấy tệp .cpignore, không có mục nào bị bỏ qua
        return 0;
    }

    char ignoreEntry[PATH_MAX];
    while (fgets(ignoreEntry, PATH_MAX, ignoreFile) != NULL)
    {
        // Loại bỏ ký tự xuống dòng
        ignoreEntry[strcspn(ignoreEntry, "\n")] = 0;
        if (strcmp(name, ignoreEntry) == 0)
        {
            fclose(ignoreFile);
            return 1;
        }
    }

    fclose(ignoreFile);
    return 0;
}

void copyDirectory(const char *sourcePath, const char *destinationPath, int ignore)
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char sourceFilePath[PATH_MAX];
    char destinationFilePath[PATH_MAX];

    mkdir(destinationPath, 0700);

    dir = opendir(sourcePath);
    if (dir == NULL)
    {
        printf("Không thể mở thư mục nguồn.\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        sprintf(sourceFilePath, "%s/%s", sourcePath, entry->d_name);
        sprintf(destinationFilePath, "%s/%s", destinationPath, entry->d_name);
        stat(sourceFilePath, &fileStat);

        if (S_ISDIR(fileStat.st_mode))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if (ignore == 1)
            {
                if (shouldIgnore(entry->d_name))
                    continue;
            }

            copyDirectory(sourceFilePath, destinationFilePath, ignore);
        }
        else if (S_ISREG(fileStat.st_mode))
        {
            if (ignore == 1)
            {
                if (shouldIgnore(entry->d_name))
                    continue;
            }
            copyFile(sourceFilePath, destinationFilePath);
        }
    }

    closedir(dir);
}
void copyDirectoryNoOverwrite(const char *sourcePath, const char *destinationPath, int ignore)
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char sourceFilePath[PATH_MAX];
    char destinationFilePath[PATH_MAX];

    mkdir(destinationPath, 0700);

    dir = opendir(sourcePath);
    if (dir == NULL)
    {
        printf("Không thể mở thư mục nguồn.\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        sprintf(sourceFilePath, "%s/%s", sourcePath, entry->d_name);
        sprintf(destinationFilePath, "%s/%s", destinationPath, entry->d_name);
        stat(sourceFilePath, &fileStat);

        if (S_ISDIR(fileStat.st_mode))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if (ignore == 1)
            {
                // Kiểm tra xem thư mục con có nên bị bỏ qua hay không
                if (shouldIgnore(entry->d_name))
                    continue;
            }

            copyDirectoryNoOverwrite(sourceFilePath, destinationFilePath, ignore);
        }
        else if (S_ISREG(fileStat.st_mode))
        {
            if (ignore == 1)
            {
                // Kiểm tra xem tệp tin có nên bị bỏ qua hay không
                if (shouldIgnore(entry->d_name))
                    continue;
            }

            // Kiểm tra xem tệp tin đích đã tồn tại chưa
            if (fileExists(destinationFilePath))
            {
                // Tệp tin đích đã tồn tại, bỏ qua
                continue;
            }

            copyFile(sourceFilePath, destinationFilePath);
        }
    }

    closedir(dir);
}
void copyFile(const char *sourcePath, const char *destinationPath)
{
    FILE *sourceFile = fopen(sourcePath, "r");
    if (sourceFile == NULL)
    {
        printf("Không thể mở file gốc.\n");
        return;
    }

    FILE *destinationFile = fopen(destinationPath, "w");
    if (destinationFile == NULL)
    {
        printf("Không thể mở file đích.\n");
        return;
    }

    char ch;
    while ((ch = fgetc(sourceFile)) != EOF)
    {
        fputc(ch, destinationFile);
    }

    printf("File đã được sao chép thành công.\n");

    fclose(sourceFile);
    fclose(destinationFile);
}
void copyFilesToFolder()
{
    char sourceFiles[MAX_FILES][MAX_FILENAME_LENGTH];
    char destinationFolder[MAX_FILENAME_LENGTH];

    int numFiles;

    printf("Nhập số lượng tệp tin: ");
    scanf("%d", &numFiles);

    if (numFiles <= 0 || numFiles > MAX_FILES)
    {
        printf("Số lượng tệp tin không hợp lệ.\n");
        return;
    }

    for (int i = 0; i < numFiles; i++)
    {
        printf("Nhập tên tệp tin %d: ", i + 1);
        scanf("%s", sourceFiles[i]);
    }

    printf("Nhập tên thư mục đích: ");
    scanf("%s", destinationFolder);

    for (int i = 0; i < numFiles; i++)
    {
        // Tạo đường dẫn đầy đủ cho tệp tin nguồn và thư mục đích
        char sourcePath[MAX_FILENAME_LENGTH + 100];
        sprintf(sourcePath, "%s/%s", sourceFiles[i], destinationFolder);

        // Mở tệp tin nguồn để đọc
        FILE *file = fopen(sourceFiles[i], "r");
        if (file == NULL)
        {
            printf("Không thể mở tệp tin nguồn %s.\n", sourceFiles[i]);
            continue;
        }

        // Tạo tên tệp tin đích
        char destinationPath[MAX_FILENAME_LENGTH + 100];
        sprintf(destinationPath, "%s/%s", destinationFolder, sourceFiles[i]);

        // Mở tệp tin đích để ghi
        FILE *destinationFile = fopen(destinationPath, "w");
        if (destinationFile == NULL)
        {
            printf("Không thể mở tệp tin đích %s.\n", sourceFiles[i]);
            fclose(file);
            continue;
        }

        // Sao chép nội dung từ tệp tin nguồn sang tệp tin đích
        int character;
        while ((character = fgetc(file)) != EOF)
        {
            fputc(character, destinationFile);
        }

        // Đóng tệp tin
        fclose(file);
        fclose(destinationFile);

        printf("Sao chép tệp tin %s thành công.\n", sourceFiles[i]);
    }
}
void printDirectoryContents(const char *directoryPath, int indentLevel)
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    struct passwd *user;
    struct group *group;
    struct tm *modificationTime;

    dir = opendir(directoryPath);
    if (dir == NULL)
    {
        printf("Không thể mở thư mục.\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char entryPath[256];
        sprintf(entryPath, "%s/%s", directoryPath, entry->d_name);

        if (stat(entryPath, &fileStat) == -1)
        {
            printf("Không thể lấy thông tin cho %s\n", entry->d_name);
            continue;
        }

        user = getpwuid(fileStat.st_uid);
        group = getgrgid(fileStat.st_gid);
        modificationTime = localtime(&fileStat.st_mtime);

        for (int i = 0; i < indentLevel; i++)
        {
            printf("  ");
        }

        printf("|-- %s", entry->d_name);
        if (S_ISDIR(fileStat.st_mode))
        {
            printf("/");
        }
        printf("\n");

        if (S_ISDIR(fileStat.st_mode))
        {
            printDirectoryContents(entryPath, indentLevel + 1);
        }
    }

    closedir(dir);
}
int main()
{
    char source[PATH_MAX];
    char destination[PATH_MAX];
    int ignore;
    int choice;

    while (1)
    {
        printf("=== Menu === \n");
        printf("1. Copy file 1-1\n");
        printf("2. Copy multiple files to folder\n");
        printf("3. Copy folder to folder without ignore\n");
        printf("4. Copy folder to folder with ignore\n");
        printf("5. Copy folder (no overwrite) \n");
        printf("6. Check directory contents\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        printf("=== DIRECTORY CONTENTS === \n");
        printDirectoryContents(".", 3);

        switch (choice)
        {
        case 1: // done
            printf("Enter source file: ");
            scanf("%s", source);

            printf("Enter destination file: ");
            scanf("%s", destination);

            copyFile(source, destination);

            break;
        case 2: // done
            copyFilesToFolder();
            break;
        case 3:
            printf("Enter source folder: ");
            scanf("%s", source);

            printf("=== DIRECTORY SOURCE === \n");
            printDirectoryContents(source, 3);

            printf("Enter destination folder: ");
            scanf("%s", destination);

            copyDirectory(source, destination, 0);

            printf("=== DIRECTORY DESTINATION === \n");
            printDirectoryContents(destination, 3);

            break;
        case 4:
            printf("Enter source folder: ");
            scanf("%s", source);

            printf("=== DIRECTORY SOURCE === \n");
            printDirectoryContents(source, 3);

            printf("Enter destination folder: ");
            scanf("%s", destination);

            copyDirectory(source, destination, 1);

            printf("=== DIRECTORY DESTINATION === \n");
            printDirectoryContents(destination, 3);

            break;
        case 5:
            printf("Enter source folder: ");
            scanf("%s", source);

            printf("=== DIRECTORY SOURCE === \n");
            printDirectoryContents(source, 3);

            printf("Enter destination folder: ");
            scanf("%s", destination);

            printf("Choose ignore or no ignore (1/0): ");
            scanf("%d", &ignore);

            copyDirectoryNoOverwrite(source, destination, ignore);

            printf("=== DIRECTORY DESTINATION === \n");
            printDirectoryContents(destination, 3);

            break;
        case 6: 
            break;
        case 7:
            printf("Exiting...\n");
            return 0;
        
        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }

        printf("\n");
    }

    return 0;
}
