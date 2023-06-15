#include <iostream>
#include <sstream>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <ctime>
#include <vector>
#include <algorithm>

struct FileInfo {
    std::string name;
    time_t lastModified;
    off_t size;
};

bool compareFiles(const FileInfo& file1, const FileInfo& file2) {
    return file1.name < file2.name;
}

std::string formatSize(double size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    while (size >= 1024 && unitIndex < 4) {
        size /= 1024;
        unitIndex++;
    }

    char buffer[100];
    std::sprintf(buffer, "%.2f %s", size, units[unitIndex]);
    return std::string(buffer);
}

std::string formatTime(const time_t& time) {
    struct tm* timeinfo;
    char buffer[80];
    timeinfo = localtime(&time);
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", timeinfo);
    return std::string(buffer);
}

int main() {
    DIR* dir;
    struct dirent* entry;
    struct stat fileStat;

    // 현재 디렉토리 열기
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    // 파일 목록 읽기
    std::vector<FileInfo> files;
    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &fileStat) < 0) {
            perror("stat");
            return 1;
        }

        FileInfo fileInfo;
        fileInfo.name = entry->d_name;
        fileInfo.lastModified = fileStat.st_mtime;
        fileInfo.size = fileStat.st_size;

        files.push_back(fileInfo);
    }

    // 파일 목록 정렬
    std::sort(files.begin(), files.end(), compareFiles);

    // HTML 헤더 출력
    std::cout << "<html>\n<head>\n<title>Directory Listing</title>\n</head>\n<body>\n";
    std::cout << "<table>\n<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";

    // 정렬된 파일 목록 출력
    for (std::vector<FileInfo>::const_iterator it = files.begin(); it != files.end(); ++it) {
        std::cout << "<tr>";
        // 파일 이름 출력
        std::cout << "<td><a href=\"" << it->name << "\">" << it->name << "</a></td>";

        // 파일 마지막 수정일 출력
        std::cout << "<td>" << formatTime(it->lastModified) << "</td>";

        // 파일 크기 출력
        double fileSize = static_cast<double>(it->size);
        std::cout << "<td>" << formatSize(fileSize) << "</td>";

        std::cout << "</tr>\n";
    }

    // HTML 푸터 출력
    std::cout << "</table>\n</body>\n</html>\n";

    // 디렉토리 닫기
    closedir(dir);

    return 0;
}